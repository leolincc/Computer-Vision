#include <stdio.h>
#include <math.h>
#include <string.h>
#include "bmp_functions.h"

// Laplacian sharpness function
int LaplacianSharpness1(YCbCr **image,int x, int y, int H, int W){
    // Edge pixels are ignored
    if(x <= 0 || x >= H-1 || y <= 0 || y >= W-1){
        return 0;
    }
    else{
        int laplacian = 5 * image[x][y].Y - image[x-1][y].Y - image[x+1][y].Y - image[x][y+1].Y - image[x][y-1].Y;
        return laplacian;
    }
    
}

// Laplacian sharpness function
int LaplacianSharpness2(YCbCr **image,int x, int y, int H, int W){
    // Edge pixels are ignored
    if(x <= 0 || x >= H-1 || y <= 0 || y >= W-1){
        return 0;
    }
    else{
        int laplacian = 9 * image[x][y].Y - image[x-1][y-1].Y - image[x-1][y].Y - image[x-1][y+1].Y - image[x][y-1].Y - image[x][y+1].Y
         - image[x+1][y-1].Y - image[x+1][y].Y - image[x+1][y+1].Y;
        return laplacian;
    }
    
}

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
	YCbCr **data_YCbCr = YCbCr_2D(H,W);
    int skip = (4-(W*3)%4);
    if(skip==4) skip=0;
	read_RGB(fp, data_RGB, H, W, skip);

    // convert to YCbCr
	RGBconvert(data_RGB, H, W, data_YCbCr);

    // output data
    YCbCr **data_YCbCr1 = YCbCr_2D(H,W);
	YCbCr **data_YCbCr2 = YCbCr_2D(H,W);
    ImgRGB **output1 = RGB_2D(H, W);
    ImgRGB **output2 = RGB_2D(H, W);

    for(int i=0; i < H; i++){
        for(int j=0; j < W; j++){
            // Laplacian sharpness function
            data_YCbCr1[i][j].Y = LaplacianSharpness1(data_YCbCr, i, j, H, W);
            data_YCbCr1[i][j].Cb = data_YCbCr[i][j].Cb;
            data_YCbCr1[i][j].Cr = data_YCbCr[i][j].Cr;

            // Gradient-based sharpness function
            data_YCbCr2[i][j].Y = LaplacianSharpness2(data_YCbCr, i, j, H, W);
            data_YCbCr2[i][j].Cb = data_YCbCr[i][j].Cb;
            data_YCbCr2[i][j].Cr = data_YCbCr[i][j].Cr;
        }
    }
    // covert to RGB
    YCbCr_convert(data_YCbCr1, output1, H, W);
    YCbCr_convert(data_YCbCr2, output2, H, W);

    // write data
    writebmpRGB("output_SmallKernel.bmp", &bmpheader, output1, skip);
    writebmpRGB("output_BigKernel.bmp", &bmpheader, output2, skip);
}