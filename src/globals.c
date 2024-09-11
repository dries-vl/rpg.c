#include <stdint.h>

/* GLOBALS */
#define BASE_WIDTH  240
#define BASE_HEIGHT 160
#define GRID_SIZE 16
uint32_t base_buffer[BASE_WIDTH * BASE_HEIGHT];
uint32_t *scaled_buffer = NULL;

