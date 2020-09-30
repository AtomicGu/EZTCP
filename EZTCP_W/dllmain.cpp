#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

void dll_process_detach()
{
	if (int error_code = WSACleanup())
	{
		ExitProcess(error_code);
	};
}

void dll_process_attach()
{
	WSADATA wsd;
	if (int error_code = WSAStartup(MAKEWORD(2, 2), &wsd))
	{
		ExitProcess(error_code);
	};
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_DETACH:
		dll_process_detach();
		break;
	case DLL_PROCESS_ATTACH:
		dll_process_attach();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
