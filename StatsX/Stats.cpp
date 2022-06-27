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

					auto ItemIndex = this->IsWeapon(Attacker, true);

					if (ItemIndex)
					{
						this->m_Data[AttackerIndex].WeaponStats[ItemIndex][WEAPON_HIT]++;

						this->m_Data[AttackerIndex].WeaponStats[ItemIndex][WEAPON_DAMAGE] += Player->m_lastDamageAmount;
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
		if (Player)
		{
			if (!Player->m_bKilledByBomb)
			{
				auto Killer = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

				if (Killer)
				{
					if (Killer->IsPlayer())
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

							auto ItemIndex = this->IsWeapon(Killer, true);

							if (ItemIndex)
							{
								this->m_Data[KillerIndex].WeaponStats[ItemIndex][WEAPON_KILL]++;

								this->m_Data[VictimIndex].WeaponStats[ItemIndex][WEAPON_DEATH]++;

								if (Player->m_bHeadshotKilled)
								{
									this->m_Data[KillerIndex].WeaponStats[ItemIndex][WEAPON_HEADSHOT]++;
								}

								if (ItemIndex != WEAPON_AWP)
								{
									if (Player->m_iLastClientHealth >= 100)
									{
										if (Player->m_lastDamageAmount >= 100)
										{
											this->m_Data[KillerIndex].HackStats[HACK_ONE_SHOT]++;
										}
									}
								}

								if (ItemIndex == WEAPON_AWP || ItemIndex == WEAPON_SCOUT || ItemIndex == WEAPON_G3SG1 || ItemIndex == WEAPON_SG550)
								{
									if (Killer->m_iClientFOV == DEFAULT_FOV)
									{
										this->m_Data[KillerIndex].HackStats[HACK_NO_SCOPE]++;
									}
								}

								if (ItemIndex != WEAPON_HEGRENADE)
								{
									if (!this->IsVisible(Killer, Player))
									{
										this->m_Data[KillerIndex].HackStats[HACK_WALL_FRAG]++;
									}
								}
							}

							if (this->Isblind(Killer))
							{
								this->m_Data[KillerIndex].HackStats[HACK_BLIND_FRAG]++;
							}

							if (this->InsideSmoke(Killer, Player, 115.0f) != 0)
							{
								this->m_Data[KillerIndex].HackStats[HACK_SMOKE_FRAG]++;

								gUtil.SayText(Killer->edict(), Killer->entindex(), "%s killed %s", STRING(Killer->edict()->v.netname), STRING(Player->edict()->v.netname));
							}

							int* PlayerCount = this->CountAlive();

							for (int i = 1; i <= gpGlobals->maxClients; ++i)
							{
								auto Temp = UTIL_PlayerByIndexSafe(i);

								if (Temp)
								{
									if (Temp->m_iTeam == TERRORIST || Temp->m_iTeam == CT)
									{
										auto TempIndex = Temp->entindex();

										if (TempIndex != Killer->entindex())
										{
											if (this->m_RoundDamage[TempIndex][VictimIndex] >= MANAGER_ASSISTANCE_DMG)
											{
												this->m_Data[TempIndex].Assists++;
											}
										}

										if (!this->m_RoundVersus[TempIndex])
										{
											if (PlayerCount[Temp->m_iTeam] == 1)
											{
												this->m_RoundVersus[TempIndex] = PlayerCount[Temp->m_iTeam == TERRORIST ? CT : TERRORIST];
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
}

void CStats::SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim)
{
	if (gManager.IsEnabled())
	{
		if (Player)
		{
			if (playerAnim == PLAYER_ATTACK1 || playerAnim == PLAYER_ATTACK2)
			{
				auto ItemIndex = this->IsWeapon(Player, false);

				if (ItemIndex)
				{
					auto EntIndex = Player->entindex();

					this->m_Data[EntIndex].Shots++;

					this->m_Data[EntIndex].WeaponStats[ItemIndex][WEAPON_SHOT]++;
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

						if (this->m_RoundVersus[TempIndex] > 0)
						{
							this->m_Data[TempIndex].Versus[this->m_RoundVersus[TempIndex]]++;
						}

						if (this->m_RoundDamageSelf[TempIndex] > 0)
						{
							float RoundWinShare = (float)((float)this->m_RoundDamageSelf[TempIndex] / (float)this->m_RoundDamageTeam[Winner]);

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

bool CStats::IsVisible(CBasePlayer* Player, CBasePlayer* Target)
{
	if (!FNullEnt(Player) && !FNullEnt(Target))
	{
		auto pEntity = Player->edict();

		auto pTarget = Target->edict();

		if (pTarget->v.effects & EF_NODRAW || pTarget->v.flags & FL_NOTARGET)
		{
			return false;
		}

		Vector vLooker = pEntity->v.origin + pEntity->v.view_ofs;

		Vector vTarget = pTarget->v.origin + pTarget->v.view_ofs;

		TraceResult tr = { 0 };

		TRACE_LINE(vLooker, vTarget, FALSE, pEntity, &tr);

		if (tr.fInOpen && tr.fInWater)
		{
			return false;
		}
		else if ((tr.flFraction == 1.0) || (ENTINDEX(tr.pHit) == ENTINDEX(pTarget)))
		{
			return true;
		}
	}

	return false;
}

bool CStats::Isblind(CBasePlayer* Player)
{
	if ((Player->m_blindStartTime + Player->m_blindHoldTime) >= gpGlobals->time)
	{
		return true;
	}

	return false;
}

int CStats::IsWeapon(CBasePlayer* Player, bool AllowKnife)
{
	if (Player->m_pActiveItem)
	{
		if (Player->m_pActiveItem->IsWeapon())
		{
			auto ItemSlot = Player->m_pActiveItem->iItemSlot();

			if (ItemSlot == PRIMARY_WEAPON_SLOT || ItemSlot == PISTOL_SLOT || ItemSlot == KNIFE_SLOT || ItemSlot == GRENADE_SLOT)
			{
				auto ItemIndex = Player->m_pActiveItem->m_iId;

				if (ItemIndex != WEAPON_SMOKEGRENADE && ItemIndex != WEAPON_FLASHBANG)
				{
					if (!AllowKnife && ItemIndex == WEAPON_KNIFE)
					{
						return WEAPON_NONE;
					}

					return false;
				}
			}
		}
	}

	return WEAPON_NONE;
}

// Result 0: No players
// Result 1: Attacker is inside smoke
// Result 2: Victim is inside smoke
// Result 3: Both are inside smoke
int CStats::InsideSmoke(CBasePlayer* Player, CBasePlayer* Target, float Distance) // 115.0f
{
	int Result = 0;

	if (g_ReGameFuncs)
	{
		CGrenade* pEntity = nullptr;

		while ((pEntity = (CGrenade*)g_ReGameFuncs->UTIL_FindEntityByString((CBaseEntity*)pEntity, "classname", "grenade")))
		{
			if (pEntity->m_bDetonated)
			{
				if (pEntity->m_SGSmoke > 0)
				{
					if ((pEntity->m_vSmokeDetonate - Player->edict()->v.origin).IsLengthLessThan(Distance))
					{
						Result += 1;
					}

					if ((pEntity->m_vSmokeDetonate - Target->edict()->v.origin).IsLengthLessThan(Distance))
					{
						Result += 2;
					}
					
					if (Result)
					{
						return Result;
					}
				}
			}
		}
	}

	return Result;
}

int* CStats::CountAlive()
{
	int Alive[SPECTATOR + 1] = { 0 };

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (Player->IsPlayer())
			{
				if (Player->IsAlive())
				{
					if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
					{
						Alive[Player->m_iTeam]++;
					}
				}
			}
		}
	}

	return Alive;
}