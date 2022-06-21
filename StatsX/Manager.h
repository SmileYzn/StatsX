#pragma once

class CManager
{
public:
	void ServerActivate();
	void ServerDeactivate();

	bool IsEnabled();

private:
	cvar_t* m_Enabled = nullptr;
};

extern CManager gManager;