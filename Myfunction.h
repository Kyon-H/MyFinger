#pragma once
#include "pch.h"
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <iostream>
#include <io.h>
#include <direct.h>
using namespace std;

#define TEMP_DIR "temp\\"//临时文件夹，用于存放指纹入库前的所有中间处理结果
#define DB_DIR "Database\\"//指纹库文件夹
#define DB_INDEX_TXT "Database\\index.txt" //指纹库索引文件

#define STEP_IMG_1 "temp\\Step1_Source.bmp"
#define STEP_TXT_1 "temp\\Step1_Source.txt"

#define STEP_IMG_2 "temp\\Step2_MidFilter.bmp"
#define STEP_TXT_2 "temp\\Step2_MidFilter.txt"

#define STEP_IMG_3 "temp\\Step3_Normalize.bmp"
#define STEP_TXT_3 "temp\\Step3_Normalize.txt"

#define STEP_IMG_4 "temp\\Step4_Direction.bmp"
#define STEP_TXT_4 "temp\\Step4_Direction.txt"

#define STEP_IMG_5 "temp\\Step5_Frequency.bmp"
#define STEP_TXT_5 "temp\\Step5_Frequency.txt"

#define STEP_TXT_6 "temp\\Step6_Mask.txt"
#define STEP_IMG_6 "temp\\Step6_Mask.bmp"

#define STEP_TXT_7 "temp\\Step7_GaborEnhance.txt"
#define STEP_IMG_7 "temp\\Step7_GaborEnhance.bmp"

#define STEP_TXT_8 "temp\\Step8_Binary.txt"
#define STEP_IMG_8 "temp\\Step8_Binary.bmp"

#define STEP_TXT_9 "temp\\Step9_Thinning.txt"
#define STEP_IMG_9 "temp\\Step9_Thinning.bmp"

#define STEP_TXT_10 "temp\\Step10_MinuExtract.txt"
#define STEP_IMG_10 "temp\\Step10_MinuExtract.bmp"

#define STEP_TXT_11 "temp\\Step11_MinuFilter.txt"
#define STEP_IMG_11 "temp\\Step11_MinuFilter.bmp"
#define STEP_IMG_11_MDL "temp\\Step11_MinuFilter_MDL.mdl"

#define STEP_IMG_12 "temp\\Step12_Result.bmp"

#define LOADTMP "temp\\loadtmp.bmp"
#define OUTTMP "temp\\outtmp.bmp"

/// <summary>
/// ASCII多字节字符串转换为Unicode宽字符串
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
wchar_t* ToWideChar(char* str)
{
	int num = MultiByteToWideChar(0, 0, str, -1, NULL, 0);
	wchar_t* wideStr = new wchar_t[num];
	MultiByteToWideChar(0, 0, str, -1, wideStr, num);
	return wideStr;
}
/// <summary>
/// 获取图像参数信息
/// </summary>
/// <param name="fileName"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="depth"></param>
/// <returns></returns>
int ReadBMPImgFilePara(char* fileName, int& width, int& height, int& depth)
{
	//载入图像
	CImage image;
	HRESULT hResult = image.Load(ToWideChar(fileName));
	if (FAILED(hResult) || image.IsNull())//图像损坏或文件不存在等因素均可导致载入失败
	{
		MessageBox(NULL, _T("图像损坏或文件不存在"), _T("提示"), MB_OK);
		return -1;
	}
	//获得图像参数
	width = image.GetWidth();//获得以像素为单位的图像宽度
	height = image.GetHeight();//获得以像素为单位的图像高度
	depth = image.GetBPP();//获得以bit为单位的图像深度（每个像素的位数）

	if (width <= 0 || height <= 0)
	{
		MessageBox(NULL, _T("图像参数错误"), _T("提示"), MB_OK);
		return -1;
	}

	if (depth != 8)//目前系统只支持8位的位图，其它类型的位图暂不支持
	{
		return -2;
	}
	//释放空间变量
	image.Destroy();
	return 0;
}
/// <summary>
/// 从文本文件读取无符号字符型的图像位图数据
/// </summary>
/// <param name="srcFile">源文件</param>
/// <param name="data">无符号字符型数据数组</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <returns></returns>
int ReadDataFromTextFile(char* srcFile, unsigned char* data, int iWidth, int iHeight)
{
	//判断文件类型
	if (strstr(srcFile, ".txt") == NULL)
	{
		MessageBox(NULL, _T("不是txt文件"), _T("警告"), MB_OK);
		return -1;
	}
	if (iWidth <= 0 || iHeight <= 0)
	{
		MessageBox(NULL, _T("图像宽高错误"), _T("警告"), MB_OK);
		return -1;
	}

	//打开源文件
	ifstream fin(srcFile, ios::in);
	if (!fin)//or if(fin.fail())
	{
		return -1;
	}
	//读取数据
	int d = 0;
	for (int i = 0; i < iHeight * iWidth; i++)
	{
		fin >> d;
		data[i] = (unsigned char)d;
	}

	//关闭文件
	fin.close();

	return 0;
}
/// <summary>
/// 将位图数据写入BMP图像文件
/// </summary>
/// <param name="dstFileName">目标文件名</param>
/// <param name="pusImgData">待保存数据数组</param>
/// <returns></returns>
int WriteBMPImgFile(char* dstFileName, unsigned char** pusImgData)
{
	//打开文件
	FILE* fp = fopen(dstFileName, "r+b");
	if (!fp)
	{
		return -1;
	}
	//读取图像参数信息（用于定位数据区以写入数据）
	int imgType, iWidth, iHeight;//图像深度/宽度/高度
	int iStartPos = 0;//位图数据区起始地址
	fseek(fp, 10L, SEEK_SET);
	fread((char*)(&iStartPos), 4, 1, fp);//获取位图数据区起始地址（以字节为单位）
	fseek(fp, 18L, SEEK_SET);
	fread((char*)(&iWidth), 4, 1, fp);//获取图像宽度（以像素为单位）
	fread((char*)(&iHeight), 4, 1, fp);//获取图像高度（以像素为单位）
	unsigned short temp;
	fseek(fp, 28L, SEEK_SET);
	fread((char*)(&temp), 2, 1, fp);//获取图像深度（每个像素的位数，以位为单位）
	imgType = temp;
	if (imgType != 8)//目前只支持8位BMP位图图像
	{
		return -2;
	}
	//向数据区写入数据
	unsigned char* usImgData = *pusImgData;//待保存数据数组地址
	int iWidthInFile = 0;//文件中每行像素宽度/长度（以字节为单位）
	if (iWidth % 4 > 0)
	{
		iWidthInFile = iWidth - iWidth % 4 + 4;
	}
	else
	{
		iWidthInFile = iWidth;
	}
	for (int i = iHeight - 1; i >= 0; i--)//从最后一行到第一行倒序存储
	{
		fseek(fp, iStartPos, SEEK_SET);//定位行首地址
		fwrite((usImgData + i * iWidth), 1, iWidth, fp);//写入一行数据
		iStartPos += iWidthInFile;//调整行首地址
	}

	//关闭文件
	fclose(fp);

	return 0;
}
/// <summary>
/// 从文本文件读取浮点数类型的其他数据
/// </summary>
/// <param name="srcFile">源文件</param>
/// <param name="data">浮点型型数据数组</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <returns></returns>
int ReadDataFromTextFile(char* srcFile, float* data, int iWidth, int iHeight)
{
	//判断文件类型
	if (strstr(srcFile, ".txt") == NULL)
	{
		MessageBox(NULL, _T("不是txt文件"), _T("警告"), MB_OK);
		return -1;
	}
	if (iWidth <= 0 || iHeight <= 0)
	{
		MessageBox(NULL, _T("图像宽高错误"), _T("警告"), MB_OK);
		return -1;
	}

	//打开源文件
	ifstream fin(srcFile, ios::in);
	if (!fin)//or if(fin.fail())
	{
		return -1;
	}
	//读取数据
	for (int i = 0; i < iHeight * iWidth; i++)
	{
		fin >> data[i];
	}

	//关闭文件
	fin.close();

	return 0;
}
/// <summary>
/// 保存数据到BMP图像文件
/// </summary>
/// <param name="srcFile">源文件名</param>
/// <param name="dstFile">目标文件名</param>
/// <param name="data">待保存数据数组</param>
/// <returns></returns>
int SaveDataToImageFile(char* srcFile, char* dstFile, unsigned char* data)
{
	//文件复制
	CopyFile(ToWideChar(srcFile), ToWideChar(dstFile), false);

	int e = 0;
	//写入数据
	e = WriteBMPImgFile(dstFile, &data);
	if (e != 0)
		return -1;

	return 0;
}
/// <summary>
/// 保存数据到图像文件
/// </summary>
/// <param name="srcFile">源文件名</param>
/// <param name="dstFile">目标文件名</param>
/// <param name="data">待保存数据数组</param>
/// <param name="scale">转换比例</param>
/// <returns></returns>
int SaveDataToImageFile(char* srcFile, char* dstFile, float* data, float scale)
{
	int e = 0;
	//读取图像参数
	int iWidth, iHeight, iDepth;//图像宽度/高度/深度
	e = ReadBMPImgFilePara(srcFile, iWidth, iHeight, iDepth);
	if (e != 0)
		return -1;

	//文件复制
	CopyFile(ToWideChar(srcFile), ToWideChar(dstFile), false);

	//数据转换
	unsigned char* tmpData = new unsigned char[iWidth * iHeight];
	for (int i = 0; i<int(iWidth * iHeight); i++)
	{
		tmpData[i] = unsigned char((scale * data[i]));
	}

	//写入数据
	e = WriteBMPImgFile(dstFile, &tmpData);
	if (e != 0)
		return -1;

	//释放内存空间
	delete[]tmpData;

	return 0;
}
/// <summary>
/// 在控件中显示图像
/// </summary>
/// <param name="picCtrl">图像控件</param>
/// <param name="filename">图像文件路径</param>
/// <returns></returns>
int ShowImageInCtrl(CStatic& picCtrl, char* filename)
{
	double cx, cy, dx, dy, k, t;//跟控件的宽和高以及图片宽和高有关的参数
	CRect rect;//定义矩形对象 用于获取图片控件的宽和高
	//载入图像
	CImage image;//为cimage图片类创建一个对象
	HRESULT hResult = image.Load(ToWideChar(filename));
	//获得图像参数
	//int width = image.GetWidth();//图像宽度
	//int height = image.GetHeight();//图像高度
	cx= image.GetWidth();//图像宽度
	cy= image.GetHeight();//图像高度
	k = cy / cx; // 获得图片的宽高比
	//设置显示区域
	picCtrl.GetClientRect(&rect);//获得picture控件所在的矩形区域
	dx = rect.Width();
	dy = rect.Height();//获得控件的宽高比
	t = dy / dx;//获得控件的宽高比
	if (k >= t)
	{

		rect.right = floor(rect.bottom / k);
		rect.left = (dx - rect.right) / 2;
		rect.right = floor(rect.bottom / k) + (dx - rect.right) / 2;
	}
	else
	{
		rect.bottom = floor(k * rect.right);
		rect.top = (dy - rect.bottom) / 2;
		rect.bottom = floor(k * rect.right) + (dy - rect.bottom) / 2;
	}
	//相关的计算为了让图片在绘图区居中按比例显示
	CDC* pDc = picCtrl.GetWindowDC();//获得picture控件的设备环境句柄
	int ModeOld = SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);//设置位图拉伸模式
	//显示图像
	//将图像画到Picture控件表示的矩形区域
	image.StretchBlt(pDc->m_hDC, rect, SRCCOPY);
	SetStretchBltMode(pDc->m_hDC, ModeOld);
	//更新控件显示
	picCtrl.Invalidate(false);
	//释放变量空间
	image.Destroy();
	picCtrl.ReleaseDC(pDc);//释放picture控件的设备环境句柄
	return 0;
}

