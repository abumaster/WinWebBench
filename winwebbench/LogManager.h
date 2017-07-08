/************************************************************************/
/* 类名称：  LogManager
/* 命名空间：无
/* 文件名：  LogManager.h LogManager.cpp
/* 创建时间: 2014-4-22
/* 作者：    wanghongyun
/* 类说明：  日志对象管理类，通过该类进行实例化日志对象，确保不会出现两个日志对象操作一个日志文件
/************************************************************************/

#ifndef _LOGMANAGER_INCLUDE_H_
#define _LOGMANAGER_INCLUDE_H_

#include <map>
#include <string>

class CLog;

class LogManager
{
public:
    static CLog* OpenLog(const char *pFilePath, int nLogLevel = 1); // 打开日志
    static void Clear();                                            // 清空并释放所有日志对象所占用的内存
    static void RemoveLog(const std::string &strLogFilePath);       // 删除并释放特定日志对象占用的内存空间

private:  // 私有默认构造函数、复制构造函数
    LogManager(void);
    LogManager(const LogManager &logManager);

private:  // 私有成员
    static std::map<std::string, CLog*> m_logMap;  // 保存日志文件路径和日志对象的字典集合

};

#endif
