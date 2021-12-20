
// MyFingerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MyFinger.h"
#include "MyFingerDlg.h"
#include "afxdialogex.h"
#include "Myfunction.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyFingerDlg 对话框



CMyFingerDlg::CMyFingerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYFINGER_DIALOG, pParent)
	, m_name(_T(""))
	, m_id(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyFingerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_STATIC_IMG_1, m_picCtrl1);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_ZKFPENGX, m_zkfpEng);
	DDX_Text(pDX, IDC_EDIT_ID, m_id);
}

BEGIN_MESSAGE_MAP(CMyFingerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ADD_IMAGE, &CMyFingerDlg::OnBnClickedBtnAddImage)
	ON_BN_CLICKED(IDC_BTN_MATCH_IMAGE, &CMyFingerDlg::OnBnClickedBtnMatchImage)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CMyFingerDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CMyFingerDlg::OnBnClickedBtnCapture)
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CMyFingerDlg::OnBnClickedBtnRegister)
	ON_BN_CLICKED(IDC_IDENTIFY, &CMyFingerDlg::OnBnClickedIdentify)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CMyFingerDlg::OnBnClickedBtnDisconnect)
	ON_BN_CLICKED(IDC_BTN_DATABASE, &CMyFingerDlg::OnBnClickedBtnDatabase)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CMyFingerDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CMyFingerDlg::OnBnClickedBtnLoad)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CMyFingerDlg, CDialog)
	ON_EVENT(CMyFingerDlg, IDC_ZKFPENGX, 8, OnImageReceivedZkfpengx, VTS_PBOOL)
END_EVENTSINK_MAP()

// CMyFingerDlg 消息处理程序

BOOL CMyFingerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitDatabase();//创建并初始化指纹库
	GetDlgItem(IDC_BTN_ADD_IMAGE)->EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyFingerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyFingerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/// <summary>