///*********************************************** Step1 载入图像 ***********************************************/

/// <summary>
/// 从图像文件读取数据
/// </summary>
/// <param name="fileName"></param>
/// <param name="data"></param>
/// <returns></returns>
int ReadBMPImgFileData(char* fileName, unsigned char* data)
{
	//载入图像
	CImage image;
	HRESULT hResult = image.Load(ToWideChar(fileName));
	if (FAILED(hResult) || image.IsNull())//图像损坏或文件不存在等因素均可导致载入失败
	{
		MessageBox(NULL, _T("图像损坏或文件不存在"), _T("提示"), MB_OK);
		return -1;
	}
	//获得图像参数
	int width = image.GetWidth();//获得以像素为单位的图像宽度
	int height = image.GetHeight();//获得以像素为单位的图像高度
	int depth = image.GetBPP();//获得以bit为单位的图像深度（每个像素的位数）

	if (width <= 0 || height <= 0)
	{
		MessageBox(NULL, _T("图像参数错误"), _T("提示"), MB_OK);
		return -1;
	}

	if (depth != 8)//目前系统只支持8位的位图，其它类型的位图暂不支持
	{
		return -2;
	}
	//初始化结果数组
	memset(data, 0, width * height);

	//读取图像数据
	int pitch = image.GetPitch();//每行行首像素的地址偏移量
	unsigned char* pData1 = (unsigned char*)image.GetBits();//图像位图数据区地址
	unsigned char* pData2 = data;
	unsigned char gray = 0;

	unsigned char* pRow1, * pRow2, * pPix1, * pPix2;
	for (int y = 0; y < height; y++)//逐行扫描
	{
		pRow1 = pData1 + pitch * y;
		pRow2 = pData2 + width * y;
		for (int x = 0; x < width; x++)//逐列扫描
		{
			//获得源图像灰度值
			pPix1 = pRow1 + x;
			gray = *pPix1;
			//保存到结果数组
			pPix2 = pRow2 + x;
			*pPix2 = gray;
		}//end for(x)
	}//end for(x)
	//释放空间变量
	image.Destroy();
	return 0;
}
/// <summary>
/// 保存位图数据到文本文件
/// </summary>
/// <param name="dstFile">目标文件</param>
/// <param name="data">无符号字符型数据数组</param>
/// <param name="width">图像宽度</param>
/// <param name="height">图像高度</param>
/// <returns></returns>
int SaveDataToTextFile(char* dstFile, unsigned char* data, int width, int height)
{
	//打开目标文件
	ofstream fout(dstFile, ios::out);//使用覆盖写入方法
	if (!fout)//or if(fout.fail())
	{
		return -1;
	}

	//按指定格式向文件写入数据
	int space = 5;//每个无符号字符型数据均用5个字符位等宽存储
	for (int i = 0; i < height * width; i++)//遍历数组
	{
		fout << setw(space) << int(data[i]);//等宽写入数据
		if (i % width == (width - 1))//行尾
		{
			fout << endl;//换行
		}
	}
	//关闭文件
	fout.close();
	return 0;
}
/// <summary>
/// 保存位图数据到文本文件
/// </summary>
/// <param name="dstFile">目标文件</param>
/// <param name="data">浮点型型数据数组</param>
/// <param name="width">图像宽度</param>
/// <param name="height">图像高度</param>
/// <returns></returns>
int SaveDataToTextFile(char* dstFile, float* data, int width, int height)
{
	//打开目标文件
	ofstream fout(dstFile, ios::out);//使用覆盖写入方法
	if (!fout)//or if(fout.fail())
	{
		return -1;
	}
	//按指定格式向文件写入数据
	int preci = 6;//每个浮点型数据均保留6个小数位
	int space = 16;//每个浮点型数据均用16个字符位等宽存储
	fout.precision(preci);
	for (int i = 0; i < height * width; i++)//遍历数组
	{
		fout << " " << setw(space) << data[i];//等宽写入数据
		if (i % width == (width - 1))//行尾
		{
			fout << endl;//换行
		}
	}
	//关闭文件
	fout.close();
	return 0;
}

///*********************************************** Step 创建和访问指纹库 ***********************************************/

/// <summary>
/// 创建并初始化文件
/// </summary>
/// <param name="filename"></param>
void InitFile(char* filename)
{
	FILE* index = fopen(filename, "a");//若文件不存在则创建，若已存在则清空其内容
	fclose(index);//关闭文件
}
/// <summary>
/// 创建并初始化指纹库
/// </summary>
void InitDatabase()
{
	_mkdir(TEMP_DIR);//创建临时文件夹
	_mkdir(DB_DIR);//创建指纹库文件夹
	InitFile(DB_INDEX_TXT);//创建索引文件
}
/// <summary>
/// 获取指纹库统计信息
/// </summary>
/// <param name="info">返回指纹库统计信息，用于系统界面显示</param>
void GetDatabaseInfo(char* info)
{
	//遍历指纹库
	int pNo = 0;//指纹序号
	char no[MAX_PATH] = { 0 };//登记人编号
	char name[MAX_PATH] = { 0 };//登记人姓名
	char srcFile[MAX_PATH] = { 0 };//指纹图像文件
	char mdlFile[MAX_PATH] = { 0 };//指纹特征文件
	FILE* index = fopen(DB_INDEX_TXT, "r");//打开索引文件
	while (!feof(index))
	{
		fscanf(index, "%d %s %s %s %s\n", &pNo, srcFile, mdlFile, no, name);
	}
	fclose(index);//关闭文件

	//统计指纹库信息
	sprintf(info, "当前指纹库中共有 %d 条记录", pNo);
}

///*********************************************** Step2 中值滤波 ***********************************************/

/// <summary>
/// 使用冒泡排序对数组进行升序排序
/// </summary>
/// <param name="data">数组</param>
/// <param name="dsize">数组长度</param>
void Sort(unsigned char* data, int dsize)
{
	unsigned char temp = 0;
	for (int i = 0; i < dsize; i++)
	{
		for (int j = dsize - 1; j > i; j--)
		{
			if (data[j] < data[j - 1])//升序排序
			{
				temp = data[j];
				data[j] = data[j - 1];
				data[j - 1] = temp;
			}
		}
	}
}
/// <summary>
/// 中值滤波算法
/// </summary>
/// <param name="ucImg">源图数据</param>
/// <param name="ucDsImg">结果图像数据</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <returns></returns>
int MidFilter(unsigned char* ucImg, unsigned char* ucDsImg, int iWidth, int iHeight)
{
	//Step1:结果图像数据初始化
	memset(ucDsImg, 0, iWidth * iHeight);
	//Step2:中心区域滤波（使用3*3邻域）
	unsigned char* pUp, * pDown, * pImg;//用来确定3*3邻域的3个图像数据指针（下文称“邻域指针”）
	unsigned char x[9];//3*3邻域图像数据数组
	for (int i = 1; i < iHeight - 1; i++)//遍历第二行到倒数第二行
	{
		//初始化邻域指针
		pUp = ucImg + (i - 1) * iWidth;
		pImg = ucImg + i * iWidth;
		pDown = ucImg + (i + 1) * iWidth;

		for (int j = 1; j < iWidth - 1; j++)//遍历第2列到倒数第2列
		{
			//移动邻域指针
			pUp++;
			pImg++;
			pDown++;

			//获取3*3邻域数据
			x[0] = *(pUp - 1);
			x[1] = *(pImg - 1);
			x[2] = *(pDown - 1);
			x[3] = *pUp;
			x[4] = *pImg;
			x[5] = *pDown;
			x[6] = *(pUp + 1);
			x[7] = *(pImg + 1);
			x[8] = *(pDown + 1);

			//数组排序
			Sort(x, 9);
			//结果图像数据取邻域中值
			*(ucDsImg + i * iWidth + j) = x[4];
		}
	}
	//Step3:第一行和最后一行滤波（使用2*3邻域）
	//第一行
	pDown = ucImg + iWidth;//邻域指针初始化
	for (int j = 1; j < iWidth - 1; j++)
	{
		//获取2*3邻域数据
		x[0] = *(ucImg + j - 1);
		x[1] = *(ucImg + j);
		x[2] = *(ucImg + j + 1);
		x[3] = *(pDown + j - 1);
		x[4] = *(pDown + j);
		x[5] = *(pDown + j + 1);

		//数组排序
		Sort(x, 6);

		//结果取中值
		*(ucDsImg + j) = x[3];
	}
	//最后一行（倒数第一行）
	pUp = ucImg + iWidth * (iHeight - 2);//邻域指针初始化
	pDown = ucImg + iWidth * (iHeight - 1);//邻域指针初始化
	for (int j = 1; j < iWidth - 1; j++)//最后一行遍历第二列到倒数第二列
	{
		//获取2*3邻域数据
		x[0] = *(pDown + j - 1);
		x[1] = *(pDown + j);
		x[2] = *(pDown + j + 1);
		x[3] = *(pUp + j - 1);
		x[4] = *(pUp + j);
		x[5] = *(pUp + j + 1);

		//数组排序
		Sort(x, 6);

		//结果取中值
		*(ucDsImg + iWidth * (iHeight - 1) + j) = x[3];
	}

	//Step4:4个角点滤波（使用2*2邻域）

	//左上角点
	x[0] = *(ucImg);//获取2*2邻域数据
	x[1] = *(ucImg + 1);
	x[2] = *(ucImg + iWidth);
	x[3] = *(ucImg + iWidth + 1);
	Sort(x, 4);//数组排序
	*(ucDsImg) = x[2];//结果取中值

	//右上角点
	x[0] = *(ucImg + iWidth - 1);//获取2*2邻域数据
	x[1] = *(ucImg + iWidth - 2);
	x[2] = *(ucImg + 2 * iWidth - 1);
	x[3] = *(ucImg + 2 * iWidth - 2);
	Sort(x, 4);//数组排序
	*(ucDsImg + iWidth - 1) = x[2];//结果取中值

	//左下角点
	x[0] = *(ucImg + (iHeight - 1) * iWidth);//获取2*2邻域数据
	x[1] = *(ucImg + (iHeight - 2) * iWidth);
	x[2] = *(ucImg + (iHeight - 1) * iWidth + 1);
	x[3] = *(ucImg + (iHeight - 2) * iWidth + 1);
	Sort(x, 4);//数组排序
	*(ucDsImg + iWidth * (iHeight - 1)) = x[2];//结果取中值

	//右下角点
	x[0] = *(ucImg + (iHeight - 0) * iWidth - 1);//获取2*2邻域数据
	x[1] = *(ucImg + (iHeight - 1) * iWidth - 1);
	x[2] = *(ucImg + (iHeight - 0) * iWidth - 2);
	x[3] = *(ucImg + (iHeight - 1) * iWidth - 2);
	Sort(x, 4);//数组排序
	*(ucDsImg + iWidth * iHeight - 1) = x[2];//结果取中值

	return 0;
}

