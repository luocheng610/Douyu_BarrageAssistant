/*******************************************************************************
* 内容摘要	: 处理和目录有关的操作
*******************************************************************************/
#ifndef __DIR_UTILITY_H_KB__  
#define __DIR_UTILITY_H_KB__

#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "kb_lib/type.h"
#include "kb_lib/tchar.h"

KB_NAMESPACE_BEGIN(filesystem)

class dir_utility
{
public:
    
    static bool     is_dir(const tchar* path);
    static bool     is_empty_directory(const tchar* dir_path);
    static tstring  get_current_working_directory();	
    static bool     set_current_working_directory(const tchar* dir_path);

    static bool     delete_dir(const tchar* dir_path);
    static bool     delete_dir_tree(const tchar* dir_path);

#if defined(WIN32)

    static bool     make_dir(const tchar* dir_path);
    static bool     make_dir_tree(const tchar* dir_path);

#elif defined(__LINUX__)
    
    static bool     is_dir(sint32 fd);
    static sint32   make_dir(const tchar* path, mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO);
    static sint32   make_dir_tree(const tchar* path, mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO);

#endif
};

KB_NAMESPACE_END(filesystem)

#endif // end of __DIR_UTILITY_H_KB__
