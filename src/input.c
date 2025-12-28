#include "input.h"
#include "config.h"

void move_up(Paddle *p) {
    if (p->y > 1) p->y--;  // не пересекать верх рамки
}

void move_down(Paddle *p) {
    if (p->y + p->height + 1 < FIELD_HEIGHT - 1) p->y++; // +1 для нижней части
}