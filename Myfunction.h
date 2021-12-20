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

#define TEMP_DIR "temp\\"//��ʱ�ļ��У����ڴ��ָ�����ǰ�������м䴦����
#define DB_DIR "Database\\"//ָ�ƿ��ļ���
#define DB_INDEX_TXT "Database\\index.txt" //ָ�ƿ������ļ�

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
/// ASCII���ֽ��ַ���ת��ΪUnicode���ַ���
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
/// ��ȡͼ�������Ϣ
/// </summary>
/// <param name="fileName"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="depth"></param>
/// <returns></returns>
int ReadBMPImgFilePara(char* fileName, int& width, int& height, int& depth)
{
	//����ͼ��
	CImage image;
	HRESULT hResult = image.Load(ToWideChar(fileName));
	if (FAILED(hResult) || image.IsNull())//ͼ���𻵻��ļ������ڵ����ؾ��ɵ�������ʧ��
	{
		MessageBox(NULL, _T("ͼ���𻵻��ļ�������"), _T("��ʾ"), MB_OK);
		return -1;
	}
	//���ͼ�����
	width = image.GetWidth();//���������Ϊ��λ��ͼ����
	height = image.GetHeight();//���������Ϊ��λ��ͼ��߶�
	depth = image.GetBPP();//�����bitΪ��λ��ͼ����ȣ�ÿ�����ص�λ����

	if (width <= 0 || height <= 0)
	{
		MessageBox(NULL, _T("ͼ���������"), _T("��ʾ"), MB_OK);
		return -1;
	}

	if (depth != 8)//Ŀǰϵͳֻ֧��8λ��λͼ���������͵�λͼ�ݲ�֧��
	{
		return -2;
	}
	//�ͷſռ����
	image.Destroy();
	return 0;
}
/// <summary>
/// ���ı��ļ���ȡ�޷����ַ��͵�ͼ��λͼ����
/// </summary>
/// <param name="srcFile">Դ�ļ�</param>
/// <param name="data">�޷����ַ�����������</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <returns></returns>
int ReadDataFromTextFile(char* srcFile, unsigned char* data, int iWidth, int iHeight)
{
	//�ж��ļ�����
	if (strstr(srcFile, ".txt") == NULL)
	{
		MessageBox(NULL, _T("����txt�ļ�"), _T("����"), MB_OK);
		return -1;
	}
	if (iWidth <= 0 || iHeight <= 0)
	{
		MessageBox(NULL, _T("ͼ���ߴ���"), _T("����"), MB_OK);
		return -1;
	}

	//��Դ�ļ�
	ifstream fin(srcFile, ios::in);
	if (!fin)//or if(fin.fail())
	{
		return -1;
	}
	//��ȡ����
	int d = 0;
	for (int i = 0; i < iHeight * iWidth; i++)
	{
		fin >> d;
		data[i] = (unsigned char)d;
	}

	//�ر��ļ�
	fin.close();

	return 0;
}
/// <summary>
/// ��λͼ����д��BMPͼ���ļ�
/// </summary>
/// <param name="dstFileName">Ŀ���ļ���</param>
/// <param name="pusImgData">��������������</param>
/// <returns></returns>
int WriteBMPImgFile(char* dstFileName, unsigned char** pusImgData)
{
	//���ļ�
	FILE* fp = fopen(dstFileName, "r+b");
	if (!fp)
	{
		return -1;
	}
	//��ȡͼ�������Ϣ�����ڶ�λ��������д�����ݣ�
	int imgType, iWidth, iHeight;//ͼ�����/���/�߶�
	int iStartPos = 0;//λͼ��������ʼ��ַ
	fseek(fp, 10L, SEEK_SET);
	fread((char*)(&iStartPos), 4, 1, fp);//��ȡλͼ��������ʼ��ַ�����ֽ�Ϊ��λ��
	fseek(fp, 18L, SEEK_SET);
	fread((char*)(&iWidth), 4, 1, fp);//��ȡͼ���ȣ�������Ϊ��λ��
	fread((char*)(&iHeight), 4, 1, fp);//��ȡͼ��߶ȣ�������Ϊ��λ��
	unsigned short temp;
	fseek(fp, 28L, SEEK_SET);
	fread((char*)(&temp), 2, 1, fp);//��ȡͼ����ȣ�ÿ�����ص�λ������λΪ��λ��
	imgType = temp;
	if (imgType != 8)//Ŀǰֻ֧��8λBMPλͼͼ��
	{
		return -2;
	}
	//��������д������
	unsigned char* usImgData = *pusImgData;//���������������ַ
	int iWidthInFile = 0;//�ļ���ÿ�����ؿ��/���ȣ����ֽ�Ϊ��λ��
	if (iWidth % 4 > 0)
	{
		iWidthInFile = iWidth - iWidth % 4 + 4;
	}
	else
	{
		iWidthInFile = iWidth;
	}
	for (int i = iHeight - 1; i >= 0; i--)//�����һ�е���һ�е���洢
	{
		fseek(fp, iStartPos, SEEK_SET);//��λ���׵�ַ
		fwrite((usImgData + i * iWidth), 1, iWidth, fp);//д��һ������
		iStartPos += iWidthInFile;//�������׵�ַ
	}

	//�ر��ļ�
	fclose(fp);

	return 0;
}
/// <summary>
/// ���ı��ļ���ȡ���������͵���������
/// </summary>
/// <param name="srcFile">Դ�ļ�</param>
/// <param name="data">����������������</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <returns></returns>
int ReadDataFromTextFile(char* srcFile, float* data, int iWidth, int iHeight)
{
	//�ж��ļ�����
	if (strstr(srcFile, ".txt") == NULL)
	{
		MessageBox(NULL, _T("����txt�ļ�"), _T("����"), MB_OK);
		return -1;
	}
	if (iWidth <= 0 || iHeight <= 0)
	{
		MessageBox(NULL, _T("ͼ���ߴ���"), _T("����"), MB_OK);
		return -1;
	}

	//��Դ�ļ�
	ifstream fin(srcFile, ios::in);
	if (!fin)//or if(fin.fail())
	{
		return -1;
	}
	//��ȡ����
	for (int i = 0; i < iHeight * iWidth; i++)
	{
		fin >> data[i];
	}

	//�ر��ļ�
	fin.close();

	return 0;
}
/// <summary>
/// �������ݵ�BMPͼ���ļ�
/// </summary>
/// <param name="srcFile">Դ�ļ���</param>
/// <param name="dstFile">Ŀ���ļ���</param>
/// <param name="data">��������������</param>
/// <returns></returns>
int SaveDataToImageFile(char* srcFile, char* dstFile, unsigned char* data)
{
	//�ļ�����
	CopyFile(ToWideChar(srcFile), ToWideChar(dstFile), false);

	int e = 0;
	//д������
	e = WriteBMPImgFile(dstFile, &data);
	if (e != 0)
		return -1;

	return 0;
}
/// <summary>
/// �������ݵ�ͼ���ļ�
/// </summary>
/// <param name="srcFile">Դ�ļ���</param>
/// <param name="dstFile">Ŀ���ļ���</param>
/// <param name="data">��������������</param>
/// <param name="scale">ת������</param>
/// <returns></returns>
int SaveDataToImageFile(char* srcFile, char* dstFile, float* data, float scale)
{
	int e = 0;
	//��ȡͼ�����
	int iWidth, iHeight, iDepth;//ͼ����/�߶�/���
	e = ReadBMPImgFilePara(srcFile, iWidth, iHeight, iDepth);
	if (e != 0)
		return -1;

	//�ļ�����
	CopyFile(ToWideChar(srcFile), ToWideChar(dstFile), false);

	//����ת��
	unsigned char* tmpData = new unsigned char[iWidth * iHeight];
	for (int i = 0; i<int(iWidth * iHeight); i++)
	{
		tmpData[i] = unsigned char((scale * data[i]));
	}

	//д������
	e = WriteBMPImgFile(dstFile, &tmpData);
	if (e != 0)
		return -1;

	//�ͷ��ڴ�ռ�
	delete[]tmpData;

	return 0;
}
/// <summary>
/// �ڿؼ�����ʾͼ��
/// </summary>
/// <param name="picCtrl">ͼ��ؼ�</param>
/// <param name="filename">ͼ���ļ�·��</param>
/// <returns></returns>
int ShowImageInCtrl(CStatic& picCtrl, char* filename)
{
	double cx, cy, dx, dy, k, t;//���ؼ��Ŀ�͸��Լ�ͼƬ��͸��йصĲ���
	CRect rect;//������ζ��� ���ڻ�ȡͼƬ�ؼ��Ŀ�͸�
	//����ͼ��
	CImage image;//ΪcimageͼƬ�ഴ��һ������
	HRESULT hResult = image.Load(ToWideChar(filename));
	//���ͼ�����
	//int width = image.GetWidth();//ͼ����
	//int height = image.GetHeight();//ͼ��߶�
	cx= image.GetWidth();//ͼ����
	cy= image.GetHeight();//ͼ��߶�
	k = cy / cx; // ���ͼƬ�Ŀ�߱�
	//������ʾ����
	picCtrl.GetClientRect(&rect);//���picture�ؼ����ڵľ�������
	dx = rect.Width();
	dy = rect.Height();//��ÿؼ��Ŀ�߱�
	t = dy / dx;//��ÿؼ��Ŀ�߱�
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
	//��صļ���Ϊ����ͼƬ�ڻ�ͼ�����а�������ʾ
	CDC* pDc = picCtrl.GetWindowDC();//���picture�ؼ����豸�������
	int ModeOld = SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);//����λͼ����ģʽ
	//��ʾͼ��
	//��ͼ�񻭵�Picture�ؼ���ʾ�ľ�������
	image.StretchBlt(pDc->m_hDC, rect, SRCCOPY);
	SetStretchBltMode(pDc->m_hDC, ModeOld);
	//���¿ؼ���ʾ
	picCtrl.Invalidate(false);
	//�ͷű����ռ�
	image.Destroy();
	picCtrl.ReleaseDC(pDc);//�ͷ�picture�ؼ����豸�������
	return 0;
}

