// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>



// reference additional headers your program requires here
#include "main.h"
#include "LTE_NR.h"
#include "LTENR_SubEvent.h"
#include "LTENR_Msg.h"

//For MSVC compiler. For GCC link via Linker command 
#pragma comment(lib,"LTE_NRLib.lib")
#pragma comment(lib,"Metrics.lib")
#pragma comment(lib,"NetworkStack.lib")
#pragma comment(lib,"Mobility.lib")
#pragma comment(lib,"PropagationModel.lib")

//LTENR-LOG
//Comment to disable logs
#define LTENR_LOG

#ifdef LTENR_LOG
#define LTENR_PROPAGATION_LOG
//#define LTENR_SDAP_LOG
//#define LTENR_PDCP_LOG
//#define LTENR_PRB_LOG
#define LTENR_RLC_LOG
#ifdef LTENR_RLC_LOG
//#define LTENR_RLC_BUFFERSTATUSREPORTING_LOG
//#define LTENR_RLC_TRANSMISSIONSTATUSNOTIFICATION_LOG
#endif
#endif
