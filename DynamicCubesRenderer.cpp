#include <iostream>
#include <cmath>
#include <cstring>

using namespace std;

#ifdef _WIN32
#include <windows.h>

void sleep_ms(__int64 milliseconds) {
    HANDLE timer;
    LARGE_INTEGER ft{};
    ft.QuadPart = -(10 * milliseconds);
    timer = CreateWaitableTimer(NULL, TRUE, NULL);

    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}

#else
#include <unistd.h>
void sleep_ms(__useconds_t milliseconds) {
    ::sleep_ms(milliseconds * 1000);
}
#endif

const int SCREEN_WIDTH = 160, SCREEN_HEIGHT = 44;
const float CUBE_SIZE = 20.0f;
const int DISTANCE_FROM_CAMERA = 100;
const float RENDER_SCALE = 40.0f;
const float MOVEMENT_SPEED = 0.6f;
static const char BACKGROUND_CHAR = ' ';

float rotationX = 0.0f, rotationY = 0.0f, rotationZ = 0.0f;
float zBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
char buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

float calculateX(int i, int j, int k) {
    return j * sin(rotationX) * sin(rotationY) * cos(rotationZ) - k * cos(rotationX) * sin(rotationY) * cos(rotationZ) +
        j * cos(rotationX) * sin(rotationZ) + k * sin(rotationX) * sin(rotationZ) + i * cos(rotationY) * cos(rotationZ);
}

float calculateY(int i, int j, int k) {
    return j * cos(rotationX) * cos(rotationZ) + k * sin(rotationX) * cos(rotationZ) -
        j * sin(rotationX) * sin(rotationY) * sin(rotationZ) + k * cos(rotationX) * sin(rotationY) * sin(rotationZ) -
        i * cos(rotationY) * sin(rotationZ);
}

float calculateZ(int i, int j, int k) {
    return k * cos(rotationX) * cos(rotationY) - j * sin(rotationX) * cos(rotationY) + i * sin(rotationY);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, char ch, float horizontalOffset) {
    float x = calculateX(cubeX, cubeY, cubeZ);
    float y = calculateY(cubeX, cubeY, cubeZ);
    float z = calculateZ(cubeX, cubeY, cubeZ) + DISTANCE_FROM_CAMERA;

    float ooz = 1.0f / z;

    int xp = static_cast<int>(static_cast<float>(SCREEN_WIDTH) / 2 + horizontalOffset + RENDER_SCALE * ooz * x * 2);
    int yp = static_cast<int>(static_cast<float>(SCREEN_HEIGHT) / 2 + RENDER_SCALE * ooz * y);

    int idx = xp + yp * SCREEN_WIDTH;
    if (idx >= 0 && idx < SCREEN_WIDTH * SCREEN_HEIGHT) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

static void hideCursor() {
    cout << "\033[?25l";
    cout.flush();
}

int main() {
    hideCursor();

    cout << "\x1b[2J";

    while (true) {
        memset(buffer, BACKGROUND_CHAR, static_cast<size_t>(SCREEN_WIDTH) * SCREEN_HEIGHT);
        memset(zBuffer, 0, static_cast<unsigned long long>(SCREEN_WIDTH) * SCREEN_HEIGHT * sizeof(float));

        float horizontalOffset = -2 * CUBE_SIZE;
        for (float cubeX = -CUBE_SIZE; cubeX < CUBE_SIZE; cubeX += MOVEMENT_SPEED) {
            for (float cubeY = -CUBE_SIZE; cubeY < CUBE_SIZE; cubeY += MOVEMENT_SPEED) {
                calculateForSurface(cubeX, cubeY, -CUBE_SIZE, '@', horizontalOffset);
                calculateForSurface(CUBE_SIZE, cubeY, cubeX, '$', horizontalOffset);
                calculateForSurface(-CUBE_SIZE, cubeY, -cubeX, '~', horizontalOffset);
                calculateForSurface(-cubeX, cubeY, CUBE_SIZE, '#', horizontalOffset);
                calculateForSurface(cubeX, -CUBE_SIZE, -cubeY, ';', horizontalOffset);
                calculateForSurface(cubeX, CUBE_SIZE, cubeY, '+', horizontalOffset);
            }
        }

        horizontalOffset = CUBE_SIZE;
        for (float cubeX = -CUBE_SIZE / 2; cubeX < CUBE_SIZE / 2; cubeX += MOVEMENT_SPEED) {
            for (float cubeY = -CUBE_SIZE / 2; cubeY < CUBE_SIZE / 2; cubeY += MOVEMENT_SPEED) {
                calculateForSurface(cubeX, cubeY, -CUBE_SIZE / 2, '@', horizontalOffset);
                calculateForSurface(CUBE_SIZE / 2, cubeY, cubeX, '$', horizontalOffset);
                calculateForSurface(-CUBE_SIZE / 2, cubeY, -cubeX, '~', horizontalOffset);
                calculateForSurface(-cubeX, cubeY, CUBE_SIZE / 2, '#', horizontalOffset);
                calculateForSurface(cubeX, -CUBE_SIZE / 2, -cubeY, ';', horizontalOffset);
                calculateForSurface(cubeX, CUBE_SIZE / 2, cubeY, '+', horizontalOffset);
            }
        }

        horizontalOffset = 12 * CUBE_SIZE / 4;
        for (float cubeX = -CUBE_SIZE / 4; cubeX < CUBE_SIZE / 4; cubeX += MOVEMENT_SPEED) {
            for (float cubeY = -CUBE_SIZE / 4; cubeY < CUBE_SIZE / 4; cubeY += MOVEMENT_SPEED) {
                calculateForSurface(cubeX, cubeY, -CUBE_SIZE / 4, '@', horizontalOffset);
                calculateForSurface(CUBE_SIZE / 4, cubeY, cubeX, '$', horizontalOffset);
                calculateForSurface(-CUBE_SIZE / 4, cubeY, -cubeX, '~', horizontalOffset);
                calculateForSurface(-cubeX, cubeY, CUBE_SIZE / 4, '#', horizontalOffset);
                calculateForSurface(cubeX, -CUBE_SIZE / 4, -cubeY, ';', horizontalOffset);
                calculateForSurface(cubeX, CUBE_SIZE / 4, cubeY, '+', horizontalOffset);
            }
        }

        cout << "\x1b[H";
        for (int k = 0; k < SCREEN_WIDTH * SCREEN_HEIGHT; k++) {
            cout << (k % SCREEN_WIDTH ? buffer[k] : '\n');
        }

        rotationX += 0.06f;
        rotationY += 0.06f;
        rotationZ += 0.06f;
        sleep_ms(static_cast<long long>(4000) * 2);
    }

    return 0;
}
