#include "stdafx.h"
#include "LogManager.h"
#include "Log.h"

using std::map;
using std::string;

map<string, CLog*> LogManager::m_logMap;

// <Summary>打开日志</Summary>
// <DateTime>2014/4/22</DateTime>
// <Parameter name="pFilePath" type="IN">日志文件路径</Parameter>
// <Parameter name="nLogLevel" type="IN">日志记录级别</Parameter>
// <Returns>成功返回日志对象指针 否则返回NULL</Returns>
CLog* LogManager::OpenLog(const char *pFilePath, int nLogLevel)
{
    if (NULL == pFilePath)
    {
        return NULL;
    }
    CLog *pLog = NULL;

    map<string, CLog*>::iterator iter = m_logMap.find(pFilePath);
    if (m_logMap.find(pFilePath) == m_logMap.end())   // 之前没有打开过该文件 则实例化一个日志对象
    {
        pLog = new CLog(pFilePath);
        if (!pLog->GetOpenStatus())  // 打开该日志文件不成功
        {
            return NULL;
        }

        m_logMap[pFilePath] = pLog;
    }
    else
    {
        pLog = iter->second;
    }

    if (pLog)   // 日志已经打开了 则设置日志级别
    {
        pLog->SetLogLevel((CLog::LOG_LEVEL)nLogLevel);
    }
    return pLog;
}

// <Summary>清空并释放所有日志对象所占用的内存</Summary>
// <DateTime>2014/4/28</DateTime>
void LogManager::Clear()
{
    map<std::string, CLog*>::const_iterator iter = m_logMap.begin();
    for (; iter != m_logMap.end(); ++iter)
    {
        delete iter->second;   // 释放指针所占用的内存空间
    }
    m_logMap.clear();
}

// <Summary>删除并释放特定日志对象占用的内存空间</Summary>
// <DateTime>2014/4/28</DateTime>
// <Parameter name="strLogFilePath" type="IN">日志文件的完整路径</Parameter>
// <Returns></Returns>
// <Exception cref=""></Exception>
void LogManager::RemoveLog(const std::string &strLogFilePath)
{
    map<std::string, CLog*>::const_iterator iter = m_logMap.find(strLogFilePath);
    if (iter != m_logMap.end())  // 找到指定的日志对象
    {
        delete iter->second;     // 释放所占用的内存空间
        m_logMap.erase(iter);    // 从map中删除该日志对象指针
    }
}