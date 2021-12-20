
// MyFingerDlg.h: 头文件
//

#pragma once
#include "CZKFPEngX.h"//引入指纹采集器SDK

// CMyFingerDlg 对话框
class CMyFingerDlg : public CDialogEx
{
// 构造
public:
	CMyFingerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFINGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CZKFPEngX m_zkfpEng;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnImageReceivedZkfpengx(BOOL FAR* AImageValid);
public:
	// info消息框
	CStatic m_staticInfo;
	CStatic m_picCtrl1;
	// 姓名
	CString m_name;
	afx_msg void OnBnClickedBtnAddImage();
	afx_msg void OnBnClickedBtnMatchImage();
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnRegister();
	afx_msg void OnBnClickedIdentify();
	afx_msg void OnBnClickedBtnDisconnect();
	afx_msg void OnBnClickedBtnDatabase();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnLoad();
	// 学号
	CString m_id;
};
