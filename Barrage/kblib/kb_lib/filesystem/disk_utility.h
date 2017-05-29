/*******************************************************************************
* 内容摘要	: 处理和磁盘有关的操作
*******************************************************************************/
#ifndef __DISK_UTILITY_H_KB__  
#define __DISK_UTILITY_H_KB__

#include "kb_lib/type.h"
#include "kb_lib/tchar.h"

KB_NAMESPACE_BEGIN(filesystem)

class disk_utility
{
public:
    static sint64	get_partition_capacity(const tchar* path);
    static sint64	get_free_partition_space(const tchar* path);
    static float64	get_partition_utilization_ratio(const tchar* path);
    static tstring  get_disk_format(const tchar* path);

#ifdef __LINUX__
    static sint64	get_partition_capacity(sint32 fd);
    static sint64	get_free_partition_space(sint32 fd);
    static float64	get_partition_utilization_ratio(sint32 fd);
#endif
};

KB_NAMESPACE_END(filesystem)

#endif // end of __DISK_UTILITY_H_KB__
