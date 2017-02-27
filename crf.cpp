#include "crf.h"


extern vector<vector<double>> TotalCost;	//得分
extern double *pSmooth;
extern vector<int> Result;
extern vector<int> CRFResult;



//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			GCO-v3.0's CRF Model
//
//////////////////////////////////////////////////////////////////////////////////////////////////////




//平滑项计算函数
double SmoothCostWeightofNeighbor(int x1, int y1, int x2, int y2, Mat&img,		//rgb值代表类别属性
    double lamda1, double lamda2,		//权重
    double theta_alpha, double theta_beta, double theta_gama)	//滤波参数
{
    int R1, R2, B1, B2, G1, G2;
    int dist_xy_2, dist_rgb_2;
    double ret = 0;
    if (x1 == x2&&y1 == y2)
        return ret;
    //两个位置（x1,y1）和（x2,y2）及其rgb值像素
    //计算距离空间向量和RGB颜色空间向量的距离
    B1 = img.at<Vec3b>(y1, x1)[0];
    G1 = img.at<Vec3b>(y1, x1)[1];
    R1 = img.at<Vec3b>(y1, x1)[2];
    B2 = img.at<Vec3b>(y2, x2)[0];
    G2 = img.at<Vec3b>(y2, x2)[1];
    R2 = img.at<Vec3b>(y2, x2)[2];

    if(x1==0||y1==0||x2==0||y2==0||x1==1199||y1==199||x2==1199||y2==199)
    {
        return ret;
    }
    else
    {
        dist_xy_2 = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
        dist_rgb_2 = (B1 - B2)*(B1 - B2) + (G1 - G2)*(G1 - G2) + (R1 - R2)*(R1 - R2);
        //返回值
        ret = lamda1*exp(double(-dist_xy_2) / (2 * theta_alpha*theta_alpha))
            + lamda2*exp(double(-dist_xy_2) / 2 * theta_beta*theta_beta + double(-dist_rgb_2) / 2 * theta_gama*theta_gama);
    }

    return ret;
}


//double* GetSmoothWeightofImg(Mat&SrcImg, int width,int height)	//原图
//{
//    int x, y;
//    double *pRes = new double[width*height * 2 * sizeof(double)];
//    double *pCur;
//    for (y = 0, pCur = pRes; y < 200; y++)
//    {
//        for (x = 0; x < 1200; x++)
//        {
//            *pCur++ = SmoothCostWeightofNeighbor(x, y, x + 1, y, SrcImg);		//H
//            *pCur++ = SmoothCostWeightofNeighbor(x, y, x, y + 1, SrcImg);		//V
//        }
//    }
//    return pRes;
//}





//-------------------------------能量函数优化算法----------------------------------------------


void OptimizationGridGraphbySmooth(int width, int height, int num_pixels, int num_labels, double lamda)
{

    // first set up the array for data costs
    double *data = new double[num_pixels*num_labels];
    for (int i = 0; i < num_pixels; i++)
        for (int l = 0; l < num_labels; l++)
            data[i*num_labels + l] = TotalCost[i][l];		//datacost


    // next set up the array for smooth costs
    double *smooth = new double[num_labels*num_labels];
    for (int l1 = 0; l1 < num_labels; l1++)
        for (int l2 = 0; l2 < num_labels; l2++)
            smooth[l1 + l2*num_labels] = l1 == l2 ? 0 : 1;		//采用默认模式 Potts模型,用weight(i,j)衡量


    // next set up spatially varying arrays V and H
    int i;
    double *pCur;
    double *V = new double[num_pixels];
    double *H = new double[num_pixels];
    for (i = 0, pCur = pSmooth; i < num_pixels; i++)
    {
        H[i] = *pCur++ * lamda;
        V[i] = *pCur++ * lamda;
    }

    try
    {
        GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width, height, num_labels);
        gc->setDataCost(data);
        gc->setSmoothCostVH(smooth, V, H);

        printf("\nBefore optimization energy is %d", gc->compute_energy());
        gc->expansion(2);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
        printf("\nAfter optimization energy is %d", gc->compute_energy());

        for (int i = 0; i < num_pixels; i++)
        {
            CRFResult[i] = gc->whatLabel(i);
        }

        delete gc;
    }
    catch (GCException e)
    {
        e.Report();
    }

    delete[] smooth;
    delete[] data;
    delete[] V;
    delete[] H;


}




void Optimization8NeighborsGraphbySmooth(int width, int height, int num_pixels, int num_labels, double lamda, Mat&img)
{

    // first set up the array for data costs
    double *data = new double[num_pixels*num_labels];
    for (int i = 0; i < num_pixels; i++)
        for (int l = 0; l < num_labels; l++)
            data[i*num_labels + l] = TotalCost[i][l];		//datacost


    // next set up the array for smooth costs
    double *smooth = new double[num_labels*num_labels];
    for (int l1 = 0; l1 < num_labels; l1++)
        for (int l2 = 0; l2 < num_labels; l2++)
            smooth[l1 + l2*num_labels] = l1 == l2 ? 0 : lamda;		// neighbor取得不同标签则有惩罚，lamda相当于权重系数，weight相当于smoothcost

    try
    {
        GCoptimizationGeneralGraph *gc = new GCoptimizationGeneralGraph(num_pixels, num_labels);
        gc->setDataCost(data);
        gc->setSmoothCost(smooth);		//采用默认Potts模型

        //每个点一共八个方向邻节点
        // first set up horizontal neighbors
        int x1, y1, x2, y2;		//从ROI坐标到原图坐标的一个转换，方便求权重
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width - 1; x++)
            {
                x1 = x;
                y1 = y;
                x2 = x1 + 1;
                y2 = y1;
                gc->setNeighbors(y*width + x, y*width + x + 1, SmoothCostWeightofNeighbor(x1, y1, x2, y2, img));		//right

            }

        // next set up vertical neighbors
        for (int y = 1; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                x1 = x;
                y1 = y;
                x2 = x1;
                y2 = y1-1;
                gc->setNeighbors(y*width + x, (y - 1)*width + x, SmoothCostWeightofNeighbor(x1, y1, x2, y2, img));		//up
            }

        // third set up 45° neighbors
        for (int y = 1; y < height; y++)
            for (int x = 0; x < width - 1; x++)
            {
                x1 = x;
                y1 = y;
                x2 = x1 + 1;
                y2 = y1 - 1;
                gc->setNeighbors(y*width + x, (y - 1)*width + x + 1, SmoothCostWeightofNeighbor(x1, y1, x2, y2, img));		//45°

            }
        // fourth set up 135° neighbors
        for (int y = 1; y < height; y++)
            for (int x = 1; x < width; x++)
            {
                x1 = x;
                y1 = y;
                x2 = x1 - 1;
                y2 = y1 - 1;
                gc->setNeighbors(y*width + x, (y - 1)*width + x - 1, SmoothCostWeightofNeighbor(x1, y1, x2, y2, img));		//135°
            }

        //设置初始标签
        for (int i = 0; i < num_pixels; i++)
        {
            gc->setLabel(i, Result[i]);			//设置初始标签为分类器的结果
        }
        gc->setLabelOrder(false);		//随机无序开始
        printf("\nBefore optimization energy is %d", gc->compute_energy());
        gc->expansion(3);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
        printf("\nAfter optimization energy is %d", gc->compute_energy());

        for (int i = 0; i < num_pixels; i++)
        {
            CRFResult[i] = gc->whatLabel(i);		//把标记结果存入Result
        }

        delete gc;
    }
    catch (GCException e)
    {
        e.Report();
    }

    delete[] smooth;
    delete[] data;


}
