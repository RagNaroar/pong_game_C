#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

// Game configuration
#define FIELD_WIDTH  70
#define FIELD_HEIGHT 15
#define PADDLE_HEIGHT 3

// Box drawing characters
#define CHAR_TL "┌"
#define CHAR_TR "┐"
#define CHAR_BL "└"
#define CHAR_BR "┘"
#define CHAR_H  "─"
#define CHAR_V  "│"
#define PADDLE_TOP "┌┐"
#define PADDLE_MID "││"
#define PADDLE_BOTTOM "└┘"

// Structures
typedef struct {
    int x, y;
    int height;
} Paddle;

typedef struct {
    double x, y;
    double dx, dy;
} Ball;

typedef struct {
    int left, right;
} Score;

// Global variables
struct termios orig_termios;
Score score = {0, 0};

// Terminal control functions
void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Drawing functions
void draw_field() {
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
    for (int i = 0; i < p.height + 2; i++) {
        int draw_y = p.y + i;
        if (draw_y <= 0 || draw_y >= FIELD_HEIGHT - 1) continue;
        printf("\033[%d;%dH", draw_y + 1, p.x + 1);
        if (i == 0) printf("%s", PADDLE_TOP);
        else if (i == p.height + 1) printf("%s", PADDLE_BOTTOM);
        else printf("%s", PADDLE_MID);
    }
}

void draw_ball(Ball b) {
    int bx = (int)(b.x + 0.5);
    int by = (int)(b.y + 0.5);
    if (bx > 0 && bx < FIELD_WIDTH - 1 && by > 0 && by < FIELD_HEIGHT - 1) {
        printf("\033[%d;%dH", by + 1, bx + 1);
        printf("●");
    }
}

void draw_score() {
    printf("\033[%d;%dH", FIELD_HEIGHT + 2, 1);
    printf("Player 1: %d  |  Player 2: %d", score.left, score.right);
}

void draw_controls() {
    printf("\033[%d;%dH", FIELD_HEIGHT + 3, 1);
    printf("Controls: W/S (Left) | I/K (Right) | Q (Quit)");
}

// Game logic
void move_paddle(Paddle *p, int direction) {
    int new_y = p->y + direction;
    if (new_y > 0 && new_y + p->height + 1 < FIELD_HEIGHT - 1) {
        p->y = new_y;
    }
}

int check_paddle_collision(Ball *b, Paddle p) {
    int ball_y = (int)(b->y + 0.5);
    
    // Check if ball is at paddle x position
    if ((b->dx < 0 && (int)(b->x + 0.5) == p.x + 1) || 
        (b->dx > 0 && (int)(b->x + 0.5) == p.x)) {
        // Check if ball is within paddle height
        if (ball_y >= p.y && ball_y <= p.y + p.height + 1) {
            return 1;
        }
    }
    return 0;
}

void update_ball(Ball *b, Paddle left, Paddle right) {
    b->x += b->dx;
    b->y += b->dy;
    
    // Top and bottom wall collision
    if (b->y <= 1 || b->y >= FIELD_HEIGHT - 2) {
        b->dy = -b->dy;
        b->y += b->dy; // Move away from wall
    }
    
    // Paddle collision
    if (check_paddle_collision(b, left)) {
        b->dx = -b->dx;
        b->x = left.x + 2;
    }
    if (check_paddle_collision(b, right)) {
        b->dx = -b->dx;
        b->x = right.x - 1;
    }
    
    // Score - left side
    if (b->x <= 1) {
        score.right++;
        b->x = FIELD_WIDTH / 2;
        b->y = FIELD_HEIGHT / 2;
        b->dx = 0.2;
    }
    
    // Score - right side
    if (b->x >= FIELD_WIDTH - 2) {
        score.left++;
        b->x = FIELD_WIDTH / 2;
        b->y = FIELD_HEIGHT / 2;
        b->dx = -0.2;
    }
}

// Main game loop
int main() {
    srand(time(NULL));
    enable_raw_mode();
    
    // Initialize game objects
    Paddle left_paddle = {2, FIELD_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_HEIGHT};
    Paddle right_paddle = {FIELD_WIDTH - 4, FIELD_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_HEIGHT};
    Ball ball = {FIELD_WIDTH / 2.0, FIELD_HEIGHT / 2.0, 0.2, 0.15};
    
    printf("\033[2J\033[?25l"); // Clear screen and hide cursor
    
    char c;
    int running = 1;
    int last_paddle_move = 0; // For smoother paddle movement
    
    while (running) {
        // Input handling
        int paddle_moved = 0;
        while (read(STDIN_FILENO, &c, 1) == 1) {
            switch (c) {
                case 'w':
                case 'W':
                    move_paddle(&left_paddle, -1);
                    paddle_moved = 1;
                    break;
                case 's':
                case 'S':
                    move_paddle(&left_paddle, 1);
                    paddle_moved = 1;
                    break;
                case 'i':
                case 'I':
                    move_paddle(&right_paddle, -1);
                    paddle_moved = 1;
                    break;
                case 'k':
                case 'K':
                    move_paddle(&right_paddle, 1);
                    paddle_moved = 1;
                    break;
                case 'q':
                case 'Q':
                    running = 0;
                    break;
            }
        }
        
        // Update game state
        update_ball(&ball, left_paddle, right_paddle);
        
        // Render
        printf("\033[H"); // Move cursor to home
        draw_field();
        draw_paddle(left_paddle);
        draw_paddle(right_paddle);
        draw_ball(ball);
        draw_score();
        draw_controls();
        fflush(stdout);
        
        usleep(16666); // 60 FPS for ultra smooth gameplay
    }
    
    // Cleanup
    printf("\033[?25h"); // Show cursor
    printf("\033[2J\033[H");
    disable_raw_mode();
    printf("Final Score - Player 1: %d | Player 2: %d\n", score.left, score.right);
    
    return 0;
}