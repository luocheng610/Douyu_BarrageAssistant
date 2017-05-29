/*******************************************************************************
* 内容摘要	: 处理和文件有关的操作
*******************************************************************************/
#ifndef __FILE_UTILITY_H_KB__  
#define __FILE_UTILITY_H_KB__

#include "kb_lib/tchar.h"

KB_NAMESPACE_BEGIN(filesystem)

class file_utility
{
public:	

	static bool			is_file(const tchar* path);	
	static bool			is_file_exist(const tchar* path);
	static sint64		get_filesize(const tchar* path);
	
#ifdef __LINUX__
	static bool			is_file(sint32 fd);	
	static sint64		get_filesize(sint32 fd); 

    static bool			is_symlink(const tchar* path);
	static sint32		create_symlink(const tchar* old_path,const tchar* new_path);
	static sint32		delete_symlink(const tchar* symlink_path);
	static sint32		get_real_path_from_symlink(const tchar* symlink_path, tchar* real_path_buff, uint32 buff_size);
#endif
};

KB_NAMESPACE_END(filesystem)

#endif // end of __FILE_UTILITY_H_KB__
