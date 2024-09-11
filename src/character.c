#include <stdint.h>
#include <math.h>

#include "load.c"

typedef struct {
    int x;
    int y;
} Vector2I;

typedef struct {
    double x;
    double y;
} Vector2F;

typedef enum {UP, RIGHT, DOWN, LEFT, IDLE} Direction;

typedef struct {
    Vector2I position; // draw position in pixels
    Vector2F position_exact; // draw position exact
    Vector2I grid_position; // grid position in tiles
    Direction direction;
    Direction move;
    int size;
    sprite_atlas *sprite_sheet;
    int status; // 0 = idle, 1 = running
    int stop; // 0 = continuous movement, 1 = stopped
} Player;

typedef struct {
    Player player;
    char **collision_map;
} Game_state;

Game_state init_game_state(Player player, Vector2I map_size) {
    Game_state game_state;
    game_state.player = player;
    return game_state;
}

Game_state game_state;
/*
                                                ----RENDERING----
*/

// Function to draw a sprite to the screen
void draw_sprite(sprite_atlas *sprite, Vector2I position, Vector2I atlas_location, Vector2I sprite_size) {
    for (int i = atlas_location.y; i < sprite_size.y + atlas_location.y; i++) {
        for (int j = atlas_location.x; j < sprite_size.x + atlas_location.x; j++) { // loop through the sprite in the atlas
            // Check if the pixel is within the bounds of the base buffer
            if ((position.y + i - atlas_location.y) < BASE_HEIGHT && (position.x + j - atlas_location.x) < BASE_WIDTH) {
                // Check if the pixel is within the bounds of the atlas
                if (i < sprite->height && j < sprite->width) {
                    // add transparency
                    uint32_t pixel = sprite->buffer[i * sprite->width + j];
                    uint32_t pixel_old = base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)];
                    double alpha = (pixel & 0xFF000000) / 0xFF000000;
                    //printf("%f\n", alpha);
                    //pixel = base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)] * (1-alpha) + pixel * alpha;
                    //pixel = base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)] * (1-alpha);
                    uint32_t R = (int)((pixel & 0x00FF0000) * alpha) & 0x00FF0000;
                    uint32_t G = (int)((pixel & 0x0000FF00) * alpha) & 0x0000FF00;
                    uint32_t B = (int)((pixel & 0x000000FF) * alpha) & 0x000000FF;
                    R += (int)((pixel_old & 0x00FF0000) * (1.0f-alpha)) & 0x00FF0000;
                    G += (int)((pixel_old & 0x0000FF00) * (1.0f-alpha)) & 0x0000FF00;
                    B += (int)((pixel_old & 0x000000FF) * (1.0f-alpha)) & 0x000000FF;
                    base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)] = R + G + B;
                }
            }
        }
    }
}
// Same but a whole image
void draw_image(sprite_atlas *image, Vector2I position) {
    draw_sprite(image, position, (Vector2I){0, 0}, (Vector2I){image->width, image->height});
}

// Draw a frame
void draw_frame(sprite_atlas *atlas, Vector2I position, int frame, Vector2I sprite_size) {
    int x_position = (frame * sprite_size.x) % atlas->width;
    int y_position = ((frame * sprite_size.x) / atlas->width) * sprite_size.y;
    draw_sprite(atlas, position, (Vector2I){x_position, y_position}, sprite_size);
}


void draw_character_idle_animation(sprite_atlas *buffer, Vector2I position, int size, int start_frame, int frame_count, double time, double speed) {
    int frame = (int)(time * speed) % frame_count;
    draw_frame(buffer, position, frame + start_frame, (Vector2I){size, size});
}

void draw_player_run_animation(Player *player, double time, double speed) {
    draw_character_idle_animation(player->sprite_sheet, player->position, player->size, 16, 4, time, speed);
}

void draw_player_idle_animation(Player *player, double time, double speed) {
    draw_character_idle_animation(player->sprite_sheet, player->position, player->size, 32, 4, time, speed);
}

/*
                                                ----PLAYER LOGIC----
*/

Player create_player(Vector2I position, sprite_atlas *sprite_sheet) {
    Player player;
    player.position = (Vector2I){position.x * GRID_SIZE, position.y * GRID_SIZE};
    player.position_exact = (Vector2F){position.x * GRID_SIZE, position.y * GRID_SIZE};
    player.grid_position = position;
    player.direction = 1; // right
    player.move = IDLE; // idle
    player.size = 21;
    player.sprite_sheet = sprite_sheet;
    player.status = 1; // running
    return player;
}

