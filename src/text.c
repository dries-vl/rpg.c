#include "character.c"
#include "request.c"

#define CHAR_SIZE_8PX 8
#define SPACE_WIDTH_8PX 4
/* The width of each character in the original 8px font atlas, to remove empty space between characters and get correct kerning */
const int FONT_8PX_WIDTHS[128] = {
     8, /* NUL */  8, /* SOH */  8, /* STX */  8, /* ETX */
     8, /* EOT */  8, /* ENQ */  8, /* ACK */  8, /* BEL */
     8, /* BS  */  8, /* HT  */  8, /* LF  */  8, /* VT  */
     8, /* FF  */  8, /* CR  */  8, /* SO  */  8, /* SI  */
     8, /* DLE */  8, /* DC1 */  8, /* DC2 */  8, /* DC3 */
     8, /* DC4 */  8, /* NAK */  8, /* SYN */  8, /* ETB */
     8, /* CAN */  8, /* EM  */  8, /* SUB */  8, /* ESC */
     8, /* FS  */  8, /* GS  */  8, /* RS  */  8, /* US  */
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

int at_bottom(Vector2I *position, Vector2I starting_position, sprite_atlas *font_atlas) {
    if (position->y - starting_position.y >= CHAR_SIZE_8PX * 3) {
        draw_char_8px(font_atlas, *position, 0);
        return 1;
    }
    return 0;
}

void move_text_down(Vector2I *position, Vector2I starting_position) {
    position->x = starting_position.x;
    position->y += CHAR_SIZE_8PX;
}

// Draw a string with 8x8 pixel characters
void draw_string_8px(sprite_atlas *font_atlas, Vector2I position, char *str) {
    Vector2I starting_position = ivec2(position.x, position.y);
    int escaped = 0;
    int bottom = 0;
    for (char *p = str; *p != '\0'; p++) {
        switch (*p) {
            case ' ': position.x += SPACE_WIDTH_8PX; break;
            case 92: /*'\\'*/ escaped = 1; break;
            case 10: /*'\n'*/ { 
                bottom = at_bottom(&position, starting_position, font_atlas);
                move_text_down(&position, starting_position);
                break;
                }
            case 'n': if (escaped) {
                bottom = at_bottom(&position, starting_position, font_atlas);
                move_text_down(&position, starting_position);
                break;
                } // FALL-THROUGH!!!
            default: draw_char_8px(font_atlas, position, *p); position.x += FONT_8PX_WIDTHS[*p]; break;
        }
        if (position.x >= (BASE_WIDTH - (6 + (*p==' ')*24)) - CHAR_SIZE_8PX) { // 6px to stay within ui, 24px extra for space to cut off words
            move_text_down(&position, starting_position);
        }
        if (*p != 92) escaped = 0;
        if (bottom) return;
    }
}


