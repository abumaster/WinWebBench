# WinWebBench
基于[WebBench](http://www.oschina.net/p/webbench)的Windows版本网站性能测试小工具  
**开发环境：** Windows 10 操作系统、Visual Studio 2015、C++、MFC框架  
**特点：** 在外层包装了一层用户操作界面，方便操作，如图。  
![](http://oo7zsi4t8.bkt.clouddn.com/17-7-8/92734844.jpg)  
**改变：** Windows端的多进程，不如Linux端的方便，一直无法找到Windows上用多进程来模拟多并发请求，最后只能用多线程来模拟请求。如果需要的客户端过多，创建线程也会耗费一定的时间，影响了性能，结果会不精确。  
下一步可以考虑线程池。  
通过学习 *WebBench* 的源码，并在 Windows 上练习，学到了很多东西，比如 http 请求方式和请求报文格式，字符串的转换（Windows的宽字节和多字节），多线程，多进程通信等。   
**感谢：** 谢谢 *wanghongyun* 的 C++ 日志操作类[源码](http://www.oschina.net/code/snippet_1402899_26572)。
