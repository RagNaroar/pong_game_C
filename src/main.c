#include <stdio.h>
#include "config.h"
#include "input.h"
#include "render.h"

int main(void) {
    Paddle left  = {1, FIELD_HEIGHT / 2 - 1, 3};
    Paddle right = {FIELD_WIDTH - 3, FIELD_HEIGHT / 2 - 1, 3};

    draw_field();
    draw_paddle(left);
    draw_paddle(right);
    return 0;
}