/*******************************************************************************
* 内容摘要  : 定义安全删除等一系列宏
*******************************************************************************/
#ifndef __SAFE_DELETE_H_KB__
#define __SAFE_DELETE_H_KB__

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)          do { if ( (p) ) { delete (p); (p) = NULL; } } while(0)
#endif // SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)    do { if ( (p) ) { delete [](p); (p) = NULL; } } while(0)
#endif // SAFE_DELETE_ARRAY

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)         do { if ( (p) ) { (p)->Release(); (p) = NULL; } } while(0)
#endif // SAFE_RELEASE

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(p)    do { if ( (p) ) { ::CloseHandle(p); (p) = NULL; } } while(0)
#endif // SAFE_CLOSE_HANDLE

#endif // end of __SAFE_DELETE_H_KB__
