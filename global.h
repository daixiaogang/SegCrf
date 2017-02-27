#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

#ifdef WIN
#include <Windows.h>
#else
typedef unsigned char BYTE;
#endif

#define IMG_WIDTH			   1200		//图像宽
#define IMG_HEIGHT			   200		//图像高
#define NUM_SAMPLES			   1200*200			//训练样本数量
#define NUM_LABLES		       2					//类别数
#define PI					   3.141592654

#endif // GLOBAL_H
