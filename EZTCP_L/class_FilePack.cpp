#include "EZTCP_L.h"
#include <cstring>

using namespace eztcp;
using namespace eztcp::err;

FilePack::FilePack(const char* file_path_ps) :
	_file_h(open(file_path_ps, O_RDONLY, S_IRUSR))
{
	if (_file_h == -1)
		throw FileIOError(errno);
	struct stat temp_stat;
	if (stat(file_path_ps, &temp_stat) == -1)
		throw FileIOError(errno);
	_size_left = temp_stat.st_size;
	_file_path_ps = new char[strlen(file_path_ps) + 1];
	strcpy(_file_path_ps, file_path_ps);
}

eztcp::FilePack::FilePack(RecvPack& recv_pack, const char* save_path_ps) :
	_file_h(open(save_path_ps, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH)),
	_size_left(recv_pack.size_left())
{
	if (_file_h == -1)
		throw FileIOError(errno);
	try
	{
		char buffer[8192];
		while (SIZET extracted = recv_pack.extract(buffer, 8192))
		{
			if (write(_file_h, &buffer, extracted) == -1)
			{
				close(_file_h);
				throw FileIOError(errno);
			}
		}
	}
	catch (SocketError& e)
	{
		close(_file_h);
		throw e;
	}
	_file_path_ps = new char[strlen(save_path_ps) + 1];
	strcpy(_file_path_ps, save_path_ps);
}

FilePack::~FilePack() noexcept
{
	delete[] _file_path_ps;
	close(_file_h);
}

SIZET eztcp::FilePack::extract(void* buffer_p, SIZET buffer_size)
{
	ssize_t extract_size;
	if ((extract_size = read(_file_h, buffer_p, buffer_size)) == -1)
		throw FileIOError(errno);
	_size_left -= extract_size;
	return extract_size;
}
