//
//  bitmapProcess.cpp
//  BinarizeOtsu
//
//  Created by mr.ji on 15/11/10.
//  Copyright (c) 2015年 mr.ji. All rights reserved.
//

#include "bitmapProcess.h"
#include <math.h>
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
    rgb = new RGBQUAD[2];
    rgb[0].rgbBlue=rgb[0].rgbRed=rgb[0].rgbGreen=rgb[0].rgbReserved=0;
    rgb[1].rgbReserved=0;
    rgb[1].rgbRed=rgb[1].rgbGreen=rgb[1].rgbBlue=255;
}
void Bitmap::setGrayRGBQUAD()
{
    rgb = new RGBQUAD[256];
    for (int i=0; i<256; i++) {
        rgb[i].rgbBlue=rgb[i].rgbGreen=rgb[i].rgbRed=i;
        rgb[i].rgbReserved=0;
    }
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
    //initialize imageData
    for (int i =0; i<height; i++) {
        for (int j=0; j<widthBytes; j++) {
            imageData[i*widthBytes+j]=0;
        }
    }
    
    //binarie the image block by block
    //set the block size be 1/100 of the image
    int wid=0,hei=0;
    int blockWidth=32, blockHeight=32;//define the block size: 32*32
    while(hei<height){
        for (wid=0; wid<weight; wid+=blockWidth) {
            BinarizeOtsu(weight, height, gray+bitSource.widthBytes*hei+wid, imageData+widthBytes*hei+wid/8, blockWidth, blockHeight);
        }
        hei+=blockHeight;
        if (hei+blockHeight>height)
            blockHeight=height-hei;
    }
//    binarize the block by whole image
//    BinarizeOtsu(weight, height, gray,imageData,weight,height);
}
void Bitmap::Dilation()
{
    int ImageWidth=ih.biWidth;
    int ImageHeight=ih.biHeight;
//    int structElement[3][3]={0,1,0,1,1,1,0,1,0};
    int widthBytes = ((ImageWidth+31)&~31)/8;
    BYTE* newImageData = new BYTE[widthBytes*ImageHeight];
    for (int y=0; y<ImageHeight; y++) {
        for (int x=0; x<widthBytes; x++) {
            newImageData[y*widthBytes+x]=255;
        }
    }
    for (int y=1; y<ImageHeight-1; y++) {
        for (int x=1; x<ImageWidth-1; x++) {
            if (getImageData(imageData, x-1, y, widthBytes)||
                getImageData(imageData, x, y, widthBytes)||
                getImageData(imageData, x+1, y, widthBytes)||
                getImageData(imageData, x, y+1, widthBytes)||
                getImageData(imageData, x, y-1, widthBytes)
                )
            {
                resetImageData(newImageData,x, y, widthBytes);
            }
        }
    }
    delete[] imageData;
    imageData=newImageData;
}
void Bitmap::Erosion()
{
    int ImageWidth=ih.biWidth;
    int ImageHeight=ih.biHeight;
//    int structElement[2][2]={1,1,1,0};
    int widthBytes = ((ImageWidth+31)&~31)/8;
    BYTE* newImageData = new BYTE[widthBytes*ImageHeight];
    for (int y=0; y<ImageHeight; y++) {
        for (int x=0; x<widthBytes; x++) {
            newImageData[y*widthBytes+x]=255;
        }
    }
    for (int y=1; y<ImageHeight; y++) {
        for (int x=0; x<ImageWidth-1; x++) {
            if (getImageData(imageData, x, y, widthBytes)&&
                getImageData(imageData, x, y-1, widthBytes)&&
                getImageData(imageData, x+1, y, widthBytes)
                )
            {
                resetImageData(newImageData,x, y, widthBytes);
            }
        }
    }
    delete[] imageData;
    imageData=newImageData;
}
void Bitmap::Opening()
{
    Erosion();
    Dilation();
}
void Bitmap::Closing()
{
    Dilation();
    Erosion();
}
void BinarizeOtsu(int ImageWeight, int ImageHeight, BYTE *gray,BYTE *imageData,int blockWid, int blockHeight)
{
    int widthBytes = ((blockWid+31)&~31)/8;//binary image line bytes
    int widBytesOfImage = ((ImageWeight+31)&~31)/8;
    int widBytesOfGrayImage = ((ImageWeight*8+31)&~31)/8;
    double histogram[256];
    int i,j;
    for (i=0;i<256;i++)
        histogram[i]=0;
    for (int i=0; i<blockHeight; i++) {
        for (int j=0; j<blockWid; j++) {
            histogram[gray[i*widBytesOfGrayImage+j]]++;
        }
    }
    for (i=0;i<256;i++)
        histogram[i]/=blockWid*blockHeight;//get the histogram
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
    T*=0.8;//manually lower the threshold, because we need it
    //write data back
    for (i=0;i<blockHeight;i++)
    {
        for (j=0; j<widthBytes; j++) {
            for (int k=0; k<8; k++) {
                int p = gray[i*widBytesOfGrayImage+j*8+k]>T?1:0;
                imageData[i*widBytesOfImage+j]=imageData[i*widBytesOfImage+j]|(p<<(7-k));
            }
        }
    }
}

