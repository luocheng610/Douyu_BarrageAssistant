#ifndef __ERROR_H_KB__
#define __ERROR_H_KB__

#include "./type.h"
#include "./tchar.h"

NAMESPACE_BEGIN(kb)

/* 错误码类型定义 */
typedef         uint32              error_code;


/* KB内部错误码范围定义 */
#define         NO_ERR              (0)
#define         ERR_KB_MIN          (20000000)
#define         ERR_KB_MAX          (30000000)

/* KB公共库 */
#define         ERR_KB_LIB_MIN      (ERR_KB_MIN + 100000)
#define         ERR_KB_DATA_MIN     (ERR_KB_MIN + 5000000)
/* kb_client */
#define         ERR_KB_CLIENT_MIN   (ERR_KB_MIN + 6000000) 

#define         ERR_WRONG_PARA		(ERR_KB_LIB_MIN + 1) // 输入参数出错
#define         ERR_WRONG_FORMAT    (ERR_KB_LIB_MIN + 2) // 格式错误
#define         ERR_NONE_SYMLINK    (ERR_KB_LIB_MIN + 3) // 非符号链接错误
#define         ERR_FILE_INVALID    (ERR_KB_LIB_MIN + 4) // 文件打开失败
#define         ERR_FILE_IO_ERROR   (ERR_KB_LIB_MIN + 5) // 文件读写失败
#define         ERR_INSUFF_BUFFER   (ERR_KB_LIB_MIN + 6) // 传入缓冲区太小
#define         ERR_CHAR_INVALID    (ERR_KB_LIB_MIN + 7) // 非法字符
#define         ERR_LOGIC_ERROR     (ERR_KB_LIB_MIN + 7) // 程序出现逻辑错误
#define         ERR_NO_MEMORY       (ERR_KB_LIB_MIN + 8) // 内存非配失败

// 定义"格式化码流编解码模块"中的错误
#define         ERR_PROTOBUF_MIN    (ERR_KB_LIB_MIN + 1000)
#define         ERR_INVALID_VERSION (ERR_PROTOBUF_MIN + 0) // 无效的message版本号
#define         ERR_DUP_VERSION     (ERR_PROTOBUF_MIN + 1) // 重迭的message版本号
#define         ERR_MESSAGE_DEFINE  (ERR_PROTOBUF_MIN + 2) // message的定义出错
#define         ERR_INVALID_FIELD   (ERR_PROTOBUF_MIN + 3) // 无效的message域信息
#define         ERR_NOT_ENOUGH_BUF  (ERR_PROTOBUF_MIN + 4) // 用来解码的缓存长度不够
#define         ERR_STRING_SIZE     (ERR_PROTOBUF_MIN + 5) // 字符串太长
#define         ERR_ARRAY_SIZE      (ERR_PROTOBUF_MIN + 6) // 数组太大了

#define         ERR_INVALID_TYPE    (ERR_KB_DATA_MIN + 1)       // kb_data 的类型非法
#define         ERR_MAP_EMPTY       (ERR_KB_DATA_MIN + 2)       // MAP 为空
#define         ERR_MAP_MISMATCH    (ERR_KB_DATA_MIN + 3)       // MAP 的结构有问题，键在访问序列中，但是不在 MAP 中
#define         ERR_INT_INVALID     (ERR_KB_DATA_MIN + 4)       // 读出的整型值不合法，超出了预期的大小
#define         ERR_IO_FAIL         (ERR_KB_DATA_MIN + 5)       // 读写不到预期的字节数
#define         ERR_DECODE_NOTHING  (ERR_KB_DATA_MIN + 6)       // 二进制解码中发现没有预期的字段
#define         ERR_LIST_INVALID    (ERR_KB_DATA_MIN + 7)       // 二进制解码中发现LIST无法解析预期的元素
#define         ERR_XML_INVALID     (ERR_KB_DATA_MIN + 8)       // XML解码中发现XML格式错误
#define         ERR_XML_NOT_UTF8    (ERR_KB_DATA_MIN + 9)       // XML不是UTF8
#define         ERR_ITEM_NOT_IN_MAP (ERR_KB_DATA_MIN + 10)       // XML中ITEM不在MAP中
#define         ERR_ITEM_NO_KEY     (ERR_KB_DATA_MIN + 11)       // XML中ITEM没有key
#define         ERR_UNKNOWN_ELEMENT (ERR_KB_DATA_MIN + 12)       // XML中出现未知的element
//#define         ERR_WSTR_IN_BEN     (ERR_KB_DATA_MIN + 13)       // bencoding 编码不支持宽字符串
//#define         ERR_UI8_IN_BEN      (ERR_KB_DATA_MIN + 14)       // bencoding 编码不支持 uint8
//#define         ERR_UI16_IN_BEN     (ERR_KB_DATA_MIN + 15)       // bencoding 编码不支持 uint16
//#define         ERR_UI32_IN_BEN     (ERR_KB_DATA_MIN + 16)       // bencoding 编码不支持 uint32
#define         ERR_BEN_INVALID     (ERR_KB_DATA_MIN + 17)       // bencoding 编码格式错误
#define         ERR_ZIP_FAIL        (ERR_KB_DATA_MIN + 18)       // zip 压缩失败
#define         ERR_UNZIP_FAIL      (ERR_KB_DATA_MIN + 19)       // zip 解压缩失败
#define         ERR_HEX_STR_INVALID (ERR_KB_DATA_MIN + 20)       // 非法的十六进制字符串
#define         ERR_INVALID_FORMAT  (ERR_KB_DATA_MIN + 21)       // 非法的格式

#define         ERR_CONNECTION_CLOSED   (ERR_KB_CLIENT_MIN + 1)  // 连接被正常关闭

#define         ERR_HTTP_REDIRECT_LOOP  (ERR_KB_CLIENT_MIN + 2)  // HTTP 重复重定向超过5次
#define         ERR_HTTP_INVALID_LOCATION (ERR_KB_CLIENT_MIN + 3)  // HTTP 重复 URL 非法
#define         ERR_HTTP_HEADER_INVALID (ERR_KB_CLIENT_MIN + 4)  // kb_client中的HttpWrapper接收的HTTP Header中不含Content-Length


/*******************************************************************************
* 函数名称  : set_last_error
* 功能描述  : 设置最后一次错误的错误码
* 参　　数  : error_code err 错误码，该错误码保存在线程私有数据中，多线程之间不会互相冲突
* 返 回 值  : void
*******************************************************************************/
void set_last_error( error_code err );

/*******************************************************************************
* 函数名称  : get_last_error
* 功能描述  : 获取最后一次错误的错误码。
* 返 回 值  : error_code 错误码
*******************************************************************************/
error_code get_last_error();

/*******************************************************************************
* 函数名称  : get_error_desc
* 功能描述  : 获取指定错误码的描述
* 参　　数  : error_code err 错误码。也可以使用windows系统的错误码。
* 返 回 值  : tstring 描述文本。如果没有找到，则返回 ""。
*******************************************************************************/
tstring get_error_desc( error_code err );

/*******************************************************************************
* 函数名称  : get_error_desc
* 功能描述  : 获取当前错误码的描述
* 返 回 值  : tstring 描述文本。如果没有找到，则返回 ""。
*******************************************************************************/
tstring get_error_desc();

/*********************************** 类定义 ***********************************/

NAMESPACE_END(kb)

#endif // end of __ERROR_H_KB__
