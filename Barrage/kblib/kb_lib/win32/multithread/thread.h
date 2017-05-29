/*******************************************************************************
* 内容摘要	: 线程实现的封装
*******************************************************************************/
#ifndef __THREAD_H_KB__  
#define __THREAD_H_KB__  

#include "kb_lib/type.h"

KB_WIN32_NAMESPACE_BEGIN(multithread)

class thread
{	
public:
    thread();
    virtual ~thread();

    void            start( bool create_suspend = false );	
    void            stop();

    void            suspend();
    void            resume();

    bool            is_in_thread();

    uint32          get_thread_id();
    HANDLE          get_thread_handle(); // 如果出现编译错误 HANDLE 未定义，请在自己的 cpp 中 #include "thread.h" 之前先 #include <Windows.h>

    HANDLE          get_stop_event();
    bool            is_stop_event_set();
	
protected:
    virtual void    thread_proc() = 0;
	
private:
    static unsigned __stdcall thread_entry(void* param);

    DWORD           m_thread_id;			// 线程的ID
    HANDLE          m_thread_handle;		// 线程的句柄
    HANDLE          m_stop_event;           // 停止线程的事件
};

KB_WIN32_NAMESPACE_END(multithread)

#endif // end of __THREAD_H_KB__  
