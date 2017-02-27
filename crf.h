#ifndef CRF_H
#define CRF_H

#include "global.h"
#include "./gco-v3.0/GCoptimization.h"


double SmoothCostWeightofNeighbor(int x1, int y1, int x2, int y2, Mat&img,		//rgb值代表类别属性
                                  double lamda1 = 1.0, double lamda2 = 1.0,		//权重
                                  double theta_alpha = 1.0, double theta_beta = 1.0, double theta_gama = 1.0);

double* GetSmoothWeightofImg(Mat&SrcImg, int x1, int x2, int y1, int y2);


double smoothFn(int p1, int p2, int l1, int l2);
void Optimization(int width, int height, int num_pixels, int num_labels, vector<int>&Result);
void OptimizationGeneralGraph(int width, int height, int num_pixels, int num_labels, vector<int>&Result);
void OptimizationGridGraphbySmooth(int width, int height, int num_pixels, int num_labels,double lamda);
void Optimization8NeighborsGraphbySmooth(int width, int height, int num_pixels, int num_labels, double lamda, Mat&img);

#endif // CRF_H
