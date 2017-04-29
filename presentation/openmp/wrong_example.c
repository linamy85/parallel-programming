#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <omp.h>

#define Bailout (2.0)

// picture resolution
typedef unsigned char pixel_t[3];

static const int ImageWidth = 800;
static const int ImageHeight = 600;
static const double ScaleX[2] = {-2.5, 1.5};
static const double ScaleY[2] = {-2.0, 2.0};

static const double Radius = Bailout * Bailout;

static const double IterMax = 150;

void drawPixel(pixel_t res, double Cx, double Cy) {
    double Zx = 0, Zy = 0, Zx2 = 0, Zy2 = 0;
    int i;
    for (i = 0; i < IterMax && ((Zx2+Zy2) < Radius); i++) {
        Zy = 2 * Zx * Zy + Cy;
        Zx = Zx2 - Zy2 + Cx;
        Zx2 = Zx * Zx; Zy2 = Zy * Zy;
    };
    if (i == IterMax) {  // inside M => black
        res[0] = 0; res[1] = 0; res[2] = 0;
    } else {
        res[0] = 150; res[1] = 0; res[2] = 0;
    }
}

int main() {
    // pixel_t *pixels = malloc(sizeof(pixel_t)*ImageHeight*ImageWidth);
    double PixelWidth = (ScaleX[1] - ScaleX[0]) / ImageWidth;
    double PixelHeight = (ScaleY[1] - ScaleY[0]) / ImageHeight;

    //write the header to the picture file
    FILE* fp = fopen("MandelbrotSet.ppm","wb");
    fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",
        "# no comment", ImageWidth, ImageHeight,
        1 << 8 /* Max color component value */);
    #pragma omp parallel for
    for(int iY = 0; iY < ImageHeight; iY++) {
        pixel_t res;
        double Cy = ScaleY[0] + iY * PixelHeight;
        if (fabs(Cy) < PixelHeight / 2)  // Main antenna
            Cy = 0.0;
        for(int iX = 0; iX < ImageWidth; iX++) {
            double Cx = ScaleX[0] + iX * PixelWidth;
            drawPixel(res, Cx, Cy);
            fwrite(res, 1, sizeof(pixel_t), fp);
        }
    }
    fclose(fp);
    // free(pixels);
    //  stop_timer ( );
    //
    //  printf("Elapsed time: %lf\n",elapsed_time ( ));
    return 0;
}