///*********************************************** Step1 ����ͼ�� ***********************************************/

/// <summary>
/// ��ͼ���ļ���ȡ����
/// </summary>
/// <param name="fileName"></param>
/// <param name="data"></param>
/// <returns></returns>
int ReadBMPImgFileData(char* fileName, unsigned char* data)
{
	//����ͼ��
	CImage image;
	HRESULT hResult = image.Load(ToWideChar(fileName));
	if (FAILED(hResult) || image.IsNull())//ͼ���𻵻��ļ������ڵ����ؾ��ɵ�������ʧ��
	{
		MessageBox(NULL, _T("ͼ���𻵻��ļ�������"), _T("��ʾ"), MB_OK);
		return -1;
	}
	//���ͼ�����
	int width = image.GetWidth();//���������Ϊ��λ��ͼ����
	int height = image.GetHeight();//���������Ϊ��λ��ͼ��߶�
	int depth = image.GetBPP();//�����bitΪ��λ��ͼ����ȣ�ÿ�����ص�λ����

	if (width <= 0 || height <= 0)
	{
		MessageBox(NULL, _T("ͼ���������"), _T("��ʾ"), MB_OK);
		return -1;
	}

	if (depth != 8)//Ŀǰϵͳֻ֧��8λ��λͼ���������͵�λͼ�ݲ�֧��
	{
		return -2;
	}
	//��ʼ���������
	memset(data, 0, width * height);

	//��ȡͼ������
	int pitch = image.GetPitch();//ÿ���������صĵ�ַƫ����
	unsigned char* pData1 = (unsigned char*)image.GetBits();//ͼ��λͼ��������ַ
	unsigned char* pData2 = data;
	unsigned char gray = 0;

	unsigned char* pRow1, * pRow2, * pPix1, * pPix2;
	for (int y = 0; y < height; y++)//����ɨ��
	{
		pRow1 = pData1 + pitch * y;
		pRow2 = pData2 + width * y;
		for (int x = 0; x < width; x++)//����ɨ��
		{
			//���Դͼ��Ҷ�ֵ
			pPix1 = pRow1 + x;
			gray = *pPix1;
			//���浽�������
			pPix2 = pRow2 + x;
			*pPix2 = gray;
		}//end for(x)
	}//end for(x)
	//�ͷſռ����
	image.Destroy();
	return 0;
}
/// <summary>
/// ����λͼ���ݵ��ı��ļ�
/// </summary>
/// <param name="dstFile">Ŀ���ļ�</param>
/// <param name="data">�޷����ַ�����������</param>
/// <param name="width">ͼ����</param>
/// <param name="height">ͼ��߶�</param>
/// <returns></returns>
int SaveDataToTextFile(char* dstFile, unsigned char* data, int width, int height)
{
	//��Ŀ���ļ�
	ofstream fout(dstFile, ios::out);//ʹ�ø���д�뷽��
	if (!fout)//or if(fout.fail())
	{
		return -1;
	}

	//��ָ����ʽ���ļ�д������
	int space = 5;//ÿ���޷����ַ������ݾ���5���ַ�λ�ȿ�洢
	for (int i = 0; i < height * width; i++)//��������
	{
		fout << setw(space) << int(data[i]);//�ȿ�д������
		if (i % width == (width - 1))//��β
		{
			fout << endl;//����
		}
	}
	//�ر��ļ�
	fout.close();
	return 0;
}
/// <summary>
/// ����λͼ���ݵ��ı��ļ�
/// </summary>
/// <param name="dstFile">Ŀ���ļ�</param>
/// <param name="data">����������������</param>
/// <param name="width">ͼ����</param>
/// <param name="height">ͼ��߶�</param>
/// <returns></returns>
int SaveDataToTextFile(char* dstFile, float* data, int width, int height)
{
	//��Ŀ���ļ�
	ofstream fout(dstFile, ios::out);//ʹ�ø���д�뷽��
	if (!fout)//or if(fout.fail())
	{
		return -1;
	}
	//��ָ����ʽ���ļ�д������
	int preci = 6;//ÿ�����������ݾ�����6��С��λ
	int space = 16;//ÿ�����������ݾ���16���ַ�λ�ȿ�洢
	fout.precision(preci);
	for (int i = 0; i < height * width; i++)//��������
	{
		fout << " " << setw(space) << data[i];//�ȿ�д������
		if (i % width == (width - 1))//��β
		{
			fout << endl;//����
		}
	}
	//�ر��ļ�
	fout.close();
	return 0;
}

///*********************************************** Step �����ͷ���ָ�ƿ� ***********************************************/

/// <summary>
/// ��������ʼ���ļ�
/// </summary>
/// <param name="filename"></param>
void InitFile(char* filename)
{
	FILE* index = fopen(filename, "a");//���ļ��������򴴽������Ѵ��������������
	fclose(index);//�ر��ļ�
}
/// <summary>
/// ��������ʼ��ָ�ƿ�
/// </summary>
void InitDatabase()
{
	_mkdir(TEMP_DIR);//������ʱ�ļ���
	_mkdir(DB_DIR);//����ָ�ƿ��ļ���
	InitFile(DB_INDEX_TXT);//���������ļ�
}
/// <summary>
/// ��ȡָ�ƿ�ͳ����Ϣ
/// </summary>
/// <param name="info">����ָ�ƿ�ͳ����Ϣ������ϵͳ������ʾ</param>
void GetDatabaseInfo(char* info)
{
	//����ָ�ƿ�
	int pNo = 0;//ָ�����
	char no[MAX_PATH] = { 0 };//�Ǽ��˱��
	char name[MAX_PATH] = { 0 };//�Ǽ�������
	char srcFile[MAX_PATH] = { 0 };//ָ��ͼ���ļ�
	char mdlFile[MAX_PATH] = { 0 };//ָ�������ļ�
	FILE* index = fopen(DB_INDEX_TXT, "r");//�������ļ�
	while (!feof(index))
	{
		fscanf(index, "%d %s %s %s %s\n", &pNo, srcFile, mdlFile, no, name);
	}
	fclose(index);//�ر��ļ�

	//ͳ��ָ�ƿ���Ϣ
	sprintf(info, "��ǰָ�ƿ��й��� %d ����¼", pNo);
}

///*********************************************** Step2 ��ֵ�˲� ***********************************************/

