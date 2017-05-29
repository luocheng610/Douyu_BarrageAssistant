/*******************************************************************************
* 内容摘要  : 文件路径处理的辅助函数。Windows依赖系统shell32.lib
*******************************************************************************/
#ifndef __PATH_UTILITY_H_KB__
#define __PATH_UTILITY_H_KB__

#include "kb_lib/tchar.h"

KB_NAMESPACE_BEGIN(filesystem)

class path_utility
{
public:
    static tstring	get_file_name(const tchar* file_path);

    static tstring	get_file_main_name(const tchar* file_path);
    static tstring	get_file_ext(const tchar* file_path);

    static tstring	get_path_root(const tchar* file_path);
    static tstring	get_path_dir(const tchar* file_path);

    static tstring  get_upper_dir(const tchar* dir_path);

    static tstring  get_current_exe_path();
    static tstring  get_current_exe_dir();
    static tstring  get_current_module_path();
    static tstring  get_current_module_dir();

    static tstring  get_common_appdata_dir();
    static tstring  get_appdata_dir();
    static tstring  get_tmpdata_dir();
};

KB_NAMESPACE_END(filesystem)

#endif // end of __PATH_UTILITY_H_KB__