/// 显示采集器获取的图像
/// </summary>
/// <param name="AImageValid"></param>
void CMyFingerDlg::OnImageReceivedZkfpengx(BOOL* AImageValid)
{
	HDC hdc = this->GetDC()->m_hDC;//获得显示设备上下文环境的句柄
	CRect rect;//定义矩形对象
	GetDlgItem(IDC_STATIC_IMG_1)->GetWindowRect(&rect);//获得picture控件所在的矩形区域
	ScreenToClient(rect);
	int x = rect.TopLeft().x;//图像绘制区左上角横坐标
	int y = rect.TopLeft().y;;//图像绘制区左上角纵坐标
	//int width = m_zkfpEng.get_ImageWidth();//图像绘制区宽度
	//int height = m_zkfpEng.get_ImageHeight();//图像绘制区高度
	int width = rect.Width();
	int height = rect.Height();
	CDC* pDc = GetDlgItem(IDC_STATIC_IMG_1)->GetWindowDC();//获得picture控件的设备环境句柄
	SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);//设置位图拉伸模式
	m_zkfpEng.PrintImageAt(int(hdc), x, y, width, height);//绘制图像
	//GetDlgItem(IDC_STATIC_IMG_1).ReleaseDC(pDc);//释放picture控件的设备环境句柄
}
/// <summary>
/// （图像）载入
/// </summary>
void CMyFingerDlg::OnBnClickedBtnLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	//初始化操作结果信息
	char info[MAX_PATH] = { 0 };
	char filename[MAX_PATH];
	//选择图像
	char srcImgFile[MAX_PATH] = { LOADTMP };
	CString filter;
	CString name;
	filter = "图像文件(*.bmp)|*.bmp||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		name = dlg.GetPathName();
		WideCharToMultiByte(CP_ACP, 0, name.GetBuffer(0), name.GetLength(), filename, MAX_PATH, 0, 0);
		filename[name.GetLength()] = '\0';
	}
	else
	{
		m_staticInfo.SetWindowTextW(_T(""));
		return;
	}
	int e = 0;
	//读取图像参数并检测有效性
	int iWidth, iHeight, iDepth;
	e= ReadBMPImgFilePara(filename, iWidth, iHeight, iDepth);
	if (e != 0)
	{
		sprintf(info, "读取图像参数错误。\n源图[%s],宽度[%d],高度[%d],深度[%d b]", filename, iWidth, iHeight, iDepth);
		//显示操作结果信息
		m_staticInfo.SetWindowTextW(ToWideChar(info));
		GetDlgItem(IDC_BTN_ADD_IMAGE)->EnableWindow(FALSE);
		return;
	}
	//复制图像
	CopyFile(name, ToWideChar(srcImgFile), false);
	//显示图像
	GetDlgItem(IDC_STATIC_IMG_1)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_IMG_1)->ShowWindow(TRUE);
	e = ShowImageInCtrl(m_picCtrl1, srcImgFile);
	if (e != 0)
	{
		sprintf(info, "显示图像错误");
		//显示操作结果信息
		m_staticInfo.SetWindowTextW(ToWideChar(info));
		GetDlgItem(IDC_BTN_ADD_IMAGE)->EnableWindow(FALSE);
	}
	else
	{
		sprintf(info, "载入图像成功, 可以进行指纹登记。\n源图[%s],宽度[%d],高度[%d],深度[%d b]", filename, iWidth, iHeight, iDepth);
		//显示操作结果信息
		m_staticInfo.SetWindowTextW(ToWideChar(info));
		GetDlgItem(IDC_BTN_ADD_IMAGE)->EnableWindow(TRUE);
	}
}
/// <summary>
/// （图像）登记
/// </summary>
void CMyFingerDlg::OnBnClickedBtnAddImage()
{
	// TODO: 在此添加控件通知处理程序代码
	//初始化操作结果信息
	char info[MAX_PATH] = { 0 };
	m_staticInfo.SetWindowTextW(_T(""));
	//获得界面输入内容（登记人姓名，学号）
	UpdateData(true);
	CString tmpnum("0123456789");
	if (m_id.SpanIncluding(tmpnum) != m_id||m_id.GetLength()<1)
	{
		MessageBox(_T("输入的编号格式不正确"), _T("提示"));
		return;
	}
	if (m_name.GetLength()<1)
	{
		MessageBox(_T("请输入姓名"), _T("提示"));
		return;
	}
	//选择图像
	char srcImgFile[MAX_PATH] = {LOADTMP};
	//指纹登记
	bool bSaveImg = true;//是否保存中间结果图像
	char outImgFile[MAX_PATH] = { OUTTMP };
	int e = 0;
	//指纹登记
	m_staticInfo.SetWindowText(_T("开始指纹登记。。。"));
	e=Enroll(ToChar(m_id.GetBuffer()), ToChar(m_name.GetBuffer()), srcImgFile, outImgFile, info, bSaveImg);
	//显示操作结果信息
	m_staticInfo.SetWindowTextW(ToWideChar(info));
	m_name.Empty();
	m_id.Empty();
	UpdateData(FALSE);
	
	if (e != 0)
		MessageBox(ToWideChar(info), _T("错误"));
	else
	{
		//显示图像
		ShowImageInCtrl(m_picCtrl1, outImgFile);
		GetDlgItem(IDC_BTN_ADD_IMAGE)->EnableWindow(FALSE);
	}
}
/// <summary>
/// （图像）识别
/// </summary>
void CMyFingerDlg::OnBnClickedBtnMatchImage()
{
	// TODO: 在此添加控件通知处理程序代码
	//初始化操作结果信息
	char info[MAX_PATH] = { 0 };
	m_staticInfo.SetWindowTextW(_T(""));
	//选择图像
	char srcImgFile[MAX_PATH] = {LOADTMP};
	CString filter;
	CString name;
	filter = "图像文件(*.bmp)|*.bmp||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		name = dlg.GetPathName();
	}
	else
	{
		return;
	}
	//复制图像
	CopyFile(name, ToWideChar(srcImgFile), false);
	GetDlgItem(IDC_STATIC_IMG_1)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_IMG_1)->ShowWindow(TRUE);
	ShowImageInCtrl(m_picCtrl1, srcImgFile);//显示原始指纹图像
	//指纹识别
	bool bSaveImg = true; // 是否保存中间结果图像
	char outImgFile[MAX_PATH] = { OUTTMP };
	int e = 0;
	m_staticInfo.SetWindowTextW(_T("开始指纹识别。。。"));
	GetDlgItem(IDC_BTN_MATCH_IMAGE)->EnableWindow(FALSE);
	e=Identify(srcImgFile, outImgFile, info, bSaveImg);
	//显示操作结果信息
	m_staticInfo.SetWindowTextW(ToWideChar(info));
	
	if (e != 0)
		MessageBox(ToWideChar(info), _T("错误"));
	else
	{
		//显示图像
		ShowImageInCtrl(m_picCtrl1, outImgFile);
	}
	GetDlgItem(IDC_BTN_MATCH_IMAGE)->EnableWindow(TRUE);
}
/// <summary>
/// （指纹采集器）启动
/// </summary>
void CMyFingerDlg::OnBnClickedBtnConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_zkfpEng.InitEngine() == 0)//启动 指纹采集器
	{
		MessageBox(_T("采集器连接成功。"), _T("提示"));//显示信息
		GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
	}
	else
	{
		MessageBox(_T("采集器连接失败。"), _T("提示"));
	}
}
/// <summary>
/// （指纹采集器）采集
/// </summary>
void CMyFingerDlg::OnBnClickedBtnCapture()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_zkfpEng.SaveBitmap(_T("capt.bmp"));//保存图像
	CString strFilter, str, m_strTmpFile;
	CStdioFile cfLogFile;

	strFilter = "图像文件(*.bmp)|*.bmp||";
	CFileDialog TmpDlg(FALSE, _T("bmp"), _T("captImg.bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter.GetBuffer(), this);
	if (TmpDlg.DoModal() == IDOK)
	{
		//获取文件路径
		m_strTmpFile = TmpDlg.GetPathName();
		str = TmpDlg.GetFileName();
		//
		m_zkfpEng.SaveBitmap(m_strTmpFile);
	}
}
/// <summary>
/// （指纹采集器）登记
/// </summary>
void CMyFingerDlg::OnBnClickedBtnRegister()
{
	// TODO: 在此添加控件通知处理程序代码
	//初始化操作结果信息
	char info[MAX_PATH] = { 0 };
	m_staticInfo.SetWindowTextW(_T(""));
	//获得界面输入内容（登记人姓名，学号）
	UpdateData(true);
	CString tmpnum("0123456789");
	if (m_id.SpanIncluding(tmpnum) != m_id)
	{
		sprintf(info, "输入的编号格式不正确");
		m_staticInfo.SetWindowTextW(ToWideChar(info));
		return;
	}
	if (m_name.GetLength() < 1)
	{
		sprintf(info, "请输入姓名");
		m_staticInfo.SetWindowTextW(ToWideChar(info));
		return;
	}
	//实时采集图像
	char srcImgFile[MAX_PATH] = "capt.bmp";
	m_zkfpEng.SaveBitmap(ToWideChar(srcImgFile));
	//指纹登记
	bool bSaveImg = true;//是否保存中间结果
	char outImgFile[MAX_PATH] = { 0 };//指纹特征结果图像文件名
	int e = 0;
	//指纹登记
	m_staticInfo.SetWindowTextW(_T("开始指纹登记。。。"));
	e=Enroll(ToChar(m_id.GetBuffer()), ToChar(m_name.GetBuffer()), srcImgFile, outImgFile, info, bSaveImg);
	//显示操作结果
	m_staticInfo.SetWindowTextW(ToWideChar(info));
	//显示图像
	ShowImageInCtrl(m_picCtrl1, outImgFile);
	if (e != 0)
		MessageBox(ToWideChar(info), _T("错误"));
}
/// <summary>
/// （指纹采集器）识别
/// </summary>
void CMyFingerDlg::OnBnClickedIdentify()
{
	// TODO: 在此添加控件通知处理程序代码
	//初始化操作结果信息
	char info[MAX_PATH] = { 0 };
	m_staticInfo.SetWindowTextW(_T(""));
	//选择图像
	char srcImgFile[MAX_PATH]="capt.bmp";
	m_zkfpEng.SaveBitmap(ToWideChar(srcImgFile));
	//指纹识别
	bool bSaveImg = true; // 是否保存中间结果图像
	char outImgFile[MAX_PATH] = { 0 };
	int e = 0;
	m_staticInfo.SetWindowTextW(_T("开始指纹识别。。。"));
	e=Identify(srcImgFile, outImgFile, info, bSaveImg);
	//显示操作结果信息
	m_staticInfo.SetWindowTextW(ToWideChar(info));
	//显示图像
	ShowImageInCtrl(m_picCtrl1, outImgFile);
	if (e != 0)
		MessageBox(ToWideChar(info), _T("错误"));
}
/// <summary>
/// （指纹采集器）停止
/// </summary>
void CMyFingerDlg::OnBnClickedBtnDisconnect()
{
	// TODO: 在此添加控件通知处理程序代码
	m_zkfpEng.EndEngine();//停止指纹采集器
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
	m_staticInfo.SetWindowTextW(_T("指纹采集器已停止"));
}
/// <summary>
/// 指纹库信息
/// </summary>
void CMyFingerDlg::OnBnClickedBtnDatabase()
{
	// TODO: 在此添加控件通知处理程序代码
	char info[MAX_PATH] = { 0 };//初始化显示信息
	GetDatabaseInfo(info);//获取指纹库信息
	m_staticInfo.SetWindowTextW(ToWideChar(info));//显示指纹库信息
}
/// <summary>
/// 退出
/// </summary>
void CMyFingerDlg::OnBnClickedBtnExit()
{
	// TODO: 在此添加控件通知处理程序代码
	if(MessageBox(_T("确定要退出该系统吗？"),_T("提示"),MB_OKCANCEL)==IDOK)
		OnOK();//关闭当前对话框（系统界面）
}


