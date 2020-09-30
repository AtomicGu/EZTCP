#include "eztcp.h"
#include <cstring>
#define min(a,b) (((a)<(b)?(a):(b)))

using namespace eztcp;
using namespace eztcp::err;

eztcp::MemPack::MemPack(const void* src_p, SIZET size) :
	_src_p(reinterpret_cast<const char*>(src_p)),
	_src_end_p(reinterpret_cast<const char*>(src_p) + size),
	_size_left(size)
{}

SIZET eztcp::MemPack::extract(void* buffer_p, SIZET buffer_size)
{
	size_t extract_size = min(buffer_size, _size_left);
	std::memcpy(buffer_p, _src_end_p - _size_left, extract_size);
	_size_left -= extract_size;
	return extract_size;
}

MemPack& eztcp::MemPack::operator=(MemPack&& mov) noexcept
{
	_src_p = mov._src_p;
	_src_end_p = mov._src_end_p;
	_size_left = mov._size_left;
	return *this;
}

eztcp::MemPack::MemPack(MemPack&& mov) noexcept :
	_src_p(mov._src_p),
	_src_end_p(mov._src_end_p),
	_size_left(mov._size_left)
{}
