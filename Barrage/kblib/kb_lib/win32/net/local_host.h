/*******************************************************************************
* 内容摘要  : 获取机器上网卡的相关配置信息，包括MAC地址，IP地址
*******************************************************************************/
/**************************** 条件编译选项和头文件 ****************************/
#ifndef __LOCAL_HOST_H_KB__
#define __LOCAL_HOST_H_KB__

#pragma warning(disable:4812)
#pragma warning(disable:4996)

#include <vector>
#include <string>
#include "kb_lib/type.h"
#include "kb_lib/tchar.h"
#include "kb_lib/text/transcode.h"
#include "kb_lib/win32/net/net_utility.h"

KB_WIN32_NAMESPACE_BEGIN(net)

typedef kb::uint32 ipv4;

template<class _char> class local_host_t;

typedef local_host_t<tchar> loacl_host;

template<class _char>
class local_host_t
{
public:
    typedef typename std::basic_string<_char> _string;

public:
    local_host_t();
    virtual ~local_host_t();

public:
    uint32  get_ipaddr_num();

    _string get_ipaddr(kb::uint32 index);

    _string get_mac_addr(kb::uint32 index);

    _string get_net_mask(kb::uint32 index);

    _string get_netcard_description(kb::uint32 index);

    _string get_gateway_address(kb::uint32 index);

    _string get_adapter_name(kb::uint32 index);

    bool    is_local_ip_addr(ipv4 ip_addr);

protected:

    void init();

    struct adapter_info
    {
        _string physical_address;
        _string card_name;
        _string description;
        _string ip_address;
        _string subnet_mask;
        _string gate_way;
    };

    std::vector<adapter_info> m_info;
};


/*******************************************************************************
* 函数名称  : local_host_t<_char>::~local_host_t<_char>
* 功能描述  : local_host_t 的析构函数
* 返 回 值  : 
*******************************************************************************/
template<class _char>
local_host_t<_char>::~local_host_t<_char>()
{
}

/*******************************************************************************
* 函数名称  : net_utility::get_ipaddr_num
* 功能描述  : 获得读取到的网卡配置信息个数
* 返 回 值  : uint32                为0时表示没有读取到信息
*******************************************************************************/
template<class _char>
uint32 local_host_t<_char>::get_ipaddr_num()
{
    return (uint32)m_info.size();
}

/*******************************************************************************
* 函数名称  : net_utility::get_ipaddr_by_index
* 功能描述  : 按照索引数得到网卡的第一个IP地址
* 参　　数  : uint32 index        信息索引号
* 返 回 值  : std::wstring         以字符串形式表现出的IP地址，形如"192.168.2.2"
如果索引超出范围返回空字符串
*******************************************************************************/
template<class _char>
std::basic_string<_char> local_host_t<_char>::get_ipaddr(uint32 index)
{
    if (index >= m_info.size())
    {
        return std::basic_string<_char>();
    }
    return m_info[index].ip_address;
}

/*************************************************************************************
* 函数名称  : net_utility::get_mac_addr
* 功能描述  : 按照索引数得到网卡的MAC地址
* 参　　数  : uint32 index        信息索引号
* 返 回 值  : std::wstring        以字符串形式表现出的MAC地址，形如"00-1B-B9-78-B5-0B"
**************************************************************************************/
template<class _char>
std::basic_string<_char> local_host_t<_char>::get_mac_addr(uint32 index)
{
    if (index >= m_info.size())
    {
        return std::basic_string<_char>();
    }
    return m_info[index].physical_address;
}

/*******************************************************************************
* 函数名称  : net_utility::get_net_mask
* 功能描述  : 按照索引数得到网卡的第一个子网掩码
* 参　　数  : uint32 index        信息索引号
* 返 回 值  : std::wstring         以字符串形式表现出的子网掩码，形如"255.255.255.0"
如果索引超出范围返回空字符串
*******************************************************************************/
template<class _char>
std::basic_string<_char> local_host_t<_char>::get_net_mask(uint32 index)
{
    if (index >= m_info.size())
    {
        return std::basic_string<_char>();
    }
    return m_info[index].subnet_mask;
}

/*******************************************************************************
* 函数名称  : net_utility::get_netcard_name
* 功能描述  : 按照索引数得到网卡的描述名称
* 参　　数  : uint32 index        信息索引号
* 返 回 值  : std::wstring        以字符串形式表现出的网卡描述名称
如果索引超出范围返回空字符串
*******************************************************************************/
template<class _char>
std::basic_string<_char> local_host_t<_char>::get_netcard_description(uint32 index)
{
    if (index >= m_info.size())
    {
        return std::basic_string<_char>();
    }
    return m_info[index].description;
}

/*******************************************************************************
* 函数名称  : net_utility::get_gateway_address
* 功能描述  : 按照索引数得到网卡的第一个网关地址
* 参　　数  : uint32 index        信息索引号
* 返 回 值  : std::wstring         以字符串形式表现出的网关地址，形如"192.168.85.1"
如果索引超出范围返回空字符串
*******************************************************************************/
template<class _char>
std::basic_string<_char> local_host_t<_char>::get_gateway_address(uint32 index)
{
    if (index >= m_info.size())
    {
        return std::basic_string<_char>();
    }
    return m_info[index].gate_way;
}

/*******************************************************************************
* 函数名称  : net_utility::get_adapter_name
* 功能描述  : 按照索引数得到网卡的网卡系统名
* 参　　数  : uint32 index        信息索引号
* 返 回 值  : std::wstring        以字符串形式表现出的网卡系统名称
如果索引超出范围返回空字符串
*******************************************************************************/
template<class _char>
std::basic_string<_char> local_host_t<_char>::get_adapter_name(uint32 index)
{
    if (index >= m_info.size())
    {
        return std::basic_string<_char>();
    }
    return m_info[index].card_name;
}

/*******************************************************************************
* 函数名称	: socket_utility::is_local_ip_addr
* 功能描述	: 判断一个ip地址是不是本机ip地址
* 参　　数	: ipv4 ip_addr		需要判断的ip地址，主机序
* 返 回 值	: bool					true表示是本机ip，false表示不是本机ip
*******************************************************************************/
template<class _char>
bool local_host_t<_char>::is_local_ip_addr(ipv4 ip_addr)
{
    _string addr = kb::win32::net::net_utility_t<_char>::ip_ntoa(ip_addr);
    for(uint32 x = 0; x < m_info.size(); x ++)
    {
        if(m_info[x].ip_address == addr)
        {
            return true;
        }
    }

    return false;
}


KB_WIN32_NAMESPACE_END(net)

#endif // end of __LOCAL_HOST_H_KB__
