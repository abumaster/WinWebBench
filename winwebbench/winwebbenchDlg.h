
// winwebbenchDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#define REQUEST_SIZE 2048
#define HOST_NAME_MAX 256


typedef struct {
	char host[HOST_NAME_MAX];
	char request[REQUEST_SIZE];
	int port;
	int benchtime;
	int speed;
	int failed;
	int bytes;
	int finish;
}PARAM,*pParam;
// CwinwebbenchDlg 对话框
class CwinwebbenchDlg : public CDialogEx
{
// 构造
public:
	CwinwebbenchDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WINWEBBENCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartTest();
	void DefaultConfig();
	// 代理服务器，0不需要，1需要
	BOOL m_proxy_server;
	bool m_have_proxy;
	BOOL m_force;
	BOOL m_reload;
	CString m_str_proxy_server;
	//http协议 0:http/0.9; 1:http/1.0; 2:http/1.1
	//int m_http_agreement;
	//请求的方法
	//0		1	2		3
	//get head options trace
	//int m_request_method;
	
	int m_http_agreement;
	int m_request_method;

	CLog *mylog;//日志指针
	int BuildRequest();//根据输入的url创建请求报文
	int m_benchtime;
	int m_clients;
	char host[HOST_NAME_MAX];
	char request[REQUEST_SIZE];
	int m_proxyport;
	int m_speed;
	int m_failed;
	int m_bytes;
	afx_msg void OnBnClickedCheck1();
	int CheckServer();//检测服务器是否可用
	// 测试的核心函数
	void BenchCore();
	PARAM *pmyparam;
private:
	CFont m_font;
	CBrush m_brush;
public:
	//线程函数用于后台处理
	HANDLE handle;
	static unsigned int _stdcall ProcessBench(LPVOID lparam);
	static int sFinishFlag;
	int CheckThreadOver();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_progress_finish;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
