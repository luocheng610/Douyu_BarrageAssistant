/*******************************************************************************
* 内容摘要	: 定义Mutex互斥锁封装类。Mutex 为Windows内核对象，效率低于临界区(critial section)
              只有在跨进程的时候建议使用 Mutex。
*******************************************************************************/
#ifndef __MUTEX_H_KB__
#define __MUTEX_H_KB__

#include "kb_lib/type.h"
#include "kb_lib/tchar.h"
#include "kb_lib/win32/multithread/lock_guard.h"

KB_WIN32_NAMESPACE_BEGIN(multithread)

class mutex
{
public:
    mutex();
    mutex( bool open_existing, const tchar* mutex_name );
    virtual ~mutex();

    void lock();
    void unlock();
    bool try_lock();

    operator HANDLE()
    {
        return m_mutex;
    }

private:
    HANDLE      m_mutex;
};

typedef lock_guard<mutex>    mx_lock_guard;
typedef lock_guard<mutex>    mutex_lock_guard;

KB_WIN32_NAMESPACE_END(multithread)

#endif // end of __MUTEX_H_KB__