/// <summary>
/// ʹ��ð����������������������
/// </summary>
/// <param name="data">����</param>
/// <param name="dsize">���鳤��</param>
void Sort(unsigned char* data, int dsize)
{
	unsigned char temp = 0;
	for (int i = 0; i < dsize; i++)
	{
		for (int j = dsize - 1; j > i; j--)
		{
			if (data[j] < data[j - 1])//��������
			{
				temp = data[j];
				data[j] = data[j - 1];
				data[j - 1] = temp;
			}
		}
	}
}
/// <summary>
/// ��ֵ�˲��㷨
/// </summary>
/// <param name="ucImg">Դͼ����</param>
/// <param name="ucDsImg">���ͼ������</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <returns></returns>
int MidFilter(unsigned char* ucImg, unsigned char* ucDsImg, int iWidth, int iHeight)
{
	//Step1:���ͼ�����ݳ�ʼ��
	memset(ucDsImg, 0, iWidth * iHeight);
	//Step2:���������˲���ʹ��3*3����
	unsigned char* pUp, * pDown, * pImg;//����ȷ��3*3�����3��ͼ������ָ�루���ĳơ�����ָ�롱��
	unsigned char x[9];//3*3����ͼ����������
	for (int i = 1; i < iHeight - 1; i++)//�����ڶ��е������ڶ���
	{
		//��ʼ������ָ��
		pUp = ucImg + (i - 1) * iWidth;
		pImg = ucImg + i * iWidth;
		pDown = ucImg + (i + 1) * iWidth;

		for (int j = 1; j < iWidth - 1; j++)//������2�е�������2��
		{
			//�ƶ�����ָ��
			pUp++;
			pImg++;
			pDown++;

			//��ȡ3*3��������
			x[0] = *(pUp - 1);
			x[1] = *(pImg - 1);
			x[2] = *(pDown - 1);
			x[3] = *pUp;
			x[4] = *pImg;
			x[5] = *pDown;
			x[6] = *(pUp + 1);
			x[7] = *(pImg + 1);
			x[8] = *(pDown + 1);

			//��������
			Sort(x, 9);
			//���ͼ������ȡ������ֵ
			*(ucDsImg + i * iWidth + j) = x[4];
		}
	}
	//Step3:��һ�к����һ���˲���ʹ��2*3����
	//��һ��
	pDown = ucImg + iWidth;//����ָ���ʼ��
	for (int j = 1; j < iWidth - 1; j++)
	{
		//��ȡ2*3��������
		x[0] = *(ucImg + j - 1);
		x[1] = *(ucImg + j);
		x[2] = *(ucImg + j + 1);
		x[3] = *(pDown + j - 1);
		x[4] = *(pDown + j);
		x[5] = *(pDown + j + 1);

		//��������
		Sort(x, 6);

		//���ȡ��ֵ
		*(ucDsImg + j) = x[3];
	}
	//���һ�У�������һ�У�
	pUp = ucImg + iWidth * (iHeight - 2);//����ָ���ʼ��
	pDown = ucImg + iWidth * (iHeight - 1);//����ָ���ʼ��
	for (int j = 1; j < iWidth - 1; j++)//���һ�б����ڶ��е������ڶ���
	{
		//��ȡ2*3��������
		x[0] = *(pDown + j - 1);
		x[1] = *(pDown + j);
		x[2] = *(pDown + j + 1);
		x[3] = *(pUp + j - 1);
		x[4] = *(pUp + j);
		x[5] = *(pUp + j + 1);

		//��������
		Sort(x, 6);

		//���ȡ��ֵ
		*(ucDsImg + iWidth * (iHeight - 1) + j) = x[3];
	}

	//Step4:4���ǵ��˲���ʹ��2*2����

	//���Ͻǵ�
	x[0] = *(ucImg);//��ȡ2*2��������
	x[1] = *(ucImg + 1);
	x[2] = *(ucImg + iWidth);
	x[3] = *(ucImg + iWidth + 1);
	Sort(x, 4);//��������
	*(ucDsImg) = x[2];//���ȡ��ֵ

	//���Ͻǵ�
	x[0] = *(ucImg + iWidth - 1);//��ȡ2*2��������
	x[1] = *(ucImg + iWidth - 2);
	x[2] = *(ucImg + 2 * iWidth - 1);
	x[3] = *(ucImg + 2 * iWidth - 2);
	Sort(x, 4);//��������
	*(ucDsImg + iWidth - 1) = x[2];//���ȡ��ֵ

	//���½ǵ�
	x[0] = *(ucImg + (iHeight - 1) * iWidth);//��ȡ2*2��������
	x[1] = *(ucImg + (iHeight - 2) * iWidth);
	x[2] = *(ucImg + (iHeight - 1) * iWidth + 1);
	x[3] = *(ucImg + (iHeight - 2) * iWidth + 1);
	Sort(x, 4);//��������
	*(ucDsImg + iWidth * (iHeight - 1)) = x[2];//���ȡ��ֵ

	//���½ǵ�
	x[0] = *(ucImg + (iHeight - 0) * iWidth - 1);//��ȡ2*2��������
	x[1] = *(ucImg + (iHeight - 1) * iWidth - 1);
	x[2] = *(ucImg + (iHeight - 0) * iWidth - 2);
	x[3] = *(ucImg + (iHeight - 1) * iWidth - 2);
	Sort(x, 4);//��������
	*(ucDsImg + iWidth * iHeight - 1) = x[2];//���ȡ��ֵ

	return 0;
}

///*********************************************** Step3 ���⻯ ***********************************************/

