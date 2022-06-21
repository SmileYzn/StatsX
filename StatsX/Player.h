#pragma once

class CPlayer
{
public:
	CPlayer(const char* Name, const  char* Auth);

	void SaveStats();

	char Name[32];
	char Auth[35];

	int		Frags;												// Test
	int		Assists;											// Test
	int		Deaths;												// Test
	int		Headshot;											// Test
	int		Shots;												// Test
	int		Hits;												// Test
	int		Damage;												// Test
	int		DamageReceive;										// Test
	float	JoinTime;											// Test
	int		Rounds[ROUND_LOSE_CT+1];							// Test
	float	RoundWinShare;										// Test
	int		BombStats[BOMB_DEFUSED + 1];						// Test
	int		HitBox[HITGROUP_SHIELD + 1];						// Test
	int		HitBoxDamage[HITGROUP_SHIELD + 1];					// Test
	int		WeaponStats[MAX_WEAPONS + 1][WEAPON_DAMAGE + 1];	// Test
	int		KillStreak[MAX_CLIENTS + 1];						// Test
	int		Versus[MAX_CLIENTS + 1];							// Test
	int		Money[RT_VIP_RESCUED_MYSELF + 1];					// Test
};
