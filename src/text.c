#include "character.c"

// Draw a character from an 8x8 pixel font atlas
void draw_char_8px(sprite_atlas *font_atlas, Vector2I position, char c) {
    Vector2I atlas_position = {(c * 8) % font_atlas->width, ((c * 8) / font_atlas->width) * 8};
    draw_sprite(font_atlas, position, atlas_position, (Vector2I){8,8});
}


