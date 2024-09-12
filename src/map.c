#include "ui.c"


void draw_tile(sprite_atlas *tile_atlas, Vector2I position, int tile) {
    draw_frame(tile_atlas, position, tile, (Vector2I){GRID_SIZE, GRID_SIZE}); // might be problems if gridsize changes or if other types of tiles are added
}

void load_map(sprite_atlas *map, Game_state *state) {
    state->collision_map = (char **)malloc(map->height * sizeof(char *));
    for (int i = 0; i < map->height; i++) {
        state->collision_map[i] = (char *)malloc((map->width + 1) * sizeof(char));
        for (int j = 0; j < map->width; j++) {
            if (map->buffer[i * map->width + j] == 0xFF000000) {
                state->collision_map[i][j] = '#';
            }
            else {
                state->collision_map[i][j] = ' ';
            }
        }
    }
}

void draw_map(sprite_atlas *map, sprite_atlas *tile_atlas) {
    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            int tile = 0;
            uint32_t pixel = 0;
            pixel = map->buffer[i * map->width + j];
            int R = ((((pixel & 0x00FF0000) >> 16) + 1) / 16); // >> is a bitshift operator shifting by 16 bits to the right
            int G = ((((pixel & 0x0000FF00) >> 8) + 1) / 16);
            int B = (((pixel & 0x000000FF) + 1) / 16);
            tile = R * 32 + G * 16 + B;
            draw_tile(tile_atlas, (Vector2I){j * GRID_SIZE, i * GRID_SIZE}, tile);
        }
    }
}
