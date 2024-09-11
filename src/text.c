#include "character.c"

// Draw a character from an 8x8 pixel font atlas
void draw_char_8px(sprite_atlas *font_atlas, Vector2I position, char c) {
    Vector2I atlas_position = {(c * 8) % font_atlas->width, ((c * 8) / font_atlas->width) * 8};
    draw_sprite(font_atlas, position, atlas_position, (Vector2I){8,8});
}

// Draw a string with 8x8 pixel characters
void draw_string_8px(sprite_atlas *font_atlas, Vector2I position, char *str) {
    for (char *p = str; *p != '\0'; p++) {
        if (*p != ' ') {
            draw_char_8px(font_atlas, position, *p);
        }
        position.x += 8;
    }
}
