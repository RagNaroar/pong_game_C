#include <stdio.h>
#include "config.h"
#include "input.h"
#include "render.h"

void draw_field(void) {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (y == 0 && x == 0) printf("%s", CHAR_TL);
            else if (y == 0 && x == FIELD_WIDTH - 1) printf("%s", CHAR_TR);
            else if (y == FIELD_HEIGHT - 1 && x == 0) printf("%s", CHAR_BL);
            else if (y == FIELD_HEIGHT - 1 && x == FIELD_WIDTH - 1) printf("%s", CHAR_BR);
            else if (y == 0 || y == FIELD_HEIGHT - 1) printf("%s", CHAR_H);
            else if (x == 0 || x == FIELD_WIDTH - 1) printf("%s", CHAR_V);
            else printf(" ");
        }
        putchar('\n');
    }
}
void draw_paddle(Paddle p) {
    for (int i = 0; i < p.height + 2; i++) { // +2 для верха и низа
        int draw_y = p.y + i;
        if (draw_y <= 0 || draw_y >= FIELD_HEIGHT - 1) continue;
        printf("\033[%d;%dH", draw_y + 1, p.x + 1); // ANSI позиция
        if (i == 0) printf(PADDLE_TOP);
        else if (i == p.height + 1) printf(PADDLE_BOTTOM);
        else printf(PADDLE_MID);
    }
    fflush(stdout); // сразу выводим
}