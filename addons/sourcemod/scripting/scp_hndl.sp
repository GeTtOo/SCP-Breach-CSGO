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
    name = "[SCP] Leak checker",
    author = "Andrey::Dono",
    description = "Memory leak checker for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/dogma4b"
};

int phs = 0;

public void OnMapStart()
{
    AddCommandListener(Command_GetHndlSize, "hs");
}

public void OnMapEnd()
{
    RemoveCommandListener(Command_GetHndlSize, "hs");
}

public void SCP_OnRoundStart() {
    SaveHandles();
    //timer.Create("DumpHandles", 250, 0, "SaveHandles");
    timer.Simple(500, "CheckLeak");
}

public void SCP_OnRoundEnd() {
    SaveHandles();
}

public void CheckLeak() {
    int chs = FileSize("/addons/sourcemod/plugins/SCP/hndl.txt") / 8;

    if (phs != 0)
    {
        PrintToConsoleAll("Leaked memory: %i Kb", chs - phs);
        gamemode.log.Debug("[Memory status] Leaked: %i Kb", chs - phs);
    }

    phs = chs;
}

public void SaveHandles()
{
    ServerCommand("sm_dump_handles /addons/sourcemod/plugins/SCP/hndl.txt");
}

public Action Command_GetHndlSize(int client, const char[] command, int argc)
{
    Player ply = player.GetByID(client);
    
    if (!ply.IsAdmin()) return Plugin_Stop;
    
    PrintToConsole(ply.id, "Handles size: %i Kb", (FileSize("/addons/sourcemod/plugins/SCP/hndl.txt") / 8));

    return Plugin_Stop;
}