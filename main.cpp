//
//  main.cpp
//  ImageProcess
//
//  Created by mr.ji on 15/11/25.
//  Copyright (c) 2015年 mr.ji. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "bitmapProcess.h"

using namespace std;
int main(int argc, const char * argv[]) {
    Bitmap image;
    string fileName="/Users/jiwentadashi/Pragramming/ImageProcessI/ImageProcessI/2.bmp";
    string des_file="/Users/jiwentadashi/Pragramming/ImageProcessI/ImageProcessI/10g.bmp";
    image.setName(fileName);
    image.ReadImage();
    image.TurnToGray();
    image.setName(des_file);
    image.WriteFile();
    
    
    return 0;
}