int move_player(Player *player, Direction move) {
    player->stop = FALSE;
    player->move = move;
    player->direction = move;
}

void move_input(Direction move) {
    printf("input: %d\n", move);
    printf("player: %d\n", game_state.player.move);
    if (move == IDLE) {
        game_state.player.stop = TRUE;
    }
    if (game_state.player.move == IDLE) {
        printf("MOVE\n");
        move_player(&game_state.player, move);
    }
}

void update_player(Player *player, double delta, double time, double speed) {
    if (player->move != IDLE) {
        switch (player->move) {
            case UP:
                if (game_state.collision_map[player->grid_position.y - 1][player->grid_position.x] == ' ') {
                    player->position_exact.y -= 6 * speed * delta; // added 6 to make it move in sync with the run animation
                    player->position.y = (int)(round(player->position_exact.y));
                }
                else if (player->stop == TRUE) {
                    player->move = IDLE;
                }
                if (player->position.y <= (player->grid_position.y - 1) * GRID_SIZE) { // if player arrives at the tile above
                    player->position.y = (player->grid_position.y - 1) * GRID_SIZE; // set player position to the tile above
                    player->position_exact.y = (double)((player->grid_position.y - 1) * GRID_SIZE); // set player position exact to the tile above
                    player->grid_position.y -= 1;
                    if (player->stop == TRUE) {
                        player->move = IDLE;
                    }
                }
                break;
            case RIGHT:
                if (game_state.collision_map[player->grid_position.y][player->grid_position.x + 1] == ' ') {
                    player->position_exact.x += 6 * speed * delta;
                    player->position.x = (int)(round(player->position_exact.x));
                }
                else if (player->stop == TRUE) {
                    player->move = IDLE;
                }
                if (player->position.x >= (player->grid_position.x + 1) * GRID_SIZE) { // if player arrives at the tile to the right
                    player->position.x = (player->grid_position.x + 1) * GRID_SIZE; // set player position to the tile to the right
                    player->position_exact.x = (double)((player->grid_position.x + 1) * GRID_SIZE); // set player position exact to the tile to the right
                    player->grid_position.x += 1;
                    if (player->stop == TRUE) {
                        player->move = IDLE;
                    }
                }
                break;
            case DOWN:
                if (game_state.collision_map[player->grid_position.y + 1][player->grid_position.x] == ' ') {
                    player->position_exact.y += 6 * speed * delta;
                    player->position.y = (int)(round(player->position_exact.y));
                }
                else if (player->stop == TRUE) {
                    player->move = IDLE;
                }
                if (player->position.y >= (player->grid_position.y + 1) * GRID_SIZE) { // if player arrives at the tile below
                    player->position.y = (player->grid_position.y + 1) * GRID_SIZE; // set player position to the tile below
                    player->position_exact.y = (double)((player->grid_position.y + 1) * GRID_SIZE); // set player position exact to the tile below
                    player->grid_position.y += 1;
                    if (player->stop == TRUE) {
                        player->move = IDLE;
                    }
                }
                break;
            case LEFT:
                if (game_state.collision_map[player->grid_position.y][player->grid_position.x - 1] == ' ') {
                    player->position_exact.x -= 6 * speed * delta;
                    player->position.x = (int)(round(player->position_exact.x));
                }
                else if (player->stop == TRUE) {
                    player->move = IDLE;
                }
                if (player->position.x <= (player->grid_position.x - 1) * GRID_SIZE) { // if player arrives at the tile to the left
                    player->position.x = (player->grid_position.x - 1) * GRID_SIZE; // set player position to the tile to the left
                    player->position_exact.x = (double)((player->grid_position.x - 1) * GRID_SIZE); // set player position exact to the tile to the left
                    player->grid_position.x -= 1;
                    if (player->stop == TRUE) {
                        player->move = IDLE;
                    }
                }
                break;
        }
    }
    switch (player->move) {
        case UP:
            draw_player_run_animation(player, time, speed);
            break;
        case RIGHT:
            draw_player_run_animation(player, time, speed);
            break;
        case DOWN:
            draw_player_run_animation(player, time, speed);
            break;
        case LEFT:
            draw_player_run_animation(player, time, speed);
            break;
        case IDLE:
            draw_player_idle_animation(player, time, speed);
            break;
    }
}

