#ifndef MY_CURL_H
#define MY_CURL_H

/* Forward declarations of types used by libcurl */
typedef void CURL;
typedef void CURLM;
typedef int CURLcode;
typedef int CURLMcode;

/* curl_waitfd struct definition */
struct curl_waitfd {
    int fd;           /* File descriptor */
    short events;     /* Events to monitor (e.g., POLLIN, POLLOUT) */
    short revents;    /* Events that were detected */
};

struct curl_slist;

/* Function prototypes for the libcurl functions you are using */
CURL *curl_easy_init(void);
CURLM *curl_multi_init(void);
struct curl_slist *curl_slist_append(struct curl_slist *list, const char *string);
CURLcode curl_easy_perform(CURL *curl);
CURLMcode curl_multi_perform(CURLM *multi_handle, int *still_running);
CURLMcode curl_multi_add_handle(CURLM *multi_handle, CURL *curl);
CURLMcode curl_multi_remove_handle(CURLM *multi_handle, CURL *curl);
CURLMcode curl_multi_cleanup(CURLM *multi_handle);
CURLMcode curl_multi_wait(CURLM *multi_handle, struct curl_waitfd extra_fds[], unsigned int extra_nfds, int timeout_ms, int *numfds);
void curl_slist_free_all(struct curl_slist *list);
void curl_easy_cleanup(CURL *curl);
CURLcode curl_easy_setopt(CURL *curl, int option, ...);
const char *curl_easy_strerror(CURLcode errornum);

/* Define options used with curl_easy_setopt */
#define CURLOPT_HTTPHEADER 10023
#define CURLOPT_URL 10002
#define CURLOPT_POSTFIELDS 10015
#define CURLOPT_WRITEFUNCTION 20011
#define CURLOPT_TIMEOUT 13        /* Timeout for entire operation */
#define CURLOPT_LOW_SPEED_TIME 19 /* Low speed time to abort */
#define CURLOPT_LOW_SPEED_LIMIT 20 /* Low speed limit */
#define CURLOPT_VERBOSE 41        /* Enable verbose output */

/* Common return codes */
#define CURLE_OK 0
#define CURLM_OK 0
#define CURLM_CALL_MULTI_PERFORM -1   /* Return code indicating more work is needed */

#endif /* MY_CURL_H */