/// <summary>
/// ���⻯
/// </summary>
/// <param name="ucImg"></param>
/// <param name="ucNormImg"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int HistoNormalize(unsigned char* ucImg, unsigned char* ucNormImg, int iWidth, int iHeight)
{
	//����Դͼ�Ҷ�ֱ��ͼ
	unsigned int Histogram[256];
	memset(Histogram, 0, 256 * sizeof(int));
	for (int i = 0; i < iHeight; i++) {
		for (int j = 0; j < iWidth; j++) {
			Histogram[ucImg[i * iWidth + j]]++;
		}
	}
	//����Դͼ�ĻҶȾ�ֵ�ͷ���
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
	//�Ը����ؽ��о��⻯����
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

///*********************************************** Step4 ������� ***********************************************/

/// <summary>
/// ���߷������
/// </summary>
/// <param name="ucImg"></param>
/// <param name="fDirc"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int ImgDirection(unsigned char* ucImg, float* fDirc, int iWidth, int iHeight)
{
	//�������
	const int SEMISIZ = 7;	//���򴰿�����뾶
	int dx[SEMISIZ * 2 + 1][SEMISIZ * 2 + 1];
	int dy[SEMISIZ * 2 + 1][SEMISIZ * 2 + 1];
	float fx, fy;
	//�����ʼ��
	memset(fDirc, 0, iWidth * iHeight * sizeof(float));
	//����ÿһ���صļ��߷���ֵ
	for (int y = SEMISIZ + 1; y < iHeight - SEMISIZ - 1; y++) {
		for (int x = SEMISIZ + 1; x < iWidth - SEMISIZ - 1; x++) {
			//�����Ե�ǰ����Ϊ���ĵ����򴰿�����ÿһ���ص��ݶ�
			for (int j = 0; j < SEMISIZ * 2 + 1; j++) {
				for (int i = 0; i < SEMISIZ * 2 + 1; i++) {
					int index1 = (y + j - SEMISIZ) * iWidth + x + i - SEMISIZ;
					int index2 = (y + j - SEMISIZ) * iWidth + x + i - SEMISIZ - 1;
					int index3 = (y + j - SEMISIZ - 1) * iWidth + x + i - SEMISIZ;
					dx[i][j] = int(ucImg[index1] - ucImg[index2]);
					dy[i][j] = int(ucImg[index1] - ucImg[index3]);
				}
			}
			///���㵱ǰ���صļ��߷���ֵ
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
/// ���߷����ͨ�˲�
/// </summary>
/// <param name="fDirc"></param>
/// <param name="fFitDirc"></param>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
/// <returns></returns>
int DircLowPass(float* fDirc, float* fFitDirc, int iWidth, int iHeight)
{
	//�������
	const int DIR_FILTER_SIZE = 2;
	int blocksize = 2 * DIR_FILTER_SIZE + 1;
	int imgsize = iWidth * iHeight;

	float* filter = new float[blocksize * blocksize];//ʹ��5*5�˲���
	float* phix = new float[imgsize];
	float* phiy = new float[imgsize];
	float* phi2x = new float[imgsize];
	float* phi2y = new float[imgsize];
	//�����ʼ��
	memset(fFitDirc, 0, sizeof(float) * iWidth * iHeight);
	//����5*5��˹��ͨ�˲���ģ��
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
	//��������ص�ķ�������ֵ������ֵ
	for (int y = 0; y < iHeight; y++) {
		for (int x = 0; x < iWidth; x++) {
			phix[y * iWidth + x] = cos(fDirc[y * iWidth + x]);
			phiy[y * iWidth + x] = sin(fDirc[y * iWidth + x]);
		}
	}
	//���������ؽ��з����ͨ�˲�
	memset(phi2x, 0, sizeof(float) * imgsize);
	memset(phi2y, 0, sizeof(float) * imgsize);
	float nx, ny;
	int val;
	for (int y = 0; y < iHeight - blocksize; y++) {	//���б�������ȥ��Ե����
		for (int x = 0; x < iWidth - blocksize; x++) {
			//���Ե�ǰ����Ϊ���ĵ��˲������ڵ���������ֵ���м�Ȩ�ۼ�
			nx = 0.0;
			ny = 0.0;
			for (int j = 0; j < blocksize; j++) {
				for (int i = 0; i < blocksize; i++) {
					val = (x + i) + (j + y) * iWidth;
					nx += filter[j * blocksize + i] * phix[val];	//�������Ҽ�Ȩ�ۼ�
					ny += filter[j * blocksize + i] * phiy[val];	//�������Ҽ�Ȩ�ۼ�
				}
			}
			//�������Ϊ��ǰ���ص��µķ�������ֵ������ֵ
			val = x + y * iWidth;
			phi2x[val] = nx;
			phi2y[val] = ny;
		}
	}
	//���ݼ�Ȩ�ۼӽ������������صķ����˲����ֵ
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

///*********************************************** Step5 ���ʼ��� ***********************************************/

/// <summary>
/// ���ʼ���
/// </summary>
/// <param name="ucImg"></param>
/// <param name="fDirection"></param>
/// <param name="fFrequency"></param>
/// <param name="iWidth"></param>
/// <param name="iHeigth"></param>
/// <returns></returns>
int Frequency(unsigned char* ucImg, float* fDirection, float* fFrequency, int iWidth, int iHeight) {
	//���ڴ�С
	const int SIZE_L = 32;
	const int SIZE_W = 16;
	const int SIZE_L2 = 16;
	const int SIZE_W2 = 8;
	//���Ҳ���ֵ��
	int peak_pos[SIZE_L];
	int peak_cnt;
	float peak_freq;
	float Xsig[SIZE_L];
	//����
	float dir = 0.0;
	float cosdir = 0.0;
	float sindir = 0.0;
	float maxPeak, minPeak;
	//�����ʼ��
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
			//��ǰ���صļ��߷���
			dir = fDirection[(y + SIZE_W2) * iWidth + (x + SIZE_W2)];
			cosdir = -sin(dir);
			sindir = cos(dir);

			//���㵱ǰ����Ϊ���ĵ�L*W���򴰿ڵķ�ֵ����
			for (k = 0; k < SIZE_L; k++) {
				Xsig[k] = 0.0;
				for (d = 0; d < SIZE_W; d++) {
					u = (int)(x + (d - SIZE_W2) * cosdir + (k - SIZE_L2) * sindir);
					v = (int)(y + (d - SIZE_W2) * sindir - (k - SIZE_L2) * cosdir);
					//�߽�㴦��
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

			//ȷ����ֵ���б仯��Χ
			maxPeak = minPeak = Xsig[0];
			for (k = 0; k < SIZE_L; k++) {
				if (minPeak > Xsig[k]) {
					minPeak = Xsig[k];
				}
				if (maxPeak < Xsig[k]) {
					maxPeak = Xsig[k];
				}
			}

			//ȷ����ֵ���λ��
			peak_cnt = 0;
			if ((maxPeak - minPeak) > 64) {
				for (k = 0; k < SIZE_L; k++) {
					if ((Xsig[k - 1] < Xsig[k]) && (Xsig[k] >= Xsig[k + 1])) {
						peak_pos[peak_cnt++] = k;
					}
				}
			}

			//�����ֵ��ƽ�����
			peak_freq = 0.0;
			if (peak_cnt >= 2) {
				for (k = 0; k < peak_cnt - 1; k++) {
					peak_freq += (peak_pos[k + 1] - peak_pos[k]);
				}
				peak_freq /= peak_cnt - 1;
			}

			//���㵱ǰ���ص�Ƶ��
			if (peak_freq < 3.0 || peak_freq>25.0) {
				frequency1[x + y * iWidth] = 0.0;
			}
			else {
				frequency1[x + y * iWidth] = 1.0 / peak_freq;
			}
		}
	}
	
	//��Ƶ�ʽ��о�ֵ�˲�
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

///*********************************************** Step6 ������� ***********************************************/

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
	//��ֵ�ָ�
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

	//���׶�
	for (k = 0; k < 4; k++) {
		//���ǰ����
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
		//�жϺ�����ǰ����
		for (y = 1; y < iHeight - 1; y++) {
			for (x = 1; x < iWidth - 1; x++) {
				if (ucMask[x + y * iWidth]) {
					ucMask[x + y * iWidth] = 0xFF;
				}
			}
		}
	}
	//ȥ����Ե�͹�����
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
		//�жϺ����ñ�����
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

///*********************************************** Step7 �˲���ǿ ***********************************************/

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
	//�������
	const float PI = 3.141592654;
	int i, j, u, v;
	int wg2 = 5;
	float sum, f, g;
	float x2, y2;
	float dx2 = 1.0 / (4.0 * 4.0);
	float dy2 = 1.0 / (4.0 * 4.0);
	//�����ʼ��
	memset(ucImgEnhanced, 0, iWidth * iHeight);
	//Gabor�˲�
	for (j = wg2; j < iHeight - wg2; j++) {
		for (i = wg2; i < iWidth - wg2; i++) {
			if (ucMask[i + j * iWidth] == 0) {
				continue;
			}
			//��ȡ��ǰ���صķ����Ƶ��
			g = fDirection[i + j * iWidth];
			f = fFrequency[i + j * iWidth];
			g += PI / 2;
			//�Ե�ǰ���ؽ����˲�
			sum = 0.0;
			for (v = -wg2; v <= wg2; v++) {
				for (u = -wg2; u <= wg2; u++) {
					x2 = -u * sin(g) + v * cos(g);
					y2 = u * cos(g) + v * sin(g);
					sum += exp(-0.5 * (x2 * x2 * dx2 + y2 * y2 * dy2)) * cos(2 * PI * x2 * f) * ucImg[(i - u) + (j - v) * iWidth];
				}
			}
			//�߽紦��
			if (sum > 255) {
				sum = 255.0;
			}
			if (sum < 0.0) {
				sum = 0.0;
			}
			//�õ���ǰ���ص��˲����
			ucImgEnhanced[i + j * iWidth] = (unsigned char)sum;
		}
	}
	return 0;
}

///*********************************************** Step8 ��ֵ�� ***********************************************/

/// <summary>
/// ͼ���ֵ��
/// </summary>
/// <param name="ucImage">Դͼ����</param>
/// <param name="ucBinImage">���ͼ����</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <param name="uThreshold">��ֵ���Ҷ���ֵ</param>
/// <returns></returns>
int BinaryImg(unsigned char* ucImage, unsigned char* ucBinImage, int iWidth, int iHeight, unsigned char uThreshold)
{
	//�ֱ���ָ��Դͼ���ݺͽ��ͼ���ݵ�����ָ��
	unsigned char* pStart = ucImage, * pEnd = ucImage + iWidth * iHeight;
	unsigned char* pDest = ucBinImage;
	//��һ������������
	while (pStart < pEnd)
	{
		*pDest = *pStart > uThreshold ? 1 : 0;//��ֵ��
		pStart++;//Դͼ����ָ�����
		pDest++;//���ͼ����ָ�����
	}
	return 0;
}
/// <summary>
/// ��ֵͼ��ʾת��([0,1]->[0,255])
/// </summary>
/// <param name="ucBinImg">Դͼ����</param>
/// <param name="ucGrayImg">���ͼ����</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <returns></returns>
int BinaryToGray(unsigned char* ucBinImg, unsigned char* ucGrayImg, int iWidth, int iHeight)
{
	//�ֱ���ָ��Դͼ���ݺͽ��ͼ���ݵ�����ָ��
	unsigned char* pStart = ucBinImg, * pEnd = ucBinImg + iWidth * iHeight;
	unsigned char* pDest = ucGrayImg;
	//��һ������������
	while (pStart < pEnd)
	{
		*pDest = (*pStart) > 0 ? 255 : 0;
		pStart++;//Դͼ����ָ�����
		pDest++;//���ͼ����ָ�����
	}
	return 0;
}

///*********************************************** Step9 ϸ�� ***********************************************/

/// <summary>
/// ͼ��ϸ��
/// </summary>
/// <param name="ucBinedImg">Դͼ����</param>
/// <param name="ucThinnedImage">���ͼ����</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <param name="iIterativeLimit">����������</param>
/// <returns></returns>
int Thinning(unsigned char* ucBinedImg, unsigned char* ucThinnedImage, int iWidth, int iHeight, int iIterativeLimit)
{
	//�������
	unsigned char x1, x2, x3, x4, x5, x6, x7, x8, xp;
	unsigned char g1, g2, g3, g4;
	unsigned char b1, b2, b3, b4;
	unsigned char np1, np2, npm;
	unsigned char* pUp, * pDown, * pImg;
	int iDeletedPoints = 0;

	//�����ʼ��
	memcpy(ucThinnedImage, ucBinedImg, iWidth * iHeight);

	for (int it = 0; it < iIterativeLimit; ++it)//�ظ�ִ�У�һֱ���޵��ɾΪֹ
	{
		iDeletedPoints = 0;//��ʼ�����ε�����ɾ����
		//���ε����ڵ�1�α�����ʹ������G1&G2&G3��
		for (int i = 1; i < iHeight - 1; ++i)//���б���
		{
			//��ʼ������ָ��
			pUp = ucBinedImg + (i - 1) * iWidth;
			pImg = ucBinedImg + i * iWidth;
			pDown = ucBinedImg + (i + 1) * iWidth;

			for (int j = 1; j < iWidth - 1; ++j)//���б���
			{
				//��������ָ��
				pUp++;
				pImg++;
				pDown++;
				//���������㣨����ֵΪ0�ĵ㣩
				if (!*pImg)
					continue;
				//��ȡ3*3���򴰿�������9�����صĻҶ�ֵ
				x6 = *(pUp - 1);
				x5 = *(pImg - 1);
				x4 = *(pDown - 1);
				x7 = *pUp;
				xp = *pImg;
				x3 = *pDown;
				x8 = *(pUp + 1);
				x1 = *(pImg + 1);
				x2 = *(pDown + 1);
				//�ж�����G1
				b1 = !x1 && (x2 == 1 || x3 == 1);
				b2 = !x3 && (x4 == 1 || x5 == 1);
				b3 = !x5 && (x6 == 1 || x7 == 1);
				b4 = !x7 && (x8 == 1 || x1 == 1);
				g1 = (b1 + b2 + b3 + b4) == 1;
				//�ж�����G2
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
				//�ж�����G3��G4
				g3 = (x1 && (x2 || x3 || !x8)) == 0;
				g4 = (x5 && (x6 || x7 || !x4)) == 0;
				//����жϣ�ʹ������G1&G2&G3��
				if (g1 && g2 && g3)
				{
					ucThinnedImage[iWidth * i + j] = 0;//ɾ����ǰ���ص�
					++iDeletedPoints;
				}
			}
		}

		//���ͬ��
		memcpy(ucBinedImg, ucThinnedImage, iWidth * iHeight);

		//���ε����ڵ�2�α�����ʹ������G1&G2&G3��
		for (int i = 1; i < iHeight - 1; ++i)//���б���
		{
			//��ʼ������ָ��
			pUp = ucBinedImg + (i - 1) * iWidth;
			pImg = ucBinedImg + i * iWidth;
			pDown = ucBinedImg + (i + 1) * iWidth;

			for (int j = 1; j < iWidth - 1; ++j)//���б���
			{
				//��������ָ��
				pUp++;
				pImg++;
				pDown++;
				//���������㣨����ֵΪ0�ĵ㣩
				if (!*pImg)
					continue;
				//��ȡ3*3���򴰿�������9�����صĻҶ�ֵ
				x6 = *(pUp - 1);
				x5 = *(pImg - 1);
				x4 = *(pDown - 1);
				x7 = *pUp;
				xp = *pImg;
				x3 = *pDown;
				x8 = *(pUp + 1);
				x1 = *(pImg + 1);
				x2 = *(pDown + 1);
				//�ж�����G1
				b1 = !x1 && (x2 == 1 || x3 == 1);
				b2 = !x3 && (x4 == 1 || x5 == 1);
				b3 = !x5 && (x6 == 1 || x7 == 1);
				b4 = !x7 && (x8 == 1 || x1 == 1);
				g1 = (b1 + b2 + b3 + b4) == 1;
				//�ж�����G2
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
				//�ж�����G3��G4
				g3 = (x1 && (x2 || x3 || !x8)) == 0;
				g4 = (x5 && (x6 || x7 || !x4)) == 0;
				//����жϣ�ʹ������G1&G2&G4��
				if (g1 && g2 && g4)
				{
					ucThinnedImage[iWidth * i + j] = 0;//ɾ����ǰ���ص�
					++iDeletedPoints;
				}
			}
		}

		//���ͬ��
		memcpy(ucBinedImg, ucThinnedImage, iWidth * iHeight);

		//������ε������޵��ɾ����ֹͣ����
		if (iDeletedPoints == 0)
			break;
	}
	//�����Ե����
	for (int i = 0; i < iHeight; ++i)
	{
		for (int j = 0; j < iWidth; ++j)
		{
			if (i < 16)//�ϱ�Ե
				ucThinnedImage[i * iWidth + j] = 0;
			else if (i >= iHeight - 16)//�±�Ե
				ucThinnedImage[i * iWidth + j] = 0;
			else if (j < 16)//���Ե
				ucThinnedImage[i * iWidth + j] = 0;
			else if (j >= iWidth - 16)//�ұ�Ե
				ucThinnedImage[i * iWidth + j] = 0;
		}
	}
	return 0;
}

///*********************************************** Step10 ������ȡ ***********************************************/

/// <summary>
/// ָ��������ȡ
/// </summary>
/// <param name="ucThinImg">Դͼ����</param>
/// <param name="ucMinuImg">���ͼ����</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <returns></returns>
int Extract(unsigned char* ucThinImg, unsigned char* ucMinuImg, int iWidth, int iHeight)
{
	//�������
	unsigned char* pDest = ucMinuImg;//���ͼ����ָ��
	unsigned char* pUp, * pDown, * pImg;//Դͼ����ָ��
	unsigned char x1, x2, x3, x4, x5, x6, x7, x8;//���ڵ�
	unsigned char nc;//���ڵ��кڵ�����
	int iMinuCount = 0;//����������

	//�����ʼ����ȫ��Ϊ0����ʾ���Ƿ������㣩
	memset(pDest, 0, iWidth * iHeight);

	//����Դͼ����ȡָ������
	for (int i = 1; i < iHeight - 1; i++)//���б���
	{
		//��ʼ������ָ��
		pUp = ucThinImg + (i - 1) * iWidth;
		pImg = ucThinImg + i * iWidth;
		pDown = ucThinImg + (i + 1) * iWidth;

		for (int j = 1; j < iWidth - 1; j++)//���б���
		{
			//��������ָ��
			pUp++;
			pImg++;
			pDown++;

			//���������㣨����������ֵΪ0��
			if (!*pImg)
			{
				continue;
			}

			//��ȡ3*3���򴰿�������8���ڵ����صĻҶ�ֵ
			x6 = *(pUp - 1);
			x5 = *(pImg - 1);
			x4 = *(pDown - 1);

			x7 = *pUp;
			x3 = *pDown;

			x8 = *(pUp + 1);
			x1 = *(pImg + 1);
			x2 = *(pDown + 1);

			//ͳ�ư��ڵ�
			//nc = x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8;
			nc = abs(x2 - x1) + abs(x3 - x2) + abs(x4 - x3) + abs(x5 - x4) + abs(x6 - x5) + abs(x7 - x6) + abs(x8 - x7) + abs(x1 - x8);
			//�������ж�
			if (nc == 2)//�˵�
			{
				pDest[i * iWidth + j] = 1;//���ͼ�ж�Ӧ���ص���Ϊ1����ʾ�˵㣩
				++iMinuCount;//������������һ
			}
			else if (nc == 6)//�ֲ��
			{
				pDest[i * iWidth + j] = 3;//���ͼ�ж�Ӧ���ص���Ϊ3����ʾ�ֲ�㣩
				++iMinuCount;//������������һ
			}
		}
	}
	return iMinuCount;//��������������
}

///*********************************************** Step11 �������� ***********************************************/

/// <summary>
/// ����������ṹ
/// </summary>
struct NEIGHBOR
{
	int x;					//�����꣨�����꣩
	int y;					//�����꣨�����꣩
	int type;				//���������ͣ�1-�˵㣬3-�ֲ�㣩
	float Theta;			//�������߽Ƕȣ����ȣ�
	float Theta2Ridge;		//���㼹�߷���нǣ����ȣ�
	float ThetaThisNibor;	//����������ļ��߷��򣨻��ȣ�
	int distance;			//������루����������
};
/// <summary>
/// ������ṹ
/// </summary>
struct MINUTIAE
{
	int x;					//�����꣨�����꣩
	int y;					//�����꣨�����꣩
	int type;				//���������ͣ�1-�˵㣬3-�ֲ�㣩
	float theta;			//�õ㴦�ļ��߷���(����)
	NEIGHBOR* neibors;		//��������������
};
/// <summary>
/// ȥ����Ե������
/// </summary>
/// <param name="minutiaes">����������</param>
/// <param name="count">����������</param>
/// <param name="ucImg">Դͼ����</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <returns></returns>
int CutEdge(MINUTIAE* minutiaes, int count, unsigned char* ucImg, int iWidth, int iHeight)
{
	//�������
	int minuCount = count;
	int x, y, type;
	bool del;

	//��ʼ���������
	int* pFlag = new int[minuCount];//������飨0-������1-ɾ����
	memset(pFlag, 0, sizeof(int) * minuCount);//��ʼ����ȫ0ȫ������

	//��������������
	for (int i = 0; i < minuCount; ++i)
	{
		//��ȡ��ǰ��������Ϣ
		y = minutiaes[i].y - 1;
		x = minutiaes[i].x - 1;
		type = minutiaes[i].type;

		//����ǰ�������ɾ����ǳ�ʼ��Ϊtrue
		del = true;

		//����ǰ�������λ���ж����Ƿ��Ե������
		if (x < iWidth / 2)//���λ��ͼ�����ͼ
		{
			if (abs(iWidth / 2 - x) > abs(iHeight / 2 - y))//���λ�����ͼ���
			{
				//������ͼ�в��ҵ�ǰ������ͬһ������Ƿ�������������
				while (--x >= 0)//��һ���Ʋ���
				{
					//��������������������㣬��˵����ǰ�����㲻�Ǳ�Ե�����㣬������ɾ��
					if (ucImg[x + y * iWidth] > 0)
					{
						del = false;
						break;//ֹͣ��ǰ����������Ʋ���
					}
				}
			}
			else//���λ�����ͼ�Ҳ�
			{
				if (y > iHeight / 2)//���λ�����ͼ���²�
				{
					//������ͼ�в��ҵ�ǰ������ͬһ���²��Ƿ�������������
					while (++y < iHeight)//��һ���Ʋ���
					{
						//������²�������������㣬��˵����ǰ�����㲻�Ǳ�Ե�����㣬������ɾ��
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
				else//���λ�����ͼ���ϲ�
				{
					//������ͼ�в��ҵ�ǰ������ͬһ���ϲ��Ƿ�������������
					while (--y >= 0)//��һ���Ʋ���
					{
						//������ϲ�������������㣬��˵����ǰ�����㲻�Ǳ�Ե�����㣬������ɾ��
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
			}
		}
		else//���λ��ͼ���Ұ�ͼ
		{
			if (abs(iWidth / 2 - x) > abs(iHeight / 2 - y))//���λ���Ұ�ͼ�Ҳ�
			{
				// ������ͼ�в��ҵ�ǰ������ͬһ���Ҳ��Ƿ�������������
				while (++x < iWidth)//��һ���Ʋ���
				{
					//������Ҳ�������������㣬��˵����ǰ�����㲻�Ǳ�Ե�����㣬������ɾ��
					if (ucImg[x + y * iWidth] > 0)
					{
						del = false;
						break;
					}
				}
			}
			else//���λ���Ұ�ͼ���
			{
				if (y > iHeight / 2)//���λ���Ұ�ͼ���²�
				{
					//������ͼ�в��ҵ�ǰ������ͬһ���²��Ƿ�������������
					while (++y < iHeight)
					{
						//������²�������������㣬��˵����ǰ�����㲻�Ǳ�Ե�����㣬������ɾ��
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
				else//���λ���Ұ�ͼ���ϲ�
				{
					//������ͼ�в��ҵ�ǰ������ͬһ���ϲ��Ƿ�������������
					while (--y >= 0)
					{
						//������ϲ�������������㣬��˵����ǰ�����㲻�Ǳ�Ե�����㣬������ɾ��
						if (ucImg[x + y * iWidth] > 0)
						{
							del = false;
							break;
						}
					}
				}
			}
		}

		//�����ǰ�������Ǳ�Ե�����㣬��ɾ��
		if (del)
		{
			pFlag[i] = 1;
			continue;
		}
	}

	//����������ṹ����(�ڵ�ǰ�ṹ�����н�������Ч������ǰ��)
	int newCount = 0;//��Ч������������ͬʱ���������Ч�����������±꣩
	for (int i = 0; i < minuCount; ++i)
	{
		if (pFlag[i] == 0)
		{
			memcpy(&minutiaes[newCount], &minutiaes[i], sizeof(MINUTIAE));//������ṹ���帴��
			newCount++;//��Ч�������±����
		}
	}

	delete[]pFlag;
	pFlag = NULL;
	//������Ч����������
	return newCount;
}
/// <summary>
/// ��������ģ���ļ�
/// </summary>
/// <param name="minutiaes">����������</param>
/// <param name="count">����������</param>
/// <param name="fileName">ģ���ļ���</param>
/// <returns></returns>
int SaveMinutiae(MINUTIAE* minutiaes, int count, char* fileName)
{
	//���ļ���������д�뷽ʽ��
	FILE* fp = fopen(fileName, "wb");
	if (!fp)
	{
		return -1;
	}

	//������������Ľṹ����������д���ļ�
	const static int TemplateFlag = 0x3571027f;
	fwrite(&TemplateFlag, sizeof(int), 1, fp);//д������ģ���ļ����
	fwrite(&count, sizeof(int), 1, fp);//д������������

	for (int i = 0; i < count; i++)
	{
		fwrite(&(minutiaes[i]), sizeof(MINUTIAE), 1, fp);//��������Ľṹ����д��
	}
	//�ر��ļ�
	fclose(fp);
	return 0;
}
/// <summary>
/// ��������
/// </summary>
/// <param name="minuData">����ͼ����</param>
/// <param name="thinData">ϸ��ͼ����</param>
/// <param name="minutiaes">����������</param>
/// <param name="minuCount">����������</param>
/// <param name="iWidth">ͼ����</param>
/// <param name="iHeight">ͼ��߶�</param>
/// <returns></returns>
int MinuFilter(unsigned char* minuData, unsigned char* thinData, MINUTIAE* minutiaes, int& minuCount, int iWidth, int iHeight)
{
	//��1��������ϸ��ͼ�и��㷽��
	float* dir = new float[iWidth * iHeight];
	memset(dir, 0, iWidth * iHeight * sizeof(float));

	//ImgDirection(thinData, dir, iWidth, iHeight);//���㼹�߷���

	//��2����������ͼ����ȡ����������
	unsigned char* pImg;
	unsigned char val;
	int temp = 0;
	for (int i = 1; i < iHeight - 1; ++i)
	{
		pImg = minuData + i * iWidth;
		for (int j = 1; j < iWidth - 1; ++j)
		{
			//��ȡ����ͼ����
			++pImg;//����ͼָ�����
			val = *pImg;//����ͼ����ֵ��0-�������㣬1-�˵㣬3-�ֲ�㣩
			//��ȡ����������
			if (val > 0)
			{
				minutiaes[temp].x = j + 1;//�����꣨��1��ʼ��
				minutiaes[temp].y = i + 1;//�����꣨��1��ʼ��
				minutiaes[temp].theta = dir[i * iWidth + j];//���߷���
				minutiaes[temp].type = int(val);//����������
				++temp;//����������ָ�����
			}
		}
	}
	delete[]dir;

	//��3����ȥ����Ե������
	minuCount = CutEdge(minutiaes, minuCount, thinData, iWidth, iHeight);

	//��4����ȥ��ë��/С��/��ϵ�α������
	//��ʼ���������
	int* pFlag = new int[minuCount];//0-������1-ɾ��
	memset(pFlag, 0, sizeof(int) * minuCount);

	//��������������
	int x1, x2, y1, y2, type1, type2;
	//������1����
	for (int i = 0; i < minuCount; ++i)
	{
		//��ȡ������1����Ϣ
		x1 = minutiaes[i].x;
		y1 = minutiaes[i].y;
		type1 = minutiaes[i].type;
		//������2����
		for (int j = i + 1; j < minuCount; ++j)
		{
			//������ɾ������
			if (pFlag[j] == 1)
				continue;

			//��ȡ������2����Ϣ
			x2 = minutiaes[j].x;
			y2 = minutiaes[j].y;
			type2 = minutiaes[j].type;

			//����������
			int r = (int)sqrt(float((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2)));

			//ɾ������С��������
			if (r <= 4)//�����಻����4����Ϊ����С
			{
				if (type1 == type2)//�������������ͬ
				{
					if (type1 == 1)//���Ƕ˵㣬��Ϊ�ǡ����߻����߼�ϡ�
						pFlag[i] = pFlag[j] = 1;//ͬʱɾ������
					else//���Ƿֲ�㣬��Ϊ�ǡ�С�ס�
						pFlag[j] = 1;//ֻɾ��2
				}
				else if (type1 == 1)//1�Ƕ˵� 2�Ƿֲ�� ��1Ϊ��ë�̡�
					pFlag[i] = 1;//ֻɾ��1
				else//1�Ƿֲ�� 2�Ƕ˵� ��2Ϊ��ë�̡�
					pFlag[j] = 1;//ֻɾ��2
			}
		}
	}

	//����������ṹ����(�ڵ�ǰ�ṹ�����н�������Ч������ǰ��)
	int newCount = 0;//��Ч������������ͬʱ���������Ч�����������±꣩
	for (int i = 0; i < minuCount; ++i)
	{
		if (pFlag[i] == 0)
		{
			memcpy(&minutiaes[newCount], &minutiaes[i], sizeof(MINUTIAE));//������ṹ���帴��
			newCount++;//��Ч�������±����
		}
	}
	delete[]pFlag;
	minuCount = newCount;

	//���ؽ��
	return 0;
}

///*********************************************** Step12 ������� ***********************************************/

/// <summary>
/// �����ָ�Ʊ��
/// </summary>
/// <returns></returns>
int GetNewIndexDB()
{
	//��index�ļ��ж�ȡ���һ����¼�ı��
	int sNo = 0;
	char no[MAX_PATH] = { 0 }, name[MAX_PATH] = { 0 }, srcFile[MAX_PATH] = { 0 }, mdlFile[MAX_PATH] = { 0 };

	FILE* index = fopen(DB_INDEX_TXT, "r");
	while (!feof(index))
	{
		fscanf(index, "%d %s %s %s %s\n", &sNo, srcFile, mdlFile, no, name);
	}
	fclose(index);

	//�����±��
	sNo = sNo + 1;
	//�����±��
	return sNo;
}
/// <summary>
/// ���ַ���ת��Ϊ���ֽ��ַ���
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

///*********************************************** Step13 ����ƥ�� ***********************************************/

/// <summary>
/// �����߶���б��
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
/// �������������ڹ�ϵ
/// </summary>
/// <param name="minutiae"></param>
/// <param name="minuCount"></param>
/// <returns></returns>
int BuildNabors(MINUTIAE* minutiae, int minuCount) {
	//�������
	const int MAX_NEIGHBOR_EACH = 10;//ÿ����������ౣ��10������������
	int x1, x2, y1, y2;
	int* pFlag = new int[minuCount];//���ڱ�����飨���ֵ��0-�����ڣ�1-���ڣ�

	//��������������
	for (int i = 0; i < minuCount; i++) {
		//��ȡ��ǰ��������Ϣ
		x1 = minutiae[i].x;
		y1 = minutiae[i].y;

		//��ʼ����ǰ����������ڱ������
		memset(pFlag, 0, sizeof(int) * minuCount);//��ʼ��Ϊȫ0�������ڣ�
		pFlag[i] = 1;//��������Ϊ�����ڡ�

		 //Ϊ��ǰ�����㴴������ʼ������������ṹ����
		minutiae[i].neibors = new NEIGHBOR[MAX_NEIGHBOR_EACH];//��������������ṹ����
		if (minutiae[i].neibors == NULL) {
			return -1;
		}
		memset(minutiae[i].neibors, 0, sizeof(NEIGHBOR) * MAX_NEIGHBOR_EACH);//��ʼ������

		 //���Һͱ���10������������
		for (int neighborNo = 0; neighborNo < MAX_NEIGHBOR_EACH; neighborNo++) {//�ظ�10��
			//��ʼ����С����Ͷ�Ӧ�������±�
			int minDistance = 1000;//��С���
			int minNo = 0;//��С����Ӧ���������±�

			 //��������������֮�������Ĳ�����������
			for (int j = 0; j < minuCount; j++) { //ÿ�ζ���������������
				//�������ҵ�������������
				if (pFlag[j] == 1) {//(���ֵ��0-�����ڣ�1-����)
					continue;
				}

				//��ȡ������2����Ϣ
				x2 = minutiae[j].x;
				y2 = minutiae[j].y;

				//����������
				int r = (int)sqrt(float((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2)));

				//������С���
				if (r < minDistance) {
					minNo = j;
					minDistance = r;

				}
			}

			//������ҽ��
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
/// ����ƥ�����ƶ��㷨
/// </summary>
/// <param name="minutiae1"></param>
/// <param name="count1"></param>
/// <param name="minutiae2"></param>
/// <param name="count2"></param>
/// <returns></returns>
float MinuSimilarity(MINUTIAE* minutiae1, int count1, MINUTIAE* minutiae2, int count2) {

	const int MAX_SIMILAR_PAIR = 200;//��ౣ��200���������������
	const int MAX_NEIGHBOR_EACH = 10;//ÿ����������ౣ��10������������

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

			//ͳ���������ڵ�����
			int thisSimilarNeigbors = 0;
			for (int m = 0; m < MAX_NEIGHBOR_EACH; m++) {
				for (int n = 0; n < MAX_NEIGHBOR_EACH; n++) {
					//�������Ͳ�ͬ�����ڵ�
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
			//���5���������ڵ����ƣ�����Ϊ��ǰ��׼���뵱ǰ�ο������ƣ�����ƥ������
			if ((thisSimilarNeigbors >= MAX_NEIGHBOR_EACH * 5 / 10) && (similarMinutiae < MAX_SIMILAR_PAIR)) {
				similarPair[similarMinutiae][0] = i;
				similarPair[similarMinutiae][1] = refSimilarNo;
				++similarMinutiae;
			}
		}
	}
	//��������ƥ�����ƶ�
	float similarity = similarMinutiae / 200.0f;
	similarity = similarMinutiae < 80 ? 0.0f : similarity;
	similarity = similarMinutiae > 200 ? 1.0f : similarity;

	return similarity;
}
/// <summary>
/// ��ȡ����ģ���ļ�
/// </summary>
/// <param name="fileName">����ģ���ļ���</param>
/// <param name="minutiae">����������</param>
/// <returns></returns>
int ReadMinutiae(char* fileName, MINUTIAE** minutiae)
{
	//���ļ��������ƶ�ȡ��ʽ��
	FILE* fp = fopen(fileName, "rb");
	if (!fp) {
		return -1;
	}
	//��һ�����ȡ����������Ľṹ������
	const static int TemplateFileFlag = 0x3571027f;
	int flag;
	fread(&flag, sizeof(int), 1, fp);//��ȡ����ģ���ļ����
	if (flag != TemplateFileFlag)
	{
		return -2;
	}

	int account;
	fread(&account, sizeof(int), 1, fp);//��ȡ����������
	*minutiae = new MINUTIAE[account];//����������ṹ����
	if (*minutiae == NULL)
	{
		return -3;
	}

	for (int i = 0; i < account; i++) {
		fread(&((*minutiae)[i]), sizeof(MINUTIAE), 1, fp);//�����ȡ������ṹ
	}

	//�ر��ļ�
	fclose(fp);
	return account;
}

///*********************************************** Step14 ָ��ʶ�� ***********************************************/

/// <summary>
/// ָ�ƿ�Ԥ�죨�ж�ָ�ƿ��Ƿ�Ϊ�գ�
/// </summary>
/// <returns></returns>
bool EmptyDB()
{
	//���ļ�
	char indexFile[MAX_PATH] = { DB_INDEX_TXT };//ָ�ƿ������ļ�
	FILE* pFile = fopen(indexFile, "r");
	if (!pFile)//�������ļ������ڻ�����쳣����ָ�ƿ�Ϊ��
	{
		return true;
	}

	//�ж�ָ�ƿ��Ƿ�Ϊ�գ������ļ������Ƿ�Ϊ�գ�
	int size = _filelength(_fileno(pFile));//�ļ����ȣ����ֽ�Ϊ��λ��

	bool bEmpty = (size < 1) ? true : false;//�������ļ�����Ϊ�գ���ָ�ƿ�Ϊ��

	//�ر��ļ�
	fclose(pFile);
	//�����жϽ��
	return bEmpty;
}

///*********************************************** Step15 ָ�ƵǼǼ��� ***********************************************/

/// <summary>
/// ָ�ƵǼǹ��̼���
/// </summary>
/// <param name="regName">�Ǽ�������</param>
/// <param name="srcImgFile">ԭͼ�ļ���</param>
/// <param name="outImgFile">�������ͼ�ļ���</param>
/// <param name="info">���ز�����ʧ����ʾ��Ϣ</param>
/// <param name="bSaveImg">�Ƿ񱣴������м���ͼ��</param>
/// <returns></returns>
int Enroll(char*regNo, char* regName, char* srcImgFile, char* outImgFile, char* info, bool bSaveImg)
{
	//��0���������м����ļ���
	//�м��������ļ���
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

	//�м���ͼ���ļ���
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

	//ָ������ģ����
	char stepMdlFile11[MAX_PATH] = { STEP_IMG_11_MDL };

	//��1����ͼ�����룬��ȡԴͼ���ݺͲ�����Ϣ
	int iWidth, iHeight, iDepth;
	int flag=ReadBMPImgFilePara(srcImgFile, iWidth, iHeight, iDepth);
	if (flag != 0) {
		sprintf(info, "ͼ�����ʧ��.");
		return -1;
	}
	unsigned char* image1 = new unsigned char[iWidth * iHeight];
	unsigned char* image2 = new unsigned char[iWidth * iHeight];
	flag=ReadBMPImgFileData(srcImgFile, image1);//��ȡԴͼ����
	if (flag != 0)
	{
		sprintf(info, "ͼ�����ݶ�ȡʧ��.");
		return -1;
	}
	memset(image2, 0, iWidth * iHeight);//���ͼ��ʼ��
	if (bSaveImg)
	{
		CopyFile(ToWideChar(srcImgFile), ToWideChar(stepImgFile1), false);
		SaveDataToTextFile(stepTxtFile1, image1, iWidth, iHeight);

	}
	//��2������ֵ�˲�
	MidFilter(image1, image2, iWidth, iHeight);//��ֵ�˲�
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile2, image2);
		SaveDataToTextFile(stepTxtFile2, image2, iWidth, iHeight);
	}
	//��3����ֱ��ͼ���⻯
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�2���˲����ΪԴͼ����
	HistoNormalize(image1, image2, iWidth, iHeight);//ֱ��ͼ���⻯
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile3, image2);
		SaveDataToTextFile(stepTxtFile3, image2, iWidth, iHeight);
	}
	//��4�����������
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�3�����⻯���ΪԴͼ����
	float* tmpDirection = new float[iWidth * iHeight];
	float* direction = new float[iWidth * iHeight];
	ImgDirection(image1, tmpDirection, iWidth, iHeight);//�������
	DircLowPass(tmpDirection, direction, iWidth, iHeight);//�����ͨ�˲�
	if (bSaveImg)
	{
		const int DIRECTION_SCALE = 100;//������ת�������������ڽ����ʾ��
		SaveDataToImageFile(srcImgFile, stepImgFile4, direction, DIRECTION_SCALE);
		SaveDataToTextFile(stepTxtFile4, direction, iWidth, iHeight);
	}
	delete[] tmpDirection;
	//��5����Ƶ�ʼ���
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�3�����⻯���ΪԴͼ����
	float* frequency = new float[iWidth * iHeight];
	Frequency(image1, direction, frequency, iWidth, iHeight);//Ƶ�ʼ���
	if (bSaveImg)
	{
		const int FREQUENCY_SCALE = 1000;//Ƶ�ʽ��ת�������������ڽ����ʾ��
		SaveDataToImageFile(srcImgFile, stepImgFile5, frequency, FREQUENCY_SCALE);
		SaveDataToTextFile(stepTxtFile5, frequency, iWidth, iHeight);
	}
	//��6�����������
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�3�����⻯���ΪԴͼ����
	unsigned char* mask = new unsigned char[iWidth * iHeight];
	GetMask(image1, direction, frequency, mask, iWidth, iHeight); //�������
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile6, mask);
		SaveDataToTextFile(stepTxtFile6, mask, iWidth, iHeight);
	}
	//��7����Gabor�˲���ǿ
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�3�����⻯���ΪԴͼ����
	GaborEnhance(image1, direction, frequency, mask, image2, iWidth, iHeight); //��ǿ
	if (bSaveImg)
	{
		SaveDataToImageFile(srcImgFile, stepImgFile7, image2);
		SaveDataToTextFile(stepTxtFile7, image2, iWidth, iHeight);
	}
	delete[]direction;
	delete[]frequency;
	delete[]mask;
	//��8������ֵ��
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�7��Gabor�˲����ΪԴͼ����
	BinaryImg(image1, image2, iWidth, iHeight, 128); //��ֵ��
	if (bSaveImg)
	{
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile8, image1);
		SaveDataToTextFile(stepTxtFile8, image2, iWidth, iHeight);
	}
	//��9����ϸ��
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�8����ֵ�����ΪԴͼ����
	Thinning(image1, image2, iWidth, iHeight, 128); //ϸ��
	if (bSaveImg)
	{
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile9, image1);
		SaveDataToTextFile(stepTxtFile9, image2, iWidth, iHeight);
	}
	unsigned char* thin = new unsigned char[iWidth * iHeight];
	memcpy(thin, image2, sizeof(unsigned char) * iWidth * iHeight);//����ϸ�����
	//��10����������ȡ
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�9��ϸ�����ΪԴͼ����
	int minuCount = Extract(image1, image2, iWidth, iHeight); //������ȡ
	if (bSaveImg)
	{
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile10, image1);
		SaveDataToTextFile(stepTxtFile10, image2, iWidth, iHeight);
	}
	//��11������������
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);//�Ե�10��������ȡ���ΪԴͼ����
	MINUTIAE* minutiaes = new MINUTIAE[minuCount];
	memset(minutiaes, sizeof(MINUTIAE), minuCount);
	MinuFilter(image1, thin, minutiaes, minuCount, iWidth, iHeight);//��������

	/*******************************************************/
	/*���������˽�����浽outImageFile*/
	unsigned char* image3 = new unsigned char[iWidth * iHeight];
	memset(image3, 0, iWidth * iHeight);
	for (int i = 0; i < minuCount; i++)
	{
		image3[(minutiaes[i].y - 1) * iWidth + (minutiaes[i].x - 1)] = 0xff;
	}

	SaveDataToImageFile(srcImgFile, outImgFile, image3);
	/*******************************************************/

	delete[]thin;

	SaveMinutiae(minutiaes, minuCount, stepMdlFile11);//�������������ļ�

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

	//��12�����������
	int sNo = GetNewIndexDB();//������ݿ���ָ�Ƽ�¼���
	char dbImgFile[MAX_PATH] = { 0 };//���ݿ���Դͼ�ļ���
	char dbMdlFile[MAX_PATH] = { 0 };//���ݿ���ģ���ļ���
	sprintf(dbImgFile, "%s%d.bmp", DB_DIR, sNo);//����Դͼ�ļ������������ݿ��ļ����ڣ����Ա������
	sprintf(dbMdlFile, "%s%d.mdl", DB_DIR, sNo);//����ģ���ļ������������ݿ��ļ����ڣ����Ա������

	if (minuCount > 0)
	{
		//����ԭʼָ��ͼ��
		CopyFile(ToWideChar(srcImgFile), ToWideChar(dbImgFile), false);
		//����ָ�ƿ�����ģ���ļ�
		CopyFile(ToWideChar(stepMdlFile11), ToWideChar(dbMdlFile), false);

		//ָ�ƵǼ���Ϣд�����ݿ������ļ�
		FILE* index = fopen(DB_INDEX_TXT, "a");//׷��ģʽд��
		fprintf(index, "%d %s %s %s %s\n", sNo, dbImgFile, dbMdlFile,regNo, regName);
		fclose(index);
	}
	delete[]image1;
	delete[]image2;
	delete[]minutiaes;
	delete[]image3;

	sprintf(info, "ָ�ƵǼǳɹ���");

	return 0;
}

///*********************************************** Step16 ָ��ʶ�𼯳� ***********************************************/

/// <summary>
/// ָ��ʶ����̼���
/// </summary>
/// <param name="srcImgFile">Դͼ�ļ���</param>
/// <param name="outImgFile">ʶ����ͼ�ļ���</param>
/// <param name="info">���ز�����ʧ����ʾ��Ϣ</param>
/// <param name="bSaveImg">�Ƿ񱣴������м���ͼ��</param>
/// <returns></returns>
int Identify(char* srcImgFile, char* outImgFile, char* info, bool bSaveImg)
{
	//��0���������м����ļ���
	//�м��������ļ���
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

	//�м���ͼ���ļ���
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

	//ָ������ģ���ļ���
	char stepMdlFile11[MAX_PATH] = { STEP_IMG_11_MDL };

	//ָ�ƿ�Ԥ�죨�ж�ָ�ƿ��Ƿ�Ϊ�գ�
	bool j = EmptyDB();
	if (j) {
		sprintf(info, "ָ�ƿ�Ϊ��");
		return 0;
	}

	//��1����ͼ�����룬��ȡԴͼ���ݺͲ�����Ϣ
	int iWidth, iHeight, iDepth;
	int flag=ReadBMPImgFilePara(srcImgFile, iWidth, iHeight, iDepth);
	if (flag != 0)
	{
		sprintf(info, "ͼ�����ʧ��.");
		return -1;
	}
	unsigned char* image1 = new unsigned char[iWidth * iHeight];
	unsigned char* image2 = new unsigned char[iWidth * iHeight];
	flag=ReadBMPImgFileData(srcImgFile, image1);
	if (flag != 0)
	{
		sprintf(info, "ͼ�����ݶ�ȡʧ��.");
		return -1;
	}
	memset(image2, 0, iWidth * iHeight);
	if (bSaveImg) {
		CopyFile(ToWideChar(srcImgFile), ToWideChar(stepImgFile1), false);
		SaveDataToTextFile(stepTxtFile1, image1, iWidth, iHeight);
	}
	//��2������ֵ�˲�
	MidFilter(image1, image2, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile2, image2);
		SaveDataToTextFile(stepTxtFile2, image2, iWidth, iHeight);
	}
	//��3����ֱ��ͼ���⻯
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	HistoNormalize(image1, image2, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile3, image2);
		SaveDataToTextFile(stepTxtFile3, image2, iWidth, iHeight);
	}

	//��4�����������
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

	//��5����Ƶ�ʼ���
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	float* frequency = new float[iWidth * iHeight];
	Frequency(image1, direction, frequency, iWidth, iHeight);
	if (bSaveImg) {
		const int FREQUENCY_SCALE = 1000;
		SaveDataToImageFile(srcImgFile, stepImgFile5, frequency, FREQUENCY_SCALE);
		SaveDataToTextFile(stepTxtFile5, frequency, iWidth, iHeight);
	}

	//��6�����������
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	unsigned char* mask = new unsigned char[iWidth * iHeight];
	GetMask(image1, direction, frequency, mask, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile6, mask);
		SaveDataToTextFile(stepTxtFile6, mask, iWidth, iHeight);
	}

	//��7����Gabor�˲���ǿ
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	GaborEnhance(image1, direction, frequency, mask, image2, iWidth, iHeight);
	if (bSaveImg) {
		SaveDataToImageFile(srcImgFile, stepImgFile7, image2);
		SaveDataToTextFile(stepTxtFile7, image2, iWidth, iHeight);
	}
	delete[] direction;
	delete[] frequency;
	delete[] mask;

	//��8������ֵ��
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	BinaryImg(image1, image2, iWidth, iHeight, 128);
	if (bSaveImg) {
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile8, image1);
		SaveDataToTextFile(stepTxtFile8, image2, iWidth, iHeight);
	}

	//��9����ϸ��
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	Thinning(image1, image2, iWidth, iHeight, 128);
	if (bSaveImg) {
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile9, image1);
		SaveDataToTextFile(stepTxtFile9, image2, iWidth, iHeight);
	}
	unsigned char* thin = new unsigned char[iWidth * iHeight];
	memcpy(thin, image2, sizeof(unsigned char) * iWidth * iHeight);

	//��10����������ȡ
	memcpy(image1, image2, sizeof(unsigned char) * iWidth * iHeight);
	int minuCount = Extract(image1, image2, iWidth, iHeight);
	if (bSaveImg) {
		BinaryToGray(image2, image1, iWidth, iHeight);
		SaveDataToImageFile(srcImgFile, stepImgFile10, image1);
		SaveDataToTextFile(stepTxtFile10, image2, iWidth, iHeight);
	}

	//��11������������
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

	//��12��������ʶ��
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

		//������ģ���ļ��ж�ȡָ������
		minuCount2 = ReadMinutiae(mdl, &minutiaes2);

		//����ƥ��
		float similar = MinuSimilarity(minutiaes, minuCount, minutiaes2, minuCount2);

		//�������ƶ���ߵ�ָ�Ƽ�¼��Ϣ
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

	//����ʶ����
	const float SIMILAR_THRED = 0.1;
	if (maxID < 0 || maxSimilar < SIMILAR_THRED) {
		sprintf(info, "ʶ��ʧ��");
		return -12;
	}

	strcpy(outImgFile, maxSrc);

	if (bSaveImg) {
		CopyFile(ToWideChar(maxSrc), ToWideChar(stepImgFile12), false);
	}
	sprintf(info, "ʶ��ɹ�.ʶ���������[%s],����[%s],Ŀ��ָ��[%s],���ƶ�[%.2f]",maxNo, maxName, maxSrc, maxSimilar);
	return 0;
}

