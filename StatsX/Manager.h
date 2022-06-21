#pragma once

#define MANAGER_ASSISTANCE_DMG  50	// Mininum damage to take assistance for player
#define MANAGER_RWS_MAP_TARGET  0.7	// Round Win Share:
#define MANAGER_RWS_C4_EXPLODE  0.3	// Round Win Share:
#define MANAGER_RWS_C4_DEFUSED  0.3	// Round Win Share:

class CManager
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void GetIntoGame(CBasePlayer* Player);
	void Disconnected(CBasePlayer* Player);
	void AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange);
	void TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	void Killed(CBasePlayer* Player, entvars_t* pevAttacker, int iGib);
	void SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim);
	void PlantBomb(entvars_t* pevOwner,bool Planted);
	void DefuseBombStart(CBasePlayer* Player);
	void DefuseBombEnd(CBasePlayer* Player, bool Defused);
	void ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType);
	void RoundFreezeEnd();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

private:
	CPlayer* m_Players[MAX_CLIENTS + 1] = { nullptr };

	int m_RoundDamage[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { 0 };
	int m_RoundDamageSelf[MAX_CLIENTS + 1];
	int m_RoundDamageTeam[SPECTATOR + 1] = { 0 };
	int m_RoundFrags[MAX_CLIENTS + 1] = { 0 };
	int m_RoundVersus[MAX_CLIENTS + 1] = { 0 };
	int m_RoundBombPlanter = -1;
	int m_RoundBombDefuser = -1;
};

extern CManager gManager;