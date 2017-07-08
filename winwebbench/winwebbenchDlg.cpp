
// winwebbenchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "winwebbench.h"
#include "winwebbenchDlg.h"
#include "afxdialogex.h"
#include "Common.h"
#include <string.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CwinwebbenchDlg 对话框

int CwinwebbenchDlg::sFinishFlag = 0;//测试时间到了

CwinwebbenchDlg::CwinwebbenchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CwinwebbenchDlg::IDD, pParent)
	, m_proxy_server(FALSE)
	, m_force(FALSE)
	, m_reload(FALSE)
	
	, m_http_agreement(0)
	, m_request_method(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_proxyport = 80;//默认端口号80
	pmyparam = NULL;
	mylog = LogManager::OpenLog("wwb.log",3);
	mylog->ClearLogFile();
	if (mylog != NULL)
		mylog->WriteLog("初始化日志文件成功",CLog::LL_INFORMATION);
	else
		mylog->WriteLog("初始化日志问件失败",CLog::LL_ERROR);
}

void CwinwebbenchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_proxy_server);
	DDX_Check(pDX, IDC_FORCE, m_force);
	DDX_Check(pDX, IDC_RELOAD, m_reload);
	DDX_Radio(pDX, IDC_RADIO2, m_http_agreement);
	DDX_Radio(pDX, IDC_RADIO_GET, m_request_method);
	DDX_Control(pDX, IDC_PROGRESS_FINISH, m_progress_finish);
}

BEGIN_MESSAGE_MAP(CwinwebbenchDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_TEST, &CwinwebbenchDlg::OnBnClickedStartTest)
	ON_BN_CLICKED(IDC_CHECK1, &CwinwebbenchDlg::OnBnClickedCheck1)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CwinwebbenchDlg 消息处理程序

BOOL CwinwebbenchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_font.CreatePointFont(100, _T("微软雅黑"), NULL);
	GetDlgItem(IDC_RESULT)->SetFont(&m_font, true);
	GetDlgItem(IDC_URL)->SetFont(&m_font, true);
	//设置默认的配置信息
	DefaultConfig();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CwinwebbenchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CwinwebbenchDlg::OnPaint()
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
HCURSOR CwinwebbenchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CwinwebbenchDlg::OnBnClickedStartTest()
{
	// TODO:  在此添加控件通知处理程序代码
	
	BuildRequest();
	if (CheckServer() == 0)
		mylog->WriteLog(CLog::LL_INFORMATION, "测试连接服务器成功");
	BenchCore();
}

void CwinwebbenchDlg::DefaultConfig()
{
	mylog->WriteLog("初始化默认配置...", CLog::LL_INFORMATION);
	CEdit *pedit = (CEdit *)GetDlgItem(IDC_TIME);
	pedit->SetWindowTextW(_T("30"));
	pedit = (CEdit *)GetDlgItem(IDC_CLIENT);
	pedit->SetWindowTextW(_T("1"));
	pedit = (CEdit *)GetDlgItem(IDC_URL);
	pedit->SetWindowTextW(_T("http://www.baidu.com/"));
	m_force = 1;
	m_reload = 1;
	m_http_agreement = 1;
	m_request_method = 0;
	if (!m_proxy_server) {
		(CEdit*)GetDlgItem(IDC_PROXY)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		m_have_proxy = 0;
	}
	UpdateData(FALSE);
}

