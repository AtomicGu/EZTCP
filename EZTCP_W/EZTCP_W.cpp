#include "EZTCP_W.h"

int mass_send(SOCKET s, const char* buf, SIZET len) noexcept
{
	while (len > 0x40000000)
	{
		if (send(s, buf, 0x40000000, NULL) == SOCKET_ERROR)
			return SOCKET_ERROR;
		buf += 0x40000000;
		len -= 0x40000000;
	}
	if (send(s, buf, len, NULL) == SOCKET_ERROR)
		return SOCKET_ERROR;
	return NULL;
}

int mass_recv(SOCKET s, char* buf, SIZET len) noexcept
{
	while (len > 0x40000000)
	{
		if (recv(s, buf, 0x40000000, MSG_WAITALL) != 0x40000000)
			return SOCKET_ERROR;
		buf += 0x40000000;
		len -= 0x40000000;
	}
	if (recv(s, buf, len, MSG_WAITALL) != len)
		return SOCKET_ERROR;
	return NULL;
}
