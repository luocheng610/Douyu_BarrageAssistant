/*******************************************************************************
* 内容摘要  : 定义一组与网络信息获取相关的工具函数
*******************************************************************************/
#ifndef __NET_UTILITY_H_KB__
#define __NET_UTILITY_H_KB__
#include <vector>
#include <string>

#include "kb_lib/type.h"
#include "kb_lib/tchar.h"

KB_WIN32_NAMESPACE_BEGIN(net)

typedef kb::uint32 ipv4;

template<class _char> class net_utility_t;

typedef net_utility_t<tchar> net_utility;

template<class _char>
class net_utility_t
{
public:
    typedef std::basic_string<_char> _string;

    /*******************************************************************************
    * 函数名称	: socket_utility::is_loopback_addr
    * 功能描述	: 判断是否为回环ip地址
    * 参　　数	: ipv4 ip_addr		需要判断的ip地址，主机序
    * 返 回 值	: bool					true表示是回环ip，false表示不是回环ip
    *******************************************************************************/
    static bool is_loopback_addr(ipv4 ip_addr)
    {
        return ((ip_addr & 0x000000ff) == 0x0000007f);
    }

    /*******************************************************************************
    * 函数名称	: socket_utility::is_multicast_addr
    * 功能描述	: 判断一个ip地址是不是组播ip地址
    * 参　　数	: ipv4 ip_addr		需要判断的ip地址，主机序
    * 返 回 值	: bool					true表示是组播ip，false表示不是组播ip
    *******************************************************************************/
    static bool is_multicast_addr(ipv4 ip_addr)
    {
        return ((ip_addr & 0x000000f0) == 0x000000e0);
    }

    /*******************************************************************************
    * 函数名称	: socket_utility::is_badclass_addr
    * 功能描述	: 判断一个ip地址是不是非法ip地址
    * 参　　数	: ipv4 ip_addr		需要判断的ip地址，主机序
    * 返 回 值	: bool					true表示是非法ip，false表示不是非法ip
    *******************************************************************************/
    static bool is_badclass_addr(ipv4 ip_addr)
    {
        return ((ip_addr & 0x000000f0) == 0x000000f0);
    }

    /*******************************************************************************
    * 函数名称	: socket_utility::is_zeronet_addr
    * 功能描述	: 判断一个ip地址是不是零网地址类
    * 参　　数	: ipv4 ip_addr		需要判断的ip地址，主机序
    * 返 回 值	: bool					true表示是零网地址类，false表示不是零网地址类
    *******************************************************************************/
    static bool is_zeronet_addr(ipv4 ip_addr)
    {
        return ((ip_addr & 0x000000ff) == 0x00000000);
    }

    /*******************************************************************************
    * 函数名称	: socket_utility::is_localmcast_addr
    * 功能描述	: 判断一个ip地址是不是局域组播ip地址
    * 参　　数	: ipv4 ip_addr			需要判断的ip地址，主机序
    * 返 回 值	: bool						true表示是局域组播ip，false表示不是局域组播ip
    *******************************************************************************/
    static bool is_localmcast_addr(ipv4 ip_addr)
    {
        return ((ip_addr & 0x00FFFFFF) == 0x000000E0);
    }

    static ipv4 ip_aton(const _char* string_ip);

    static _string ip_ntoa(ipv4 ip_addr);

    static bool is_ip(const _char* string_ip);

    static _string resolve_to_address(const _char* domain_name);

    static void resolve_to_address(const _char* domain_name, std::vector<_string>& ip);

    static _string get_localhost_name();

};

KB_WIN32_NAMESPACE_END(net)
#endif // end of __NET_UTILITY_H_KB__


