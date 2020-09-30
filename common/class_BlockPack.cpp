#include "eztcp.h"
#include <cstring>
#include <utility>

using namespace eztcp;
using namespace eztcp::err;

eztcp::BlockPack::BlockPack() :
	MemPack(nullptr, 0)
{}

eztcp::BlockPack::BlockPack(const void* src_p, SIZET size) :
	MemPack(new char[size], size)
{
	std::memcpy(const_cast<char*>(_src_p), src_p, size);
}

eztcp::BlockPack::BlockPack(RecvPack& recv_pack) :
	MemPack(new char[static_cast<SIZET>(recv_pack._size_left)],
		static_cast<SIZET>(recv_pack._size_left))
{
	try
	{
		recv_pack.extract(const_cast<char*>(_src_p), -1);
	}
	catch (SocketError& e)
	{
		delete[] _src_p;
		throw e;
	}
}

eztcp::BlockPack::BlockPack(const BlockPack& cpy) :
	MemPack(new char[cpy._size_left], cpy._size_left)
{
	std::memcpy(const_cast<char*>(_src_p), cpy._src_p, cpy._size_left);
}

eztcp::BlockPack::BlockPack(BlockPack&& mov) noexcept :
	MemPack(std::move(mov))
{
	mov._src_p = nullptr;
}

eztcp::BlockPack::~BlockPack() noexcept
{
	if (_src_p) delete[] _src_p;
}

BlockPack& eztcp::BlockPack::operator=(const BlockPack& cpy)
{
	this->~BlockPack();
	reinterpret_cast<MemPack&>(*this) = MemPack(new char[cpy._size_left], cpy._size_left);
	std::memcpy(const_cast<char*>(_src_p), cpy._src_p, cpy._size_left);
	return *this;
}

BlockPack& eztcp::BlockPack::operator=(BlockPack&& mov) noexcept
{
	this->~BlockPack();
	reinterpret_cast<MemPack&>(*this) = std::move(mov);
	mov._src_p = nullptr;
	return *this;
}