///*********************************************** Step3 均衡化 ***********************************************/

/// <summary>
/// 均衡化
/// </summary>
/// <param name="ucImg"></param>
/// <param name="ucNormImg"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int HistoNormalize(unsigned char* ucImg, unsigned char* ucNormImg, int iWidth, int iHeight)
{
	//构建源图灰度直方图
	unsigned int Histogram[256];
	memset(Histogram, 0, 256 * sizeof(int));
	for (int i = 0; i < iHeight; i++) {
		for (int j = 0; j < iWidth; j++) {
			Histogram[ucImg[i * iWidth + j]]++;
		}
	}
	//计算源图的灰度均值和方差
	double dMean = 0;
	for (int i = 1; i < 255; i++) {
		dMean += i * Histogram[i];
	}
	dMean = int(dMean / (iWidth * iHeight));
	double dSigma = 0;
	for (int i = 0; i < 255; i++) {
		dSigma += Histogram[i] * (i - dMean) * (i - dMean);
	}
	dSigma /= (iWidth * iHeight);
	dSigma = sqrt(dSigma);
	//对各像素进行均衡化操作
	double dMean0 = 128, dSigma0 = 128;
	double dCoeff = dSigma0 / dSigma;
	for (int i = 0; i < iHeight; i++) {
		for (int j = 0; j < iWidth; j++) {
			double dVal = ucImg[i * iWidth + j];
			dVal = dMean0 + dCoeff * (dVal - dMean0);
			if (dVal < 0) {
				dVal = 0;
			}
			else if (dVal > 255) {
				dVal = 255;
			}
			ucNormImg[i * iWidth + j] = (unsigned char)dVal;
		}
	}
	return 0;
}

///*********************************************** Step4 方向计算 ***********************************************/

/// <summary>
/// 脊线方向计算
/// </summary>
/// <param name="ucImg"></param>
/// <param name="fDirc"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int ImgDirection(unsigned char* ucImg, float* fDirc, int iWidth, int iHeight)
{
	//定义变量
	const int SEMISIZ = 7;	//领域窗口区域半径
	int dx[SEMISIZ * 2 + 1][SEMISIZ * 2 + 1];
	int dy[SEMISIZ * 2 + 1][SEMISIZ * 2 + 1];
	float fx, fy;
	//结果初始化
	memset(fDirc, 0, iWidth * iHeight * sizeof(float));
	//计算每一像素的脊线方向值
	for (int y = SEMISIZ + 1; y < iHeight - SEMISIZ - 1; y++) {
		for (int x = SEMISIZ + 1; x < iWidth - SEMISIZ - 1; x++) {
			//计算以当前像素为中心的领域窗口区内每一像素的梯度
			for (int j = 0; j < SEMISIZ * 2 + 1; j++) {
				for (int i = 0; i < SEMISIZ * 2 + 1; i++) {
					int index1 = (y + j - SEMISIZ) * iWidth + x + i - SEMISIZ;
					int index2 = (y + j - SEMISIZ) * iWidth + x + i - SEMISIZ - 1;
					int index3 = (y + j - SEMISIZ - 1) * iWidth + x + i - SEMISIZ;
					dx[i][j] = int(ucImg[index1] - ucImg[index2]);
					dy[i][j] = int(ucImg[index1] - ucImg[index3]);
				}
			}
			///计算当前像素的脊线方向值
			fx = 0.0;
			fy = 0.0;
			for (int j = 0; j < SEMISIZ * 2 + 1; j++) {
				for (int i = 0; i < SEMISIZ * 2 + 1; i++) {
					fx += 2 * dx[i][j] * dy[i][j];
					fy += (dx[i][j] * dx[i][j] - dy[i][j] * dy[i][j]);
				}
			}
			fDirc[y * iWidth + x] = atan2(fx, fy);
		}
	}
	return 0;
}
/// <summary>
/// 脊线方向低通滤波
/// </summary>
/// <param name="fDirc"></param>
/// <param name="fFitDirc"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int DircLowPass(float* fDirc, float* fFitDirc, int iWidth, int iHeight)
{
	//定义变量
	const int DIR_FILTER_SIZE = 2;
	int blocksize = 2 * DIR_FILTER_SIZE + 1;
	int imgsize = iWidth * iHeight;

	float* filter = new float[blocksize * blocksize];//使用5*5滤波器
	float* phix = new float[imgsize];
	float* phiy = new float[imgsize];
	float* phi2x = new float[imgsize];
	float* phi2y = new float[imgsize];
	//结果初始化
	memset(fFitDirc, 0, sizeof(float) * iWidth * iHeight);
	//设置5*5高斯低通滤波器模板
	float tempSum = 0.0;
	for (int y = 0; y < blocksize; y++) {
		for (int x = 0; x < blocksize; x++) {
			filter[y * blocksize + x] = (float)(blocksize - (abs(DIR_FILTER_SIZE - x) + abs(DIR_FILTER_SIZE - y)));
			tempSum += filter[y * blocksize + x];
		}
	}

	for (int y = 0; y < blocksize; y++) {
		for (int x = 0; x < blocksize; x++) {
			filter[y * blocksize + x] /= tempSum;
		}
	}
	//计算各像素点的方向正弦值和余弦值
	for (int y = 0; y < iHeight; y++) {
		for (int x = 0; x < iWidth; x++) {
			phix[y * iWidth + x] = cos(fDirc[y * iWidth + x]);
			phiy[y * iWidth + x] = sin(fDirc[y * iWidth + x]);
		}
	}
	//对所有像素进行方向低通滤波
	memset(phi2x, 0, sizeof(float) * imgsize);
	memset(phi2y, 0, sizeof(float) * imgsize);
	float nx, ny;
	int val;
	for (int y = 0; y < iHeight - blocksize; y++) {	//逐行遍历，除去边缘区段
		for (int x = 0; x < iWidth - blocksize; x++) {
			//对以当前像素为中心的滤波窗口内的所有像素值进行加权累加
			nx = 0.0;
			ny = 0.0;
			for (int j = 0; j < blocksize; j++) {
				for (int i = 0; i < blocksize; i++) {
					val = (x + i) + (j + y) * iWidth;
					nx += filter[j * blocksize + i] * phix[val];	//方向余弦加权累加
					ny += filter[j * blocksize + i] * phiy[val];	//方向正弦加权累加
				}
			}
			//将结果作为当前像素的新的方向正弦值和余弦值
			val = x + y * iWidth;
			phi2x[val] = nx;
			phi2y[val] = ny;
		}
	}
	//根据加权累加结果，计算各像素的方向滤波结果值
	for (int y = 0; y < iHeight - blocksize; y++) {
		for (int x = 0; x < iWidth - blocksize; x++) {
			val = x + y * iWidth;
			fFitDirc[val] = atan2(phi2y[val], phi2x[val]) * 0.5;
		}
	}
	delete[] phi2y;
	delete[] phi2x;
	delete[] phiy;
	delete[] phix;;
	return 0;
}

///*********************************************** Step5 概率计算 ***********************************************/

/// <summary>
/// 概率计算
/// </summary>
/// <param name="ucImg"></param>
/// <param name="fDirection"></param>
/// <param name="fFrequency"></param>
/// <param name="iWidth"></param>
/// <param name="iHeigth"></param>
/// <returns></returns>
int Frequency(unsigned char* ucImg, float* fDirection, float* fFrequency, int iWidth, int iHeight) {
	//窗口大小
	const int SIZE_L = 32;
	const int SIZE_W = 16;
	const int SIZE_L2 = 16;
	const int SIZE_W2 = 8;
	//正弦波峰值点
	int peak_pos[SIZE_L];
	int peak_cnt;
	float peak_freq;
	float Xsig[SIZE_L];
	//方向
	float dir = 0.0;
	float cosdir = 0.0;
	float sindir = 0.0;
	float maxPeak, minPeak;
	//结果初始化
	float* frequency1 = new float[iWidth * iHeight];
	memset(fFrequency, 0, sizeof(float) * iWidth * iHeight);
	memset(frequency1, 0, sizeof(float) * iWidth * iHeight);

	int x, y;
	int d, k;
	int u, v;
	for (y = SIZE_L2; y < iHeight - SIZE_L2; y++)
	{
		for (x = SIZE_L2; x < iWidth - SIZE_L2; x++) 
		{
			//当前像素的脊线方向
			dir = fDirection[(y + SIZE_W2) * iWidth + (x + SIZE_W2)];
			cosdir = -sin(dir);
			sindir = cos(dir);

			//计算当前像素为中心的L*W邻域窗口的幅值序列
			for (k = 0; k < SIZE_L; k++) {
				Xsig[k] = 0.0;
				for (d = 0; d < SIZE_W; d++) {
					u = (int)(x + (d - SIZE_W2) * cosdir + (k - SIZE_L2) * sindir);
					v = (int)(y + (d - SIZE_W2) * sindir - (k - SIZE_L2) * cosdir);
					//边界点处理
					if (u < 0) {
						u = 0;
					}
					else if (u > iWidth - 1) {
						u = iWidth - 1;
					}
					if (v < 0) {
						v = 0;
					}
					else if (v > iHeight - 1) {
						v = iHeight - 1;
					}
					Xsig[k] += ucImg[u + v * iWidth];
				}
				Xsig[k] /= SIZE_W;
			}

			//确定幅值序列变化范围
			maxPeak = minPeak = Xsig[0];
			for (k = 0; k < SIZE_L; k++) {
				if (minPeak > Xsig[k]) {
					minPeak = Xsig[k];
				}
				if (maxPeak < Xsig[k]) {
					maxPeak = Xsig[k];
				}
			}

			//确定峰值点的位置
			peak_cnt = 0;
			if ((maxPeak - minPeak) > 64) {
				for (k = 0; k < SIZE_L; k++) {
					if ((Xsig[k - 1] < Xsig[k]) && (Xsig[k] >= Xsig[k + 1])) {
						peak_pos[peak_cnt++] = k;
					}
				}
			}

			//计算峰值点平均间距
			peak_freq = 0.0;
			if (peak_cnt >= 2) {
				for (k = 0; k < peak_cnt - 1; k++) {
					peak_freq += (peak_pos[k + 1] - peak_pos[k]);
				}
				peak_freq /= peak_cnt - 1;
			}

			//计算当前像素的频率
			if (peak_freq < 3.0 || peak_freq>25.0) {
				frequency1[x + y * iWidth] = 0.0;
			}
			else {
				frequency1[x + y * iWidth] = 1.0 / peak_freq;
			}
		}
	}
	
	//对频率进行均值滤波
	for (y = SIZE_L2; y < iHeight - SIZE_L2; y++) {
		for (x = SIZE_L2; x < iWidth - SIZE_L2; x++) {
			k = x + y * iWidth;
			peak_freq = 0.0;
			for (v = -2; v <= 2; v++) {
				for (u = -2; u <= 2; u++) {
					peak_freq += frequency1[(x + u) + (y + v) * iWidth];
				}
			}
			fFrequency[k] = peak_freq / 25;
		}
	}
	delete[] frequency1;
	return 0;
}