//get image data by bit
//1->black 0->white
int getImageData(BYTE* imageData,int x, int y, int widthBytes)
{
    int wid;    //width
    int remain; //remain
    wid=x/8;
    remain=x%8;
    if((((imageData[y*widthBytes+wid])>>(7-remain))&1)==1)
        return 0;
    return 1;
}
//reset image data by bit
//which equals set the bit black
void resetImageData(BYTE* imageData, int x, int y, int widthBytes)
{
    int wid;    //width
    int remain; //remain
    wid=x/8;
    remain=x%8;
    imageData[y*widthBytes+wid]&=(~(1<<(7-remain)))&0b11111111; //bit operation
}
//set image data by bit
//which equals set the bit white
void setImageData(BYTE* imageData, int x, int y, int widthBytes)
{
    int wid;    //width
    int remain; //remain
    wid=x/8;
    remain=x%8;
    imageData[y*widthBytes+wid]|=(1<<(7-remain))&0b11111111; //bit opertation
}
//change luminance for a 24-bit image
void Bitmap::ChangeLuminance(int k){
        int     i, j;
        int     rr,gg,bb;
        double  Y,U,V;
        //for debug
        /*
         printf("bftype:%d, bfSize:%d, bfReserved1:%d, bfReserved2:%d, bfOff:%d\n",
         sizeof(fh.bfType),sizeof(fh.bfSize),sizeof(fh.bfReserved1),sizeof(fh.bfReserved2)
         ,sizeof(fh.bfOffBits));
         printf("fh: %d\n ih:%d\n",sizeof(fh),sizeof(ih));
         printf("DFH:bftype:%x, bfsize:%x,bfReserved1:%x,bfreserved2:%x,bfOffBits:%x\n",
         dfh.bfType,dfh.bfSize,dfh.bfReserved1,dfh.bfReserved2, dfh.bfOffBits);
         printf("bftype:%d, bfsize:%d,bfresered1:%d,bfreserved2:%d,bfoffbit:%d\n",
         sizeof(fh.bfType),sizeof(fh.bfSize),sizeof(fh.bfReserved1),
         sizeof(fh.bfReserved2),sizeof(fh.bfOffBits));
         printf("bfSize=%ld\n", fh.bfSize);
         printf("Image: %ld x %ld\n", ih.biWidth, ih.biHeight);
         printf("biBitCount=%d\n", ih.biBitCount);
         */
        
        
        //
        widthBytes = ((ih.biWidth*24+31)&~31)/8;//3
        for(i=0;i<ih.biHeight;i++)
        {
            for(j=0;j<widthBytes/3;j++)//1
            {
                //calculate YUV
                Y=imageData[i*widthBytes+j*3]*0.299+imageData[i*widthBytes+j*3+1]*0.587+imageData[i*widthBytes+j*3+2]*0.114;
                U=0.493*(imageData[i*widthBytes+j*3+2]-Y);
                V=0.877*(imageData[i*widthBytes+j*3]-Y);
                //change Y
                Y=Y+k;
                //reset RGB
                rr=Y+1.14*V;
                gg=Y-0.394*U-0.581*V;
                bb=Y+2.032*U;
                if(rr>255)
                    rr=255;
                if(gg>255)
                    gg=255;
                if(bb>255)
                    bb=255;
                if(rr<0)
                    rr=0;
                if(gg<0)
                    gg=0;
                if(bb<0)
                    bb=0;
                imageData[i*widthBytes+j*3+0]=rr;
                imageData[i*widthBytes+j*3+1]=gg;
                imageData[i+j*3+2]=bb;
            }
        }
}
void Bitmap::TurnToGray()
{
    int     i, j;
    BYTE*   gray;//u用来存每行的图像数据，gray用来存每行的灰度数据
    DWRD    widthBytes,DFwidthBytes;//图像存储时真实的每行长度
    
    //initialization
    int width,height;
    width=ih.biWidth;
    height=ih.biHeight;
    setFH(GrayScale,width,height);
    setIH(GrayScale,width,height);
    setGrayRGBQUAD();
    
    widthBytes = ((ih.biWidth*24+31)&~31)/8;//3
    DFwidthBytes = ((ih.biWidth*8+31)&~31)/8;
//    this->widthBytes= DFwidthBytes;
    gray = new BYTE[DFwidthBytes*ih.biHeight];
//    printf("widthBytes:%d\n",widthBytes );
//    printf("DFwidthBytes:%d\n",DFwidthBytes);

    for(i=0;i<ih.biHeight;i++)
    {
        for(j=0;j<DFwidthBytes;j++)//1
        {
//            cout<<(int)gray[i*widthBytes+j]<<endl;
//            cout<<(int)imageData[i*widthBytes+j*3]<<' '<<(int)imageData[i*widthBytes+j*3+1]<<' '<<(int)imageData[i*widthBytes+j*3+2]<<' ';
            gray[i*DFwidthBytes+j]=
             imageData[i*widthBytes+j*3+0]*0.299    //r
            +imageData[i*widthBytes+j*3+1]*0.587    //g
            +imageData[i*widthBytes+j*3+2]*0.114;   //b
//            cout<<(int)gray[i*DFwidthBytes+j]<<endl;
        }
        
    }
    delete[] imageData;
    imageData = gray;
    
}
void Bitmap::VisibilityEnhancement()
{
    int Lmax=0;
    //find Lmax
    for (int i=0; i<ih.biHeight; i++) {
        for (int j=0; j<ih.biWidth; j++) {
            if (imageData[i*widthBytes+j]>Lmax) {
                Lmax=imageData[i*ih.biWidth+j];
            }
        }
    }
    for (int i=0; i<ih.biHeight; i++) {
        for (int j=0; j<ih.biWidth; j++) {
            imageData[i*widthBytes+j]=255*(log(imageData[i*widthBytes+j]+1)/log(Lmax+1));
        }
    }
}
void Bitmap::HistogramEqualization()
{
    double histogram[256];
    double T[256];
    for (int i=0; i<256; i++) {
        histogram[i]=0;
    }
    for (int i=0; i<256; i++) {
        T[i]=0;
    }
    for (int i=0; i<ih.biHeight; i++) {
        for (int j=0; j<ih.biWidth; j++) {
            histogram[imageData[i*widthBytes+j]]++;
        }
    }
    for (int i=0; i<256; i++) {
        histogram[i]/=ih.biWidth*ih.biHeight;
    }
    T[0]=histogram[0];
    for(int i=1;i<256;i++){
        T[i]=T[i-1]+histogram[i];
//        cout<<T[i]*256<<' ';
    }
    for (int i=0; i<ih.biHeight; i++) {
        for (int j=0; j<ih.biWidth; j++) {
            imageData[i*widthBytes+j]=255*T[imageData[i*widthBytes+j]];
        }
    }
    
}
void Bitmap::RealVisiEnhance()
{
    int     i, j;
    int Rmax=0,Gmax=0,Bmax=0;
    //find Lmax
    for (int i=0; i<ih.biHeight; i++) {
        for (int j=0; j<widthBytes/3; j++) {
            if (imageData[i*widthBytes+j*3]>Rmax)
                Rmax=imageData[i*widthBytes+j*3];
            if (imageData[i*widthBytes+j*3+1]>Gmax)
                Gmax=imageData[i*widthBytes+j*3+1];
            if (imageData[i*widthBytes+j*3+2]>Bmax)
                Bmax=imageData[i*widthBytes+j*3+2];
        }
    }
    cout<<Rmax<<' '<<Gmax<<' '<<Bmax<<endl;
    widthBytes = ((ih.biWidth*24+31)&~31)/8;//3
    for(i=0;i<ih.biHeight;i++)
    {
        for(j=0;j<widthBytes/3;j++)//1
        {
            int rr,gg,bb;
            rr=imageData[i*widthBytes+j*3];
            gg=imageData[i*widthBytes+j*3+1];
            bb=imageData[i*widthBytes+j*3+2];
            imageData[i*widthBytes+j*3]=255*(log(rr+1)/log(Rmax)+1);
            imageData[i*widthBytes+j*3+1]=255*(log(gg+1)/log(Gmax)+1);
            imageData[i*widthBytes+j*3+2]=255*(log(bb+1)/log(Bmax)+1);
        }
    }
}
void Bitmap::RealHistogramEqual()
{
    double Rhistogram[256],Ghistogram[256],Bhistogram[256];
    double Tr[256],Tg[256],Tb[256];
    for (int i=0; i<256; i++) {
        Rhistogram[i]=0;
        Ghistogram[i]=0;
        Bhistogram[i]=0;
    }
    for (int i=0; i<256; i++) {
        Tr[i]=0;Tg[i]=0;Tb[i]=0;
    }
    for (int i=0; i<ih.biHeight; i++) {
        for (int j=0; j<ih.biWidth; j++) {
            Rhistogram[imageData[i*widthBytes+j*3]]++;
            Ghistogram[imageData[i*widthBytes+j*3+1]]++;
            Bhistogram[imageData[i*widthBytes+j*3+2]]++;
        }
    }
    for (int i=0; i<256; i++) {
        Rhistogram[i]/=ih.biWidth*ih.biHeight;Ghistogram[i]/=ih.biWidth*ih.biHeight;Bhistogram[i]/=ih.biWidth*ih.biHeight;
    }
    Tr[0]=Rhistogram[0];Tg[0]=Ghistogram[0];Tb[0]=Bhistogram[0];
    for(int i=1;i<256;i++){
        Tr[i]=Tr[i-1]+Rhistogram[i];Tg[i]=Tg[i-1]+Ghistogram[i];Tb[i]=Tb[i-1]+Rhistogram[i];
//        cout<<Tr[i]*256<<' ';
    }
    for (int i=0; i<ih.biHeight; i++) {
        for (int j=0; j<ih.biWidth; j++) {
            //change rgb
            BYTE r=imageData[i*widthBytes+j*3+0],g=imageData[i*widthBytes+j*3+1],b=imageData[i*widthBytes+j*3+2];
//            cout<<"before:"<<(int)r<<' '<<(int)g<<' '<<(int)b<<endl;
            imageData[i*widthBytes+j*3+0]=255*Tr[r];
            imageData[i*widthBytes+j*3+1]=255*Tg[g];
            imageData[i*widthBytes+j*3+2]=255*Tb[b];
//            cout<<"after:"<<(int)imageData[i*widthBytes+j*3+0]<<' '<<(int)imageData[i*widthBytes+j*3+1]<<' '<<(int)imageData[i*widthBytes+j*3+2]<<endl;
        }
    }
}
