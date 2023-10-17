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
    
    // up & down scaling
    int H1_up = H1 * 1.5;
    int W1_up = W1 * 1.5;
    int H1_down = H1 / 1.5;
    int W1_down = W1 / 1.5;
    int H2_up = H2 * 1.5;
    int W2_up = W2 * 1.5;
    int H2_down = H2 / 1.5;
    int W2_down = W2 / 1.5;

    ImgRGB **output1_up = RGB_2D(H1_up, W1_up);
    ImgRGB **output1_down = RGB_2D(H1_down, W1_down);
    ImgRGBA **output2_up = RGBA_2D(H2_up, W2_up);
    ImgRGBA **output2_down = RGBA_2D(H2_down, W2_down);


    double x, y, dx, dy;
    int x1, x2, y1, y2;

    for(int i=0; i<H1_up; i++){
        for(int j=0; j<W1_up; j++){
            x = i / 1.5;
            y = j / 1.5;
            x1 = floor(x);
            x2 = ceil(x);
            y1 = floor(y);
            y2 = ceil(y);
            if(x2 > H1 - 1) x2 = H1 - 1;
            if(y2 > W1 - 1) x2 = W1 - 1;
            dx = x - x1;
            dy = y - y1;

            output1_up[i][j].R = (1-dx)*(1-dy)*input1_RGB[x1][y1].R + (1-dx)*dy*input1_RGB[x1][y2].R + dx*(1-dy)*input1_RGB[x2][y1].R + dx*dy*input1_RGB[x2][y2].R;
            output1_up[i][j].G = (1-dx)*(1-dy)*input1_RGB[x1][y1].G + (1-dx)*dy*input1_RGB[x1][y2].G + dx*(1-dy)*input1_RGB[x2][y1].G + dx*dy*input1_RGB[x2][y2].G;
            output1_up[i][j].B = (1-dx)*(1-dy)*input1_RGB[x1][y1].B + (1-dx)*dy*input1_RGB[x1][y2].B + dx*(1-dy)*input1_RGB[x2][y1].B + dx*dy*input1_RGB[x2][y2].B;
        }
    }
    bmpheader1.width = W1_up;
    bmpheader1.height = H1_up;
    skip = (4-(bmpheader1.width*3)%4);
    if(skip==4) skip=0;
    writebmpRGB("output1_up.bmp", &bmpheader1, output1_up, skip);

    for(int i=0; i<H1_down; i++){
        for(int j=0; j<W1_down; j++){
            x = i * 1.5;
            y = j * 1.5;
            x1 = floor(x);
            x2 = ceil(x);
            y1 = floor(y);
            y2 = ceil(y);
            if(x2 > H1 - 1) x2 = H1 - 1;
            if(y2 > W1 - 1) x2 = W1 - 1;
            dx = x - x1;
            dy = y - y1;

            output1_down[i][j].R = (1-dx)*(1-dy)*input1_RGB[x1][y1].R + (1-dx)*dy*input1_RGB[x1][y2].R + dx*(1-dy)*input1_RGB[x2][y1].R + dx*dy*input1_RGB[x2][y2].R;
            output1_down[i][j].G = (1-dx)*(1-dy)*input1_RGB[x1][y1].G + (1-dx)*dy*input1_RGB[x1][y2].G + dx*(1-dy)*input1_RGB[x2][y1].G + dx*dy*input1_RGB[x2][y2].G;
            output1_down[i][j].B = (1-dx)*(1-dy)*input1_RGB[x1][y1].B + (1-dx)*dy*input1_RGB[x1][y2].B + dx*(1-dy)*input1_RGB[x2][y1].B + dx*dy*input1_RGB[x2][y2].B;
        }
    }
    bmpheader1.width = W1_down;
    bmpheader1.height = H1_down;
    skip = (4-(bmpheader1.width*3)%4);
    if(skip==4) skip=0;

    writebmpRGB("output1_down.bmp", &bmpheader1, output1_down, skip);

    for(int i=0; i<H2_up; i++){
        for(int j=0; j<W2_up; j++){
            x = i / 1.5;
            y = j / 1.5;
            x1 = floor(x);
            x2 = ceil(x);
            y1 = floor(y);
            y2 = ceil(y);
            if(x2 > H2 - 1) x2 = H2 - 1;
            if(y2 > W2 - 1) x2 = W2 - 1;
            dx = x - x1;
            dy = y - y1;

            output2_up[i][j].R = (1-dx)*(1-dy)*input2_RGBA[x1][y1].R + (1-dx)*dy*input2_RGBA[x1][y2].R + dx*(1-dy)*input2_RGBA[x2][y1].R + dx*dy*input2_RGBA[x2][y2].R;
            output2_up[i][j].G = (1-dx)*(1-dy)*input2_RGBA[x1][y1].G + (1-dx)*dy*input2_RGBA[x1][y2].G + dx*(1-dy)*input2_RGBA[x2][y1].G + dx*dy*input2_RGBA[x2][y2].G;
            output2_up[i][j].B = (1-dx)*(1-dy)*input2_RGBA[x1][y1].B + (1-dx)*dy*input2_RGBA[x1][y2].B + dx*(1-dy)*input2_RGBA[x2][y1].B + dx*dy*input2_RGBA[x2][y2].B;
            output2_up[i][j].A = (1-dx)*(1-dy)*input2_RGBA[x1][y1].A + (1-dx)*dy*input2_RGBA[x1][y2].A + dx*(1-dy)*input2_RGBA[x2][y1].A + dx*dy*input2_RGBA[x2][y2].A;
        }
    }
    bmpheader2.width = W2_up;
    bmpheader2.height = H2_up;
    writebmpRGBA("output2_up.bmp", &bmpheader2, output2_up);

    for(int i=0; i<H2_down; i++){
        for(int j=0; j<W2_down; j++){
            x = i * 1.5;
            y = j * 1.5;
            x1 = floor(x);
            x2 = ceil(x);
            y1 = floor(y);
            y2 = ceil(y);
            if(x2 > H2 - 1) x2 = H2 - 1;
            if(y2 > W2 - 1) x2 = W2 - 1;
            dx = x - x1;
            dy = y - y1;

            output2_down[i][j].R = (1-dx)*(1-dy)*input2_RGBA[x1][y1].R + (1-dx)*dy*input2_RGBA[x1][y2].R + dx*(1-dy)*input2_RGBA[x2][y1].R + dx*dy*input2_RGBA[x2][y2].R;
            output2_down[i][j].G = (1-dx)*(1-dy)*input2_RGBA[x1][y1].G + (1-dx)*dy*input2_RGBA[x1][y2].G + dx*(1-dy)*input2_RGBA[x2][y1].G + dx*dy*input2_RGBA[x2][y2].G;
            output2_down[i][j].B = (1-dx)*(1-dy)*input2_RGBA[x1][y1].B + (1-dx)*dy*input2_RGBA[x1][y2].B + dx*(1-dy)*input2_RGBA[x2][y1].B + dx*dy*input2_RGBA[x2][y2].B;
            output2_down[i][j].A = (1-dx)*(1-dy)*input2_RGBA[x1][y1].A + (1-dx)*dy*input2_RGBA[x1][y2].A + dx*(1-dy)*input2_RGBA[x2][y1].A + dx*dy*input2_RGBA[x2][y2].A;
        }
    }
    bmpheader2.width = W2_down;
    bmpheader2.height = H2_down;

    writebmpRGBA("output2_down.bmp", &bmpheader2, output2_down);

}