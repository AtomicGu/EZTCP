#include "EZTCP_W.h"

using namespace eztcp;
using namespace eztcp::err;

eztcp::Listener::Listener(unsigned short port, int backlog) :
	_s(socket(AF_INET, SOCK_STREAM, NULL))
{
	if (_s == INVALID_SOCKET)
		throw SocketError(WSAGetLastError());
	_s_addr.sin_family = AF_INET;
	_s_addr.sin_port = htons(port);
	_s_addr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	if (int error_code = bind(_s, reinterpret_cast<sockaddr*>(&_s_addr), sizeof(_s_addr)))
	{
		closesocket(_s);
		throw SocketError(error_code);
	}
	if (int error_code = ::listen(_s, backlog))
	{
		closesocket(_s);
		throw SocketError(error_code);
	}
}

eztcp::Listener::Listener(Listener&& mov) noexcept :
	_s(mov._s),
	_s_addr(mov._s_addr)
{
	mov._s = NULL;
}

eztcp::Listener::~Listener()
{
	if (_s) closesocket(_s);
}

Listener& eztcp::Listener::operator=(Listener&& mov) noexcept
{
	this->~Listener();
	_s = mov._s;
	_s_addr = mov._s_addr;
	mov._s = NULL;
	return *this;
}

Session eztcp::Listener::accept(SIZET buffer_size)
{
	return Session(_s, buffer_size);
}