/*
	从输入的URL中构造一个报文
*/
int CwinwebbenchDlg::BuildRequest()
{
	UpdateData(TRUE);
	char temp[10];
	int i;
	memset(host, 0, HOST_NAME_MAX);
	memset(request, 0, REQUEST_SIZE);
	USES_CONVERSION;
	CString strurl;
	CString strtime, strclient;
	int http10;

	CEdit *pedit = (CEdit *)GetDlgItem(IDC_URL);
	pedit->GetWindowTextW(strurl);//获得输入的URL
	if (strurl.IsEmpty()) return -1;//空的url退出
	//获得测试时间，默认60s
	GetDlgItem(IDC_TIME)->GetWindowText(strtime);
	m_benchtime = atoi(T2A(strtime.GetBuffer(0)));
	if (m_benchtime == 0) m_benchtime = 30;
	//获得客户端数，默认1个
	GetDlgItem(IDC_CLIENT)->GetWindowText(strclient);
	m_clients = atoi(T2A(strclient.GetBuffer(0)));
	if (m_clients == 0) m_clients = 1;

	char *purl = T2A(strurl.GetBuffer(0));
	mylog->WriteLog(CLog::LL_INFORMATION,"测试URL：%s",purl);
	mylog->WriteLog(CLog::LL_INFORMATION, "客户端数量: %d, 测试时间: %d s  ",m_clients,m_benchtime);
	//存在代理服务器则获取
	if (m_have_proxy) {
		GetDlgItem(IDC_PROXY)->GetWindowTextW(m_str_proxy_server);
		mylog->WriteLog(CLog::LL_INFORMATION, "使用代理服务器: %s", T2A(m_str_proxy_server.GetBuffer(0)));
	}
	else
	{
		mylog->WriteLog(CLog::LL_INFORMATION, "不使用代理服务器");
	}
	
	if (m_force) mylog->WriteLog(CLog::LL_INFORMATION, "early socket close");
	if (m_reload) mylog->WriteLog(CLog::LL_INFORMATION, "forcing reload");
	//使用的http协议
	switch (m_http_agreement)
	{
	case 0:
		http10 = 0;
		mylog->WriteLog("using HTTP/0.9", CLog::LL_INFORMATION);
		break;
	default:
	case 1:
		http10 = 1;
		mylog->WriteLog("using HTTP/1.0", CLog::LL_INFORMATION);
		break;
	case 2:
		http10 = 2;
		mylog->WriteLog("using HTTP/1.1", CLog::LL_INFORMATION);
		break;
	}
	if (m_reload && m_have_proxy && http10 < 1) http10 = 1;
	if (m_request_method == 1 && http10 < 1) http10 = 1;
	if (m_request_method == 2 && http10 < 2) http10 = 2;
	if (m_request_method == 3 && http10 < 2) http10 = 2;
	//报文的请求方式
	switch (m_request_method)
	{
	default:
	case 0:
		strcpy(request, "GET");
		mylog->WriteLog("GET", CLog::LL_INFORMATION);
		break;
	case 1:
		strcpy(request, "HEAD");
		mylog->WriteLog("HEAD", CLog::LL_INFORMATION);
		break;
	case 2:
		strcpy(request, "OPTIONS");
		mylog->WriteLog("OPTIONS", CLog::LL_INFORMATION);
		break;
	case 3:
		strcpy(request, "TRACE");
		mylog->WriteLog("TRACE", CLog::LL_INFORMATION);
		break;
	}
	strcat(request, " ");

	if (NULL == strstr(purl, "://"))
	{
		mylog->WriteLog(CLog::LL_ERROR, "url: %s 无效", purl);
		MessageBox(_T("URL 无效，格式如：http://xxx.xxx/"), _T("错误"));
		return -1;
	}
	if (strlen(purl) > 1500)
	{
		mylog->WriteLog(CLog::LL_ERROR, "url 过长");
		return -1;
	}
	if (!m_have_proxy)
	if (0 != strncmp("http://", purl, 7))
	{
		mylog->WriteLog(CLog::LL_ERROR, "只支持http协议");
		return -1;
	}
	//设置主机名
	//http://www.baidu.com/index1.html
	//主机名以第一个/结尾
	i = strstr(purl, "://") - purl + 3;
	if (strchr(purl + i, '/') == NULL) 
	{
		mylog->WriteLog("主机名要用 / 结尾");
		return -1;
	}
	if (!m_have_proxy)
	{
		if (strchr(purl + i, ':') != NULL &&
			strchr(purl + i, ':') < strchr(purl + i, '/'))
		{
			strncpy(host, purl + i, strchr(purl + i, ':') - purl - i);
			memset(temp, 0, 10);
			strncpy(temp, strchr(purl + i, ':') + 1, strchr(purl + i, '/') - strchr(purl + i, ':') - 1);
			m_proxyport = atoi(temp);
			if (m_proxyport == 0) m_proxyport = 80;
			mylog->WriteLog(CLog::LL_INFORMATION, "端口号: %d", m_proxyport);
		}
		else
		{
			strncpy(host, purl + i, strcspn(purl + i, "/"));
		}
		mylog->WriteLog(CLog::LL_INFORMATION, "Host=%s", host);
		strcat(request + strlen(request), purl + i + strcspn(purl + i, "/"));
	}
	else
	{
		CString proxyname;
		GetDlgItem(IDC_PROXY)->GetWindowTextW(proxyname);
		mylog->WriteLog(CLog::LL_INFORMATION, "使用代理服务器:%s", T2A(proxyname.GetBuffer(0)));
		strcat(request, purl);
	}
	//设置请求协议
	if (http10 == 1)
		strcat(request, " HTTP/1.0");
	else if (http10 == 2)
		strcat(request, " HTTP/1.1");
	strcat(request, "\r\n");
	if (http10 > 0)
		strcat(request, "User-Agent: WinWebBench 1.0\r\n");
	if (!m_have_proxy && http10 > 0)
	{
		strcat(request, "Host: ");
		strcat(request, host);
		strcat(request, "\r\n");
	}
	if (m_reload && m_have_proxy)
	{
		strcat(request, "Pragma: no-cache\r\n");
	}
	if (http10 > 1)
		strcat(request, "Connection: close\r\n");
	if (http10 > 0)
		strcat(request, "\r\n");

	mylog->WriteLog(CLog::LL_INFORMATION, "请求报文:\n%s", request);

	strurl.ReleaseBuffer();
	
	mylog->WriteLog(CLog::LL_INFORMATION, "创建请求成功");

	return 0;
}


