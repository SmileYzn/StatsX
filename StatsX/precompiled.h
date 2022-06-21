#pragma once

// Disable security function warnings (MSVC)
#define _CRT_SECURE_NO_WARNINGS

// If is not MSVC build
#ifndef _WIN32
#define _GLIBCXX_USE_CXX11_ABI		0
#define _stricmp					strcasecmp
#define _strnicmp					strncasecmp
#define _strdup						strdup
#define _unlink						unlink
#define _vsnprintf					vsnprintf
#define _write						write
#define _close						close
#define _acces						access
#define _vsnwprintf					vswprintf
#endif

// System Includes
#include <string>
#include <vector>
#include <map>
#include <array>
#include <iterator>

// CSSDK
#include <extdll.h>
#include <eiface.h>

// MetaMod SDK
#include <meta_api.h> 

// ReHLDS Api
#include <rehlds_api.h>
#include <rehlds_interfaces.h>

// ReGameDLL Api
#include <regamedll_api.h>

// Plugin Includes
#include "MetaMod.h"
#include "MetaDLL.h"
#include "MetaEngine.h"
#include "ReAPI.h"
#include "ReGameDLL.h"

// Stats X
#include "StatsX.h"
#include "StatsConst.h"
#include "Util.h"
#include "Player.h"
#include "Manager.h"
