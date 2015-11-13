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
    ~Bitmap(){};//析构rgb和imageData,暂未实现
    void setName(string Name){fileName=Name;};
    void setFH(ImageType i, int weight, int height);
    void setIH(ImageType i, int weight, int height);
    void setBinaryRGBQUAD();
    BYTE* getImage();
    void ReadImage();
    void WriteFile();
    void TurnBinarize(Bitmap bitSource);
};
void BinarizeOtsu(int weight, int height, BYTE* gray,BYTE *imageData);
#endif /* defined(__bitmapProcess__) */