///*********************************************** Step6 掩码计算 ***********************************************/

/// <summary>
/// 
/// </summary>
/// <param name="ucImg"></param>
/// <param name="fDirection"></param>
/// <param name="fFrequency"></param>
/// <param name="ucMask"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int GetMask(unsigned char* ucImg, float* fDirection, float* fFrequency, unsigned char* ucMask, int iWidth, int iHeight)
{
	//阈值分割
	float freqMin = 1.0 / 25.0;
	float freqMax = 1.0 / 3.0;
	int x, y, k;
	int pos, posout;
	memset(ucMask, 0, iWidth * iHeight);
	for (y = 0; y < iHeight; y++) {
		for (x = 0; x < iWidth; x++) {
			pos = x + y * iWidth;
			posout = x + y * iWidth;
			ucMask[posout] = 0;
			if (fFrequency[pos] >= freqMin && fFrequency[pos] <= freqMax) {
				ucMask[posout] = 255;
			}
		}
	}

	//填充孔洞
	for (k = 0; k < 4; k++) {
		//标记前景点
		for (y = 1; y < iHeight - 1; y++) {
			for (x = 1; x < iWidth - 1; x++) {
				if (ucMask[x + y * iWidth] == 0xFF) {
					ucMask[x - 1 + y * iWidth] |= 0x80;
					ucMask[x + 1 + y * iWidth] |= 0x80;
					ucMask[x + (y - 1) * iWidth] |= 0x80;
					ucMask[x + (y + 1) * iWidth] |= 0x80;
				}
			}
		}
		//判断和设置前景点
		for (y = 1; y < iHeight - 1; y++) {
			for (x = 1; x < iWidth - 1; x++) {
				if (ucMask[x + y * iWidth]) {
					ucMask[x + y * iWidth] = 0xFF;
				}
			}
		}
	}
	//去除边缘和孤立点
	for (k = 0; k < 12; k++) {
		for (y = 1; y < iHeight - 1; y++) {
			for (x = 1; x < iWidth - 1; x++) {
				if (ucMask[x + y * iWidth] == 0x0) {
					ucMask[x - 1 + y * iWidth] &= 0x80;
					ucMask[x + 1 + y * iWidth] &= 0x80;
					ucMask[x + (y - 1) * iWidth] &= 0x80;
					ucMask[x + (y + 1) * iWidth] &= 0x80;
				}
			}
		}
		//判断和设置背景点
		for (y = 1; y < iHeight - 1; y++) {
			for (x = 1; x < iWidth - 1; x++) {
				if (ucMask[x + y * iWidth] != 0xFF) {
					ucMask[x + y * iWidth] = 0x0;
				}
			}
		}
	}

	return 0;
}

///*********************************************** Step7 滤波增强 ***********************************************/

/// <summary>
/// 
/// </summary>
/// <param name="ucImg"></param>
/// <param name="fDirection"></param>
/// <param name="fFrequency"></param>
/// <param name="ucMask"></param>
/// <param name="ucImgEnhanced"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int GaborEnhance(unsigned char* ucImg, float* fDirection, float* fFrequency, unsigned char* ucMask, unsigned char* ucImgEnhanced, int iWidth, int iHeight)
{
	//定义变量
	const float PI = 3.141592654;
	int i, j, u, v;
	int wg2 = 5;
	float sum, f, g;
	float x2, y2;
	float dx2 = 1.0 / (4.0 * 4.0);
	float dy2 = 1.0 / (4.0 * 4.0);
	//结果初始化
	memset(ucImgEnhanced, 0, iWidth * iHeight);
	//Gabor滤波
	for (j = wg2; j < iHeight - wg2; j++) {
		for (i = wg2; i < iWidth - wg2; i++) {
			if (ucMask[i + j * iWidth] == 0) {
				continue;
			}
			//获取当前像素的方向和频率
			g = fDirection[i + j * iWidth];
			f = fFrequency[i + j * iWidth];
			g += PI / 2;
			//对当前像素进行滤波
			sum = 0.0;
			for (v = -wg2; v <= wg2; v++) {
				for (u = -wg2; u <= wg2; u++) {
					x2 = -u * sin(g) + v * cos(g);
					y2 = u * cos(g) + v * sin(g);
					sum += exp(-0.5 * (x2 * x2 * dx2 + y2 * y2 * dy2)) * cos(2 * PI * x2 * f) * ucImg[(i - u) + (j - v) * iWidth];
				}
			}
			//边界处理
			if (sum > 255) {
				sum = 255.0;
			}
			if (sum < 0.0) {
				sum = 0.0;
			}
			//得到当前像素的滤波结果
			ucImgEnhanced[i + j * iWidth] = (unsigned char)sum;
		}
	}
	return 0;
}

///*********************************************** Step8 二值化 ***********************************************/

/// <summary>
/// 图像二值化
/// </summary>
/// <param name="ucImage">源图数据</param>
/// <param name="ucBinImage">结果图数据</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <param name="uThreshold">二值化灰度阈值</param>
/// <returns></returns>
int BinaryImg(unsigned char* ucImage, unsigned char* ucBinImage, int iWidth, int iHeight, unsigned char uThreshold)
{
	//分别定义指向源图数据和结果图数据的数据指针
	unsigned char* pStart = ucImage, * pEnd = ucImage + iWidth * iHeight;
	unsigned char* pDest = ucBinImage;
	//逐一遍历所有像素
	while (pStart < pEnd)
	{
		*pDest = *pStart > uThreshold ? 1 : 0;//二值化
		pStart++;//源图数据指针后移
		pDest++;//结果图数据指针后移
	}
	return 0;
}
/// <summary>
/// 二值图显示转换([0,1]->[0,255])
/// </summary>
/// <param name="ucBinImg">源图数据</param>
/// <param name="ucGrayImg">结果图数据</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <returns></returns>
int BinaryToGray(unsigned char* ucBinImg, unsigned char* ucGrayImg, int iWidth, int iHeight)
{
	//分别定义指向源图数据和结果图数据的数据指针
	unsigned char* pStart = ucBinImg, * pEnd = ucBinImg + iWidth * iHeight;
	unsigned char* pDest = ucGrayImg;
	//逐一遍历所有像素
	while (pStart < pEnd)
	{
		*pDest = (*pStart) > 0 ? 255 : 0;
		pStart++;//源图数据指针后移
		pDest++;//结果图数据指针后移
	}
	return 0;
}

///*********************************************** Step9 细化 ***********************************************/

/// <summary>
/// 图像细化
/// </summary>
/// <param name="ucBinedImg">源图数据</param>
/// <param name="ucThinnedImage">结果图数据</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <param name="iIterativeLimit">最大迭代次数</param>
/// <returns></returns>
int Thinning(unsigned char* ucBinedImg, unsigned char* ucThinnedImage, int iWidth, int iHeight, int iIterativeLimit)
{
	//定义变量
	unsigned char x1, x2, x3, x4, x5, x6, x7, x8, xp;
	unsigned char g1, g2, g3, g4;
	unsigned char b1, b2, b3, b4;
	unsigned char np1, np2, npm;
	unsigned char* pUp, * pDown, * pImg;
	int iDeletedPoints = 0;

	//结果初始化
	memcpy(ucThinnedImage, ucBinedImg, iWidth * iHeight);

	for (int it = 0; it < iIterativeLimit; ++it)//重复执行，一直到无点可删为止
	{
		iDeletedPoints = 0;//初始化本次迭代所删点数
		//本次迭代内第1次遍历（使用条件G1&G2&G3）
		for (int i = 1; i < iHeight - 1; ++i)//逐行遍历
		{
			//初始化邻域指针
			pUp = ucBinedImg + (i - 1) * iWidth;
			pImg = ucBinedImg + i * iWidth;
			pDown = ucBinedImg + (i + 1) * iWidth;

			for (int j = 1; j < iWidth - 1; ++j)//逐列遍历
			{
				//调整邻域指针
				pUp++;
				pImg++;
				pDown++;
				//跳过背景点（像素值为0的点）
				if (!*pImg)
					continue;
				//获取3*3邻域窗口内所有9个像素的灰度值
				x6 = *(pUp - 1);
				x5 = *(pImg - 1);
				x4 = *(pDown - 1);
				x7 = *pUp;
				xp = *pImg;
				x3 = *pDown;
				x8 = *(pUp + 1);
				x1 = *(pImg + 1);
				x2 = *(pDown + 1);
				//判断条件G1
				b1 = !x1 && (x2 == 1 || x3 == 1);
				b2 = !x3 && (x4 == 1 || x5 == 1);
				b3 = !x5 && (x6 == 1 || x7 == 1);
				b4 = !x7 && (x8 == 1 || x1 == 1);
				g1 = (b1 + b2 + b3 + b4) == 1;
				//判断条件G2
				np1 = x1 || x2;
				np1 += x3 || x4;
				np1 += x5 || x6;
				np1 += x7 || x8;
				np2 = x2 || x3;
				np2 += x4 || x5;
				np2 += x6 || x7;
				np2 += x8 || x1;
				npm = np1 > np2 ? np2 : np1;
				g2 = npm >= 2 && npm <= 3;
				//判断条件G3和G4
				g3 = (x1 && (x2 || x3 || !x8)) == 0;
				g4 = (x5 && (x6 || x7 || !x4)) == 0;
				//组合判断（使用条件G1&G2&G3）
				if (g1 && g2 && g3)
				{
					ucThinnedImage[iWidth * i + j] = 0;//删掉当前像素点
					++iDeletedPoints;
				}
			}
		}

		//结果同步
		memcpy(ucBinedImg, ucThinnedImage, iWidth * iHeight);

		//本次迭代内第2次遍历（使用条件G1&G2&G3）
		for (int i = 1; i < iHeight - 1; ++i)//逐行遍历
		{
			//初始化邻域指针
			pUp = ucBinedImg + (i - 1) * iWidth;
			pImg = ucBinedImg + i * iWidth;
			pDown = ucBinedImg + (i + 1) * iWidth;

			for (int j = 1; j < iWidth - 1; ++j)//逐列遍历
			{
				//调整邻域指针
				pUp++;
				pImg++;
				pDown++;
				//跳过背景点（像素值为0的点）
				if (!*pImg)
					continue;
				//获取3*3邻域窗口内所有9个像素的灰度值
				x6 = *(pUp - 1);
				x5 = *(pImg - 1);
				x4 = *(pDown - 1);
				x7 = *pUp;
				xp = *pImg;
				x3 = *pDown;
				x8 = *(pUp + 1);
				x1 = *(pImg + 1);
				x2 = *(pDown + 1);
				//判断条件G1
				b1 = !x1 && (x2 == 1 || x3 == 1);
				b2 = !x3 && (x4 == 1 || x5 == 1);
				b3 = !x5 && (x6 == 1 || x7 == 1);
				b4 = !x7 && (x8 == 1 || x1 == 1);
				g1 = (b1 + b2 + b3 + b4) == 1;
				//判断条件G2
				np1 = x1 || x2;
				np1 += x3 || x4;
				np1 += x5 || x6;
				np1 += x7 || x8;
				np2 = x2 || x3;
				np2 += x4 || x5;
				np2 += x6 || x7;
				np2 += x8 || x1;

				npm = np1 > np2 ? np2 : np1;
				g2 = npm >= 2 && npm <= 3;
				//判断条件G3和G4
				g3 = (x1 && (x2 || x3 || !x8)) == 0;
				g4 = (x5 && (x6 || x7 || !x4)) == 0;
				//组合判断（使用条件G1&G2&G4）
				if (g1 && g2 && g4)
				{
					ucThinnedImage[iWidth * i + j] = 0;//删掉当前像素点
					++iDeletedPoints;
				}
			}
		}

		//结果同步
		memcpy(ucBinedImg, ucThinnedImage, iWidth * iHeight);

		//如果本次迭代已无点可删，则停止迭代
		if (iDeletedPoints == 0)
			break;
	}
	//清除边缘区域
	for (int i = 0; i < iHeight; ++i)
	{
		for (int j = 0; j < iWidth; ++j)
		{
			if (i < 16)//上边缘
				ucThinnedImage[i * iWidth + j] = 0;
			else if (i >= iHeight - 16)//下边缘
				ucThinnedImage[i * iWidth + j] = 0;
			else if (j < 16)//左边缘
				ucThinnedImage[i * iWidth + j] = 0;
			else if (j >= iWidth - 16)//右边缘
				ucThinnedImage[i * iWidth + j] = 0;
		}
	}
	return 0;
}

