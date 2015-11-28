//
//  main.cpp
//  ImageProcess
//
//  Created by mr.ji on 15/11/10.
//  Copyright (c) 2015年 mr.ji. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "bitmapProcess.h"

using namespace std;
int main(int argc, const char * argv[]) {
    Bitmap image;
    string fileName="/Users/jiwentadashi/Downloads/t.bmp";   //source image
    string des_file="/Users/jiwentadashi/Desktop/reak.bmp";    //destination
    image.setName(fileName);
    image.ReadImage();
//    image.TurnToGray();
//    image.VisibilityEnhancement();
//    image.HistogramEqualization();
//    image.RealVisiEnhance();
    image.RealHistogramEqual();
    image.setName(des_file);
    image.WriteFile();
    
}
