#include "precompiled.h"

CStats gStats;

void CStats::ServerActivate()
{
	memset(this->m_Data, 0, sizeof(this->m_Data));
}

void CStats::ServerDeactivate()
{
	memset(this->m_Data, 0, sizeof(this->m_Data));
}

void CStats::GetIntoGame(CBasePlayer* Player)
{
	if (Player)
	{
		auto pEntity = Player->edict();

		if (!FNullEnt(pEntity))
		{
			auto EntIndex = Player->entindex();

			if (Player->IsBot())
			{
				this->m_Data[EntIndex].Init(STRING(pEntity->v.netname), STRING(pEntity->v.netname));
			}
			else
			{
				this->m_Data[EntIndex].Init(STRING(pEntity->v.netname), GETPLAYERAUTHID(pEntity));
			}
		}
	}
}

void CStats::Disconnected(CBasePlayer* Player)
{
	this->m_Data[Player->entindex()].Clear();
}

void CStats::AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	if (gManager.IsEnabled())
	{
		if (Player)
		{
			if (type >= RT_NONE && type <= RT_VIP_RESCUED_MYSELF)
			{
				this->m_Data[Player->entindex()].Money[type] += amount;
			}
		}
	}
}

void CStats::TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	if (gManager.IsEnabled())
	{
		if (Player)
		{
			if (pevAttacker)
			{
				auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

				if (Attacker)
				{
					auto AttackerIndex = Attacker->entindex();

					this->m_Data[AttackerIndex].Hits++;

					this->m_Data[AttackerIndex].Damage += Player->m_lastDamageAmount;

					auto VictimIndex = Player->entindex();

					this->m_Data[VictimIndex].DamageReceive += Player->m_lastDamageAmount;

					this->m_Data[AttackerIndex].HitBox[Player->m_LastHitGroup]++;

					this->m_Data[AttackerIndex].HitBoxDamage[Player->m_LastHitGroup] += Player->m_lastDamageAmount;

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
								this->m_Data[AttackerIndex].WeaponStats[info.iId][WEAPON_HIT]++;

								this->m_Data[AttackerIndex].WeaponStats[info.iId][WEAPON_DAMAGE]++;
							}
						}
					}
				}
			}
		}
	}
}

void CStats::Killed(CBasePlayer* Player, entvars_t* pevAttacker, int iGib)
{
	if (gManager.IsEnabled())
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
					this->m_Data[VictimIndex].Deaths++;

					this->m_Data[KillerIndex].Frags++;

					this->m_RoundFrags[KillerIndex]++;

					if (Player->m_bHeadshotKilled)
					{
						this->m_Data[KillerIndex].Headshot++;
					}

					if (Killer->m_pActiveItem)
					{
						if (Killer->m_pActiveItem->IsWeapon())
						{
							ItemInfo info = { 0 };

							if (Killer->m_pActiveItem->GetItemInfo(&info) != 0)
							{
								this->m_Data[KillerIndex].WeaponStats[info.iId][WEAPON_KILL]++;

								this->m_Data[VictimIndex].WeaponStats[info.iId][WEAPON_DEATH]++;

								if (Player->m_bHeadshotKilled)
								{
									this->m_Data[KillerIndex].WeaponStats[info.iId][WEAPON_HEADSHOT]++;
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

							if ((this->m_RoundDamage[TempIndex][VictimIndex] >= MANAGER_ASSISTANCE_DMG) && (TempIndex != Killer->entindex()))
							{
								this->m_Data[TempIndex].Assists++;
							}

							if (!this->m_RoundVersus[TempIndex])
							{
								if (Temp->IsAlive())
								{
									if (Temp->m_iTeam == TERRORIST || Temp->m_iTeam == CT)
									{
										int FriendsAlive = 0;

										for (int j = 1; j <= gpGlobals->maxClients; ++j)
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
			}
		}
	}
}

void CStats::SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim)
{
	if (gManager.IsEnabled())
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

							this->m_Data[EntIndex].Shots++;

							this->m_Data[EntIndex].WeaponStats[info.iId][WEAPON_SHOT]++;
						}
					}
				}
			}
		}
	}
}

