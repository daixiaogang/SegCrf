#ifndef DATA_H
#define DATA_H

//#include <mat.h>
#include "global.h"

bool ReadBinData(char* filename,int width,int height);
void GetDataCost(int width,int height,double weight);

#endif // DATA_H
