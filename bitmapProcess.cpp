//
//  bitmapProcess.cpp
//  BinarizeOtsu
//
//  Created by mr.ji on 15/11/10.
//  Copyright (c) 2015年 mr.ji. All rights reserved.
//

#include "bitmapProcess.h"
void Bitmap::setFH(ImageType i, int weight, int height)
{
    WORD TYPE=0x4d42;
    int bitCount = (i==RealImage)?24:(i==GrayScale)?8:1;
    int colorNum = (bitCount==24)?0:(bitCount==8)?256:2;
    widthBytes=((weight*bitCount+31)&~31)/bitCount;
    fh.bfType=TYPE;
    fh.bfReserved1=0;
    fh.bfReserved2=0;
    fh.bfSize=widthBytes*height+54+4*colorNum;
    fh.bfOffBits=54+4*colorNum;
}
void Bitmap::setIH(ImageType i, int weight, int height)
{
    int bitCount = (i==RealImage)?24:(i==GrayScale)?8:1;
    widthBytes=((weight*bitCount+31)&~31)/8;
    ih.biSize=40;
    ih.biWidth=weight;
    ih.biHeight=height;
    ih.biPlanes=1;
    ih.biBitCount=bitCount;
    ih.bmCompression=0;
    ih.biXPelsPerMeter=ih.biYPelsPerMeter=88;//don't know,randomly put
    ih.biSizeImage=widthBytes*height;
    ih.biClrUsed=0;
}
void Bitmap::setBinaryRGBQUAD()
{
    rgb = (RGBQUAD *)malloc(2*sizeof(RGBQUAD));
    rgb[0].rgbBlue=rgb[0].rgbRed=rgb[0].rgbGreen=rgb[0].rgbReserved=0;
    rgb[1].rgbReserved=0;
    rgb[1].rgbRed=rgb[1].rgbGreen=rgb[1].rgbBlue=255;
}
void Bitmap::ReadImage()
{
    FILE *fp;
    fp=fopen(fileName.c_str(), "rb");
    if (fp==NULL) {
        printf("error:cannot open file\n");
        exit(0);
    }
    fread(&fh, 14, 1, fp);
    fread(&ih, 40, 1, fp);
    if (ih.biBitCount==1) {
        rgb=new RGBQUAD[2];
        fread(rgb, sizeof(RGBQUAD), 2, fp);
    }
    if (ih.biBitCount==8) {
        rgb=new RGBQUAD[256];
        fread(rgb, sizeof(RGBQUAD), 256, fp);
    }
    widthBytes = ((ih.biWidth*ih.biBitCount+31)&~31)/8;
    imageData = new BYTE[ih.biHeight*widthBytes];
    fread(imageData, widthBytes, ih.biHeight, fp);
    
}
void Bitmap::WriteFile()
{
    FILE *fp;
    fp=fopen(fileName.c_str(), "wb");
    if (fp==NULL) {
        printf("error:cannot create file!");
        exit(0);
    }
    fwrite(&fh, 1, 14, fp);
    fwrite(&ih, 1, 40, fp);
    if(ih.biBitCount==1)
        fwrite(rgb, 2, sizeof(RGBQUAD), fp);
    if(ih.biBitCount==8)
        fwrite(rgb, 256, sizeof(RGBQUAD), fp);
    fwrite(imageData, widthBytes, ih.biHeight, fp);
}
BYTE* Bitmap::getImage()
{
    return imageData;
}
void Bitmap::TurnBinarize(Bitmap bitSource)
{
    int weight,height;
    BYTE *gray;
    weight=bitSource.ih.biWidth;
    height=bitSource.ih.biHeight;
    gray=bitSource.getImage();
    //initialize the file header and info header
    setFH(BinaryImage, weight, height);
    setIH(BinaryImage, weight, height);
    ih.biXPelsPerMeter=bitSource.ih.biXPelsPerMeter;
    ih.biYPelsPerMeter=bitSource.ih.biYPelsPerMeter;
    setBinaryRGBQUAD();
    //alloc memory for image data
    imageData = new BYTE[height*widthBytes];
    
    //binarie the image block by block
    //set the block size be 1/100 of the image
    int wid=0,hei=0;
    int blockWidth=weight/10, blockHeight=height/10;
    //turn the block size to be multiples of 4
    blockWidth=(blockWidth+3)&~3;
    blockHeight=(blockHeight+3)&~3;
    for (int i=0; i<10; i++) {
        for (int j=0; j<10; j++) {
            BinarizeOtsu(wid, hei, gray+weight*hei+wid, imageData+widthBytes*hei+wid);
            wid+=blockWidth;
        }
        hei+=blockHeight;
    }
//    binarize the block by whole image
//    BinarizeOtsu(weight, height, gray,imageData);
    
    
}
void BinarizeOtsu(int weight, int height, BYTE *gray,BYTE *imageData)
{
    int widthBytes = ((weight+31)&~31)/8;//binary image line bytes
    double histogram[256];
    int i,j;
    for (i=0;i<256;i++)
        histogram[i]=0;
    for (int i=0; i<height; i++) {
        for (int j=0; j<weight; j++) {
            histogram[gray[i*weight+j]]++;
        }
    }
    for (i=0;i<256;i++)
        histogram[i]/=weight*height;//get the histogram
    double U=0;
    for (i=0;i<256;i++)
        U+=i*histogram[i];  //total u
    double Wf=0.0,Wb=0.0;   //weight of foregroung and background
    double Uf=0.0,Ub=0.0;   //foreground u, and background u
    double T=0.0;           //threshold
    double maxVariance=0.0;    //maxVariance
    //determine threshold
    for (i=0; i<256; i++) {
        Wf+=histogram[i];
        Wb=1-Wf;
        Uf+=i*histogram[i];
        Ub=(U-Uf*Wf)/Wb;
        double variance=Wb*Wf*(Ub-Uf)*(Ub-Uf);
        if (variance>maxVariance) {
            maxVariance=variance;
            T=i;
        }
    }
    //write data back
    for (i=0;i<height;i++)
    {
        for (j=0; j<widthBytes; j++) {
            imageData[i*widthBytes+j]=0;
            for (int k=0; k<8; k++) {
                int p = gray[i*weight+j*8+k]>T?1:0;
                imageData[i*widthBytes+j]=imageData[i*widthBytes+j]|(p<<(7-k));//test
//                cout<<(int)imageData[i*height+j]<<' ';
            }
        }
    }
}