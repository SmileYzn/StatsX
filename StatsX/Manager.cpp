#include "precompiled.h"

CManager gManager;

void CManager::ServerActivate()
{
	memset(this->m_Players, NULL, sizeof(this->m_Players));
}

void CManager::ServerDeactivate()
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		if (this->m_Players[i] != nullptr)
		{
			delete this->m_Players[i];
		}
	}
}

void CManager::GetIntoGame(CBasePlayer* Player)
{
	if (Player)
	{
		auto EntIndex = Player->entindex();

		auto pEntity = Player->edict();

		if (!FNullEnt(pEntity))
		{
			if (Player->IsBot())
			{
				this->m_Players[EntIndex] = new CPlayer(STRING(pEntity->v.netname), STRING(pEntity->v.netname));
			}
			else
			{
				this->m_Players[EntIndex] = new CPlayer(STRING(pEntity->v.netname), GETPLAYERAUTHID(pEntity));
			}
		}
	}
}

void CManager::Disconnected(CBasePlayer* Player)
{
	auto EntIndex = Player->entindex();

	if (this->m_Players[EntIndex] != nullptr)
	{
		// Save Stats
		if (!Player->IsBot())
		{
			this->m_Players[EntIndex]->SaveStats();
		}

		// Delete memory
		this->m_Players[EntIndex] = nullptr;
	}
}

void CManager::AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	if (Player)
	{
		if (type >= 0)
		{
			auto EntIndex = Player->entindex();

			if (this->m_Players[EntIndex] != nullptr)
			{
				this->m_Players[EntIndex]->Money[type] += amount;
			}
		}
	}
}

void CManager::TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	if (Player)
	{
		if (pevAttacker)
		{
			auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

			if (Attacker)
			{
				auto AttackerIndex = Attacker->entindex();

				if (this->m_Players[AttackerIndex] != nullptr)
				{
					this->m_Players[AttackerIndex]->Hits++;

					this->m_Players[AttackerIndex]->Damage += Player->m_lastDamageAmount;

					auto VictimIndex = Player->entindex();

					if (this->m_Players[VictimIndex] != nullptr)
					{
						this->m_Players[VictimIndex]->DamageReceive += Player->m_lastDamageAmount;
					}

					this->m_Players[AttackerIndex]->HitBox[Player->m_LastHitGroup]++;

					this->m_Players[AttackerIndex]->HitBoxDamage[Player->m_LastHitGroup] += Player->m_lastDamageAmount;

					this->m_RoundDamage[AttackerIndex][Player->entindex()] += Player->m_lastDamageAmount;

					this->m_RoundDamageSelf[AttackerIndex] += Player->m_lastDamageAmount;

					this->m_RoundDamageTeam[Attacker->m_iTeam] += Player->m_lastDamageAmount;

					if (Attacker->m_pActiveItem)
					{
						if (Attacker->m_pActiveItem->IsWeapon())
						{
							ItemInfo info = { 0 };

							if (Attacker->m_pActiveItem->GetItemInfo(&info))
							{
								this->m_Players[AttackerIndex]->WeaponStats[info.iId][WEAPON_HIT]++;

								this->m_Players[AttackerIndex]->WeaponStats[info.iId][WEAPON_DAMAGE]++;
							}
						}
					}
				}
			}
		}
	}
}

