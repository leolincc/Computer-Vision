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
    ImgRGB **output1 = RGB_2D(H, W);
    ImgRGB **output2 = RGB_2D(H, W);

    // gamma correction factor
    double gamma1 = 1.5;
    double gamma2 = 2;

    // gamma correction
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            output1[i][j].R = round(pow(data_RGB[i][j].R/255.0, 1/gamma1)*255); 
            output1[i][j].G = round(pow(data_RGB[i][j].G/255.0, 1/gamma1)*255); 
            output1[i][j].B = round(pow(data_RGB[i][j].B/255.0, 1/gamma1)*255); 
            output2[i][j].R = round(pow(data_RGB[i][j].R/255.0, 1/gamma2)*255); 
            output2[i][j].G = round(pow(data_RGB[i][j].G/255.0, 1/gamma2)*255); 
            output2[i][j].B = round(pow(data_RGB[i][j].B/255.0, 1/gamma2)*255); 
        }
    }

    // write data
    writebmpRGB("output_Gamma_1.bmp", &bmpheader, output1, skip);
    writebmpRGB("output_Gamma_2.bmp", &bmpheader, output2, skip);
}