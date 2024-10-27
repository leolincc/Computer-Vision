#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_functions.h"

#define PI acos(-1)
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

void conv2d(ImgRGB **image, int H, int W, ImgRGB **output, double kernel[][5]);
void GenGaussiankernel(double std, double output[][5]);

int main(){
    FILE *fp;
	fp = fopen("input.bmp", "rb");

    // read bmp header
    bmp bmpheader;
	read_header(fp, &bmpheader);

    // get height and width
	int H = bmpheader.height;
	int W = bmpheader.width;

    // read RGB data
	ImgRGB **data_RGB = RGB_2D(H, W);
    int skip = (4-(W*3)%4);
    if(skip==4) skip=0;
	read_RGB(fp, data_RGB, H, W, skip);

    // output data
    ImgRGB **output1 = RGB_2D(H, W);
    ImgRGB **output2 = RGB_2D(H, W);

    // Generate Gaussian Kernel
    double Gkernel1[5][5];
    double Gkernel2[5][5];
    GenGaussiankernel(1, Gkernel1);
    GenGaussiankernel(2, Gkernel2);

    // Gaussian smooth
    conv2d(data_RGB, H, W, output1, Gkernel1);
    conv2d(data_RGB, H, W, output2, Gkernel2);

    // write data
    writebmpRGB("output_GuassianKernel1.bmp", &bmpheader, output1, skip);
    writebmpRGB("output_GuassianKernel2.bmp", &bmpheader, output2, skip);
}

void conv2d(ImgRGB **image, int H, int W, ImgRGB **output, double kernel[][5]){
    for(int x=0; x<H; x++){
        for(int y=0; y<W; y++){
            double tempR = 0;
            double tempG = 0;
            double tempB = 0;
            for(int i=0; i<5; i++){
                for(int j=0; j<5; j++){
                    int dx = i-2;
                    int dy = j-2;
                    if(x+dx>=0 && y+dy>=0 && x+dx<H && y+dy<W){
                        tempR += image[x+dx][y+dy].R * kernel[i][j];
                        tempG += image[x+dx][y+dy].G * kernel[i][j];
                        tempB += image[x+dx][y+dy].B * kernel[i][j];
                    }
                    
                }
            }
            output[x][y].R = tempR;
            output[x][y].G = tempG;
            output[x][y].B = tempB;
        }
    }
}

void GenGaussiankernel(double std, double output[][5]){
    double sum = 0;
    for(int i=-2; i<=2; i++){
        for(int j=-2; j<=2; j++){
            output[i+2][j+2] = 1/(2*PI*std*std) * exp(-(i*i+j*j)/(2*std*std));
            sum += output[i+2][j+2];
        }
    }
    // normalization
    for(int i=0; i<5; i++){
        for(int j=0; j<5; j++){
            output[i][j] /= sum;
        }
    }
}
