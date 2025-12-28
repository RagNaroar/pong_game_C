#ifndef INPUT_H
#define INPUT_H

typedef struct {
    int x;       // горизонтальная позиция (левая/правая сторона)
    int y;       // верхняя клетка ракетки
    int height;  // длина середины ракетки
} Paddle;

void move_up(Paddle *p);
void move_down(Paddle *p);

#endif