///*********************************************** Step10 特征提取 ***********************************************/

/// <summary>
/// 指纹特征提取
/// </summary>
/// <param name="ucThinImg">源图数据</param>
/// <param name="ucMinuImg">结果图数据</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <returns></returns>
int Extract(unsigned char* ucThinImg, unsigned char* ucMinuImg, int iWidth, int iHeight)
{
	//定义变量
	unsigned char* pDest = ucMinuImg;//结果图数据指针
	unsigned char* pUp, * pDown, * pImg;//源图邻域指针
	unsigned char x1, x2, x3, x4, x5, x6, x7, x8;//八邻点
	unsigned char nc;//八邻点中黑点数量
	int iMinuCount = 0;//特征点数量

	//结果初始化（全设为0，表示都是非特征点）
	memset(pDest, 0, iWidth * iHeight);

	//遍历源图以提取指纹特征
	for (int i = 1; i < iHeight - 1; i++)//逐行遍历
	{
		//初始化邻域指针
		pUp = ucThinImg + (i - 1) * iWidth;
		pImg = ucThinImg + i * iWidth;
		pDown = ucThinImg + (i + 1) * iWidth;

		for (int j = 1; j < iWidth - 1; j++)//逐列遍历
		{
			//调整邻域指针
			pUp++;
			pImg++;
			pDown++;

			//跳过背景点（背景点像素值为0）
			if (!*pImg)
			{
				continue;
			}

			//获取3*3邻域窗口内所有8个邻点像素的灰度值
			x6 = *(pUp - 1);
			x5 = *(pImg - 1);
			x4 = *(pDown - 1);

			x7 = *pUp;
			x3 = *pDown;

			x8 = *(pUp + 1);
			x1 = *(pImg + 1);
			x2 = *(pDown + 1);

			//统计八邻点
			//nc = x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8;
			nc = abs(x2 - x1) + abs(x3 - x2) + abs(x4 - x3) + abs(x5 - x4) + abs(x6 - x5) + abs(x7 - x6) + abs(x8 - x7) + abs(x1 - x8);
			//特征点判断
			if (nc == 2)//端点
			{
				pDest[i * iWidth + j] = 1;//结果图中对应像素点设为1（表示端点）
				++iMinuCount;//特征点数量加一
			}
			else if (nc == 6)//分叉点
			{
				pDest[i * iWidth + j] = 3;//结果图中对应像素点设为3（表示分叉点）
				++iMinuCount;//特征点数量加一
			}
		}
	}
	return iMinuCount;//返回特征点数量
}

///*********************************************** Step11 特征过滤 ***********************************************/

