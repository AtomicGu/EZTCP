#pragma once
#include "eztcp.h"

// 以阻塞模式发送缓冲区数据，只有当全部发送完毕（返回NULL）或出错（返回SOCKET_ERROR）时才返回
int mass_send(SOCKET s, const char* buf, SIZET len) noexcept;

// 以阻塞模式接收缓冲区数据，只有当全部接收完毕（返回NULL）或出错（返回SOCKET_ERROR）时才返回
int mass_recv(SOCKET s, char* buf, SIZET len) noexcept;

// 发送包头，成功返回sizeof(HEADER_TYPE)，失败返回SOCKET_ERROR
inline int send_header(SOCKET s, HEADER_TYPE* size_p)
{
	return send(s, reinterpret_cast<char*>(size_p), sizeof(HEADER_TYPE), NULL);
}

// 接收包头，成功返回sizeof(HEADER_TYPE)，失败返回SOCKET_ERROR
inline int recv_header(SOCKET s, HEADER_TYPE* size_p)
{
	if (recv(s, reinterpret_cast<char*>(size_p), sizeof(HEADER_TYPE), MSG_WAITALL) != sizeof(HEADER_TYPE))
		return SOCKET_ERROR;
	else
		return sizeof(HEADER_TYPE);
}
