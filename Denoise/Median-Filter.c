#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_functions.h"

#define PI acos(-1)
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

void quickSort(int number[], int left, int right);
void MedianFilter(ImgRGB **image, int x, int y, int H, int W, ImgRGB **output);

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

    // Median filter
    for(int i=0; i<H; i++){
        for(int j=0; j<W;j++){
            MedianFilter(data_RGB, i, j, H, W, output);
        }
    }

    // write data
    writebmpRGB("output_MedianFilter.bmp", &bmpheader, output, skip);
}


void quickSort(int number[], int left, int right) { 
    int temp;
    if(left < right) { 
        int s = number[(left+right)/2]; 
        int i = left - 1; 
        int j = right + 1; 

        while(1) { 
            while(number[++i] < s) ;
            while(number[--j] > s) ;
            if(i >= j) 
                break; 
            temp = number[i];
            number[i] = number[j];
            number[j] = temp;
        } 

        quickSort(number, left, i-1);
        quickSort(number, j+1, right); 
    } 
} 

void MedianFilter(ImgRGB **image, int x, int y, int H, int W, ImgRGB **output){
    // get neighbors
    int x1 = max(0, x-1);
    int y1 = max(0, y-1);
    int x2 = min(x+1, H-1);
    int y2 = min(y+1, W-1);
    int size = (x2-x1)*(y2-y1);
    int arrayR[size], arrayG[size], arrayB[size];
    int index=0;
    for(int i=x1; i<=x2; i++){
        for(int j=y1; j<=y2; j++){
            arrayR[index] = image[i][j].R;
            arrayG[index] = image[i][j].G;
            arrayB[index] = image[i][j].B;
            index++;
        }
    }

    // bubble sort
    quickSort(arrayR, 0, size-1);
    quickSort(arrayG, 0, size-1);
    quickSort(arrayB, 0, size-1);

    int med = size/2 - 1;
    if(size%2 == 0){
        output[x][y].R = (arrayR[med] + arrayR[med+1])/2;
        output[x][y].G = (arrayG[med] + arrayG[med+1])/2;
        output[x][y].B = (arrayB[med] + arrayB[med+1])/2;
    }
    else{
        output[x][y].R = arrayR[med+1];
        output[x][y].G = arrayG[med+1];
        output[x][y].B = arrayB[med+1];
    }
}
