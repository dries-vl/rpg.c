#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinycthread/tinycthread.h>
#include <curl/curl.h>

#define OPENROUTER_API_KEY "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"
#define OPENROUTER_URL "https://openrouter.ai/api/v1/chat/completions"

#define MAX_RESPONSE_SIZE 1024
#define MAX_PROMPT_SIZE 512

// Global response buffer
char current_llm_response[MAX_RESPONSE_SIZE] = {0};
size_t current_response_len = 0; // Track current length of accumulated response

// Helper function to process packets of potentially multiple JSON objects containing llm output
void process_json(const char *json_data) {
    const char *start_ptr = json_data;
    const char *content_key = "\"content\":\"";
    size_t content_key_len = strlen(content_key);
    // Loop over all occurrences of the content key
    while ((start_ptr = strstr(start_ptr, content_key)) != NULL) {
        start_ptr += content_key_len;  // Move the pointer past "content":" 
        const char *end_ptr = start_ptr; // keep track of the final character of this content block
        int in_escape = 0; // To track if the current character is escaped
        // Loop through the content until we find the correct closing quote
        while (*end_ptr) {
            if (in_escape) {
                in_escape = 0; // The next character after '\' is part of the content
            } else if (*end_ptr == '\\') {
                in_escape = 1; // Start escape sequence when encountering '\'
            } else if (*end_ptr == '"') {
                // Found the end of the content
                break;
            }
            end_ptr++;
        }
        size_t content_len = (size_t)(end_ptr - start_ptr);
        // Append the content to the response buffer
        snprintf(current_llm_response + current_response_len, MAX_RESPONSE_SIZE - current_response_len, "%.*s", (int)content_len, start_ptr);
        // Update the current response length
        current_response_len += content_len;
        start_ptr = end_ptr;
    }
}

// Callback to handle incoming streamed data
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    char *data = (char*)malloc(total_size + 1);
    if (data) {
        memcpy(data, ptr, total_size);
        data[total_size] = '\0';  // Null-terminate the string
        process_json(data);
        free(data);
    }
    return total_size;
}

// Blocking request function; use async below to non-block
int llm_request_stream(void *prompt) {
    CURL *curl;
    CURLM *multi_handle;
    int still_running = 0; // To check when the transfer is done
    CURLMcode mc;
    CURLcode res;

    // Initialize libcurl
    curl = curl_easy_init();
    multi_handle = curl_multi_init();
    
    if(curl && multi_handle) {
        // Set the URL for the POST request
        curl_easy_setopt(curl, CURLOPT_URL, OPENROUTER_URL); // Replace with actual URL

        // Set the headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", OPENROUTER_API_KEY);
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // JSON body of the POST request
        char post_data[MAX_PROMPT_SIZE]; // Adjust size as needed
        snprintf(post_data, sizeof(post_data), "{"
                 "\"model\":\"cohere/command-r-plus-08-2024\","
                 "\"messages\":[{\"role\":\"user\", \"content\":\"%s\"}],"
                 "\"stream\": true,"
                 "\"seed\": 1"
                 "}", (char *) prompt);

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

        // Set the callback function to handle streamed data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Add some settings
        // curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // Timeout after 30 seconds
        // curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5L); // Abort if speed is below the threshold for 5 seconds
        // curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 10L); // If less than 10 bytes per second are received
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);  // Enable verbose output
        // Add the easy handle to the multi handle
        curl_multi_add_handle(multi_handle, curl);

        // Perform the request in a non-blocking way
        curl_multi_perform(multi_handle, &still_running);

        // Poll the multi handle until the transfer is complete
        while(still_running) {
            int numfds = 0;
            mc = curl_multi_wait(multi_handle, NULL, 0, 1000, &numfds); // Wait for activity or timeout
            
            if (mc != CURLM_OK) {
                fprintf(stderr, "curl_multi_wait() failed, code %d.\n", mc);
                break;
            }

            // Perform the request, check if still running
            curl_multi_perform(multi_handle, &still_running);
        }

        // Cleanup
        curl_multi_remove_handle(multi_handle, curl);
        curl_easy_cleanup(curl);
        curl_multi_cleanup(multi_handle);
        curl_slist_free_all(headers);
    }

    return 0;
}

void llm_request_async(const char *prompt) {
    thrd_t thread_id;
    
    // Create a thread to handle the request asynchronously
    if (thrd_create(&thread_id, llm_request_stream, (void *)prompt) != thrd_success) {
        fprintf(stderr, "Error creating thread\n");
        return;
    }

    // Optionally, join the thread if you need to wait for completion (not necessary for fire-and-forget)
    // thrd_join(thread_id, NULL);
}

