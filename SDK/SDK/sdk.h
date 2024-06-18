
#include "Overlay/overlay.h"
#include "../includes.h"
#include "client.h"
#include "offsets.h"
class sdk
{
public:
	HANDLE driver;
	LPCSTR proc_classname = "SDL_app";
	LPCSTR proc_windowname = "Counter-Strike 2";
	const wchar_t* procname = L"cs2.exe";
	DWORD pid;
	uintptr_t procbase;
	uintptr_t clientdll;
	const wchar_t* clientdllname = L"client.dll";
	int error;
	class player
	{
	public:
		//C_BaseEntity 
		// Memory address
		static uintptr_t localplayer;
		static uintptr_t health;
		static uintptr_t teamnum;
		static uintptr_t m_lifeState;
		// Memory Values
		static int playerhealth;
		static int playerteamnum;
		static int playerm_lifeState;

		// CCSPlayerController // idk
		string m_sSanitizedPlayerName;
		bool m_bPawnIsAlive;
		int m_iPawnHealth;
		int m_iPawnArmor;
		int m_iScore;
		int m_iMVPs;
	};

	class camera
	{
	public:
		// CCSPlayerBase_CameraServices 
		int m_iFOV;
	};
	class gamerules
	{
	public:
		// C_CSGameRules
		bool m_bFreezePeriod;
		bool m_bWarmupPeriod;
		int m_iRoundTime;
		// can buy
		bool m_bTCantBuy;
		bool m_bCTCantBuy;
	};
	// functions
	BOOL IsElevated();
	void init();
	void checkproc();
	uintptr_t setupmodules();
	void setupoffsets();
	void initdriver();
	DWORD get_process_id(const wchar_t* process_name);
    uintptr_t get_module_base(const DWORD pid, const wchar_t* module_name); 
};