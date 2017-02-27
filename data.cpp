#include "data.h"

extern vector<vector<double>> ImageCost;
extern vector<vector<double>> LidarCost;
extern vector<vector<double>> TotalCost;
extern vector<int> Result;

//// 读取.mat文件（例：mat文件名为"initUrban.mat"，其中包含"initA"）
//Mat ReadMatFile(char* filename,char* matname)
//{
//    MATFile *pmatFile = NULL;
//    mxArray *pMxArray = NULL;
//    double *pData;
//    Mat ret;
//    pmatFile = matOpen(filename,"r");
//    pMxArray = matGetVariable(pmatFile, matname);
//    pData = (double*) mxGetData(pMxArray);
//    int M = mxGetM(pMxArray);
//    int N = mxGetN(pMxArray);
//    ret.create(M,N,CV_32F);
//    memcpy(ret.data,pData,M*N*sizeof(double));
//    matClose(pmatFile);
//    mxFree(pData);
//    return ret;
//}

//// 生成.mat文件
//bool WriteMatFile(char* filename,char *matname,int M,int N,double*pData )
//{
//    MATFile *pmatFile = NULL;
//    mxArray *pMxArray = NULL;
//    pmatFile = matOpen("A.mat","w");
//    pMxArray = mxCreateDoubleMatrix(M, N, mxREAL);
//    mxSetData(pMxArray, pData);
//    matPutVariable(pmatFile, matname, pMxArray);
//    matClose(pmatFile);
//    return true;
//}


//Read bin data from file
bool ReadBinData(char* filename,int width,int height,float *pData)
{
    FILE* fp=NULL;
    fp=fopen(filename,"rt");
    if(!fp)
    {
        printf("No such file !\n");
        return false;
    }
    fread(pData,sizeof(float),width*height,fp);
    return true;
}

//Get cost idata
void GetDataCost(int width,int height,double weight)
{
    int i;
    char filename[125];
    float *pData_0=new float[width*height*sizeof(float)];
    float *pData_1=new float[width*height*sizeof(float)];
    //read image cost
    sprintf(filename,"/home/yangfei/Data/kitti_iter_0_0.bin");
    ReadBinData(filename,width,height,pData_0);
    sprintf(filename,"/home/yangfei/Data/kitti_iter_0_1.bin");
    ReadBinData(filename,width,height,pData_1);
    for(i=0;i<width*height;i++)
    {
        ImageCost[i][0]=-log(pData_0[i]);
        ImageCost[i][1]=-log(pData_1[i]);
    }
    //read lidar cost
    sprintf(filename,"/home/yangfei/Data/lidar_iter_0_0.bin");
    ReadBinData(filename,width,height,pData_0);
    sprintf(filename,"/home/yangfei/Data/lidar_iter_0_1.bin");
    ReadBinData(filename,width,height,pData_1);
    for(i=0;i<width*height;i++)
    {
        LidarCost[i][0]=-log(pData_0[i]);
        LidarCost[i][1]=-log(pData_1[i]);
    }
    //weight the 2 cost to total cost
    double cost_0=0;
    double cost_1=0;
    for(i=0;i<width*height;i++)
    {
        cost_0=ImageCost[i][0]*weight+LidarCost[i][0]*(1-weight);
        cost_1=ImageCost[i][1]*weight+LidarCost[i][1]*(1-weight);
        TotalCost[i][0]=cost_0;
        TotalCost[i][1]=cost_1;
        Result[i]=cost_0<cost_1?0:1;

    }

    if(pData_0) delete[] pData_0;
    if(pData_1) delete[] pData_1;
}
