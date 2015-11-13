//
//  main.cpp
//  BinarizeOtsu
//
//  Created by mr.ji on 15/11/10.
//  Copyright (c) 2015年 mr.ji. All rights reserved.
//

#include <iostream>
#include "bitmapProcess.h"
using namespace std;
int main() {
    cout<<"input the file name: ";
    string fileName;
    cin>>fileName;
    Bitmap sourceImage(fileName);
    sourceImage.ReadImage();
    cout<<"input the destination file name:";
    string desFileName;
    cin>>desFileName;
    Bitmap desImage(desFileName);
    desImage.TurnBinarize(sourceImage);
    desImage.WriteFile();
    return 0;
}
