#include "precompiled.h"

CManager gManager;

void CManager::ServerActivate()
{
	this->m_Enabled = gUtil.CvarRegister("stats_enabled", "0");
}

void CManager::ServerDeactivate()
{
	/**/
}

bool CManager::IsEnabled()
{
	if (this->m_Enabled)
	{
		return (this->m_Enabled->value != 0);
	}
	
	return false;
}