void CwinwebbenchDlg::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_have_proxy)
	{
		m_have_proxy = 1;
		GetDlgItem(IDC_PROXY)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
	}
	else
	{
		m_have_proxy = 0;
		GetDlgItem(IDC_PROXY)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
	}
}


int CwinwebbenchDlg::CheckServer()
{
	int i;
	USES_CONVERSION;
	char *pProxyName;
	pProxyName = T2A(m_str_proxy_server.GetBuffer(0));

	i = Socket(m_have_proxy ? pProxyName : host, m_proxyport);
	mylog->WriteLog(CLog::LL_INFORMATION, "开始测试服务器的连接状态： %s %d",host,m_proxyport);
	if (i < 0)
	{
		mylog->WriteLog(CLog::LL_ERROR, "连接服务器错误");
		return -1;
	}
	m_str_proxy_server.ReleaseBuffer();
	m_progress_finish.SetRange(0, m_benchtime);
	m_progress_finish.SetStep(1);
	UpdateData(TRUE);
	return 0;
}


// 测试的核心函数
void CwinwebbenchDlg::BenchCore()
{
	if (pmyparam) {
		delete[] pmyparam;
		pmyparam = NULL;
	}
	pmyparam=new PARAM[m_clients];//坑，要用指针传递参数给线程，用局部变量出了函数就会失效//变为数组分给不同的进程
	memset(pmyparam, 0, sizeof(PARAM)*m_clients);
	for (int i = 0; i < m_clients; ++i) {
		pmyparam[i].benchtime = m_benchtime;
		memset(pmyparam[i].host, 0, HOST_NAME_MAX);
		memset(pmyparam[i].request, 0, REQUEST_SIZE);
		memcpy(pmyparam[i].host, host, HOST_NAME_MAX);
		memcpy(pmyparam[i].request, request, REQUEST_SIZE);
		pmyparam[i].port = m_proxyport;
		pmyparam[i].bytes = 0;
		pmyparam[i].speed = 0;
		pmyparam[i].failed = 0;
		pmyparam[i].finish = 0;//未完成
	}
	sFinishFlag = 0;
	//启动线程
	mylog->WriteLog(CLog::LL_INFORMATION, "启动处理请求的线程，数目: %d ",m_clients);
	for (int i = 0; i < m_clients; ++i) {
		handle = (HANDLE)_beginthreadex(NULL, 0, ProcessBench,
			(LPVOID)&pmyparam[i], 0, NULL);
		if ((unsigned long)handle == 0)
			mylog->WriteLog(CLog::LL_ERROR, "创建线程失败");
		CloseHandle(handle);
	}
	SetTimer(1, 1000,0);
	
}
//线程函数
unsigned int CwinwebbenchDlg::ProcessBench(LPVOID lparam)
{
	PARAM *ppara = (PARAM*)lparam;
	int rlen;
	char buf[1500];
	int force = 0;
	int s, i;
	CLog *ptemp=LogManager::OpenLog("wwb.log",3);
	ptemp->WriteLog(CLog::LL_INFORMATION, "启动线程");
	ptemp->WriteLog(CLog::LL_INFORMATION, 
		"线程参数: host=%s port= %d benchtime=%d speed=%d failed=%d bytes=%d",
		ppara->host, ppara->port,ppara->benchtime, ppara->speed,ppara->failed,ppara->bytes);
	//ptemp->WriteLog(CLog::LL_INFORMATION, "请求报文: %s %d", ppara->request, strlen(ppara->request));
	rlen = strlen(ppara->request);//获取报文长度
	//LARGE_INTEGER nFreq;
	//LARGE_INTEGER nBeginTime;
	//LARGE_INTEGER nEndTime;
	//double time;
	//QueryPerformanceFrequency(&nFreq);
	//QueryPerformanceCounter(&nBeginTime);
nexttry:
	while (!sFinishFlag)//没有到时间的
	{
		
		s = Socket(ppara->host, ppara->port);
		if (s < 0) { ppara->failed++; continue; }
		if (rlen != send(s, ppara->request, rlen, 0)) { ppara->failed++; closesocket((SOCKET)s); continue; }
		if (force == 0)
		{
			while (1)
			{
				i = recv(s, buf, 1500, 0);
				if (i < 0)
				{
					ppara->failed++;
					closesocket(s);
					goto nexttry;
				}
				else
				{
					if (i == 0) break;
					else {
						//ptemp->WriteLog(CLog::LL_INFORMATION, "接收的报文:%s", buf);
						ppara->bytes += i;
					}
				}
			}
		}//接收完了一次数据
		if (closesocket(s)) { ppara->failed++;continue; }
		ppara->speed++;
		//QueryPerformanceCounter(&nEndTime);
		//time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
		//if (ppara->benchtime - time <= 0.00001 || time - ppara->benchtime >= 0.00001)//运行时间
		//{
			//ptemp->WriteLog(CLog::LL_INFORMATION, "线程退出，实际运行时间: %f", (time));
			//break;
		//}
	}
	ppara->finish = 1;
	
	return 0;
}
int CwinwebbenchDlg::CheckThreadOver()
{
	int len = m_clients;
	for (int i = 0; i < len; ++i)
	{
		if (pmyparam[i].finish == 0) { mylog->WriteLog(CLog::LL_INFORMATION, "线程 %d 还没有结束", i);  }//还有没有结束的线程
	}
	mylog->WriteLog(CLog::LL_INFORMATION, "speed:%d faild:%d bytes:%d",
		pmyparam[0].speed, pmyparam[0].failed, pmyparam[0].bytes);
	for (int i = 1; i < len; ++i)//汇总线程返回的结果
	{
		pmyparam[0].speed += pmyparam[i].speed;
		pmyparam[0].failed += pmyparam[i].failed;
		pmyparam[0].bytes += pmyparam[i].bytes;
		
		mylog->WriteLog(CLog::LL_INFORMATION, "线程 %d 结束", i);
		mylog->WriteLog(CLog::LL_INFORMATION, "speed:%d faild:%d bytes:%d",
		pmyparam[i].speed, pmyparam[i].failed, pmyparam[i].bytes);
	}
	return 1;
}
//进行检查处理线程是否结束
void CwinwebbenchDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	static int count_num = 0;
	if (nIDEvent == 1) {
		if (count_num++ == m_benchtime)
		{
			KillTimer(1);
			count_num = 0;
			sFinishFlag = 1;
			Sleep(1000);
			CheckThreadOver();
			
			string strtemp;
			char chtemp[1024];
			memset(chtemp, 0, 1024);
			CStatic *presult = (CStatic *)GetDlgItem(IDC_RESULT);
			CString str;
			sprintf_s(chtemp, "执行结果:\n %d pages/min\n %d bytes/sec\n 成功请求 %d 次; 失败 %d 次",
				(int)((pmyparam->speed + pmyparam->failed) / ((float)m_benchtime / 60.0f)),
				(int)(pmyparam->bytes / (float)m_benchtime),
				pmyparam->speed,pmyparam->failed);
			USES_CONVERSION;
			strtemp = chtemp;
			str = A2T(strtemp.c_str());
			presult->SetWindowTextW(str);
			UpdateData(FALSE);
			mylog->WriteLog(CLog::LL_INFORMATION, "线程执行结束");
			mylog->WriteLog(CLog::LL_INFORMATION, "执行结果: %d pages/min %d bytes/sec request: %d",
				(int)((pmyparam->speed + pmyparam->failed) / ((float)m_benchtime / 60.0f)),
				(int)(pmyparam->bytes / (float)m_benchtime), pmyparam->speed);
			//MessageBox(_T("完成"), _T("结果"));
			
		}
		else
			m_progress_finish.StepIt();
	}
	CDialogEx::OnTimer(nIDEvent);
}


HBRUSH CwinwebbenchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_RESULT)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SetBkMode(TRANSPARENT);
		return m_brush;
	}
	
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
