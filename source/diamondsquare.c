#include "diamondsquare.h"

static int DISQ_WIDTH, DISQ_HEIGHT;

void diamondSquare(double* map, int width, int height) {
    DISQ_WIDTH = width;
    DISQ_HEIGHT = height;
    assert(DISQ_WIDTH == DISQ_HEIGHT && isPowerOfTwo(DISQ_WIDTH-1));
    initMap(map);
    diamondSquareRecurse(map, 0, 0, DISQ_WIDTH, DISQ_HEIGHT, ROUGHNESS);
    printf("diamond square generated heightmap size (%d,%d)\n", DISQ_WIDTH, DISQ_HEIGHT);
}

int isPowerOfTwo(int x) {
    while (((x % 2) == 0) && x > 1) { /* While x is even and > 1 */
        x /= 2;
    }
    return x == 1;
}

void initMap(double* map) {
    for (int y = 0; y < DISQ_HEIGHT; y++) {
        for (int x = 0; x < DISQ_WIDTH; x++) {
            double setTo = -1.0;
            set(map, y, x, setTo);
        }
    }

    set(map, 0, 0, randomDouble());
    set(map, 0, DISQ_WIDTH-1, randomDouble());
    set(map, DISQ_HEIGHT-1, 0, randomDouble());
    set(map, DISQ_HEIGHT-1, DISQ_WIDTH-1, randomDouble());
}

void diamondSquareRecurse(double* map, int x, int y, int w, int h, double noiseScale) {
    if (w <= 2 || h <= 2) {
        return;
    }

    //do diamond square for this region
    int x2 = x + w-1; //rightmost cell
    int y2 = y + h-1; //bottom cell
    double topLeft = get(map, y, x);
    double topRght = get(map, y, x2);
    double botLeft = get(map, y2, x);
    double botRght = get(map, y2, x2);

    //diamond step
    double averageCorners = (topLeft + topRght + botLeft + botRght)/4.0;
    double newCenter = averageCorners - ((randomDouble() - 0.5) * noiseScale * 2);
    int centerX = x + w / 2;
    int centerY = y + h / 2;
    set(map, centerY, centerX, newCenter);

    //square step
    int midX = x + w/2;
    int midY = y + h/2;

    //top = avg(tl, tr) + noise
    double top = (topLeft + topRght)/2.0 + (randomDouble() - 0.5) * noiseScale;
    if (get(map, y, midX) == -1.0)
        set(map, y, midX, top);
    //bot = avg(bl, br) + noise
    double bot = (botLeft + botRght)/2.0 + (randomDouble() - 0.5) * noiseScale;
    if (get(map, y2, midX) == -1.0)
        set(map, y2, midX, bot);
    //lef = avg(tl, bl) + noise
    double lef = (topLeft + botLeft)/2.0 + (randomDouble() - 0.5) * noiseScale;
    if (get(map, midY, x) == -1.0)
        set(map, midY, x, lef);
    //rig = avg(tr, br) + noise
    double rig = (topRght + botRght)/2.0 + (randomDouble() - 0.5) * noiseScale;
    if (get(map, midY, x2) == -1.0)
        set(map, midY, x2, rig);

    //if can divide, do so for each quarter
    int newW = w/2+1;
    int newH = h/2+1;
    if (newW < 3 && newH < 3) {
        return;
    }
    newW = max(newW, 3);
    newH = max(newH, 3);

    double newNoiseScale = noiseScale * pow(2.0, -0.75);
    diamondSquareRecurse(map, x,       y,       newW, newH, newNoiseScale);
    diamondSquareRecurse(map, centerX, y,       newW, newH, newNoiseScale);
    diamondSquareRecurse(map, x,       centerY, newW, newH, newNoiseScale);
    diamondSquareRecurse(map, centerX, centerY, newW, newH, newNoiseScale);
}

double randomDouble() {
    return (double)rand()/(double)RAND_MAX;
}

int max(int a, int b) {
    return a > b ? a : b;
}

double get(double* map, int y, int x) {
    assert(x >= 0);
    assert(x < DISQ_WIDTH);
    assert(y >= 0);
    assert(y < DISQ_HEIGHT);
    int index = y * DISQ_WIDTH + x;
    return map[index];
}

void set(double* map, int y, int x, double set) {
    assert(x >= 0);
    assert(x < DISQ_WIDTH);
    assert(y >= 0);
    assert(y < DISQ_HEIGHT);
    int index = y * DISQ_WIDTH + x;
    map[index] = set;
}
