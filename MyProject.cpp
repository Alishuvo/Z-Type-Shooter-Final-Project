#include <GL/glut.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

enum GameState { PLAYING, TRANSITION, GAME_OVER, VICTORY };

struct Enemy {
    std::string word;
    std::string typed;
    float x, y;
    float flashTimer = 0.0f;
    bool active = true;
};

struct Star {
    float x, y, speed;
};

struct Explosion {
    float x, y;
    float timer;
};

std::vector<Enemy> enemies;
std::vector<Star> stars;
std::vector<Explosion> explosions;

std::vector<std::string> wordPool = {
    "code", "glut", "game", "type", "enemy", "attack", "laser", "alpha", "delta", "vector",
    "shoot", "blast", "rapid", "focus", "logic", "array", "loop", "debug", "input", "score"
};

GameState gameState = PLAYING;
int level = 1;
int score = 0;
float speed = 0.002f;
int maxEnemies = 5;
float transitionTimer = 0.0f;
int windowWidth = 800, windowHeight = 600;

// -------- UTILS -------- //
void drawText(float x, float y, const std::string& text, float r, float g, float b, void* font = GLUT_BITMAP_HELVETICA_18) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(font, c);
}

float randX() {
    return -0.9f + static_cast<float>(rand() % 100) / 100.0f * 1.8f;
}

// -------- BACKGROUND -------- //
void initStars(int count = 100) {
    stars.clear();
    for (int i = 0; i < count; ++i) {
        stars.push_back({ randX(), static_cast<float>((rand() % 200) / 100.0f), 0.001f + static_cast<float>(rand() % 5) / 1000.0f });
    }
}

// -------- GAME SETUP -------- //
void spawnEnemies(int count) {
    enemies.clear();
    std::random_shuffle(wordPool.begin(), wordPool.end());
    for (int i = 0; i < count; ++i) {
        Enemy e;
        e.word = wordPool[i % wordPool.size()];
        e.typed = "";
        e.x = randX();
        e.y = 1.2f + (i * 0.2f);
        enemies.push_back(e);
    }
}

void resetGame() {
    level = 1;
    score = 0;
    speed = 0.002f;
    maxEnemies = 5;
    gameState = PLAYING;
    spawnEnemies(maxEnemies);
    explosions.clear();
    initStars();
}

// -------- DISPLAY -------- //
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Draw stars
    for (auto& star : stars) {
        glColor3f(1, 1, 1);
        glBegin(GL_POINTS);
        glVertex2f(star.x, star.y);
        glEnd();
    }

    if (gameState == TRANSITION) {
        drawText(-0.3f, 0.0f, level > 10 ? "YOU WIN!" : "Level " + std::to_string(level), 1, 1, 0);
    }
    else if (gameState == GAME_OVER) {
        drawText(-0.3f, 0.2f, "GAME OVER", 1, 0.2f, 0.2f);
        drawText(-0.4f, 0.0f, "Final Score: " + std::to_string(score), 1, 1, 0.4f);
        drawText(-0.5f, -0.2f, "Press R to Restart or ESC to Exit", 0.6f, 0.8f, 1.0f);
    }
    else if (gameState == VICTORY) {
        drawText(-0.3f, 0.2f, "YOU WIN!", 0.4f, 1, 0.4f);
        drawText(-0.4f, 0.0f, "Final Score: " + std::to_string(score), 1, 1, 0.4f);
        drawText(-0.5f, -0.2f, "Press R to Play Again or ESC to Exit", 0.6f, 0.8f, 1.0f);
    }
    else {
        // Enemies
        for (auto& e : enemies) {
            if (!e.active) continue;
            float flash = std::max(0.0f, e.flashTimer);
            drawText(e.x, e.y, e.word.substr(e.typed.size()), 1.0f, 0.2f + flash, 0.2f);
            if (!e.typed.empty())
                drawText(e.x, e.y - 0.05f, e.typed, 0.4f, 1.0f, 0.4f);
        }

        // Explosions
        for (auto& ex : explosions) {
            float alpha = ex.timer * 2.0f;
            glColor4f(1.0f, 0.5f, 0.0f, alpha);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(ex.x, ex.y);
            for (int i = 0; i <= 10; ++i) {
                float angle = i * 2.0f * 3.14159f / 10;
                float radius = 0.05f * alpha;
                glVertex2f(ex.x + cos(angle) * radius, ex.y + sin(angle) * radius);
            }
            glEnd();
        }

        // HUD
        drawText(-0.95f, 0.9f, "Score: " + std::to_string(score), 1, 1, 0.2f);
        drawText(0.6f, 0.9f, "Level: " + std::to_string(level), 0.4f, 0.8f, 1.0f);
    }

    glutSwapBuffers();
}

// -------- TIMER -------- //
void update(int value) {
    // Star animation
    for (auto& star : stars) {
        star.y -= star.speed;
        if (star.y < -1.0f) {
            star.x = randX();
            star.y = 1.0f;
        }
    }

    // Explosions update
    for (auto& ex : explosions) {
        ex.timer -= 0.05f;
    }
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(),
        [](Explosion& ex) { return ex.timer <= 0; }), explosions.end());

    // Game state logic
    if (gameState == TRANSITION) {
        transitionTimer -= 0.05f;
        if (transitionTimer <= 0.0f) {
            gameState = (level > 10) ? VICTORY : PLAYING;
        }
    }

    if (gameState == PLAYING) {
        bool allDone = true;
        for (auto& e : enemies) {
            if (!e.active) continue;
            allDone = false;
            e.y -= speed;
            if (e.flashTimer > 0) e.flashTimer -= 0.05f;
            if (e.y <= -1.1f) {
                gameState = GAME_OVER;
            }
        }

        if (allDone && !enemies.empty()) {
            level++;
            if (level > 10) {
                gameState = TRANSITION;
                transitionTimer = 2.0f;
            }
            else {
                maxEnemies += 2;
                speed += 0.0005f;
                spawnEnemies(maxEnemies);
                gameState = TRANSITION;
                transitionTimer = 2.0f;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// -------- INPUT -------- //
void keyboard(unsigned char key, int x, int y) {
    if (gameState == GAME_OVER || gameState == VICTORY) {
        if (key == 'r' || key == 'R') {
            resetGame();
        }
        else if (key == 27) {
            exit(0);
        }
        return;
    }

    for (auto& e : enemies) {
        if (!e.active) continue;
        std::string remaining = e.word.substr(e.typed.size());
        if (!remaining.empty() && key == remaining[0]) {
            e.typed += key;
            e.flashTimer = 0.5f;
            if (e.typed == e.word) {
                e.active = false;
                score += 10;
                explosions.push_back({ e.x, e.y, 0.5f }); // Add explosion effect
            }
            break;
        }
    }
}

// -------- INIT -------- //
void init() {
    glClearColor(0, 0, 0.05f, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    std::srand((unsigned int)time(0));
    resetGame();
    initStars();
}

// -------- MAIN -------- //
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Z-Type Typing Shooter - GLUT");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
