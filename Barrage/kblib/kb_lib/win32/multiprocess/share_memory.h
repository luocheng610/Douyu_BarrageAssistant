/*******************************************************************************
* 内容摘要  : 共享内存相关
*******************************************************************************/
#ifndef __SHARE_MEMORY_H_KB__
#define __SHARE_MEMORY_H_KB__

#include "kb_lib/type.h"
#include "kb_lib/tchar.h"

KB_NAMESPACE_BEGIN( win32 )
NAMESPACE_BEGIN( multiprocess )


class share_memory
{
public:
    share_memory();
    virtual ~share_memory();

public:
    bool       open( const char* name, kb::uint32 size, bool open_existing = false, kb::uint32 read_only = false );
    void       close();

    kb::byte*  get_ptr();
    kb::uint32 get_size();

    kb::byte*  lock( kb::uint32 wait_millisec = INFINITE );
    void       unlock();

    class scope_lock
    {
    public:
        scope_lock( share_memory& mem )
            : m_mem(mem)
        {
            m_mem_ptr = m_mem.lock(INFINITE);
        }
        ~scope_lock()
        {
            m_mem.unlock();
        }
        kb::byte* get_ptr()
        {
            return m_mem_ptr;
        }
    protected:
        share_memory& m_mem;
        kb::byte*     m_mem_ptr;
    };

protected:
    // 如果需要初始化共享内存内容，则可以派生该类，重载该函数。默认为清空内存。
    virtual void on_init_memory( kb::byte* mem_ptr, kb::uint32 mem_size );

public:
    HANDLE      m_memory;
    HANDLE      m_mutex;
    kb::byte*   m_memory_ptr;
};

NAMESPACE_END( multiprocess )
KB_NAMESPACE_END( win32 )

#endif // end of __SHARE_MEMORY_H_KB__
