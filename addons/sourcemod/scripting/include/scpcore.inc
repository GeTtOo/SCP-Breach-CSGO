/**
 * =============================================================================
 * Copyright (C) 2021 Eternity team (Andrey::Dono, GeTtOo).
 * =============================================================================
 *
 * This file is part of the SCP Breach CS:GO.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 **/

#if defined _scpcore_included
    #endinput
#endif
#define _scpcore_included

#pragma dynamic 131072

// ¯\_(ツ)_/¯
#include "scpcore\structures\base"
#include "scpcore\structures\vector"
#include "scpcore\structures\angle"
#include "scpcore\structures\color"
#include "scpcore\structures\json"

#include "scpcore\utils"

#include "scpcore\classes\meta"
#include "scpcore\classes\timers"
Timers timer;

#include "scpcore\classes\gamemode"
#include "scpcore\classes\logger"

GameMode gamemode;

#include "scpcore\classes\statuseffect"
StatusEffectSingleton statuseffect;

#include "scpcore\entities\entity"
#include "scpcore\entities\player"
#include "scpcore\entities\worldtext"
#include "scpcore\classes\singletons"

ClientSingleton player;
EntitySingleton ents;
WorldTextSingleton worldtext;

#include "scpcore\classes\manager"
#include "scpcore\classes\nuke"

forward void SCP_RegisterMetaData();

forward void SCP_OnLoad();

forward void SCP_OnUnload();

forward void SCP_OnRoundStart();

forward void SCP_OnRoundEnd();

forward void SCP_OnAlphaWarhead(AlphaWarhead status);

forward void SCP_OnPlayerJoin(Player &ply);

forward void SCP_OnPlayerLeave(Player &ply);

forward void SCP_PrePlayerSpawn(Player &ply);

forward void SCP_OnPlayerSpawn(Player &ply);

forward void SCP_PostPlayerSpawn(Player &ply);

forward void SCP_OnPlayerClear(Player &ply);

forward void SCP_OnPlayerSetupOverlay(Player &ply);

forward bool SCP_OnPlayerPickupItem(Player &ply, Entity &ent);

forward void SCP_OnPlayerPickupWeapon(Player &ply, Entity &ent);

forward void SCP_OnPlayerSwitchWeapon(Player &ply, Entity &ent);

forward Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor);

forward void SCP_OnPlayerDeath(Player &vic, Player &atk);

forward EscapeInfo SCP_OnPlayerEscape(Player &ply, EscapeInfo &data);

forward void SCP_OnButtonPressed(Player &ply, int doorId);

forward void SCP_OnInput(Player &ply, int buttons);

forward void SCP_OnCallAction(Player &ply);

forward bool SCP_Log_PlayerDeath(Player &vic, Player &atk, float damage, int damagetype, int inflictor);