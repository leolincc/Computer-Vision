#include <stdio.h>
#include <math.h>
#include <string.h>
#include "bmp_functions.h"

int main(int argc, char *argv[]){
    FILE *fp;
	fp = fopen(argv[1], "rb");

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
    YCbCr **output_YCbCr = YCbCr_2D(H, W);

    // convert to YCbCr
    YCbCr **data_YCbCr = YCbCr_2D(H, W);
    RGBconvert(data_RGB, H, W, data_YCbCr);

    // construct histogram
    double hist[256] = {0};
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            int index = round(data_YCbCr[i][j].Y);
            if(index > 255) index = 255;
            hist[index] += 1;
        }
    }
    // turn to CDF
    int size = H*W;
    double sum=0;
    for(int i=1; i<256; i++){
        hist[i] += hist[i-1];
    }
    for(int i=0; i<256; i++){
        hist[i] /= size;
    }

    // Histogram Equalzier
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            int index = round(data_YCbCr[i][j].Y);
            if(index > 255) index = 255;
            output_YCbCr[i][j].Y = 255*hist[index];
            output_YCbCr[i][j].Cb = data_YCbCr[i][j].Cb;
            output_YCbCr[i][j].Cr= data_YCbCr[i][j].Cr;
        }
    }
    // Convert back to RGB
    YCbCr_convert(output_YCbCr, output, H, W);

    // write data
    writebmpRGB(argv[2], &bmpheader, output, skip);
}