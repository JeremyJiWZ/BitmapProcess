//
//  bitmapProcess.h
//  BinarizeOtsu
//
//  Created by mr.ji on 15/11/10.
//  Copyright (c) 2015年 mr.ji. All rights reserved.
//

#ifndef __bitmapProcess__
#define __bitmapProcess__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
typedef unsigned char	BYTE;	/*1 byte  */
typedef unsigned short	WORD;	/*2 bytes */
typedef unsigned int	DWRD;	/*4 bytes */
#pragma pack(2)
typedef struct tagBITMAPFILEHEADER
{
    WORD	bfType;		/*文件标识 BM */
    DWRD	bfSize;		/*文件总长度  */
    WORD	bfReserved1;/*保留，总为0*/
    WORD	bfReserved2;/*保留，总为0*/
    DWRD	bfOffBits;	/*  偏移量    */
}
BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWRD biSize;	/*BITMAPINFOHEADER的大小*/
    DWRD biWidth;	/*位图宽度*/
    DWRD biHeight;	/*位图高度*/
    WORD biPlanes;	/*颜色位面数，总为1*/
    WORD biBitCount;	/*每象数颜色位（1，2，4，8，24）*/
    DWRD bmCompression;/*压缩模式（0=无）*/
    DWRD biSizeImage;	/*位图大小*/
    DWRD biXPelsPerMeter;	/*水平分辨率*/
    DWRD biYPelsPerMeter;	/*垂直分辨率*/
    DWRD biClrUsed;		/*所使用的颜色数*/
    DWRD beClrImportant;	/*重要颜色数*/
}
BITMAPINFOHEADER;

enum ImageType
{
    RealImage,
    GrayScale,
    BinaryImage
};
typedef struct tagRGBQUAD
{
    BYTE	rgbBlue;
    BYTE	rgbGreen;
    BYTE	rgbRed;
    BYTE	rgbReserved;
}
RGBQUAD;
typedef struct color_RGB{
    BYTE R,G,B;
}RGB;
typedef struct point_x_y
{
    int x;
    int y;
}POINT;
class Bitmap
{
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER ih;
    RGBQUAD *rgb;
    BYTE *imageData;
    string fileName;
    int widthBytes;
public:
    Bitmap(string FN){fileName=FN;};
    Bitmap(){fileName="";};
    ~Bitmap(){};//析构rgb和imageData,暂未实现
    void setName(string Name){fileName=Name;};
    void setFH(ImageType i, int weight, int height);
    void setIH(ImageType i, int weight, int height);
    void setBinaryRGBQUAD();
    void setGrayRGBQUAD();
    BYTE* getImage();
    void ReadImage();
    void WriteFile();
    
    //for grayscale
    void TurnBinarize(Bitmap bitSource);
    
    //for binary
    void Dilation();
    void Erosion();
    void Opening();
    void Closing();
    
    //for 24-bit:
    //changne the luminance
    void ChangeLuminance(int k);
    //turn to grayscale
    void TurnToGray();
    
    //for grayscale
    //enhance visibility
    void VisibilityEnhancement();
    //equalize histogram
    void HistogramEqualization();
    
    //for 24-bit:
    //enhance visibility
    void RealVisiEnhance();
    //equalize histogram
    void RealHistogramEqual();
    
    //geometry transformation:
    //translate 24-bit image
    void tranlate(int x, int y);
    //rotate 24-bit image, anti-clock
    void rotate(float theta);
    //scale 24-bit image
    void scale(float c,float d);
    //mirror by x axis
    void mirror_by_x();
    //mirror by y axis
    void mirror_by_y();
    //shear the picture
    void shear_on_x(float dx);
    void shear_on_y(float dy);

    
    //mean filtering
    void mean_filter();
    //laplacian filtering
    void laplacian_filter();
    //biliteral filtering
    void bilteral_filter();
    
};
enum MaskType{
    Laplacian,
    Linear
};
class Mask
{
    int* mask;
public:
    Mask(){
        mask = new int[9];
        mask[0]=mask[2]=mask[6]=mask[8]=1;
        mask[1]=mask[3]=mask[5]=mask[7]=2;
        mask[4]=4;
    };
    Mask(MaskType tp){
        if (tp==Linear) {
            Mask();
        }
        if (tp==Laplacian) {
            mask = new int[9];
            mask[0]=mask[1]=mask[2]=
            mask[3]=mask[5]=
            mask[6]=mask[7]=mask[8]=1;
            mask[4]=-8;
        }
    }
    ~Mask(){};
    double involution(BYTE* image,int widthBytes,int type,int x,int y);

};
BYTE get_r(BYTE* imageData,int widthBytes,int x,int y);//new interface,get the R of 24-bit image
BYTE get_g(BYTE* imageData,int widthBytes,int x,int y);//new interface,get the G of 24-bit image
BYTE get_b(BYTE* imageData,int widthBytes,int x,int y);//new interface,get the B of 24-bit image

void BinarizeOtsu(int ImageWeight, int ImageHeight, BYTE* gray,BYTE *imageData,int blockWid, int blockHeight);
void resetImageData(BYTE* imageData, int x, int y, int widthBytes);
void setImageData(BYTE* imageData, int x, int y, int widthBytes);
int getImageData(BYTE* imageData,int x, int y, int widthBytes);
//双线性插值
RGB BiLinearInterpolation(float x, float y,RGB A,RGB B,RGB C, RGB D);
#endif /* defined(__bitmapProcess__) */
