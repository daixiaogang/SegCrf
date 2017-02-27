#include "global.h"
#include "data.h"
#include "crf.h"



vector<int> Result(NUM_SAMPLES);		//分类结果
vector<int> CRFResult(NUM_SAMPLES);
vector<vector<double>> ImageCost(NUM_SAMPLES, vector<double>(NUM_LABLES));
vector<vector<double>> LidarCost(NUM_SAMPLES, vector<double>(NUM_LABLES));
vector<vector<double>> TotalCost(NUM_SAMPLES, vector<double>(NUM_LABLES));


double *pSmooth;

int main()
{
    char filename[256];
    int i,j,x,y;
    int frameID=1;
    int width=IMG_WIDTH;
    int height=IMG_HEIGHT;
    double weight=0.5;
    sprintf(filename,"/home/yangfei/Data/1.png");
    Mat SrcImg=imread(filename);
    //如果读入图像失败
    if(SrcImg.empty())
    {
            fprintf(stderr, "Can not load image %s\n", filename);
            return -1;
    }
    int rows=SrcImg.rows;
    int cols=SrcImg.cols;
 //   printf("the size of img is %d\t%d\n",SrcImg.rows,SrcImg.cols);
    GetDataCost(width,height,weight);
    Optimization8NeighborsGraphbySmooth(width, height, width*height, 2, 3.0, SrcImg);

    BYTE *pResImg = new BYTE[width*height * 3];
    BYTE *pResCRFImg = new BYTE[width*height * 3];
    BYTE *pCur, *pCurCRF;
    for (y = 0, i = 0, pCur = pResImg, pCurCRF = pResCRFImg; y < height; y++)
    {
        for (x = 0; x < width; x++, i++)
        {
            //	int val = gBayse.GetWhatLableofPixel(x, y);//Result[i++];
            int val = Result[i];
            //	int val_crf = Labels(y, x);
            int val_crf = CRFResult[i];
            //分类器结果
            if (val == 1)
            {
                *pCur++ = 0;	//b
                *pCur++ = 255;		//g
                *pCur++ = 0;		//r
            }
            else
            {
                *pCur++ = 0;	//b
                *pCur++ = 0;		//g
                *pCur++ = 255;		//r
            }
            //CRF结果
            if (val_crf == 1)
            {
                *pCurCRF++ = 0;
                *pCurCRF++ = 255;
                *pCurCRF++ = 0;
            }
            else
            {
                *pCurCRF++ = 0;
                *pCurCRF++ = 0;
                *pCurCRF++ = 255;
            }
        }
    }
    sprintf(filename, "Result_um_%06d.bmp", frameID);
  //  Rmw_Write24BitImg2BmpFile(pResImg, width, height, filename);
    sprintf(filename, "ResultCRF_um_%06d.bmp", frameID);
  //  Rmw_Write24BitImg2BmpFile(pResCRFImg, width, height, filename);
    Mat ResBefore(height,width,CV_8UC3);
    Mat ResAfter(height,width,CV_8UC3);
    memcpy(ResBefore.data,pResImg,width*height*3);
    memcpy(ResAfter.data,pResCRFImg,width*height*3);
    namedWindow("Before");
    imshow("Before",ResBefore);
    namedWindow("After");
    imshow("After",ResAfter);
    waitKey(0);
    destroyAllWindows();


    if (pResImg)	delete[]	pResImg;
    if (pResCRFImg)	delete[]	pResCRFImg;
    if (pSmooth)	delete[]	pSmooth;
    printf("执行完毕，按任意键结束 ！\n");
    getchar();
    return 0;
}
