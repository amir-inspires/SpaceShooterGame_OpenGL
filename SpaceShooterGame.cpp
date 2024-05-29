#ifdef _APPLE_CC_
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <ctime>
#include <vector>
#include <cmath>
#include <string>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

using namespace std;

float spaceshipPositionX = 0.0f;
int score = 0;
bool gameOver = false;
bool gameStarted = false;
bool gameWon = false;
bool restartKeyPressed = false;

class Star {
public:
    float x, y;
    float speed;
    bool isShot;

    // Constructor
    Star(float x, float y, float speed) : x(x), y(y), speed(speed), isShot(false) {}
};


class Bullet {
public:
    float x, y;
    float speed;
    bool hit;

    Bullet(float x, float y, float speed) : x(x), y(y), speed(speed), hit(false) {}
};

vector<Star> fallingStars;
vector<Bullet> bullets;
vector<pair<float, float>> backgroundStars;

// Generate a random number within range
int randomInRange(int min, int max) {
    return min + rand() % (max - min + 1);
}


void drawSpaceship() {
    // Draw the rectangle
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, 2.0);
    glVertex2f(-1.0, 2.0);
    glEnd();

    // Draw the triangles
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(1.0, 1.0);
    glVertex2f(2.0, -1.0);
    glVertex2f(1.0, -1.0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(-1.0, 1.0);
    glVertex2f(-2.0, -1.0);
    glVertex2f(-1.0, -1.0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(-1.0, 2.0);
    glVertex2f(1.0, 2.0);
    glVertex2f(0.0, 3.0);
    glEnd();
}

void drawStar() {
    glBegin(GL_POLYGON);
    glColor3f(1, 1, 1);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.0, -1.0);
    glVertex2f(0.5, 0.0);
    glVertex2f(0.0, 1.0);
    glEnd();
}

void drawBullet() {
    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);
    glVertex2f(-0.1f, -0.1f);
    glVertex2f(0.1f, -0.1f);
    glVertex2f(0.1f, 0.1f);
    glVertex2f(-0.1f, 0.1f);
    glEnd();
}

void generateFallingStar() {
    float randomX = static_cast<float>(randomInRange(-8, 8));
    fallingStars.push_back(Star(randomX, 10.0f, 0.1f));
}

void generateBackgroundStars() {
    for (int i = 0; i < 100; i++) {
        float randomX = static_cast<float>(randomInRange(-10, 10));
        float randomY = static_cast<float>(randomInRange(-10, 10));
        backgroundStars.push_back(make_pair(randomX, randomY));
    }
}

void drawMoon(float x, float y, float radius) {
    int numSegments = 100; // Number of segments to approximate the circle
    glColor3f(0.9098f, 0.9098f, 0.917f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float dx = radius * cosf(theta); // Calculate x component
        float dy = radius * sinf(theta); // Calculate y component
        glVertex2f(x + dx, y + dy); // Output vertex
    }
    glEnd();
    glColor3f(0.584f, 0.596f, 0.65f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float dx = radius / 5 * cosf(theta); // Calculate x component
        float dy = radius / 5 * sinf(theta); // Calculate y component
        glVertex2f(x + dx - 0.8, y - 0.8 + dy); // Output vertex
    }
    glEnd();
    glColor3f(0.678f, 0.6705f, 0.7137f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float dx = radius / 5 * cosf(theta); // Calculate x component
        float dy = radius / 5 * sinf(theta); // Calculate y component
        glVertex2f(x + dx + 0.2, y + 0.2 + dy); // Output vertex
    }
    glEnd();
}


void drawBackground() {

    drawMoon(7, 7, 2);

    for (const auto& pos : backgroundStars) {
        glPushMatrix();
        glTranslatef(pos.first, pos.second, 0.0f);
        glScalef(0.1, 0.1, 1);
        drawStar();
        glScalef(1, 1, 1);
        glPopMatrix();
    }
}

void drawGrid() {
    glColor3f(0.1f, 0.1f, 0.1f); // Set color to gray

    // Draw horizontal grid lines
    glBegin(GL_LINES);
    for (float y = -10.0f; y <= 10.0f; y += 1.0f) {
        glVertex2f(-10.0f, y);
        glVertex2f(10.0f, y);
    }
    glEnd();

    // Draw vertical grid lines
    glBegin(GL_LINES);
    for (float x = -10.0f; x <= 10.0f; x += 1.0f) {
        glVertex2f(x, -10.0f);
        glVertex2f(x, 10.0f);
    }
    glEnd();
}

