#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// quantization table (luminance)
const double Q_lumi[8][8]={
    16,  11,  10,  16,  24,  40,  51,  61,
    12,  12,  14,  19,  26,  58,  60,  55,
    14,  13,  16,  24,  40,  57,  69,  56,
    14,  17,  22,  29,  51,  87,  80,  62,
    18,  22,  37,  56,  68, 109, 103,  77,
    24,  35,  55,  64,  81, 104, 113,  92,
    49,  64,  78,  87, 103, 121, 120, 101,
    72,  92,  95,  98, 112, 100, 103,  99,
};

// quantization table (chorminance)
const double Q_chorm[8][8]={
    17,  18,  24,  47,  99,  99,  99,  99,
    18,  21,  26,  66,  99,  99,  99,  99,
    24,  26,  56,  99,  99,  99,  99,  99,
    47,  66,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
};

// zigzag row
const int row[64]={
    0,0,1,2,1,0,0,1,
    2,3,4,3,2,1,0,0,
    1,2,3,4,5,6,5,4,
    3,2,1,0,0,1,2,3,
    4,5,6,7,7,6,5,4,
    3,2,1,2,3,4,5,6,
    7,7,6,5,4,3,4,5,
    6,7,7,6,5,6,7,7};

// zigzag col
const int col[64]={
    0,1,0,0,1,2,3,2,
    1,0,0,1,2,3,4,5,
    4,3,2,1,0,0,1,2,
    3,4,5,6,7,7,6,5,
    4,3,2,1,0,1,2,3,
    4,5,6,7,7,6,5,4,
    3,2,3,4,5,6,7,7,
    6,5,4,5,6,7,6,7};

