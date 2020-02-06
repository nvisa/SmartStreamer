#ifndef DEBUG_H
#define DEBUG_H

#include "libsmart/loguru.hpp"

namespace aselsmart {

#define gError(...) LOG_F(1, __VA_ARGS__)
#define gWarn(...) LOG_F(1, __VA_ARGS__)
#define gInfo(...) LOG_F(2, __VA_ARGS__)
#define gDebug(...) LOG_F(3, __VA_ARGS__)
#define gLog(...) LOG_F(4, __VA_ARGS__)
#define gLogV(...) LOG_F(5, __VA_ARGS__)
#define gLogVV(...) LOG_F(6, __VA_ARGS__)
#define gLogVVV(...) LOG_F(7, __VA_ARGS__)

#define gErrorS() LOG_S(1)
#define gWarnS() LOG_S(1)
#define gInfoS() LOG_S(2)
#define gDebugS() LOG_S(3)
#define gLogS() LOG_S(4)
#define gLogVS() LOG_S(5)
#define gLogVVS() LOG_S(6)
#define gLogVVVS() LOG_S(7)

static inline void initSmartDebugging(int level = 3)
{
	loguru::g_stderr_verbosity = level;
	char *var = getenv("SMART_DEBUG");
	if (var)
		loguru::g_stderr_verbosity = std::stoi(var);
}

}

#endif // DEBUG_H

