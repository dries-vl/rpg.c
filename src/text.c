#include "character.c"
#include "request.c"

#define CHAR_SIZE_8PX 8
#define SPACE_WIDTH_8PX 4
/* The width of each character in the original 8px font atlas, to remove empty space between characters and get correct kerning */
const int FONT_8PX_WIDTHS[128] = {
     0, /* NUL */  0, /* SOH */  0, /* STX */  0, /* ETX */
     0, /* EOT */  0, /* ENQ */  0, /* ACK */  0, /* BEL */
     0, /* BS  */  0, /* HT  */  8, /* LF  */  0, /* VT  */
     0, /* FF  */  8, /* CR  */  0, /* SO  */  0, /* SI  */
     0, /* DLE */  0, /* DC1 */  0, /* DC2 */  0, /* DC3 */
     0, /* DC4 */  0, /* NAK */  0, /* SYN */  0, /* ETB */
     0, /* CAN */  0, /* EM  */  0, /* SUB */  0, /* ESC */
     0, /* FS  */  0, /* GS  */  0, /* RS  */  0, /* US  */
     8, /* ' ' */  2, /* '!' */  4, /* '"' */  7, /* '#' */
     6, /* '$' */  8, /* '%' */  6, /* '&' */  2, /* '\'' */
     4, /* '(' */  4, /* ')' */  6, /* '*' */  6, /* '+' */
     3, /* ',' */  5, /* '-' */  2, /* '.' */  4, /* '/' */
     6, /* '0' */  4, /* '1' */  6, /* '2' */  6, /* '3' */
     6, /* '4' */  6, /* '5' */  6, /* '6' */  6, /* '7' */
     6, /* '8' */  6, /* '9' */  2, /* ':' */  3, /* ';' */
     4, /* '<' */  5, /* '=' */  4, /* '>' */  6, /* '?' */
     8, /* '@' */  6, /* 'A' */  6, /* 'B' */  6, /* 'C' */
     6, /* 'D' */  6, /* 'E' */  6, /* 'F' */  6, /* 'G' */
     6, /* 'H' */  2, /* 'I' */  6, /* 'J' */  6, /* 'K' */
     6, /* 'L' */  8, /* 'M' */  6, /* 'N' */  6, /* 'O' */
     6, /* 'P' */  6, /* 'Q' */  6, /* 'R' */  6, /* 'S' */
     6, /* 'T' */  6, /* 'U' */  6, /* 'V' */  8, /* 'W' */
     6, /* 'X' */  6, /* 'Y' */  6, /* 'Z' */  4, /* '[' */
     4, /* '\\' */ 4, /* ']' */  6, /* '^' */  6, /* '_' */
     3, /* '`' */  6, /* 'a' */  6, /* 'b' */  6, /* 'c' */
     6, /* 'd' */  6, /* 'e' */  5, /* 'f' */  6, /* 'g' */
     6, /* 'h' */  2, /* 'i' */  6, /* 'j' */  6, /* 'k' */
     3, /* 'l' */  8, /* 'm' */  6, /* 'n' */  6, /* 'o' */
     6, /* 'p' */  6, /* 'q' */  6, /* 'r' */  6, /* 's' */
     5, /* 't' */  6, /* 'u' */  6, /* 'v' */  8, /* 'w' */
     6, /* 'x' */  6, /* 'y' */  6, /* 'z' */  5, /* '{' */
     3, /* '|' */  5, /* '}' */  7, /* '~' */  7  /* DEL */
};

// Draw a character from an 8x8 pixel font atlas
void draw_char_8px(sprite_atlas *font_atlas, Vector2I position, char c) {
    Vector2I atlas_position = {(c * 8) % font_atlas->width, ((c * 8) / font_atlas->width) * 8};
    draw_sprite(font_atlas, position, atlas_position, (Vector2I){FONT_8PX_WIDTHS[c],8});
}

// Draw a string with 8x8 pixel characters
void draw_string_8px(sprite_atlas *font_atlas, Vector2I position, char *str) {
    int original_position_x = position.x;
    for (char *p = str; *p != '\0'; p++) {
        switch (*p) {
            case ' ': position.x += SPACE_WIDTH_8PX; break;
            case '\n': position.y += CHAR_SIZE_8PX; position.x = original_position_x; break;
            default: draw_char_8px(font_atlas, position, *p); position.x += FONT_8PX_WIDTHS[*p];
        }
        if (position.x >= BASE_WIDTH - CHAR_SIZE_8PX) {
            position.x = original_position_x;
            position.y += CHAR_SIZE_8PX;
        }
    }
}
