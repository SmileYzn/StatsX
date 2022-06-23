#pragma once

enum UTIL_SAY_TEXT_COLOR
{
	PRINT_TEAM_DEFAULT = 0,
	PRINT_TEAM_GREY = -1,
	PRINT_TEAM_RED = -2,
	PRINT_TEAM_BLUE = -3,
};

class CUtil
{
public:
	cvar_t* CvarRegister(const char* Name, const char* Value);
	void ServerPrint(const char* Format, ...);
	void SayText(edict_t* pEntity, int Sender, const char* Format, ...);
};

extern CUtil gUtil;