/// <summary>
/// 相邻特征点结构
/// </summary>
struct NEIGHBOR
{
	int x;					//横坐标（列坐标）
	int y;					//纵坐标（行坐标）
	int type;				//特征点类型（1-端点，3-分叉点）
	float Theta;			//两点连线角度（弧度）
	float Theta2Ridge;		//两点脊线方向夹角（弧度）
	float ThetaThisNibor;	//相邻特征点的脊线方向（弧度）
	int distance;			//两点距离（像素数量）
};
/// <summary>
/// 特征点结构
/// </summary>
struct MINUTIAE
{
	int x;					//横坐标（列坐标）
	int y;					//纵坐标（行坐标）
	int type;				//特征点类型（1-端点，3-分叉点）
	float theta;			//该点处的脊线方向(弧度)
	NEIGHBOR* neibors;		//相邻特征点序列
};
/// <summary>
/// 去除边缘特征点
/// </summary>
/// <param name="minutiaes">特征点数组</param>
/// <param name="count">特征点数量</param>
/// <param name="ucImg">源图数据</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <returns></returns>
int CutEdge(MINUTIAE* minutiaes, int count, unsigned char* ucImg, int iWidth, int iHeight)
{
	//定义变量
	int minuCount = count;
	int x, y, type;
	bool del;

	//初始化标记数组
	int* pFlag = new int[minuCount];//标记数组（0-保留，1-删除）
	memset(pFlag, 0, sizeof(int) * minuCount);//初始化（全0全保留）

	//遍历所有特征点
	for (int i = 0; i < minuCount; ++i)
	{
		//获取当前特征点信息
		y = minutiaes[i].y - 1;
		x = minutiaes[i].x - 1;
		type = minutiaes[i].type;

		//将当前特征点的删除标记初始化为true
		del = true;

		//将当前特征点的位置判断其是否边缘特征点
		if (x < iWidth / 2)//如果位于图像左半图
		{
			if (abs(iWidth / 2 - x) > abs(iHeight / 2 - y))//如果位于左半图左侧
			{
				//在特征图中查找当前特征点同一行左侧是否还有其他特征点
				while (--x >= 0)//逐一左移查找
				{
					//如果在左侧存在其他特征点，则说明当前特征点不是边缘特征点，就无需删除
					if (ucImg[x + y * iWidth] > 0)
					{
						del = false;
						break;//停止当前特征点的左移查找
					}
				}
			}
			else//如果位于左半图右侧
			{
				if (y > iHeight / 2)//如果位于左半图右下侧
				{
					//在特征图中查找当前特征点同一列下侧是否还有其他特征点
					while (++y < iHeight)//逐一下移查找
					{
						//如果在下侧存在其他特征点，则说明当前特征点不是边缘特征点，就无需删除
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
				else//如果位于左半图右上侧
				{
					//在特征图中查找当前特征点同一列上侧是否还有其他特征点
					while (--y >= 0)//逐一上移查找
					{
						//如果在上侧存在其他特征点，则说明当前特征点不是边缘特征点，就无需删除
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
			}
		}
		else//如果位于图像右半图
		{
			if (abs(iWidth / 2 - x) > abs(iHeight / 2 - y))//如果位于右半图右侧
			{
				// 在特征图中查找当前特征点同一行右侧是否还有其他特征点
				while (++x < iWidth)//逐一右移查找
				{
					//如果在右侧存在其他特征点，则说明当前特征点不是边缘特征点，就无需删除
					if (ucImg[x + y * iWidth] > 0)
					{
						del = false;
						break;
					}
				}
			}
			else//如果位于右半图左侧
			{
				if (y > iHeight / 2)//如果位于右半图左下侧
				{
					//在特征图中查找当前特征点同一列下侧是否还有其他特征点
					while (++y < iHeight)
					{
						//如果在下侧存在其他特征点，则说明当前特征点不是边缘特征点，就无需删除
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
				else//如果位于右半图左上侧
				{
					//在特征图中查找当前特征点同一列上侧是否还有其他特征点
					while (--y >= 0)
					{
						//如果在上侧存在其他特征点，则说明当前特征点不是边缘特征点，就无需删除
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
			}
		}

		//如果当前特征点是边缘特征点，则删除
		if (del)
		{
			pFlag[i] = 1;
			continue;
		}
	}

	//重组特征点结构数组(在当前结构数组中将所有有效特征点前移)
	int newCount = 0;//有效特征点数量（同时是重组后有效特征点数组下标）
	for (int i = 0; i < minuCount; ++i)
	{
		if (pFlag[i] == 0)
		{
			memcpy(&minutiaes[newCount], &minutiaes[i], sizeof(MINUTIAE));//特征点结构整体复制
			newCount++;//有效特征点下标后移
		}
	}

	delete[]pFlag;
	pFlag = NULL;
	//返回有效特征点数量
	return newCount;
}
/// <summary>
/// 保存特征模板文件
/// </summary>
/// <param name="minutiaes">特征点数组</param>
/// <param name="count">特征点数量</param>
/// <param name="fileName">模板文件名</param>
/// <returns></returns>
int SaveMinutiae(MINUTIAE* minutiaes, int count, char* fileName)
{
	//打开文件（二进制写入方式）
	FILE* fp = fopen(fileName, "wb");
	if (!fp)
	{
		return -1;
	}

	//将所有特征点的结构体数据整体写入文件
	const static int TemplateFlag = 0x3571027f;
	fwrite(&TemplateFlag, sizeof(int), 1, fp);//写入特征模板文件标记
	fwrite(&count, sizeof(int), 1, fp);//写入特征点数量

	for (int i = 0; i < count; i++)
	{
		fwrite(&(minutiaes[i]), sizeof(MINUTIAE), 1, fp);//将特征点的结构整体写入
	}
	//关闭文件
	fclose(fp);
	return 0;
}
/// <summary>
/// 特征过滤
/// </summary>
/// <param name="minuData">特征图数据</param>
/// <param name="thinData">细化图数据</param>
/// <param name="minutiaes">特征点数组</param>
/// <param name="minuCount">特征点数量</param>
/// <param name="iWidth">图像宽度</param>
/// <param name="iHeight">图像高度</param>
/// <returns></returns>
int MinuFilter(unsigned char* minuData, unsigned char* thinData, MINUTIAE* minutiaes, int& minuCount, int iWidth, int iHeight)
{
	//第1步：计算细化图中各点方向
	float* dir = new float[iWidth * iHeight];
	memset(dir, 0, iWidth * iHeight * sizeof(float));

	//ImgDirection(thinData, dir, iWidth, iHeight);//计算脊线方向

	//第2步：从特征图中提取特征点数据
	unsigned char* pImg;
	unsigned char val;
	int temp = 0;
	for (int i = 1; i < iHeight - 1; ++i)
	{
		pImg = minuData + i * iWidth;
		for (int j = 1; j < iWidth - 1; ++j)
		{
			//获取特征图数据
			++pImg;//特征图指针后移
			val = *pImg;//特征图像素值（0-非特征点，1-端点，3-分叉点）
			//提取特征点数据
			if (val > 0)
			{
				minutiaes[temp].x = j + 1;//横坐标（从1开始）
				minutiaes[temp].y = i + 1;//纵坐标（从1开始）
				minutiaes[temp].theta = dir[i * iWidth + j];//脊线方向
				minutiaes[temp].type = int(val);//特征点类型
				++temp;//特征点数组指针后移
			}
		}
	}
	delete[]dir;

	//第3步：去除边缘特征点
	minuCount = CutEdge(minutiaes, minuCount, thinData, iWidth, iHeight);

	//第4步：去除毛刺/小孔/间断等伪特征点
	//初始化标记数组
	int* pFlag = new int[minuCount];//0-保留，1-删除
	memset(pFlag, 0, sizeof(int) * minuCount);

	//遍历所有特征点
	int x1, x2, y1, y2, type1, type2;
	//特征点1遍历
	for (int i = 0; i < minuCount; ++i)
	{
		//获取特征点1的信息
		x1 = minutiaes[i].x;
		y1 = minutiaes[i].y;
		type1 = minutiaes[i].type;
		//特征点2遍历
		for (int j = i + 1; j < minuCount; ++j)
		{
			//跳过已删特征点
			if (pFlag[j] == 1)
				continue;

			//获取特征点2的信息
			x2 = minutiaes[j].x;
			y2 = minutiaes[j].y;
			type2 = minutiaes[j].type;

			//计算两点间距
			int r = (int)sqrt(float((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2)));

			//删除间距过小的特征点
			if (r <= 4)//如果间距不大于4则认为间距过小
			{
				if (type1 == type2)//如果两点类型相同
				{
					if (type1 == 1)//都是端点，认为是“短线或纹线间断”
						pFlag[i] = pFlag[j] = 1;//同时删掉两点
					else//都是分叉点，认为是“小孔”
						pFlag[j] = 1;//只删掉2
				}
				else if (type1 == 1)//1是端点 2是分叉点 则1为“毛刺”
					pFlag[i] = 1;//只删掉1
				else//1是分叉点 2是端点 则2为“毛刺”
					pFlag[j] = 1;//只删掉2
			}
		}
	}

	//重组特征点结构数组(在当前结构数组中将所有有效特征点前移)
	int newCount = 0;//有效特征点数量（同时是重组后有效特征点数组下标）
	for (int i = 0; i < minuCount; ++i)
	{
		if (pFlag[i] == 0)
		{
			memcpy(&minutiaes[newCount], &minutiaes[i], sizeof(MINUTIAE));//特征点结构整体复制
			newCount++;//有效特征点下标后移
		}
	}
	delete[]pFlag;
	minuCount = newCount;

	//返回结果
	return 0;
}

///*********************************************** Step12 特征入库 ***********************************************/

/// <summary>
/// 获得新指纹编号
/// </summary>
/// <returns></returns>
int GetNewIndexDB()
{
	//从index文件中读取最后一条记录的编号
	int sNo = 0;
	char no[MAX_PATH] = { 0 }, name[MAX_PATH] = { 0 }, srcFile[MAX_PATH] = { 0 }, mdlFile[MAX_PATH] = { 0 };

	FILE* index = fopen(DB_INDEX_TXT, "r");
	while (!feof(index))
	{
		fscanf(index, "%d %s %s %s %s\n", &sNo, srcFile, mdlFile, no, name);
	}
	fclose(index);

	//生成新编号
	sNo = sNo + 1;
	//返回新编号
	return sNo;
}
/// <summary>
/// 宽字符串转换为多字节字符串
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
char* ToChar(wchar_t* str)
{
	int num = WideCharToMultiByte(0, 0, str, -1, NULL, 0, NULL, false);
	char* chStr = new char[num];
	WideCharToMultiByte(0, 0, str, -1, chStr, num, NULL, false);

	return chStr;

}

///*********************************************** Step13 特征匹配 ***********************************************/

/// <summary>
/// 计算线段倾斜度
/// </summary>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="x2"></param>
/// <param name="y2"></param>
/// <returns></returns>
float Angle2Points(int x1, int y1, int x2, int y2) {
	const float PI = 3.141592654;
	float diffY, diffX;
	float theta = 0.0;

	diffY = y2 - y1;
	diffX = x2 - x1;

	if (diffY < 0 && diffX>0) {
		theta = atan2(-1 * diffY, diffX);
	}
	else if (diffY < 0 && diffX < 0) {
		theta = PI - atan2(-1 * diffY, -1 * diffX);
	}
	else if (diffY > 0 && diffX < 0) {
		theta = atan2(diffY, -1 * diffX);
	}
	else if (diffY > 0 && diffX > 0) {
		theta = PI - atan2(diffY, diffX);
	}
	else if (diffX == 0) {
		theta = PI / 2;
	}
	else {
		theta = 0.0;
	}
	return theta;
}
/// <summary>
/// 构建特征点相邻关系
/// </summary>
/// <param name="minutiae"></param>
/// <param name="minuCount"></param>
/// <returns></returns>
int BuildNabors(MINUTIAE* minutiae, int minuCount) {
	//定义变量
	const int MAX_NEIGHBOR_EACH = 10;//每个特征点最多保存10个相邻特征点
	int x1, x2, y1, y2;
	int* pFlag = new int[minuCount];//相邻标记数组（标记值：0-不相邻，1-相邻）

	//遍历特征点数组
	for (int i = 0; i < minuCount; i++) {
		//获取当前特征点信息
		x1 = minutiae[i].x;
		y1 = minutiae[i].y;

		//初始化当前特征点的相邻标记数组
		memset(pFlag, 0, sizeof(int) * minuCount);//初始化为全0（不相邻）
		pFlag[i] = 1;//将自身标记为“相邻”

		 //为当前特征点创建并初始化相邻特征点结构数组
		minutiae[i].neibors = new NEIGHBOR[MAX_NEIGHBOR_EACH];//创建相邻特征点结构数组
		if (minutiae[i].neibors == NULL) {
			return -1;
		}
		memset(minutiae[i].neibors, 0, sizeof(NEIGHBOR) * MAX_NEIGHBOR_EACH);//初始化数组

		 //查找和保存10个相邻特征点
		for (int neighborNo = 0; neighborNo < MAX_NEIGHBOR_EACH; neighborNo++) {//重复10次
			//初始化最小间隔和对应特征点下标
			int minDistance = 1000;//最小间隔
			int minNo = 0;//最小间距对应的特征点下标

			 //查找相邻特征点之外的最近的不相邻特征点
			for (int j = 0; j < minuCount; j++) { //每次都遍历所有特征点
				//跳过已找到的相邻特征点
				if (pFlag[j] == 1) {//(标记值：0-不相邻，1-相邻)
					continue;
				}

				//获取特征点2的信息
				x2 = minutiae[j].x;
				y2 = minutiae[j].y;

				//计算两点间距
				int r = (int)sqrt(float((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2)));

				//查找最小间距
				if (r < minDistance) {
					minNo = j;
					minDistance = r;

				}
			}

			//保存查找结果
			pFlag[minNo] = 1;
			minutiae[i].neibors[neighborNo].x = minutiae[minNo].x;
			minutiae[i].neibors[neighborNo].y = minutiae[minNo].y;
			minutiae[i].neibors[neighborNo].type = minutiae[minNo].type;
			minutiae[i].neibors[neighborNo].Theta = Angle2Points(minutiae[minNo].x, minutiae[minNo].y, x1, y1);
			minutiae[i].neibors[neighborNo].Theta2Ridge = minutiae[minNo].theta - minutiae[i].theta;
			minutiae[i].neibors[neighborNo].ThetaThisNibor = minutiae[minNo].theta;
			minutiae[i].neibors[neighborNo].distance = minDistance;
		}
	}
	delete[] pFlag;
	return 0;
}
/// <summary>
/// 特征匹配相似度算法
/// </summary>
/// <param name="minutiae1"></param>
/// <param name="count1"></param>
/// <param name="minutiae2"></param>
/// <param name="count2"></param>
/// <returns></returns>
float MinuSimilarity(MINUTIAE* minutiae1, int count1, MINUTIAE* minutiae2, int count2) {

	const int MAX_SIMILAR_PAIR = 200;//最多保存200对配对相似特征点
	const int MAX_NEIGHBOR_EACH = 10;//每个特征点最多保存10个相邻特征点

	BuildNabors(minutiae1, count1);
	BuildNabors(minutiae2, count2);

	int similarPair[MAX_SIMILAR_PAIR][2];
	memset(similarPair, 0, 100 * 2 * sizeof(int));

	MINUTIAE* baseMinutiae;
	MINUTIAE* refMinutiae;

	int baseAccount, refAccount;
	if (count1 < count2) {
		baseMinutiae = minutiae1;
		baseAccount = count1;
		refMinutiae = minutiae2;
		refAccount = count2;
	}
	else {
		baseMinutiae = minutiae2;
		baseAccount = count2;
		refMinutiae = minutiae1;
		refAccount = count1;
	}
	NEIGHBOR* baseNeighbors = NULL;
	NEIGHBOR* refNeighbors = NULL;
	int similarMinutiae = 0;
	float baseTheta, refTheta;

	for (int i = 0; i < baseAccount; i++) {
		baseNeighbors = baseMinutiae[i].neibors;
		baseTheta = baseMinutiae[i].theta;

		int refSimilarNo = 0;
		int maxSimilarNeibors = 0;
		for (int j = 0; j < refAccount; j++) {
			if (refMinutiae[j].type != baseMinutiae[i].type) {
				continue;
			}
			refNeighbors = refMinutiae[j].neibors;
			refTheta = refMinutiae[j].theta;

			//统计相似相邻点数量
			int thisSimilarNeigbors = 0;
			for (int m = 0; m < MAX_NEIGHBOR_EACH; m++) {
				for (int n = 0; n < MAX_NEIGHBOR_EACH; n++) {
					//跳过类型不同的相邻点
					if (baseNeighbors[m].type != refNeighbors[n].type) {
						continue;
					}
					int dist = abs(int(baseNeighbors[m].distance - refNeighbors[n].distance));
					float theta1 = abs(float((baseNeighbors[m].Theta - baseTheta) - (refNeighbors[n].Theta - refTheta)));
					float theta2 = abs(float(baseNeighbors[m].Theta2Ridge - refNeighbors[n].Theta2Ridge));
					float theta3 = abs(float((baseNeighbors[m].Theta - baseNeighbors[m].ThetaThisNibor) - (refNeighbors[n].Theta - refNeighbors[n].ThetaThisNibor)));

					if (dist < 3.9 && theta1 < 0.148f && theta2 < 0.147f && theta3 < 0.147f) {
						++thisSimilarNeigbors;
						break;
					}
				}
			}
			//如果5对以上相邻点相似，则认为当前基准点与当前参考点相似，保存匹配结果。
			if ((thisSimilarNeigbors >= MAX_NEIGHBOR_EACH * 5 / 10) && (similarMinutiae < MAX_SIMILAR_PAIR)) {
				similarPair[similarMinutiae][0] = i;
				similarPair[similarMinutiae][1] = refSimilarNo;
				++similarMinutiae;
			}
		}
	}
	//计算特征匹配相似度
	float similarity = similarMinutiae / 200.0f;
	similarity = similarMinutiae < 80 ? 0.0f : similarity;
	similarity = similarMinutiae > 200 ? 1.0f : similarity;

	return similarity;
}
/// <summary>
/// 读取特征模板文件
/// </summary>
/// <param name="fileName">特征模板文件名</param>
/// <param name="minutiae">特征点数组</param>
/// <returns></returns>
int ReadMinutiae(char* fileName, MINUTIAE** minutiae)
{
	//打开文件（二进制读取方式）
	FILE* fp = fopen(fileName, "rb");
	if (!fp) {
		return -1;
	}
	//逐一整体读取所有特征点的结构体数据
	const static int TemplateFileFlag = 0x3571027f;
	int flag;
	fread(&flag, sizeof(int), 1, fp);//读取特征模板文件标记
	if (flag != TemplateFileFlag)
	{
		return -2;
	}

	int account;
	fread(&account, sizeof(int), 1, fp);//读取特征点数量
	*minutiae = new MINUTIAE[account];//创建特征点结构数组
	if (*minutiae == NULL)
	{
		return -3;
	}

	for (int i = 0; i < account; i++) {
		fread(&((*minutiae)[i]), sizeof(MINUTIAE), 1, fp);//整体读取特征点结构
	}

	//关闭文件
	fclose(fp);
	return account;
}

///*********************************************** Step14 指纹识别 ***********************************************/

/// <summary>
/// 指纹库预检（判断指纹库是否为空）
/// </summary>
/// <returns></returns>
bool EmptyDB()
{
	//打开文件
	char indexFile[MAX_PATH] = { DB_INDEX_TXT };//指纹库引索文件
	FILE* pFile = fopen(indexFile, "r");
	if (!pFile)//若引索文件不存在或存在异常，则指纹库为空
	{
		return true;
	}

	//判断指纹库是否为空（引索文件内容是否为空）
	int size = _filelength(_fileno(pFile));//文件长度（以字节为单位）

	bool bEmpty = (size < 1) ? true : false;//若索引文件内容为空，则指纹库为空

	//关闭文件
	fclose(pFile);
	//返回判断结果
	return bEmpty;
}

///*********************************************** Step15 指纹登记集成 ***********************************************/

/// <summary>
/// 指纹登记过程集成
/// </summary>
/// <param name="regName">登记人姓名</param>
/// <param name="srcImgFile">原图文件名</param>
/// <param name="outImgFile">特征结果图文件名</param>
/// <param name="info">返回操作或失败提示信息</param>
/// <param name="bSaveImg">是否保存所有中间结果图像</param>
/// <returns></returns>
int Enroll(char*regNo, char* regName, char* srcImgFile, char* outImgFile, char* info, bool bSaveImg)
{
	//第0步：设置中间结果文件名
	//中间结果数据文件名
	char stepTxtFile1[MAX_PATH] = { STEP_TXT_1 };
	char stepTxtFile2[MAX_PATH] = { STEP_TXT_2 };
	char stepTxtFile3[MAX_PATH] = { STEP_TXT_3 };
	char stepTxtFile4[MAX_PATH] = { STEP_TXT_4 };
	char stepTxtFile5[MAX_PATH] = { STEP_TXT_5 };
	char stepTxtFile6[MAX_PATH] = { STEP_TXT_6 };
	char stepTxtFile7[MAX_PATH] = { STEP_TXT_7 };
	char stepTxtFile8[MAX_PATH] = { STEP_TXT_8 };
	char stepTxtFile9[MAX_PATH] = { STEP_TXT_9 };
	char stepTxtFile10[MAX_PATH] = { STEP_TXT_10 };
	char stepTxtFile11[MAX_PATH] = { STEP_TXT_11 };

	//中间结果图像文件名
	char stepImgFile1[MAX_PATH] = { STEP_IMG_1 };
	char stepImgFile2[MAX_PATH] = { STEP_IMG_2 };
	char stepImgFile3[MAX_PATH] = { STEP_IMG_3 };
	char stepImgFile4[MAX_PATH] = { STEP_IMG_4 };
	char stepImgFile5[MAX_PATH] = { STEP_IMG_5 };
	char stepImgFile6[MAX_PATH] = { STEP_IMG_6 };
	char stepImgFile7[MAX_PATH] = { STEP_IMG_7 };
	char stepImgFile8[MAX_PATH] = { STEP_IMG_8 };
	char stepImgFile9[MAX_PATH] = { STEP_IMG_9 };
	char stepImgFile10[MAX_PATH] = { STEP_IMG_10 };
	char stepImgFile11[MAX_PATH] = { STEP_IMG_11 };
	char stepImgFile12[MAX_PATH] = { STEP_IMG_12 };

	//指纹特征模板名
	char stepMdlFile11[MAX_PATH] = { STEP_IMG_11_MDL };

	//第1步：图像载入，读取源图数据和参数信息
	int iWidth, iHeight, iDepth;
	int flag=ReadBMPImgFilePara(srcImgFile, iWidth, iHeight, iDepth);
	if (flag != 0) {
		sprintf(info, "图像加载失败.");
		return -1;
	}
	unsigned char* image1 = new unsigned char[iWidth * iHeight];
	unsigned char* image2 = new unsigned char[iWidth * iHeight];
	flag=ReadBMPImgFileData(srcImgFile, image1);//读取源图数据
	if (flag != 0)
	{
		sprintf(info, "图像数据读取失败.");
		return -1;
	}
	memset(image2, 0, iWidth * iHeight);//结果图初始化
	if (bSaveImg)
	{
		CopyFile(ToWideChar(srcImgFile), ToWideChar(stepImgFile1), false);
		SaveDataToTextFile(stepTxtFile1, image1, iWidth, iHeight);

	}
	//第2步：中值滤波
	MidFilter(image1, image2, iWidth, iHeight);//中值滤波
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile2, image2);
		SaveDataToTextFile(stepTxtFile2, image2, iWidth, iHeight);
	}
	//第3步：直方图均衡化
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第2步滤波结果为源图数据
	HistoNormalize(image1, image2, iWidth, iHeight);//直方图均衡化
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile3, image2);
		SaveDataToTextFile(stepTxtFile3, image2, iWidth, iHeight);
	}
	//第4步：方向计算
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第3步均衡化结果为源图数据
	float* tmpDirection = new float[iWidth * iHeight];
	float* direction = new float[iWidth * iHeight];
	ImgDirection(image1, tmpDirection, iWidth, iHeight);//方向计算
	DircLowPass(tmpDirection, direction, iWidth, iHeight);//方向低通滤波
	if (bSaveImg)
	{
		const int DIRECTION_SCALE = 100;//方向结果转换比例（仅用于结果显示）
		SaveDataToImageFile(srcImgFile, stepImgFile4, direction, DIRECTION_SCALE);
		SaveDataToTextFile(stepTxtFile4, direction, iWidth, iHeight);
	}
	delete[] tmpDirection;
	//第5步：频率计算
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第3步均衡化结果为源图数据
	float* frequency = new float[iWidth * iHeight];
	Frequency(image1, direction, frequency, iWidth, iHeight);//频率计算
	if (bSaveImg)
	{
		const int FREQUENCY_SCALE = 1000;//频率结果转换比例（仅用于结果显示）
		SaveDataToImageFile(srcImgFile, stepImgFile5, frequency, FREQUENCY_SCALE);
		SaveDataToTextFile(stepTxtFile5, frequency, iWidth, iHeight);
	}
	//第6步：掩码计算
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第3步均衡化结果为源图数据
	unsigned char* mask = new unsigned char[iWidth * iHeight];
	GetMask(image1, direction, frequency, mask, iWidth, iHeight); //掩码计算
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile6, mask);
		SaveDataToTextFile(stepTxtFile6, mask, iWidth, iHeight);
	}
	//第7步：Gabor滤波增强
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第3步均衡化结果为源图数据
	GaborEnhance(image1, direction, frequency, mask, image2, iWidth, iHeight); //增强
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile7, image2);
		SaveDataToTextFile(stepTxtFile7, image2, iWidth, iHeight);
	}
	delete[]direction;
	delete[]frequency;
	delete[]mask;
	//第8步：二值化
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第7步Gabor滤波结果为源图数据
	BinaryImg(image1, image2, iWidth, iHeight, 128); //二值化
	if (bSaveImg)
	{
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile8, image1);
		SaveDataToTextFile(stepTxtFile8, image2, iWidth, iHeight);
	}
	//第9步：细化
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第8步二值化结果为源图数据
	Thinning(image1, image2, iWidth, iHeight, 128); //细化
	if (bSaveImg)
	{
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile9, image1);
		SaveDataToTextFile(stepTxtFile9, image2, iWidth, iHeight);
	}
	unsigned char* thin = new unsigned char[iWidth * iHeight];
	memcpy(thin, image2, sizeof(unsigned char) * iWidth * iHeight);//保存细化结果
	//第10步：特征提取
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第9步细化结果为源图数据
	int minuCount = Extract(image1, image2, iWidth, iHeight); //特征提取
	if (bSaveImg)
	{
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile10, image1);
		SaveDataToTextFile(stepTxtFile10, image2, iWidth, iHeight);
	}
	//第11步：特征过滤
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//以第10步特征提取结果为源图数据
	MINUTIAE* minutiaes = new MINUTIAE[minuCount];
	memset(minutiaes, sizeof(MINUTIAE), minuCount);
	MinuFilter(image1, thin, minutiaes, minuCount, iWidth, iHeight);//特征过滤

	/*******************************************************/
	/*将特征过滤结果保存到outImageFile*/
	unsigned char* image3 = new unsigned char[iWidth * iHeight];
	memset(image3, 0, iWidth * iHeight);
	for (int i = 0; i < minuCount; i++)
	{
		image3[(minutiaes[i].y - 1) * iWidth + (minutiaes[i].x - 1)] = 0xff;
	}

	SaveDataToImageFile(srcImgFile, outImgFile, image3);
	/*******************************************************/

	delete[]thin;

	SaveMinutiae(minutiaes, minuCount, stepMdlFile11);//保存特征数据文件

	if (bSaveImg)
	{
		memset(image2, 0, iWidth * iHeight);
		for (int i = 0; i < minuCount; i++)
		{
			image2[(minutiaes[i].y - 1) * iWidth + (minutiaes[i].x - 1)] = 0xff;
		}
		SaveDataToImageFile(srcImgFile, stepImgFile11, image2);
		SaveDataToTextFile(stepTxtFile11, image2, iWidth, iHeight);

	}

	//第12步：特征入库
	int sNo = GetNewIndexDB();//获得数据库新指纹记录编号
	char dbImgFile[MAX_PATH] = { 0 };//数据库中源图文件名
	char dbMdlFile[MAX_PATH] = { 0 };//数据库中模板文件名
	sprintf(dbImgFile, "%s%d.bmp", DB_DIR, sNo);//设置源图文件名（存入数据看文件夹内），以编号命名
	sprintf(dbMdlFile, "%s%d.mdl", DB_DIR, sNo);//设置模板文件名（存入数据看文件夹内），以编号命名

	if (minuCount > 0)
	{
		//保存原始指纹图像
		CopyFile(ToWideChar(srcImgFile), ToWideChar(dbImgFile), false);
		//保存指纹库特征模板文件
		CopyFile(ToWideChar(stepMdlFile11), ToWideChar(dbMdlFile), false);

		//指纹登记信息写入数据库引索文件
		FILE* index = fopen(DB_INDEX_TXT, "a");//追加模式写入
		fprintf(index, "%d %s %s %s %s\n", sNo, dbImgFile, dbMdlFile,regNo, regName);
		fclose(index);
	}
	delete[]image1;
	delete[]image2;
	delete[]minutiaes;
	delete[]image3;

	sprintf(info, "指纹登记成功。");

	return 0;
}

