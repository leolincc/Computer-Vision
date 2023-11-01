#include <stdio.h>
#include <math.h>
#include <string.h>
#include "bmp_functions.h"

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
    ImgRGB **output = RGB_2D(H, W);

    // construct histogram
    double histR[256] = {0};
    double histG[256] = {0};
    double histB[256] = {0};
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            histR[data_RGB[i][j].R] += 1;
            histG[data_RGB[i][j].G] += 1;
            histB[data_RGB[i][j].B] += 1;
        }
    }
    // turn to CDF
    int size = H*W;
    double sum=0;
    for(int i=1; i<256; i++){
        histR[i] += histR[i-1];
        histG[i] += histG[i-1];
        histB[i] += histB[i-1];
    }
    for(int i=0; i<256; i++){
        histR[i] /= size;
        histG[i] /= size;
        histB[i] /= size;
    }

    // Histogram Equalzier
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            output[i][j].R = 255*histR[data_RGB[i][j].R]; 
            output[i][j].G = 255*histG[data_RGB[i][j].G]; 
            output[i][j].B = 255*histB[data_RGB[i][j].B]; 
        }
    }

    // write data
    writebmpRGB("output_HE.bmp", &bmpheader, output, skip);
}