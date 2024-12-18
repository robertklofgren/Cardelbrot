#include "M5Cardputer.h"

// Mandelbrot Globals
double minRe = -2.0, maxRe = 1.0, minIm, maxIm;  // Real and imaginary ranges
int MAX_ITER = 100;
bool needsRedraw = true;

// Function Prototypes
void drawMandelbrot();
void displayStartupInstructions();
void handleInput();
void resetView();
void pan(double deltaX, double deltaY);
void zoom(bool in);

void setup() {
    M5Cardputer.begin(true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);

    // Initialize Mandelbrot Set Dimensions
    minIm = -(maxRe - minRe) * M5Cardputer.Display.height() / M5Cardputer.Display.width() / 2.0;
    maxIm = -minIm;

    // Display Instructions
    displayStartupInstructions();

    // Initial Render
    needsRedraw = true;
}

void loop() {
    M5Cardputer.update();
    handleInput();

    if (needsRedraw) {
        drawMandelbrot();
        needsRedraw = false;
    }
}

// Function to Render Mandelbrot
void drawMandelbrot() {
    int width = M5Cardputer.Display.width();
    int height = M5Cardputer.Display.height();

    double reFactor = (maxRe - minRe) / width;
    double imFactor = (maxIm - minIm) / height;

    // Dynamically adjust MAX_ITER for finer details
    MAX_ITER = 100 + (int)(100 * log10(2.0 / (maxRe - minRe)));

    M5Cardputer.Display.startWrite();
    for (int y = 0; y < height; y++) {
        double c_im = maxIm - y * imFactor;

        for (int x = 0; x < width; x++) {
            double c_re = minRe + x * reFactor;

            double Z_re = c_re, Z_im = c_im;
            int iter = 0;

            while (iter < MAX_ITER) {
                double Z_re2 = Z_re * Z_re;
                double Z_im2 = Z_im * Z_im;

                if (Z_re2 + Z_im2 > 4.0) break;

                Z_im = 2.0 * Z_re * Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;

                iter++;
            }

            uint16_t color = iter == MAX_ITER ? TFT_BLACK : M5Cardputer.Display.color565(iter % 16 * 16, iter % 32 * 8, iter % 8 * 32);
            M5Cardputer.Display.drawPixel(x, y, color);
        }
    }
    M5Cardputer.Display.endWrite();
}

// Function to Display Startup Instructions
void displayStartupInstructions() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);

    const char* line1 = "Pan: arrows";
    const char* line2 = "Zoom: +/-";
    const char* line3 = "Reset: del";

    int screenWidth = M5Cardputer.Display.width();
    int yOffset = M5Cardputer.Display.height() / 2 - 20;

    M5Cardputer.Display.setCursor((screenWidth - M5Cardputer.Display.textWidth(line1)) / 2, yOffset);
    M5Cardputer.Display.println(line1);

    M5Cardputer.Display.setCursor((screenWidth - M5Cardputer.Display.textWidth(line2)) / 2, yOffset + 15);
    M5Cardputer.Display.println(line2);

    M5Cardputer.Display.setCursor((screenWidth - M5Cardputer.Display.textWidth(line3)) / 2, yOffset + 30);
    M5Cardputer.Display.println(line3);

    delay(3000);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
}

// Function to Handle Input
void handleInput() {
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

        // Check for Del or Backspace
        if (status.del) {
            resetView();
            return;
        }

        for (char key : status.word) {
            switch (key) {
                case ';': pan(0, (maxIm - minIm) / 10); break;   // Up 
                case '/': pan((maxRe - minRe) / 10, 0); break;   // Right
                case '.': pan(0, -(maxIm - minIm) / 10); break;  // Down 
                case ',': pan(-(maxRe - minRe) / 10, 0); break;  // Left
                case '=': case '+': zoom(true); break;          // Zoom In
                case '-': case '_': zoom(false); break;         // Zoom Out
            }
        }
    }
}

// Function to Reset View
void resetView() {
    minRe = -2.0;
    maxRe = 1.0;
    minIm = -(maxRe - minRe) * M5Cardputer.Display.height() / M5Cardputer.Display.width() / 2.0;
    maxIm = -minIm;
    MAX_ITER = 100;
    needsRedraw = true;
}

// Function to Pan the View
void pan(double deltaX, double deltaY) {
    double reCenter = (minRe + maxRe) / 2.0;
    double imCenter = (minIm + maxIm) / 2.0;

    reCenter += deltaX;
    imCenter += deltaY;

    double reRange = maxRe - minRe;
    double imRange = maxIm - minIm;

    minRe = reCenter - reRange / 2.0;
    maxRe = reCenter + reRange / 2.0;
    minIm = imCenter - imRange / 2.0;
    maxIm = imCenter + imRange / 2.0;

    needsRedraw = true;
}

// Function to Zoom the View
void zoom(bool in) {
    double factor = in ? 0.8 : 1.25;  // Zoom In: 80%, Zoom Out: 125%
    double reCenter = (minRe + maxRe) / 2.0;
    double imCenter = (minIm + maxIm) / 2.0;

    double reRange = (maxRe - minRe) * factor;
    double imRange = (maxIm - minIm) * factor;

    minRe = reCenter - reRange / 2.0;
    maxRe = reCenter + reRange / 2.0;
    minIm = imCenter - imRange / 2.0;
    maxIm = imCenter + imRange / 2.0;

    needsRedraw = true;
}
