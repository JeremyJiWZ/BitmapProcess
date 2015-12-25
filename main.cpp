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
    string fileName="/Users/jiwentadashi/Desktop/eye.bmp";   //source image
    string des_file="/Users/jiwentadashi/Desktop/lena_filter.bmp";    //destination
    image.setName(fileName);
    image.ReadImage();
//    image.TurnToGray();
//    image.VisibilityEnhancement();
//    image.HistogramEqualization();
//    image.RealVisiEnhance();
//    image.tranlate(10, 10);
//    image.mirror_by_y();
//    image.mirror_by_x();
//    image.rotate(30);
//    image.scale(1.3, 1.3);
//    image.scale(10, 10);
//    image.scale(0.9, 0.5);
//    image.shear_on_x(0.5);
//    image.shear_on_y(0.5);
//    image.mean_filter();
//    image.laplacian_filter();
    image.bilteral_filter();
    image.bilteral_filter();
//    image.bilteral_filter();
//    image.bilteral_filter();
    image.setName(des_file);
    image.WriteFile();
    
}
