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
    YCbCr **data_YCbCr = YCbCr_2D(H, W);
    int skip = (4-(W*3)%4);
    if(skip==4) skip=0;
	read_RGB(fp, data_RGB, H, W, skip);

    // output data
    YCbCr **output1_YCbCr = YCbCr_2D(H, W);
    YCbCr **output2_YCbCr = YCbCr_2D(H, W);
    ImgRGB **output1 = RGB_2D(H, W);
    ImgRGB **output2 = RGB_2D(H, W);

    //  convert to Y, Cb, Cr
    RGBconvert(data_RGB, H, W, data_YCbCr);

    // factors
    double factor1 = 1.3;
    double factor2 = 1.5;
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            // with factor 1
            output1_YCbCr[i][j].Y = data_YCbCr[i][j].Y * factor1;
            if(output1_YCbCr[i][j].Y > 255) output1_YCbCr[i][j].Y = 255;
            output1_YCbCr[i][j].Cb = data_YCbCr[i][j].Cb;
            output1_YCbCr[i][j].Cr = data_YCbCr[i][j].Cr;

            // with factor 2
            output2_YCbCr[i][j].Y = data_YCbCr[i][j].Y * factor2;
            if(output2_YCbCr[i][j].Y > 255) output1_YCbCr[i][j].Y = 255;
            output2_YCbCr[i][j].Cb = data_YCbCr[i][j].Cb;
            output2_YCbCr[i][j].Cr = data_YCbCr[i][j].Cr;
        }
    }
    
    // convert back to RGB
    YCbCr_convert(output1_YCbCr, output1, H, W);
    YCbCr_convert(output2_YCbCr, output2, H, W);

    // write data
    writebmpRGB("output_BrightnessAdjust_1.bmp", &bmpheader, output1, skip);
    writebmpRGB("output_BrightnessAdjust_2.bmp", &bmpheader, output2, skip);
}