void CManager::Killed(CBasePlayer* Player, entvars_t* pevAttacker, int iGib)
{
	if (Player && pevAttacker)
	{
		auto Killer = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

		if (Killer)
		{
			auto VictimIndex = Player->entindex();

			auto KillerIndex = Killer->entindex();

			if (VictimIndex != KillerIndex)
			{
				if (this->m_Players[VictimIndex] != nullptr)
				{
					this->m_Players[VictimIndex]->Deaths++;
				}

				if (this->m_Players[KillerIndex] != nullptr)
				{
					this->m_Players[KillerIndex]->Frags++;

					this->m_RoundFrags[KillerIndex]++;

					if (Player->m_bHeadshotKilled)
					{
						this->m_Players[KillerIndex]->Headshot++;
					}

					if (Killer->m_pActiveItem)
					{
						if (Killer->m_pActiveItem->IsWeapon())
						{
							ItemInfo info = { 0 };

							if (Killer->m_pActiveItem->GetItemInfo(&info) != 0)
							{
								this->m_Players[KillerIndex]->WeaponStats[info.iId][WEAPON_KILL]++;

								if (this->m_Players[VictimIndex] != nullptr)
								{
									this->m_Players[VictimIndex]->WeaponStats[info.iId][WEAPON_DEATH]++;
								}
								
								if (Player->m_bHeadshotKilled)
								{
									this->m_Players[KillerIndex]->WeaponStats[info.iId][WEAPON_HEADSHOT]++;
								}
							}
						}
					}
				}

				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					auto Temp = UTIL_PlayerByIndexSafe(i);

					if (Temp)
					{
						auto TempIndex = Temp->entindex();

						if (this->m_Players[TempIndex] != nullptr)
						{
							if ((this->m_RoundDamage[TempIndex][VictimIndex] >= MANAGER_ASSISTANCE_DMG) && (TempIndex != Killer->entindex()))
							{
								this->m_Players[TempIndex]->Assists++;
							}

							if (!this->m_RoundVersus[TempIndex])
							{
								if (Temp->IsAlive())
								{
									if (Temp->m_iTeam == TERRORIST || Temp->m_iTeam == CT)
									{
										int FriendsAlive = 0;

										for (int j = 1;j <= gpGlobals->maxClients; ++j)
										{
											auto Friend = UTIL_PlayerByIndexSafe(j);

											if (Friend)
											{
												if (Friend->m_iTeam == Temp->m_iTeam)
												{
													if (Friend->IsAlive())
													{
														FriendsAlive++;
													}
												}
											}
										}

										if (FriendsAlive == 1)
										{
											int EnemyAlive = 0;

											for (int j = 1; j <= gpGlobals->maxClients; ++j)
											{
												auto Enemy = UTIL_PlayerByIndexSafe(j);

												if (Enemy)
												{
													if (Enemy->m_iTeam != Temp->m_iTeam)
													{
														if (Enemy->IsAlive())
														{
															EnemyAlive++;
														}
													}
												}
											}

											this->m_RoundVersus[TempIndex] = EnemyAlive;
										}
									}
								}
							}
						}
					}
				}

				if (!Killer->IsBot())
				{
					this->m_Players[KillerIndex]->SaveStats();
				}
				
				if (!Player->IsBot())
				{
					this->m_Players[VictimIndex]->SaveStats();
				}
			}
		}
	}
}

void CManager::SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim)
{
	if (Player)
	{
		if (playerAnim == PLAYER_ATTACK1)
		{
			if (Player->m_pActiveItem)
			{
				if (Player->m_pActiveItem->IsWeapon())
				{
					ItemInfo info = { 0 };

					if (Player->m_pActiveItem->GetItemInfo(&info))
					{
						auto EntIndex = Player->entindex();

						if (this->m_Players[EntIndex] != nullptr)
						{
							this->m_Players[EntIndex]->Shots++;

							this->m_Players[EntIndex]->WeaponStats[info.iId][WEAPON_SHOT]++;
						}
					}
				}
			}
		}
	}
}

void CManager::PlantBomb(entvars_t* pevOwner,bool Planted)
{
	auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pevOwner));

	if (Player)
	{
		auto EntIndex = Player->entindex();

		if (this->m_Players[EntIndex] != nullptr)
		{
			if (Planted)
			{
				this->m_RoundBombPlanter = EntIndex;

				this->m_Players[EntIndex]->BombStats[BOMB_PLANTED]++;
			}
			else
			{
				this->m_RoundBombPlanter = -1;

				this->m_Players[EntIndex]->BombStats[BOMB_PLANTING]++;
			}
		}
	}
}

void CManager::DefuseBombStart(CBasePlayer* Player)
{
	if (Player)
	{
		auto EntIndex = Player->entindex();

		if (this->m_Players[EntIndex] != nullptr)
		{
			this->m_Players[EntIndex]->BombStats[BOMB_DEFUSING]++;
		}
	}
}

