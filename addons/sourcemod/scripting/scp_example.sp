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

#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] Plugin example",
    author = "Anyone",
    description = "Plugin example for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/author/plugin"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "ent_id", "Function name", true); // @arg1 Client, @arg2 Entity, @arg3 disable pickup to inventory (def false).
    gamemode.meta.RegEntEvent(ON_TOUCH, "ent_id", "Function name"); // @arg1 Entity, @arg2 Entity
    gamemode.meta.RegEntEvent(ON_USE, "ent_id", "Function name"); // @arg1 Client, @arg2 Entity
    gamemode.meta.RegEntEvent(ON_DROP, "ent_id", "Function name"); // @arg1 Client, @arg2 Entity
}

public void SCP_OnLoad() {
    
}

public void SCP_OnUnload() {
    
}

public void SCP_OnPlayerJoin(Client &ply) {
    
}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerClear(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    
}

public Action SCP_OnTakeDamage(Client &vic, Client &atk, float &damage, int &damagetype) {
    
}

public void SCP_OnPlayerDeath(Client &vic, Client &atk) {

}

public void SCP_OnPlayerReset(Client &ply) {

}

public void SCP_OnRoundStart() {

}

public void SCP_OnRoundEnd() {

}

public void SCP_OnInput(Client &ply, int buttons) {

}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    
}

public void SCP_OnCallActionMenu(Client &ply) {
    
}