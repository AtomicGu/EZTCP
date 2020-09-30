#include "EZTCP_W.h"
#include <MSWSock.h>

using namespace eztcp;
using namespace eztcp::err;

eztcp::Session::Session() :
	_s(NULL),
	_buffer_p(nullptr),
	_buffer_size(0),
	_s_addr({ 0 })
{}

eztcp::Session::Session(const char* ip_ps, unsigned short port, int buffer_size) :
	_s(socket(AF_INET, SOCK_STREAM, NULL))
{
	if (_s == INVALID_SOCKET)
		throw SocketError(WSAGetLastError());
	_s_addr.sin_family = AF_INET;
	_s_addr.sin_port = htons(port);
	_s_addr.sin_addr.S_un.S_addr = inet_addr(ip_ps);
	if (int error_code = connect(_s, reinterpret_cast<sockaddr*>(&_s_addr), sizeof(_s_addr)))
		throw SocketError(error_code);
	_buffer_p = new char[buffer_size];
	_buffer_size = buffer_size;
}

eztcp::Session::Session(SOCKET s_listening, int buffer_size) :
	_s(accept(s_listening, reinterpret_cast<sockaddr*>(&_s_addr), nullptr))
{
	if (_s == INVALID_SOCKET)
		throw SocketError(WSAGetLastError());
	_buffer_p = new char[buffer_size];
	_buffer_size = buffer_size;
}

eztcp::Session::Session(Session&& mov) noexcept :
	_s(mov._s),
	_s_addr(mov._s_addr),
	_buffer_p(mov._buffer_p),
	_buffer_size(mov._buffer_size)
{
	mov._s = NULL;
	mov._buffer_p = nullptr;
}

eztcp::Session::~Session()
{
	if (_s) closesocket(_s);
	if (_buffer_p) delete[] _buffer_p;
}

Session& eztcp::Session::operator=(Session&& mov) noexcept
{
	this->~Session();
	_s = mov._s;
	_s_addr = mov._s_addr;
	_buffer_p = mov._buffer_p;
	_buffer_size = mov._buffer_size;
	mov._s = NULL;
	mov._buffer_p = nullptr;
	return *this;
}

RecvPack& eztcp::Session::recv()
{
	if (_recving._size_left)
		while (_recving.extract(_buffer_p, _buffer_size));
	_recving = RecvPack(_s);
	return _recving;
}

void eztcp::Session::send(Package& package)
{
	HEADER_TYPE size = package.size_left();
	if (send_header(_s, &size) == SOCKET_ERROR)
		throw SocketError(WSAGetLastError());
	while (int n = package.extract(_buffer_p, _buffer_size))
	{
		if (::send(_s, _buffer_p, n, NULL) == SOCKET_ERROR)
			throw SocketError(WSAGetLastError());
	}
}

void eztcp::Session::send(MemPack& package)
{
	HEADER_TYPE size = package._size_left;
	if (send_header(_s, &size) == SOCKET_ERROR)
		throw SocketError(WSAGetLastError());
	if (mass_send(_s, package._src_end_p - package._size_left, package._size_left) == SOCKET_ERROR)
		throw SocketError(WSAGetLastError());
	package._size_left = 0;
}

void eztcp::Session::send(FilePack& package)
{
	if (send_header(_s, &package._size_left) == SOCKET_ERROR)
		throw SocketError(WSAGetLastError());
	if (!TransmitFile(_s, package._file_h, 0, NULL, nullptr, nullptr, NULL))
		throw SocketError(WSAGetLastError());	// HACK: 包大小的限制问题？
	package._size_left = 0;
}

unsigned short eztcp::Session::get_port() const
{
	return htons(_s_addr.sin_port);
}

const char* Session::get_ip() const
{
	return inet_ntoa(_s_addr.sin_addr);
}