// constucrt structure of bmp header
typedef struct _bmp{
    char ID[2];
    int fileSize;
    int reserved;
    int offset;
    int headerSize;
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

// construct structure of RGB (unsigned short) data
typedef struct RGB{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} ImgRGB;

// construct structure of YCbCr (int) data
typedef struct _YCbCr{
    double Y;
    double Cb;
    double Cr;
} YCbCr;

// construct struct of node in Huffman tree
typedef struct treenode{
    int symbol;
    int count;
    int parent;
    int lchild,rchild;
} node;

// construct struct of symbol and binary code
typedef struct _codebook{
    char bit[2000];
    int symbol;
    int num;
} codebook;

ImgRGB** RGB_2D(int row, int col){
    ImgRGB **Array=(ImgRGB**)calloc(row,sizeof(ImgRGB*));
    int i;
    for(i=0;i<row;i++){
        Array[i]=(ImgRGB*)calloc(col,sizeof(ImgRGB));
    }
    return Array;
}

YCbCr** YCbCr_2D(int row, int col){
    YCbCr **Array=(YCbCr**)calloc(row,sizeof(YCbCr*));
    int i;
    for(i=0;i<row;i++){
        Array[i]=(YCbCr*)calloc(col,sizeof(YCbCr));
    }
    return Array;
}

double** cosi_2D(int row, int col){
    double **Array=(double**)calloc(row,sizeof(double*));
    int i;
    for(i=0;i<row;i++){
        Array[i]=(double*)calloc(col,sizeof(double));
    }
    return Array;
}

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

void read_data(FILE *fp, ImgRGB **array, int H, int W, int skip){
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

YCbCr** zero_padding(YCbCr **input, int H, int W, int Nh, int Nw){
    YCbCr **output = YCbCr_2D(Nh, Nw);
    int i,j;
    for(i=0;i<Nh;i++){
        for(j=0;j<Nw;j++){
            if(i<H&&j<W){
                output[i][j].Y = input[i][j].Y;
                output[i][j].Cb = input[i][j].Cb;
                output[i][j].Cr = input[i][j].Cr;
            }
            else{
                output[i][j].Y = 0;
                output[i][j].Cb = 0;
                output[i][j].Cr = 0;
            }
        }
    }
    return output;
}

void cos_table(int x, int y, double **cosi){
    int i,j;
    for(i=0;i<x;i++){
        for(j=0;j<y;j++){
            cosi[i][j] = cos((2*i+1)*j*M_PI/16);
        }
    }
}

YCbCr** DCT(YCbCr **input, int Nh, int Nw, double **cosi){
    YCbCr **output = YCbCr_2D(Nh, Nw);
    int m,n,u,v,x,y;
    double *z,c,temp;
    z=calloc(3, sizeof(double));
    for(m=0;m<Nh;m+=8){
        for(n=0;n<Nw;n+=8){
            for(u=0;u<8;u++){
                for(v=0;v<8;v++){
                    c=1.0;
                    z[0]=0.0;
                    z[1]=0.0;
                    z[2]=0.0;
                    for(x=0;x<8;x++){
                        for(y=0;y<8;y++){
                            temp = cosi[x][u] * cosi[y][v];
                            z[0]+=(input[m+x][n+y].Y-128) * temp;
                            z[1]+=input[m+x][n+y].Cb * temp;
                            z[2]+=input[m+x][n+y].Cr * temp;
                        }
                    }
                    if(u==0) c/=sqrt(2);
                    if(v==0) c/=sqrt(2);
                    output[m+u][n+v].Y = c * z[0] / 4;
                    output[m+u][n+v].Cb = c * z[1] / 4;
                    output[m+u][n+v].Cr = c * z[2] / 4;
                }
            }
        }
    }
    free(z);
    return output;
}

YCbCr** IDCT(YCbCr **input, int Nh, int Nw, double **cosi){
    YCbCr **output = YCbCr_2D(Nh, Nw);
    int m,n,u,v,x,y;
    double *z,c;
    z=calloc(3, sizeof(double));
    for(m=0;m<Nh;m+=8){
        for(n=0;n<Nw;n+=8){
            for(x=0;x<8;x++){
                for(y=0;y<8;y++){
                    z[0]=0.0;
                    z[1]=0.0;
                    z[2]=0.0;
                    for(u=0;u<8;u++){
                        for(v=0;v<8;v++){
                            c=cosi[x][u] * cosi[y][v];
                            if(u==0) c/=sqrt(2);
                            if(v==0) c/=sqrt(2);
                            z[0]+=input[m+u][n+v].Y * c;
                            z[1]+=input[m+u][n+v].Cb * c;
                            z[2]+=input[m+u][n+v].Cr * c;
                        }
                    }
                    output[m+x][n+y].Y = z[0]/4+128;
                    output[m+x][n+y].Cb = z[1]/4;
                    output[m+x][n+y].Cr = z[2]/4;
                }
            }
            
        }
    }
    free(z);
    return output;
}

void quantization(YCbCr **input, int Nh, int Nw, YCbCr **output){
    // quantize
    int m,n,i,j;
    for(m=0;m<Nh;m+=8){
        for(n=0;n<Nw;n+=8){
            for(i=0;i<8;i++){
                for(j=0;j<8;j++){
                    output[m+i][n+j].Y = round(input[m+i][n+j].Y / Q_lumi[i][j]); 
                    output[m+i][n+j].Cb = round(input[m+i][n+j].Cb / Q_chorm[i][j]);
                    output[m+i][n+j].Cr = round(input[m+i][n+j].Cr / Q_chorm[i][j]);
                }
            }
        }
    }
}

void dequantization(YCbCr **input, int Nh, int Nw, YCbCr **output){
    // dequantize
    int m,n,i,j;
    for(m=0;m<Nh;m+=8){
        for(n=0;n<Nw;n+=8){
            for(i=0;i<8;i++){
                for(j=0;j<8;j++){
                    output[m+i][n+j].Y = input[m+i][n+j].Y * Q_lumi[i][j]; 
                    output[m+i][n+j].Cb = input[m+i][n+j].Cb * Q_chorm[i][j];
                    output[m+i][n+j].Cr = input[m+i][n+j].Cr * Q_chorm[i][j];
                }
            }
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

void DPCM(YCbCr **input, int Nh, int Nw, int *DC_Y, int *DC_Cb, int *DC_Cr){
    int i,j,index=0;
    // do in every row(DC)
    for(i=0;i<Nh;i+=8){
        for(j=Nw-8;j>0;j-=8){
            input[i][j].Y -= input[i][j-8].Y;
            input[i][j].Cb -= input[i][j-8].Cb;
            input[i][j].Cr -= input[i][j-8].Cr;
        }
    }
    // do in first column(DC)
    for(i=Nh-8;i>0;i-=8){
        input[i][0].Y -= input[i-8][0].Y;
        input[i][0].Cb -= input[i-8][0].Cb;
        input[i][0].Cr -= input[i-8][0].Cr;
    }
    // get DC
    for(i=0;i<Nh/8;i++){
        for(j=0;j<Nw/8;j++){
            DC_Y[index] = input[i*8][j*8].Y;
            DC_Cb[index] = input[i*8][j*8].Cb;
            DC_Cr[index] = input[i*8][j*8].Cr;
            index++;
        }
    }
}

void zigzag(YCbCr **input, int Nh, int Nw, int *AC_Y, int *AC_Cb, int *AC_Cr){
     int m,n,i,index=0;
     for(m=0;m<Nh;m+=8){
         for(n=0;n<Nw;n+=8){
             for(i=1;i<64;i++){
                 AC_Y[index]=(int)input[m+row[i]][n+col[i]].Y;
                 AC_Cb[index]=(int)input[m+row[i]][n+col[i]].Cb;
                 AC_Cr[index]=(int)input[m+row[i]][n+col[i]].Cr;
                 index++;
             }
         }
     }
}

YCbCr** dezigzag(int Nh,int Nw,int *AC_Y,int *AC_Cb,int *AC_Cr,int *DC_Y,int *DC_Cb,int *DC_Cr){
    int m,n,i,j,index=0;
    YCbCr **output=YCbCr_2D(Nh,Nw);
     for(m=0;m<Nh;m+=8){
         for(n=0;n<Nw;n+=8){
             for(i=1;i<64;i++){
                 output[m+row[i]][n+col[i]].Y = AC_Y[index];
                 output[m+row[i]][n+col[i]].Cb = AC_Cb[index];
                 output[m+row[i]][n+col[i]].Cr = AC_Cr[index];
                 index++;
             }
         }
     }
    // put in DC
    index=0;
    for(i=0;i<Nh/8;i++){
        for(j=0;j<Nw/8;j++){
            output[i*8][j*8].Y = DC_Y[index];
            output[i*8][j*8].Cb = DC_Cb[index];
            output[i*8][j*8].Cr = DC_Cr[index];
            index++;
        }
    }
    // do in first column (DC)
    for(i=8;i<Nh;i+=8){
        output[i][0].Y += output[i-8][0].Y;
        output[i][0].Cb += output[i-8][0].Cb;
        output[i][0].Cr += output[i-8][0].Cr;
    }
    // do in every row (DC)
    for(i=0;i<Nh;i+=8){
        for(j=8;j<Nw;j+=8){
            output[i][j].Y += output[i][j-8].Y;
            output[i][j].Cb += output[i][j-8].Cb;
            output[i][j].Cr += output[i][j-8].Cr;
        }
    }
    return output;
}

int* RLE(int *input, int size, int *rle_size){
    int i=0,count=0,j=0;
    int *temp, m;
    temp = malloc(2*size*sizeof(int));
    for(m=0;m<size;m+=63){
        for(i=0;i<63;i++){
            if(input[m+i]==0){
                count++;
            }
            else{
                temp[j]=count;
                temp[j+1]=input[m+i];
                count=0;
                j+=2;
            }
        }
        temp[j]=0;
        temp[j+1]=0;
        count=0;
        j+=2;
    }
    *rle_size = j;
    int *output;
    output=calloc(j,sizeof(int));
    for(i=0;i<j;i++){
        output[i]=temp[i];
    }
    free(temp);
    return output;
}

int* IRLE(int *input,int rle_size,int size){
    int i,j=0,z,index=0,*output=calloc(size,sizeof(int));
    for(i=1;i<=size/63;i++){
        while(input[j]!=0||input[j+1]!=0){
            for(z=0;z<input[j];z++){
                output[index]=0;
                index++;
            }
            output[index]=input[j+1];
            index++;
            j+=2;
        }
        while(index<(i*63)){
            output[index]=0;
            index++;
        }
        j+=2;
    }
    return output;
}

int getinfo(int *input, int rle_size, int *output){
    int i,j,legal,amount=0;
    for(i=0;i<rle_size;i++){
        legal=0;
        for(j=0;j<2*amount;j+=2){
            if(input[i]==output[j]){
                output[j+1]++;
                legal=1;
                break;
            }
        }
        if(legal==0){
            output[2*amount]=input[i];
            output[2*amount+1]=1;
            amount++;
        }
    }
    return amount;
}

void output_codebook(node *node, codebook *book, int amount, char *filename){
    FILE *fp;
    fp=fopen(filename,"w+");

    char temp[amount];
    int index,start;
    int i,j,c,p;

    for(i=0;i<amount;i++){
        start=amount-1;
        c=i;
        p=node[c].parent;
        while(p!=-1){
            if(node[p].lchild==c)
                temp[start]='0';
            else  // rchild
                temp[start]='1';
            start--;
            c=p;
            p=node[c].parent;
        }
        index=0;
        for(j=start+1;j<amount;j++){
            book[i].bit[index]=temp[j];
            index++;
        }
        book[i].num=index;
    }
    // output book
    for(i=0;i<amount;i++){
        fprintf(fp,"%d  ",book[i].symbol);
        for(j=0;j<book[i].num;j++){
            fprintf(fp,"%c",book[i].bit[j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}

node* build_treemnode(int *temp, int amount){
    int i;
    node *output=calloc(2*amount,sizeof(node));
    // init
    for(i=0;i<2*amount-1;i++){
        output[i].symbol=0;
        output[i].count=0;
        output[i].lchild=-1;
        output[i].rchild=-1;
        output[i].parent=-1;
    }
    // put in data
    for(i=0;i<amount;i++){
        output[i].symbol=temp[2*i];
        output[i].count=temp[2*i+1];
    }
    return output;
}

codebook* build_codebook(node *node, int amount){
    int i;
    codebook *book=malloc(amount*sizeof(codebook));
    for(i=0;i<amount;i++){
        book[i].symbol=node[i].symbol;
    }
    return book;
}

void Huffman(node *node, int amount, codebook *book, char *bookname){
    int i,j;
    // build tree
    int min1,min2,p1,p2;
    for(i=0;i<amount-1;i++){
        min1=min2=10000000;
        for(j=0;j<amount+i;j++){
            if(node[j].count<min1 && node[j].parent==-1){
                min2=min1;
                p2=p1;
                min1=node[j].count;
                p1=j;
            }
            else if(node[j].count<min2 && node[j].parent==-1){
                min2=node[j].count;
                p2=j;
            }
        }
        node[p1].parent=amount+i;
        node[p2].parent=amount+i;
        node[amount+i].parent=-1;
        node[amount+i].count=min1+min2;
        node[amount+i].lchild=p1;
        node[amount+i].rchild=p2;
    }
    // output coedbook
    output_codebook(node, book, amount, bookname);
}

void Huffman_encode(codebook *book, char *filename, int *rle_data, int rle_size, int amount){
    FILE *fp;
    fp=fopen(filename,"w+");
    int i,j,index,num=0;
    char BUFF[8],out;
    for(i=0;i<rle_size;i++){
        index=0;
        while(rle_data[i]!=book[index].symbol){
            index++;
        }
        for(j=0;j<book[index].num;j++){
            BUFF[num]=book[index].bit[j];
            num++;
            if(num>7){
                out=strtol(BUFF,0,2);
                fprintf(fp,"%c",out);
                num=0;
            }
        }
    }
}
void c2int(char bit, int *BUFF){
    int i=0;
    for(i=0;i<8;i++){
        BUFF[i] = ((bit << i)&0x80)?1:0;
    }
}

void Huffman_decode(node *node, int amount,int *rle_data,char *filename){
    FILE *fp;
    fp=fopen(filename,"r+");
    char bit;
    int BUFF[8];
    int index=2*amount-2;
    int count=0,i=0,k;

    while(fscanf(fp,"%c",&bit)!=EOF){
        c2int(bit,BUFF);
        for(k=0;k<8;k++){
            // determine lchild or rchild
            if(BUFF[k]==1)
                index=node[index].rchild;
            else if(BUFF[k]==0)
                index=node[index].lchild;
            else
                printf("error");

            // determine bottom
            if(node[index].rchild==-1||node[index].lchild==-1){
                rle_data[i]=node[index].symbol;
                index=2*amount-2;
                i++;
            }
        }
    }
}

void writebmp(char *output, bmp *bmpheader, ImgRGB **RGB, int skip){
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

int main(int argc, char **argv){
    char *filename = argv[1];
    char *output = argv[2];
    FILE *fp;
    fp = fopen(filename, "rb");

    // read header
    bmp bmpheader;
    read_header(fp, &bmpheader);

    // set arguments
    int i, j;
    int H = bmpheader.height;
    int W = bmpheader.width;
    int skip = (4-(bmpheader.width*3)%4);
    if(skip==4) skip=0;

    // read data
    ImgRGB **data_RGB = RGB_2D(H, W);
    read_data(fp, data_RGB, H, W, skip);
    fclose(fp);

    // RGB convert YCbCr
    YCbCr **data_YCbCr = YCbCr_2D(H,W);
    RGBconvert(data_RGB, H, W, data_YCbCr);

    // make Nh*Nw size of data
    int Nh, Nw;
    if(H%8==0) Nh=H;
    else Nh=(H/8+1)*8;
    if(W%8==0) Nw=W;
    else Nw=(W/8+1)*8;

    // zero padding
    YCbCr **data_padding = YCbCr_2D(Nh,Nw);
    data_padding = zero_padding(data_YCbCr, H, W, Nh, Nw);

    // set DCT and quantization arguments
    YCbCr **data_DCT = YCbCr_2D(Nh,Nw);
    YCbCr **data_quan = YCbCr_2D(Nh,Nw);

    // cosine table
    double **cosi = cosi_2D(8,8);
    cos_table(8, 8, cosi);
    
    // blocking and DCT
    data_DCT = DCT(data_padding, Nh, Nw, cosi);
    for(i=0;i<Nh;i++){
        free(data_padding[i]);
    }
    free(data_padding);

    // quantatize
    // YCBCR **ata_quan = YCBCR_2D(Nh,Nw);
    quantization(data_DCT, Nh, Nw,data_quan);

    // DPCM
    int Mh=Nh/8;
    int Mw=Nw/8;
    int *DC_Y=calloc(Mh*Mw,sizeof(int));
    int *DC_Cb=calloc(Mh*Mw,sizeof(int));
    int *DC_Cr=calloc(Mh*Mw,sizeof(int));
    DPCM(data_quan, Nh, Nw, DC_Y, DC_Cb, DC_Cr);
    
    // zigzag
    int *AC_Y, *AC_Cb, *AC_Cr, size=Mh*Mw*63;
    AC_Y=calloc(size,sizeof(int));
    AC_Cb=calloc(size,sizeof(int));
    AC_Cr=calloc(size,sizeof(int));
    zigzag(data_quan, Nh, Nw, AC_Y, AC_Cb, AC_Cr);

    //RLE
    int *rle_Y, *rle_Cb, *rle_Cr;
    int size_Y, size_Cb, size_Cr;
    rle_Y = RLE(AC_Y,size,&size_Y);
    rle_Cb = RLE(AC_Cb,size,&size_Cb);
    rle_Cr = RLE(AC_Cr,size,&size_Cr);

    // Huffman bulid tree->encode->decode
    int amount,*temp,size_DC=Mh*Mw;

    // AC_Y //
    temp=calloc(size_Y,sizeof(int));
    amount=getinfo(rle_Y,size_Y,temp);
    node *AC_Y_node=build_treemnode(temp,amount);
    codebook *book=build_codebook(AC_Y_node,amount);
    
    Huffman(AC_Y_node,amount,book,"AC_Y_tree.txt");
    Huffman_encode(book,"AC_Y.txt",rle_Y,size_Y,amount);
    Huffman_decode(AC_Y_node,amount,rle_Y,"AC_Y.txt");

    free(book);
    free(temp);

    // AC_Cb //
    temp=calloc(size_Cb,sizeof(int));
    amount=getinfo(rle_Cb,size_Cb,temp);
    node *AC_Cb_node=build_treemnode(temp,amount);
    book=build_codebook(AC_Cb_node,amount);

    Huffman(AC_Cb_node, amount, book,"AC_Cb_tree.txt");
    Huffman_encode(book, "AC_Cb.txt", rle_Cb, size_Cb, amount);
    Huffman_decode(AC_Cb_node,amount,rle_Cb,"AC_Cb.txt");

    free(book);
    free(temp);

    // AC_Cr //
    temp=calloc(size_Cr,sizeof(int));
    amount=getinfo(rle_Cr,size_Cr,temp);
    node *AC_Cr_node=build_treemnode(temp,amount);
    book=build_codebook(AC_Cr_node,amount);

    Huffman(AC_Cr_node, amount, book,"AC_Cr_tree.txt");
    Huffman_encode(book, "AC_Cr.txt", rle_Cr, size_Cr, amount);
    Huffman_decode(AC_Cr_node,amount,rle_Cr,"AC_Cr.txt");

    free(book);
    free(temp);

    // DC_Y //
    temp=calloc(size_DC,sizeof(int));
    amount=getinfo(DC_Y,size_DC,temp);
    node *DC_Y_node=build_treemnode(temp,amount);
    book=build_codebook(DC_Y_node,amount);
       
    Huffman(DC_Y_node,amount,book,"DC_Y_tree.txt");
    Huffman_encode(book,"DC_Y.txt",DC_Y,size_DC,amount);
    Huffman_decode(DC_Y_node,amount,DC_Y,"DC_Y.txt");

    free(book);
    free(temp);

    // DC_Cb //
    temp=calloc(size_DC,sizeof(int));
    amount=getinfo(DC_Cb,size_DC,temp);
    node *DC_Cb_node=build_treemnode(temp,amount);
    book=build_codebook(DC_Cb_node,amount);
       
    Huffman(DC_Cb_node,amount,book,"DC_Cb_tree.txt");
    Huffman_encode(book,"DC_Cb.txt",DC_Cb,size_DC,amount);
    Huffman_decode(DC_Cb_node,amount,DC_Cb,"DC_Cb.txt");

    free(book);
    free(temp);

    // DC_Cr //
    temp=calloc(size_DC,sizeof(int));
    amount=getinfo(DC_Cr,size_DC,temp);
    node *DC_Cr_node=build_treemnode(temp,amount);
    book=build_codebook(DC_Cr_node,amount);
       
    Huffman(DC_Cr_node,amount,book,"DC_Cr_tree.txt");
    Huffman_encode(book,"DC_Cr.txt",DC_Cr,size_DC,amount);
    Huffman_decode(DC_Cr_node,amount,DC_Cr,"DC_Cr.txt");

    free(book);
    free(temp);

    // IRLE
    AC_Y=IRLE(rle_Y,size_Y,size);
    AC_Cb=IRLE(rle_Cb,size_Cb,size);
    AC_Cr=IRLE(rle_Cr,size_Cr,size);

    // de-zigzag
    data_quan=dezigzag(Nh,Nw,AC_Y,AC_Cb,AC_Cr,DC_Y,DC_Cb,DC_Cr);

    // dequantatize
    dequantization(data_quan, Nh, Nw,data_DCT);

    // IDCT
    data_padding = IDCT(data_DCT, Nh, Nw, cosi);

    // YCbCr convert to RGB
    YCbCr_convert(data_padding, data_RGB, H, W);
    
    // output
    writebmp(output, &bmpheader, data_RGB, skip);
}