void CStats::PlantBomb(entvars_t* pevOwner, bool Planted)
{
	if (gManager.IsEnabled())
	{
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pevOwner));

		if (Player)
		{
			auto EntIndex = Player->entindex();

			if (Planted)
			{
				this->m_RoundBombPlanter = EntIndex;

				this->m_Data[EntIndex].BombStats[BOMB_PLANTED]++;
			}
			else
			{
				this->m_RoundBombPlanter = -1;

				this->m_Data[EntIndex].BombStats[BOMB_PLANTING]++;
			}
		}
	}
}

void CStats::DefuseBombStart(CBasePlayer* Player)
{
	if (gManager.IsEnabled())
	{
		if (Player)
		{
			auto EntIndex = Player->entindex();

			this->m_Data[EntIndex].BombStats[BOMB_DEFUSING]++;
		}
	}
}

void CStats::DefuseBombEnd(CBasePlayer* Player, bool Defused)
{
	if (gManager.IsEnabled())
	{
		if (Player)
		{
			if (Defused)
			{
				auto EntIndex = Player->entindex();

				this->m_Data[EntIndex].BombStats[BOMB_DEFUSED]++;

				this->m_RoundBombDefuser = EntIndex;
			}
		}
	}
}

void CStats::ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType)
{
	if (gManager.IsEnabled())
	{
		if (this->m_RoundBombPlanter != -1)
		{
			auto Player = UTIL_PlayerByIndexSafe(this->m_RoundBombPlanter);

			if (Player)
			{
				auto EntIndex = Player->entindex();

				this->m_Data[EntIndex].BombStats[BOMB_EXPLODED]++;
			}
		}
	}
}

void CStats::RoundFreezeEnd()
{
	if (gManager.IsEnabled())
	{
		memset(this->m_RoundDamage, 0, sizeof(this->m_RoundDamage));

		memset(this->m_RoundDamageSelf, 0, sizeof(this->m_RoundDamageSelf));

		memset(this->m_RoundDamageTeam, 0, sizeof(this->m_RoundDamageTeam));

		memset(this->m_RoundFrags, 0, sizeof(this->m_RoundFrags));

		memset(this->m_RoundVersus, 0, sizeof(this->m_RoundVersus));

		this->m_RoundBombPlanter = -1;

		this->m_RoundBombDefuser = -1;
	}
}

void CStats::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (gManager.IsEnabled())
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

						this->m_Data[DefuserIndex].RoundWinShare += MANAGER_RWS_C4_DEFUSED;
					}
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

						this->m_Data[PlanterIndex].RoundWinShare += MANAGER_RWS_C4_EXPLODE;
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

					this->m_Data[TempIndex].Rounds[ROUND_PLAY]++;

					if (this->m_RoundFrags[TempIndex] > 0)
					{
						this->m_Data[TempIndex].KillStreak[this->m_RoundFrags[TempIndex]]++;
					}

					if (Temp->m_iTeam == Winner)
					{
						this->m_Data[TempIndex].Rounds[(Temp->m_iTeam == TERRORIST) ? ROUND_WIN_TR : ROUND_WIN_CT]++;

						// Versus
						if (this->m_RoundVersus[TempIndex] > 0)
						{
							this->m_Data[TempIndex].Versus[this->m_RoundVersus[TempIndex]]++;
						}

						// Round Win Share
						if (this->m_RoundDamageSelf[TempIndex] > 0)
						{
							float RoundWinShare = (this->m_RoundDamageSelf[TempIndex] / this->m_RoundDamageTeam[Winner]);

							if (event == ROUND_BOMB_DEFUSED || event == ROUND_TARGET_BOMB)
							{
								RoundWinShare = (MANAGER_RWS_MAP_TARGET * RoundWinShare);
							}

							this->m_Data[TempIndex].RoundWinShare += RoundWinShare;
						}
					}
					else
					{
						this->m_Data[TempIndex].Rounds[(Temp->m_iTeam == TERRORIST) ? ROUND_LOSE_TR : ROUND_LOSE_CT]++;
					}
				}
			}
		}
	}
}