void CManager::DefuseBombEnd(CBasePlayer* Player, bool Defused)
{
	if (Player)
	{
		if (Defused)
		{
			auto EntIndex = Player->entindex();

			if (this->m_Players[EntIndex] != nullptr)
			{
				this->m_Players[EntIndex]->BombStats[BOMB_DEFUSED]++;

				this->m_RoundBombDefuser = EntIndex;
			}
		}
	}
}

void CManager::ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType)
{
	if (this->m_RoundBombPlanter != -1)
	{
		auto Player = UTIL_PlayerByIndexSafe(this->m_RoundBombPlanter);

		if (Player)
		{
			auto EntIndex = Player->entindex();

			if (this->m_Players[EntIndex] != nullptr)
			{
				this->m_Players[EntIndex]->BombStats[BOMB_EXPLODED]++;
			}
		}
	}
}

void CManager::RoundFreezeEnd()
{
	memset(this->m_RoundDamage, 0, sizeof(this->m_RoundDamage));

	memset(this->m_RoundDamageSelf, 0, sizeof(this->m_RoundDamageSelf));

	memset(this->m_RoundDamageTeam, 0, sizeof(this->m_RoundDamageTeam));

	memset(this->m_RoundFrags, 0, sizeof(this->m_RoundFrags));

	memset(this->m_RoundVersus, 0, sizeof(this->m_RoundVersus));

	this->m_RoundBombPlanter = -1;

	this->m_RoundBombDefuser = -1;
}

void CManager::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (winStatus == WINSTATUS_TERRORISTS || winStatus == WINSTATUS_CTS)
	{
		if (event == ROUND_BOMB_DEFUSED)
		{
			if (this->m_RoundBombDefuser != -1)
			{
				auto Defuser = UTIL_PlayerByIndexSafe(this->m_RoundBombDefuser);

				if (Defuser)
				{
					auto DefuserIndex = Defuser->entindex();

					if (this->m_Players[DefuserIndex] != nullptr)
					{
						this->m_Players[DefuserIndex]->RoundWinShare += MANAGER_RWS_C4_DEFUSED;
					}				}
			}
		}
		else if (event == ROUND_TARGET_BOMB)
		{
			if (this->m_RoundBombPlanter != -1)
			{
				auto Planter = UTIL_PlayerByIndexSafe(this->m_RoundBombPlanter);

				if (Planter)
				{
					auto PlanterIndex = Planter->entindex();

					if (this->m_Players[PlanterIndex] != nullptr)
					{
						this->m_Players[PlanterIndex]->RoundWinShare += MANAGER_RWS_C4_EXPLODE;
					}
				}
			}
		}

		TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			auto Temp = UTIL_PlayerByIndexSafe(i);

			if (Temp)
			{
				auto TempIndex = Temp->entindex();

				if (this->m_Players[TempIndex] != nullptr)
				{
					this->m_Players[TempIndex]->Rounds[ROUND_PLAY]++;

					if (this->m_RoundFrags[TempIndex] > 0)
					{
						this->m_Players[TempIndex]->KillStreak[this->m_RoundFrags[TempIndex]]++;
					}

					if (Temp->m_iTeam == Winner)
					{
						this->m_Players[TempIndex]->Rounds[(Temp->m_iTeam == TERRORIST) ? ROUND_WIN_TR : ROUND_WIN_CT]++;

						// Versus
						if (this->m_RoundVersus[TempIndex] > 0)
						{
							this->m_Players[TempIndex]->Versus[this->m_RoundVersus[TempIndex]]++;
						}

						// Round Win Share
						if (this->m_RoundDamageSelf[TempIndex] > 0)
						{
							float RoundWinShare = (this->m_RoundDamageSelf[TempIndex] / this->m_RoundDamageTeam[Winner]);

							if (event == ROUND_BOMB_DEFUSED || event == ROUND_TARGET_BOMB)
							{
								RoundWinShare = (MANAGER_RWS_MAP_TARGET * RoundWinShare);
							}

							this->m_Players[TempIndex]->RoundWinShare += RoundWinShare;
						}
					}
					else
					{
						this->m_Players[TempIndex]->Rounds[(Temp->m_iTeam == TERRORIST) ? ROUND_LOSE_TR : ROUND_LOSE_CT]++;
					}
				}
			}
		}
	}
}
