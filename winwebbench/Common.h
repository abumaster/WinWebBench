#ifndef _COMMON_H_
#define _COMMON_H_

#include <WinSock2.h>
#include <stdlib.h>
#include <stdarg.h>
#include "LogManager.h"

#pragma comment(lib,"ws2_32.lib")

//返回创建连接的套接字
int Socket(const char* host, int clientport);


#endif