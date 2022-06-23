#pragma once

/**
* Bomb Group
*/
enum BombGroup
{
	BOMB_PLANTING = 0, // Test
	BOMB_PLANTED = 1, // Test
	BOMB_EXPLODED = 2, // Test
	BOMB_DEFUSING = 3, // Test
	BOMB_DEFUSED = 4, // Test
};

/**
* Round Group
*/
enum RoundGroup
{
	ROUND_PLAY = 0, // Test
	ROUND_WIN_TR = 1, // Test
	ROUND_LOSE_TR = 2, // Test
	ROUND_WIN_CT = 3, // Test
	ROUND_LOSE_CT = 4, // Test
};

/**
* Hitbox Group
*/
enum HitBoxGroup
{
	HITGROUP_GENERIC = 0, // Test
	HITGROUP_HEAD = 1, // Test
	HITGROUP_CHEST = 2, // Test
	HITGROUP_STOMACH = 3, // Test
	HITGROUP_LEFTARM = 4, // Test
	HITGROUP_RIGHTARM = 5, // Test
	HITGROUP_LEFTLEG = 6, // Test
	HITGROUP_RIGHTLEG = 7, // Test
	HITGROUP_SHIELD = 8, // Test
};

/**
* Weapon Stats Group
*/
enum WeaponStatsGroup
{
	WEAPON_KILL = 0, // Test
	WEAPON_DEATH = 1, // Test
	WEAPON_HEADSHOT = 2, // Test
	WEAPON_SHOT = 3, // Test
	WEAPON_HIT = 4, // Test
	WEAPON_DAMAGE = 5, // Test
};

/**
* Hack Stats Group
*/
enum HackStatsGroup
{
	HACK_WALL_FRAG		= 0, // OK Counting
	HACK_BLIND_FRAGS	= 1, // OK Counting
	HACK_ONE_SHOT       = 3, // TODO: Lacking todo if is really the first shot made
	HACK_NO_SCOPE		= 2, // OK Counting
};

class CPlayer
{
public:
	void Init(const char* Name, const  char* Auth);
	void Clear();

	char Name[32] = { 0 };
	char Auth[35] = { 0 };
	
	int Frags = 0;												// Ok
	int Assists = 0;											// Ok
	int Deaths = 0;												// Ok
	int Headshot = 0;											// Ok
	int Shots = 0;												// Ok
	int Hits = 0;												// Ok
	int Damage = 0;												// Ok
	int DamageReceive = 0;										// Ok
	float JoinTime = 0.0f;										// Ok
	float GameTime = 0.0f;										// To do: Add Total Game Time
	int Rounds[ROUND_LOSE_CT + 1] = { 0 };						// Ok
	float RoundWinShare = 0.0f;									// Ok
	int BombStats[BOMB_DEFUSED + 1] = { 0 };					// Test
	int HitBox[HITGROUP_SHIELD + 1] = { 0 };					// OK
	int HitBoxDamage[HITGROUP_SHIELD + 1] = { 0 };				// OK
	int WeaponStats[MAX_WEAPONS + 1][WEAPON_DAMAGE + 1] = { 0 };// Test
	int KillStreak[MAX_CLIENTS + 1] = { 0 };					// Test
	int Versus[MAX_CLIENTS + 1] = { 0 };						// Test
	int Money[RT_VIP_RESCUED_MYSELF + 1] = { 0 };				// OK
	int HackStats[HACK_NO_SCOPE + 1] = { 0 };											// To do: detect wall kills
};
