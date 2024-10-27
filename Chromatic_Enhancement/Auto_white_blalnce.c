#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_functions.h"

void chromantic_enhancement();

int main(int argc, char *argv[]){
    chromantic_enhancement(argv[1], argv[2]);
}

void chromantic_enhancement(char *filename, char*outputname){
    FILE *fp;
	fp = fopen(filename, "rb");
    
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

    // Auto white balance
    // calculate average
    double avgR = 0;
    double avgG = 0;
    double avgB = 0;

    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            avgR += data_RGB[i][j].R;
            avgG += data_RGB[i][j].G;
            avgB += data_RGB[i][j].B;
        }
    }
    avgR /= (H*W);
    avgG /= (H*W);
    avgB /= (H*W);
    
    double Kr, Kg, Kb;
    Kr = (avgR + avgG + avgB) / (3 * avgR);
    Kg = (avgR + avgG + avgB) / (3 * avgG);
    Kb = (avgR + avgG + avgB) / (3 * avgB);

    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            output[i][j].R = data_RGB[i][j].R * Kr;
            output[i][j].G = data_RGB[i][j].G * Kg;
            output[i][j].B = data_RGB[i][j].B * Kb;
        }
    }
    
    // write data
    writebmpRGB(outputname, &bmpheader, output, skip);
}
