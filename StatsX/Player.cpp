#include "precompiled.h"

CPlayer::CPlayer(const char* Name, const  char* Auth)
{
	strncpy(this->Name, Name, sizeof(this->Name));

	strncpy(this->Auth, Auth, sizeof(this->Auth));

	this->Frags = 0;

	this->Assists = 0;

	this->Deaths = 0;

	this->Headshot = 0;

	this->Shots = 0;

	this->Hits = 0;

	this->Damage = 0;

	this->DamageReceive = 0;

	this->JoinTime = gpGlobals->time;

	memset(this->Rounds, 0, sizeof(this->Rounds));

	this->RoundWinShare = 0.0f;

	memset(this->BombStats, 0, sizeof(this->BombStats));

	memset(this->HitBox, 0, sizeof(this->HitBox));

	memset(this->WeaponStats, 0, sizeof(this->WeaponStats));

	memset(this->KillStreak, 0, sizeof(this->KillStreak));

	memset(this->Versus, 0, sizeof(this->Versus));

	memset(this->Money, 0, sizeof(this->Money));
}

void CPlayer::SaveStats()
{
	//
}