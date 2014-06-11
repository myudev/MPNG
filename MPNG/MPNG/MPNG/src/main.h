#define _RELEASE


#ifndef _WIN32 
#include "../../../SDK/amx/amx.h"
#include "../../../SDK/plugincommon.h"
#else
#include "../../../SDK/amx/amx.h"
#include "../../../SDK/plugincommon.h"
#endif

#ifndef _WIN32
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <ctype.h>
#define MAX_PATH 260
#include <dlfcn.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include <png.hpp>
#include <pthread.h>
#define SLP(t) { usleep(t * 1000); }
//#include "../lib/win32/pngpp-0.2.5/
#else
#define NOMINMAX
#define PTW32_STATIC_LIB
#define _CRT_SECURE_NO_WARNINGS // well... lol
#include <windows.h>
#include <string>
#include <vector>

#include <png.hpp>
#include <pthread.h>
#define SLP(t) { Sleep(t); }
#endif

#include <list>

#ifndef _WIN32 
#	define INT		int
#	define FLOAT	float
#	define BOOL		int
#	define DWORD	unsigned long
#	define BYTE		unsigned char
#	define WORD		unsigned short
#	define TRUE		(1)
#	define FALSE	(0)
typedef char *PCHAR;
#endif

#include "Manager.h"

#ifndef CHECK_PARAMS
	#define CHECK_PARAMS(m,n)                                                                                           \
		do                                                                                                              \
		{                                                                                                               \
			if (params[0] != (m * 4))                                                                                   \
			{                                                                                                           \
				logprintf("[Send Bullet:] Error: Incorrect parameter count on \"" n "\", %d != %d\n", m, params[0] / 4); \
				return 0;                                                                                               \
			}                                                                                                           \
		}                                                                                                               \
		while (0)
#endif

#define _MAKE_STRING(x) #x
#define MAKE_STRING(x) _MAKE_STRING(x)
typedef void (*logprintf_t)(char* format, ...);
extern logprintf_t logprintf;
extern void **p_SomeInfo;