#include "EZTCP_L.h"

using namespace eztcp;
using namespace eztcp::err;

eztcp::RecvPack::RecvPack() :
	_s(NULL),
	_size_left(0)
{}

eztcp::RecvPack::RecvPack(SOCKET s) :
	_s(s)
{
	if (recv_header(s, &_size_left) == SOCKET_ERROR)
		throw SocketError(WSAGetLastError());
}

RecvPack& eztcp::RecvPack::operator=(RecvPack&& mov) noexcept
{
	_s = mov._s;
	_size_left = mov._size_left;
	return *this;
}

SIZET eztcp::RecvPack::extract(void* buffer_p, SIZET buffer_size)
{
	SIZET extract_size = min(buffer_size, _size_left);
	if (!extract_size) return extract_size;
	if ((mass_recv(_s, reinterpret_cast<char*>(buffer_p), extract_size) == SOCKET_ERROR))
		throw SocketError(WSAGetLastError());
	_size_left -= extract_size;
	return extract_size;
}
