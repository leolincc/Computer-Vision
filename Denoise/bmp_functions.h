#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

typedef struct _bmp{
    uint16_t ID[2];
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t offset;
    uint32_t headerSize;
    int width;
    int height;
    short planes;
    short BitsPerPixel;
    int compression;
    int dataSize;
    int Hresolution;
    int Vresolution;
    int color;
    int impcolor;
} bmp;

typedef struct RGB{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} ImgRGB;

typedef struct _YCbCr{
	double Y;
	double Cb;
	double Cr;
} YCbCr;

void read_header(FILE *fp, bmp *bmpheader){
    // initial fp position
    fseek(fp, 0, SEEK_CUR);

    // read header
    if(fread(bmpheader->ID, 1, 2, fp)==0||
        fread(&bmpheader->fileSize, 4, 1, fp)==0||
        fread(&bmpheader->reserved, 4, 1, fp)==0||
        fread(&bmpheader->offset, 4, 1, fp)==0||
        fread(&bmpheader->headerSize, 4, 1, fp)==0||
        fread(&bmpheader->width, 4, 1, fp)==0||
        fread(&bmpheader->height, 4, 1, fp)==0||
        fread(&bmpheader->planes, 2, 1, fp)==0||
        fread(&bmpheader->BitsPerPixel, 2, 1, fp)==0||
        fread(&bmpheader->compression, 4, 1, fp)==0||
        fread(&bmpheader->dataSize, 4, 1, fp)==0||
        fread(&bmpheader->Hresolution, 4, 1, fp)==0||
        fread(&bmpheader->Vresolution, 4, 1, fp)==0||
        fread(&bmpheader->color, 4, 1, fp)==0||
        fread(&bmpheader->impcolor, 4, 1, fp)==0)
        printf("read header error!");
}

void read_RGB(FILE *fp, ImgRGB **array, int H, int W, int skip){
    // read RGB data
    int i, j, temp;
    char skip_buf[3];
    for (i = 0; i<H; i++){
        for (j = 0; j<W; j++){
            temp = fgetc(fp);
            array[i][j].B = temp;
            temp = fgetc(fp);
            array[i][j].G = temp;
            temp = fgetc(fp);
            array[i][j].R = temp;
        }
        if (skip != 0) fread(skip_buf, skip, 1, fp);
    }
}

ImgRGB** RGB_2D(int row, int col){
    ImgRGB **Array=(ImgRGB**)calloc(row,sizeof(ImgRGB*));
    int i;
    for(i=0;i<row;i++){
        Array[i]=(ImgRGB*)calloc(col,sizeof(ImgRGB));
    }
    return Array;
}

YCbCr** YCbCr_2D(int row, int col){
	YCbCr **Array, *Data;
	int i;
	Array = (YCbCr**)malloc(row*sizeof(YCbCr *));
	Data = (YCbCr*)malloc(row*col*sizeof(YCbCr));
	for (i = 0; i<row; i++, Data += col){
		Array[i] = Data;
	}
	return Array;
}

void RGBconvert(ImgRGB **array, int H, int W, YCbCr **output){
	int i, j, R, G, B;
	for(i=0; i<H; i++){
		for(j=0; j<W; j++){
			R = (int)array[i][j].R;
			G = (int)array[i][j].G;
			B = (int)array[i][j].B;
			output[i][j].Y = 0.299 * R + 0.587 * G + 0.114 * B;
			output[i][j].Cb = 0.564*(B-output[i][j].Y);
			output[i][j].Cr = 0.713*(R-output[i][j].Y);
		}
	}
}

void YCbCr_convert(YCbCr **input, ImgRGB **output, int H, int W){
	int i, j, temp;
	unsigned char tempR,tempG,tempB;
	for(i=0;i<H;++i){
		for(j=0;j<W;j++){
			// R (prevent overflow)
			temp = input[i][j].Y+1.402*input[i][j].Cr;
			if(temp>=255) tempR=255;
			else if(temp<0) tempR=0;
			else tempR = (unsigned char)temp;

			// G (prevent overflow)
			temp = input[i][j].Y-0.344136*input[i][j].Cb-0.714136*input[i][j].Cr;
			if(temp>=255) tempG=255;
			else if(temp<0) tempG=0;
			else tempG = (unsigned char)temp;


			// B (prevent overflow)
			temp = input[i][j].Y+1.772*input[i][j].Cb;
			if(temp>=255) tempB=255;
			else if(temp<0) tempB=0;
			else tempB = (unsigned char)temp;

			// save data
			output[i][j].R = tempR;
			output[i][j].G = tempG;
			output[i][j].B = tempB;
		}
	}
}

void writebmpRGB(char *output, bmp *bmpheader, ImgRGB **RGB, int skip){
    FILE *fp;
    fp = fopen(output, "wb");
    char skip_buf[3];
    if(fwrite(bmpheader->ID, 1, 2, fp)==0||
        fwrite(&bmpheader->fileSize, 4, 1, fp)==0||
        fwrite(&bmpheader->reserved, 4, 1, fp)==0||
        fwrite(&bmpheader->offset, 4, 1, fp)==0||
        fwrite(&bmpheader->headerSize, 4, 1, fp)==0||
        fwrite(&bmpheader->width, 4, 1, fp)==0||
        fwrite(&bmpheader->height, 4, 1, fp)==0||
        fwrite(&bmpheader->planes, 2, 1, fp)==0||
        fwrite(&bmpheader->BitsPerPixel, 2, 1, fp)==0||
        fwrite(&bmpheader->compression, 4, 1, fp)==0||
        fwrite(&bmpheader->dataSize, 4, 1, fp)==0||
        fwrite(&bmpheader->Hresolution, 4, 1, fp)==0||
        fwrite(&bmpheader->Vresolution, 4, 1, fp)==0||
        fwrite(&bmpheader->color, 4, 1, fp)==0||
        fwrite(&bmpheader->impcolor, 4, 1, fp)==0)
        printf("write header error!");
     
    // bitmap data
    int i, j, zero=0;
    for (i = 0; i<bmpheader->height; i++){
        for (j = 0; j<bmpheader->width; j++){
            fwrite(&RGB[i][j].B, sizeof(char), 1, fp);
            fwrite(&RGB[i][j].G, sizeof(char), 1, fp);
            fwrite(&RGB[i][j].R, sizeof(char), 1, fp);
        }
        if (skip != 0)  fwrite(skip_buf, skip, 1, fp);
    }
    fclose(fp);
}
