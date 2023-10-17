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

typedef struct RGBA{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
} ImgRGBA;

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

void read_RGBA(FILE *fp, ImgRGBA **array, int H, int W){
    // read RGB data
    int i, j, temp;
    char skip_buf[3];
    for (i = 0; i<H; i++){
        for (j = 0; j<W; j++){
            temp = fgetc(fp);
            array[i][j].A = temp;
            temp = fgetc(fp);
            array[i][j].B = temp;
            temp = fgetc(fp);
            array[i][j].G = temp;
            temp = fgetc(fp);
            array[i][j].R = temp;
        }
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

ImgRGBA** RGBA_2D(int row, int col){
    ImgRGBA **Array=(ImgRGBA**)calloc(row,sizeof(ImgRGBA*));
    int i;
    for(i=0;i<row;i++){
        Array[i]=(ImgRGBA*)calloc(col,sizeof(ImgRGBA));
    }
    return Array;
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

void writebmpRGBA(char *output, bmp *bmpheader, ImgRGBA **RGBA){
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
            fwrite(&RGBA[i][j].A, sizeof(char), 1, fp);
            fwrite(&RGBA[i][j].B, sizeof(char), 1, fp);
            fwrite(&RGBA[i][j].G, sizeof(char), 1, fp);
            fwrite(&RGBA[i][j].R, sizeof(char), 1, fp);
        }
    }
    fclose(fp);
}

int main(int argc, char const *argv[])
{
    // read file
	FILE *fp1, *fp2;
	fp1 = fopen("input1.bmp", "rb");
    fp2 = fopen("input2.bmp", "rb");

    // read header
	bmp bmpheader1, bmpheader2;
    read_header(fp1, &bmpheader1);
    read_header(fp2, &bmpheader2);

    // get size
    int H1 = bmpheader1.height;
    int W1 = bmpheader1.width;
    int skip = (4-(bmpheader1.width*3)%4);
    if(skip==4) skip=0;

    int H2 = bmpheader2.height;
    int W2 = bmpheader2.width;

    // read data
    ImgRGB **input1_RGB = RGB_2D(H1, W1);
    read_RGB(fp1, input1_RGB, H1, W1, skip);
    fclose(fp1);

    ImgRGBA **input2_RGBA = RGBA_2D(H2, W2);
    read_RGBA(fp2, input2_RGBA, H2, W2);
    fclose(fp2);
    
    // output data
    ImgRGB **output1_1 = RGB_2D(H1, W1);
    ImgRGB **output1_2 = RGB_2D(H1, W1);
    ImgRGB **output1_3 = RGB_2D(H1, W1);
    ImgRGBA **output2_1 = RGBA_2D(H2, W2);
    ImgRGBA **output2_2 = RGBA_2D(H2, W2);
    ImgRGBA **output2_3 = RGBA_2D(H2, W2);
    
    for(int i=0; i<H1; i++){
        for(int j = 0; j<W1; j++){
            // Quantization
            output1_1[i][j].R = (input1_RGB[i][j].R >> 2) << 2;
            output1_1[i][j].G = (input1_RGB[i][j].G >> 2) << 2;
            output1_1[i][j].B = (input1_RGB[i][j].B >> 2) << 2;
            output1_2[i][j].R = (input1_RGB[i][j].R >> 4) << 4;
            output1_2[i][j].G = (input1_RGB[i][j].G >> 4) << 4;
            output1_2[i][j].B = (input1_RGB[i][j].B >> 4) << 4;
            output1_3[i][j].R = (input1_RGB[i][j].R >> 6) << 6;
            output1_3[i][j].G = (input1_RGB[i][j].G >> 6) << 6;
            output1_3[i][j].B = (input1_RGB[i][j].B >> 6) << 6;

            // Bilinear Interpolation
            
        }
    }
    
    writebmpRGB("output1_1.bmp", &bmpheader1, output1_1, skip);
    writebmpRGB("output1_2.bmp", &bmpheader1, output1_2, skip);
    writebmpRGB("output1_3.bmp", &bmpheader1, output1_3, skip);

    
    for(int i=0; i<H2; i++){
        for(int j = 0; j<W2; j++){
            // Quantization
            output2_1[i][j].R = (input2_RGBA[i][j].R >> 2) << 2;
            output2_1[i][j].G = (input2_RGBA[i][j].G >> 2) << 2;
            output2_1[i][j].B = (input2_RGBA[i][j].B >> 2) << 2;
            output2_1[i][j].A = (input2_RGBA[i][j].A >> 2) << 2;
            output2_2[i][j].R = (input2_RGBA[i][j].R >> 4) << 4;
            output2_2[i][j].G = (input2_RGBA[i][j].G >> 4) << 4;
            output2_2[i][j].B = (input2_RGBA[i][j].B >> 4) << 4;
            output2_2[i][j].A = (input2_RGBA[i][j].A >> 4) << 4;
            output2_3[i][j].R = (input2_RGBA[i][j].R >> 6) << 6;
            output2_3[i][j].G = (input2_RGBA[i][j].G >> 6) << 6;
            output2_3[i][j].B = (input2_RGBA[i][j].B >> 6) << 6;
            output2_3[i][j].A = (input2_RGBA[i][j].A >> 6) << 6;
        }
    }
    writebmpRGBA("output2_1.bmp", &bmpheader2, output2_1);
    writebmpRGBA("output2_2.bmp", &bmpheader2, output2_2);
    writebmpRGBA("output2_3.bmp", &bmpheader2, output2_3);
}