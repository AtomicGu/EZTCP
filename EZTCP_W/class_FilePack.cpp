#include "EZTCP_W.h"
#include <cstring>

using namespace eztcp;
using namespace eztcp::err;

FilePack::FilePack(const char* file_path_ps) :
	_file_h(CreateFileA(
		file_path_ps,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	))
{
	if (_file_h == INVALID_HANDLE_VALUE)
		throw FileIOError(static_cast<int>(GetLastError()));
	_file_path_ps = new char[strlen(file_path_ps) + 1];
	strcpy(_file_path_ps, file_path_ps);
	GetFileSizeEx(_file_h, reinterpret_cast<PLARGE_INTEGER>(&_size_left));
}

eztcp::FilePack::FilePack(RecvPack& recv_pack, const char* save_path_ps) :
	_file_h(CreateFileA(
		save_path_ps,
		GENERIC_READ | GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	)),
	_size_left(recv_pack.size_left())
{
	if (_file_h == INVALID_HANDLE_VALUE)
		throw FileIOError(static_cast<int>(GetLastError()));
	try
	{
		char buffer[8192];
		while (SIZET extracted = recv_pack.extract(buffer, 8192))
		{
			DWORD written;
			if (!WriteFile(_file_h, buffer, extracted, &written, NULL))
			{
				CloseHandle(_file_h);
				throw FileIOError(GetLastError());
			}
		}
	}
	catch (SocketError& e)
	{
		CloseHandle(_file_h);
		throw e;
	}
	_file_path_ps = new char[strlen(save_path_ps) + 1];
	strcpy(_file_path_ps, save_path_ps);
}

FilePack::~FilePack() noexcept
{
	delete[] _file_path_ps;
	CloseHandle(_file_h);
}

SIZET eztcp::FilePack::extract(void* buffer_p, SIZET buffer_size)
{
	DWORD extract_size;
	if (!ReadFile(_file_h, buffer_p, buffer_size, &extract_size, NULL))
		throw FileIOError(static_cast<int>(GetLastError()));
	_size_left -= extract_size;
	return extract_size;
}