void drawText(const string& text, float x, float y) {

    glColor3f(1, 1, 1);
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawBackground();

    if (!gameStarted) {
        drawText("Press any key to start the game", -5.5f, 0.0f);
        drawText("Use left/right arrows to move, space to shoot", -8.0f, -1.0f);
        drawText("Use space key to shoot", -4.5f, -2.0f);
    }
    else {
        // Draw Falling Stars
        for (const auto& star : fallingStars) {
            if (!star.isShot) {
                glPushMatrix();
                glTranslatef(star.x, star.y, 0.0f);
                drawStar();
                glPopMatrix();
            }
        }

        // Draw Bullets
        for (const auto& bullet : bullets) {
            glPushMatrix();
            glTranslatef(bullet.x, bullet.y, 0.0f);
            drawBullet();
            glPopMatrix();
        }

        // Draw Spaceship
        glPushMatrix();
        glTranslatef(spaceshipPositionX, -9.0f, 0.0f);
        drawSpaceship();
        glPopMatrix();

        // Draw Score
        drawText("Score: " + to_string(score), -9.5f, 9.0f);

        if (gameOver) {
            drawText("Gamee Over! Press 'R' to Restart or 'Q' to Quit", -7.5f, 0.0f);
        }
        if (gameWon) {
            drawText("You Win!", -1.5, 0);
            drawText("Press 'R' to Restart & 'Q' to Quit", -5.5, -1.0);
        }
    }

    glutSwapBuffers();
}

void updateDisplay(int value) {
    if (gameStarted && !gameOver && !gameWon) {

        for (auto& star : fallingStars) {
            star.y -= star.speed;
            if (star.y < -10.0f) {
                star.y = 10.0f;
                star.x = static_cast<float>(randomInRange(-9, 9));
                star.isShot = false; // Reset the shot status when star reappears
            }
        }

        // Check collision between spaceship and stars
        for (const auto& star : fallingStars) {
            float distanceX = star.x - spaceshipPositionX;
            float distanceY = star.y + 9.0f;
            float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

            if (distance <= 1.0) {
                PlaySoundA("gameover.wav", NULL, SND_ASYNC);
                gameOver = true;
            }
        }

        // Move bullets and check for collisions
        for (auto& bullet : bullets) {
            bullet.y += bullet.speed;
            for (auto& star : fallingStars) {
                float distanceX = star.x - bullet.x;
                float distanceY = star.y - bullet.y;
                float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

                if (distance <= 0.5 && !star.isShot) {
                    star.isShot = true;
                    bullet.hit = true;
                    score++;
                }
            }
        }

        // Remove hit bullets and stars
        bullets.erase(remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return b.hit; }), bullets.end());
        fallingStars.erase(remove_if(fallingStars.begin(), fallingStars.end(), [](Star& s) { return s.isShot; }), fallingStars.end());

        // Ensure there are at most 2 falling stars at a time
        while (fallingStars.size() < 2) {
            generateFallingStar();
        }

        if (score == 30) {
            gameWon = true;
        }

        glutPostRedisplay();
        glutTimerFunc(30, updateDisplay, 0);
    }
    else if (!gameStarted || gameOver || gameWon) {

        if (gameOver && restartKeyPressed) {
            spaceshipPositionX = 0.0f;
            score = 0;
            gameOver = false;
            gameWon = false;
            bullets.clear();
            fallingStars.clear();
            for (int i = 0; i < 2; i++) {
                generateFallingStar();
            }
            restartKeyPressed = false;
            PlaySoundA("BG_loop_music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        }

        glutPostRedisplay();
        glutTimerFunc(30, updateDisplay, 0);
    }
}

void handleArrows(int key, int x, int y) {
    if (gameStarted && !gameOver) {
        switch (key) {
        case GLUT_KEY_LEFT:
            spaceshipPositionX -= 1.0f;
            if (spaceshipPositionX <= -9.0f) {
                spaceshipPositionX = -9.0f;
            }
            break;
        case GLUT_KEY_RIGHT:
            spaceshipPositionX += 1.0f;
            if (spaceshipPositionX >= 9.0f) {
                spaceshipPositionX = 9.0f;
            }
            break;
        }
    }
}

void handleKeys(unsigned char key, int x, int y) {
    if (!gameStarted) {
        PlaySoundA("BG_loop_music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        gameStarted = true;
    }
    else if (!gameOver) {
        switch (key) {
        case ' ':
            bullets.push_back(Bullet(spaceshipPositionX, -7.5f, 0.2f));
            //PlaySoundA("shoot.wav", NULL, SND_ASYNC);
            break;
        }
    }
    else if (key == 'r' || key == 'R') {
        restartKeyPressed = true;
    }
    //quit with q or exc key
    if (key == 'q' || key == 'Q' || key == 27) { // 27 is the ASCII code for the Escape key
        exit(0);
    }
}

void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10.0, +10.0, -10.0, +10.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    srand(static_cast<unsigned int>(time(nullptr)));

    generateBackgroundStars();
    for (int i = 0; i < 2; i++) {
        generateFallingStar();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Star Avoider Game: move with left/right arrows");

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(handleArrows);
    glutKeyboardFunc(handleKeys);
    glutTimerFunc(30, updateDisplay, 0);

    glutMainLoop();
    return 0;
}
