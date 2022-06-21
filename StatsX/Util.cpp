#include "precompiled.h"

CUtil gUtil;

void CUtil::ServerPrint(const char* Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[255] = { 0 };

	int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (Length > 254)
	{
		Length = 254;
	}

	Buffer[Length++] = '\n';
	Buffer[Length] = 0;

	SERVER_PRINT(Buffer);
}

void CUtil::SayText(edict_t* pEntity, int Sender, const char* Format, ...)
{
	static int iMsgSayText;

	if (iMsgSayText || (iMsgSayText = GET_USER_MSG_ID(PLID, "SayText", NULL)))
	{
		va_list argList;

		va_start(argList, Format);

		char Buffer[191] = { 0 };

		vsnprintf(Buffer, sizeof(Buffer), Format, argList);

		va_end(argList);

		char SayText[191] = { 0 };

		snprintf(SayText, sizeof(SayText), "\4[%s]\1 %s", Plugin_info.logtag, Buffer);

		if (Sender < PRINT_TEAM_BLUE || Sender > gpGlobals->maxClients)
		{
			Sender = PRINT_TEAM_DEFAULT;
		}
		else if (Sender < PRINT_TEAM_DEFAULT)
		{
			Sender = abs(Sender) + 32;
		}

		if (pEntity && !FNullEnt(pEntity))
		{
			if (!(pEntity->v.flags & FL_FAKECLIENT))
			{
				MESSAGE_BEGIN(MSG_ONE, iMsgSayText, nullptr, pEntity);
				WRITE_BYTE(Sender ? Sender : ENTINDEX(pEntity));
				WRITE_STRING("%s");
				WRITE_STRING(SayText);
				MESSAGE_END();
			}
		}
		else
		{
			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				edict_t* pTempEntity = INDEXENT(i);

				if (!FNullEnt(pTempEntity))
				{
					if (!(pTempEntity->v.flags & FL_FAKECLIENT))
					{
						MESSAGE_BEGIN(MSG_ONE, iMsgSayText, nullptr, pTempEntity);
						WRITE_BYTE(Sender ? Sender : i);
						WRITE_STRING("%s");
						WRITE_STRING(SayText);
						MESSAGE_END();
					}
				}
			}
		}
	}
}