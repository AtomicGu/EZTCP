#ifdef DEBUG2

#ifdef _DEBUG
#pragma comment(lib,"../Debug/EZTCP.lib")
#pragma comment(lib,"E:/WAS/Works.old/MyLib/Class_Console/Debug/CLASSCONSOLE.lib")
#else
#endif

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "EZTCP.h"
#include "E:/WAS/Works.old/MyLib/Class_Console/Class_Console/class_console.h"

using namespace eztcp;
using namespace eztcp::err;

class RecvThread
{
public:
	RecvThread(Session& recv_session) :
		_recv_session(recv_session),
		_recv_thread(&RecvThread::main, this)
	{
		_recv_thread.detach();
	}

private:
	Session& _recv_session;
	std::thread _recv_thread;

private:
	void main()
	{
		try
		{
			while (true)
			{
				BlockPack recv_pack = _recv_session.recv();
				std::cout << "RE: " << recv_pack.get_buf() << std::endl;
			}
		}
		catch (SocketError& e)
		{
			std::cerr << "[RecvThread] SocketError: " << e.what() << std::endl;
		}
	}
};

Session* g_to_server_p;
RecvThread* g_recv_thread_p;

void send_message(Console* p_csl, std::istream& args_in)
{
	std::string message;
	args_in >> message;
	MemPack bp(message.c_str(), message.size() + 1);
	g_to_server_p->send(bp);
}

void send_file(Console* p_csl, std::istream& args_in)
{
	std::string file_path;
	args_in >> file_path;
	try
	{
		FilePack fp(file_path.c_str());
		g_to_server_p->send(fp);
	}
	catch (FileIOError& e)
	{
		p_csl->_out << "[main] FileIOError: " << e.what() << std::endl;
	}
}

void send_file_2(Console* p_csl, std::istream& args_in)
{
	std::string file_path;
	args_in >> file_path;
	try
	{
		FilePack fp(file_path.c_str());
		char cut_off[10];
		cut_off[9] = 0;
		fp.extract(cut_off, 9);
		p_csl->_out << "cut off :" << cut_off << "[end]\n" << std::flush;
		g_to_server_p->send(fp);
	}
	catch (FileIOError& e)
	{
		p_csl->_out << "[main] FileIOError: " << e.what() << std::endl;
	}
}

void reconnect(Console* p_csl, std::istream& args_in)
{
	delete g_to_server_p;
	g_to_server_p = new Session("127.0.0.1", 23333);
	delete g_recv_thread_p;
	g_recv_thread_p = new RecvThread(*g_to_server_p);
}

CmdInfo sm = {
	send_message,
	"sm",
	"send message"
};

CmdInfo sf = {
	send_file,
	"sf",
	"send file"
};

CmdInfo sf2 = {
	send_file_2,
	"sf2",
	"send file but cut a little off"
};

CmdInfo rc =
{
	reconnect,
	"rc",
	"reconnect"
};

CMD_MAP_BEGIN(gs_cmd_map)
CMD_MAP(sm)
CMD_MAP(sf)
CMD_MAP(rc)
CMD_MAP(sf2)
CMD_MAP(default_help)
CMD_MAP(default_exit)
CMD_MAP_END()

int main()
{
	g_to_server_p = new Session("47.101.35.106", 23333);
	g_recv_thread_p = new RecvThread(*g_to_server_p);
	try
	{
		return Console(gs_cmd_map).main();
	}
	catch (SocketError& e)
	{
		std::cerr << "[main] SocketError: " << e.what() << std::endl;
	}
}

#endif