///*********************************************** Step16 指纹识别集成 ***********************************************/

/// <summary>
/// 指纹识别过程集成
/// </summary>
/// <param name="srcImgFile">源图文件名</param>
/// <param name="outImgFile">识别结果图文件名</param>
/// <param name="info">返回操作或失败提示信息</param>
/// <param name="bSaveImg">是否保存所有中间结果图像</param>
/// <returns></returns>
int Identify(char* srcImgFile, char* outImgFile, char* info, bool bSaveImg)
{
	//第0步：设置中间结果文件名
	//中间结果数据文件名
	char stepTxtFile1[MAX_PATH] = { STEP_TXT_1 };
	char stepTxtFile2[MAX_PATH] = { STEP_TXT_2 };
	char stepTxtFile3[MAX_PATH] = { STEP_TXT_3 };
	char stepTxtFile4[MAX_PATH] = { STEP_TXT_4 };
	char stepTxtFile5[MAX_PATH] = { STEP_TXT_5 };
	char stepTxtFile6[MAX_PATH] = { STEP_TXT_6 };
	char stepTxtFile7[MAX_PATH] = { STEP_TXT_7 };
	char stepTxtFile8[MAX_PATH] = { STEP_TXT_8 };
	char stepTxtFile9[MAX_PATH] = { STEP_TXT_9 };
	char stepTxtFile10[MAX_PATH] = { STEP_TXT_10 };
	char stepTxtFile11[MAX_PATH] = { STEP_TXT_11 };

	//中间结果图像文件名
	char stepImgFile1[MAX_PATH] = { STEP_IMG_1 };
	char stepImgFile2[MAX_PATH] = { STEP_IMG_2 };
	char stepImgFile3[MAX_PATH] = { STEP_IMG_3 };
	char stepImgFile4[MAX_PATH] = { STEP_IMG_4 };
	char stepImgFile5[MAX_PATH] = { STEP_IMG_5 };
	char stepImgFile6[MAX_PATH] = { STEP_IMG_6 };
	char stepImgFile7[MAX_PATH] = { STEP_IMG_7 };
	char stepImgFile8[MAX_PATH] = { STEP_IMG_8 };
	char stepImgFile9[MAX_PATH] = { STEP_IMG_9 };
	char stepImgFile10[MAX_PATH] = { STEP_IMG_10 };
	char stepImgFile11[MAX_PATH] = { STEP_IMG_11 };
	char stepImgFile12[MAX_PATH] = { STEP_IMG_12 };

	//指纹特征模板文件名
	char stepMdlFile11[MAX_PATH] = { STEP_IMG_11_MDL };

	//指纹库预检（判断指纹库是否为空）
	bool j = EmptyDB();
	if (j) {
		sprintf(info, "指纹库为空");
		return 0;
	}

	//第1步：图像载入，读取源图数据和参数信息
	int iWidth, iHeight, iDepth;
	int flag=ReadBMPImgFilePara(srcImgFile, iWidth, iHeight, iDepth);
	if (flag != 0)
	{
		sprintf(info, "图像加载失败.");
		return -1;
	}
	unsigned char* image1 = new unsigned char[iWidth * iHeight];
	unsigned char* image2 = new unsigned char[iWidth * iHeight];
	flag=ReadBMPImgFileData(srcImgFile, image1);
	if (flag != 0)
	{
		sprintf(info, "图像数据读取失败.");
		return -1;
	}
	memset(image2, 0, iWidth * iHeight);
	if (bSaveImg) {
		CopyFile(ToWideChar(srcImgFile), ToWideChar(stepImgFile1), false);
		SaveDataToTextFile(stepTxtFile1, image1, iWidth, iHeight);
	}
	//第2步：中值滤波
	MidFilter(image1, image2, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile2, image2);
		SaveDataToTextFile(stepTxtFile2, image2, iWidth, iHeight);
	}
	//第3步：直方图均衡化
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	HistoNormalize(image1, image2, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile3, image2);
		SaveDataToTextFile(stepTxtFile3, image2, iWidth, iHeight);
	}

	//第4步：方向计算
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	float* tmpDirection = new float[iWidth * iHeight];
	float* direction = new float[iWidth * iHeight];
	ImgDirection(image1, tmpDirection, iWidth, iHeight);
	DircLowPass(tmpDirection, direction, iWidth, iHeight);
	if (bSaveImg) {
		const int DIRECTION_SCALE = 100;
		SaveDataToImageFile(srcImgFile, stepImgFile4, direction, DIRECTION_SCALE);
		SaveDataToTextFile(stepTxtFile4, direction, iWidth, iHeight);
	}
	delete[] tmpDirection;

	//第5步：频率计算
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	float* frequency = new float[iWidth * iHeight];
	Frequency(image1, direction, frequency, iWidth, iHeight);
	if (bSaveImg) {
		const int FREQUENCY_SCALE = 1000;
		SaveDataToImageFile(srcImgFile, stepImgFile5, frequency, FREQUENCY_SCALE);
		SaveDataToTextFile(stepTxtFile5, frequency, iWidth, iHeight);
	}

	//第6步：掩码计算
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	unsigned char* mask = new unsigned char[iWidth * iHeight];
	GetMask(image1, direction, frequency, mask, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile6, mask);
		SaveDataToTextFile(stepTxtFile6, mask, iWidth, iHeight);
	}

	//第7步：Gabor滤波增强
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	GaborEnhance(image1, direction, frequency, mask, image2, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile7, image2);
		SaveDataToTextFile(stepTxtFile7, image2, iWidth, iHeight);
	}
	delete[] direction;
	delete[] frequency;
	delete[] mask;

	//第8步：二值化
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	BinaryImg(image1, image2, iWidth, iHeight, 128);
	if (bSaveImg) {
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile8, image1);
		SaveDataToTextFile(stepTxtFile8, image2, iWidth, iHeight);
	}

	//第9步：细化
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	Thinning(image1, image2, iWidth, iHeight, 128);
	if (bSaveImg) {
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile9, image1);
		SaveDataToTextFile(stepTxtFile9, image2, iWidth, iHeight);
	}
	unsigned char* thin = new unsigned char[iWidth * iHeight];
	memcpy(thin, image2, sizeof(unsigned char) * iWidth * iHeight);

	//第10步：特征提取
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	int minuCount = Extract(image1, image2, iWidth, iHeight);
	if (bSaveImg) {
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile10, image1);
		SaveDataToTextFile(stepTxtFile10, image2, iWidth, iHeight);
	}

	//第11步：特征过滤
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	MINUTIAE* minutiaes = new MINUTIAE[minuCount];
	memset(minutiaes, sizeof(MINUTIAE), minuCount);
	MinuFilter(image1, thin, minutiaes, minuCount, iWidth, iHeight);
	delete[] thin;
	SaveMinutiae(minutiaes, minuCount, stepMdlFile11);
	if (bSaveImg) {
		memset(image2, 0, iWidth * iHeight);
		for (int i = 0; i < minuCount; i++) {
			image2[(minutiaes[i].y - 1) * iWidth + (minutiaes[i].x - 1)] = 0xff;
		}
		SaveDataToImageFile(srcImgFile, stepImgFile11, image2);
		SaveDataToTextFile(stepTxtFile11, image2, iWidth, iHeight);
	}
	delete[] image1;
	delete[] image2;

	//第12步：特征识别
	int id = 0;
	char no[MAX_PATH] = { 0 }, name[MAX_PATH] = { 0 }, src[MAX_PATH] = { 0 }, mdl[MAX_PATH] = { 0 };
	float maxSimilar = -1;
	int maxID = -1;
	char maxNo[MAX_PATH] = {0}, maxName[MAX_PATH] = { 0 }, maxSrc[MAX_PATH] = { 0 };
	MINUTIAE* minutiaes2 = NULL;
	int minuCount2 = 0;

	FILE* index = fopen(DB_INDEX_TXT, "r");

	while (!feof(index)) {
		fscanf(index, "%d %s %s %s %s\n", &id, src, mdl, no, name);

		//从特征模板文件中读取指纹特征
		minuCount2 = ReadMinutiae(mdl, &minutiaes2);

		//特征匹配
		float similar = MinuSimilarity(minutiaes, minuCount, minutiaes2, minuCount2);

		//保存相似度最高的指纹记录信息
		if (similar > maxSimilar) {
			maxSimilar = similar;
			maxID = id;
			sprintf(maxNo, no);
			sprintf(maxName, name);
			sprintf(maxSrc, src);
		}
	}

	fclose(index);
	delete[] minutiaes;
	delete[] minutiaes2;

	//返回识别结果
	const float SIMILAR_THRED = 0.1;
	if (maxID < 0 || maxSimilar < SIMILAR_THRED) {
		sprintf(info, "识别失败");
		return -12;
	}

	strcpy(outImgFile, maxSrc);

	if (bSaveImg) {
		CopyFile(ToWideChar(maxSrc), ToWideChar(stepImgFile12), false);
	}
	sprintf(info, "识别成功.识别结果：编号[%s],姓名[%s],目标指纹[%s],相似度[%.2f]",maxNo, maxName, maxSrc, maxSimilar);
	return 0;
}

