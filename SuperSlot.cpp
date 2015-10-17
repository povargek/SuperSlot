#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

#if defined __LINUX__
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <ctype.h>
#else
#include <Windows.h>
#endif



#if defined __LINUX__
#define CConsolePool 0x81CA4B8
#define CConsole_SetIntVar 0x80A0040
#define RakServerOffset 0x40
#define RakServerInterface_SetAllowedPlayers 0x38
#else
#define CConsolePool 0x4F5FBC 
#define CConsole_SetIntVar 0x48B5D0
#define RakServerOffset 0x40
#define RakServerInterface_SetAllowedPlayers 0x34
#endif


typedef void (*logprintf_t)(char* format, ...);

logprintf_t logprintf;
void **ppPluginData;
extern void *pAMXFunctions;
void **ppPlayers;
#if defined __LINUX__
unsigned char bServer;
#endif

#if defined __LINUX__
typedef void (*SetIntVar_t)(void *PoolPtr, char *varname, int data);
typedef void (*SetAllowedPlayers_t)(void *PoolPtr, unsigned short usPlayersNumber);
#else
typedef void(__thiscall* SetIntVar_t)(void *PoolPtr, char *varname, int data);
typedef void(__thiscall* SetAllowedPlayers_t)(void *PoolPtr, unsigned short usPlayersNumber);
#endif

void SetSampLevelMaxPlayers(int iCount)
{
	void **pConsolePool = (void **)(*(unsigned int *)CConsolePool);
	if (pConsolePool == 0) logprintf("Error. Console Pool is null");
	#if defined __LINUX__
	((SetIntVar_t)(CConsole_SetIntVar))(pConsolePool, "maxplayers", iCount);
	#else
	((SetIntVar_t)(CConsole_SetIntVar))(pConsolePool, "maxplayers", iCount);
	#endif
	
}

void SetLowNetworkpLevelMaxPlayers(unsigned short usCount)
{
	void *pRakServer = (void*)*(unsigned long*)((unsigned char*)*(unsigned long*)ppPlayers + RakServerOffset);
	((SetAllowedPlayers_t)(*(unsigned long*)((unsigned char*)*(unsigned long*)pRakServer + RakServerInterface_SetAllowedPlayers)))(pRakServer, usCount);
}



static cell AMX_NATIVE_CALL n_SetServerMaxPlayers(AMX* amx, cell* params)
{
	if (
			(unsigned long*)ppPlayers && *(unsigned long*)ppPlayers &&
			*(unsigned long*)((unsigned char*)*(unsigned long*)ppPlayers + RakServerOffset) &&
			*(unsigned long*)*(unsigned long*)((unsigned char*)*(unsigned long*)ppPlayers + RakServerOffset)
		)
		{		
			unsigned short iMaxPlayerID = params[1];
			if (iMaxPlayerID < 1 || iMaxPlayerID > 1000) return 0;
				 
			SetSampLevelMaxPlayers((int)iMaxPlayerID);
			SetLowNetworkpLevelMaxPlayers(iMaxPlayerID);
			return 1;

		}
	return -1;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
#if defined __LINUX__
	ppPlayers = (void**)*(unsigned long*)((unsigned char*)ppData[PLUGIN_DATA_CALLPUBLIC_FS] + 10);
#else
	ppPlayers = (void**)*(unsigned long*)((unsigned char*)ppData[PLUGIN_DATA_CALLPUBLIC_FS] + 1);
#endif

	logprintf(" SuperSlot plugin for SA:MP Server 0.3.7 R2-1 loaded! (http://vk.com/povargek)");
	return true;
}
PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	logprintf(" SuperSlot plugin for SA:MP Server 0.3.7 R2-1 unloaded! (http://vk.com/povargek)");
}

AMX_NATIVE_INFO pluginNatives[] =
{
	{ "SetServerMaxPlayers", n_SetServerMaxPlayers },
	{ 0,		0 }
};

#define USENAMETABLE(hdr) \
                        ((hdr)->defsize==sizeof(AMX_FUNCSTUBNT))
#define NUMENTRIES(hdr,field,nextfield) \
                        (unsigned)(((hdr)->nextfield - (hdr)->field) / (hdr)->defsize)
#define GETENTRY(hdr,table,index) \
                        (AMX_FUNCSTUB *)((unsigned char*)(hdr) + (unsigned)(hdr)->table + (unsigned)index*(hdr)->defsize)
#define GETENTRYNAME(hdr,entry) \
                        ( USENAMETABLE(hdr) \
                           ? (char *)((unsigned char*)(hdr) + (unsigned)((AMX_FUNCSTUBNT*)(entry))->nameofs) \
                           : ((AMX_FUNCSTUB*)(entry))->name )

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{	
	return amx_Register( amx, pluginNatives, -1 );
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
}
