#include "stdafx.h"
#include "Common.h"

int Socket(const char* host, int clientport)
{
	CLog *plog = LogManager::OpenLog("wwb.log", 3);
	//plog->WriteLog("初始化网络套接字",CLog::LL_INFORMATION);
	WSADATA Ws;
	SOCKET sock;
	unsigned long inaddr;
	struct sockaddr_in servaddr;
	struct hostent *hp;
	memset(&servaddr, 0, sizeof(servaddr));

	//初始化套接字结构
	if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0)
	{
		//cout << "Init Windows Socket Failed:" << GetLastError() << endl;
		plog->WriteLog(CLog::LL_ERROR,"初始化 Windows Socket 失败：%s",GetLastError());
		return -1;
	}

	//设置服务器的地址
	servaddr.sin_family = AF_INET;

	inaddr = inet_addr(host);
	if (inaddr != INADDR_NONE)//以前写的INADDR_ANY，错误，无法else
	{
		memcpy(&servaddr.sin_addr, &inaddr, sizeof(inaddr));
		//plog->WriteLog("inet_addr");
	}
	else
	{
		hp = gethostbyname(host);
		if (hp == NULL)
		{
			plog->WriteLog("gethostbyname函数调用失败");
			return -1;
		}
		memcpy(&servaddr.sin_addr, hp->h_addr, hp->h_length);
	}
	//设置端口号
	servaddr.sin_port = htons(clientport);

	//创建客户端套接字
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return sock;
	//plog->WriteLog(CLog::LL_INFORMATION, "创建套接字成功: %d",sock);
	//连接
	if (connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr))<0)
	{
		return -1;
	}
	//plog->WriteLog("创建套接字并连接成功",CLog::LL_INFORMATION);
	return sock;
}