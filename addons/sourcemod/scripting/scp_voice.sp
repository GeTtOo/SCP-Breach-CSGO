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
    name = "[SCP] Voice",
    author = "Andrey::Dono",
    description = "Voice plugin for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnRoundStart() {
    ResetListening();

    gamemode.timer.Create("AdvancedVoice", 250, 0, "VoiceLogicHandler");
    gamemode.log.Debug("Channels updater started");
}

public void ResetListening()
{
    ArrayList players = Clients.GetAll();
    Client firstply;
    Client secondply;

    for (int i=0; i < players.Length; i++)
    {
        firstply = players.Get(i);
        for (int k=0; k < players.Length; k++)
        {
            secondply = players.Get(k);
            if (firstply != secondply)
                firstply.SetListen(secondply, false);
        }
    }

    delete players;

    gamemode.log.Debug("Channels reseted...");
}

public void VoiceLogicHandler()
{
    ArrayList players = Clients.GetAll();
    Client firstply;
    Client secondply;

    for (int i=0; i < players.Length; i++)
    {
        firstply = players.Get(i);

        for (int k=0; k < players.Length; k++)
        {
            secondply = players.Get(k);

            if ((firstply.IsSCP && secondply.IsSCP) || (firstply.inv.Have("radio") && secondply.inv.Have("radio"))) // SCP can hear other SCP players with no range limits
                firstply.SetListen(secondply, true);
            else
                firstply.SetListen(secondply, false);
        }

        if (!firstply.IsAlive()) continue;

        float distance = float(gamemode.plconfig.GetInt("distance", 500));

        char filter[1][32] = {"player"};
        ArrayList ents = Ents.FindInBox(firstply.GetPos() - new Vector(distance, distance, distance), firstply.GetPos() + new Vector(distance, distance, distance), filter, sizeof(filter));

        for (int k=0; k < ents.Length; k++)
        {
            secondply = ents.Get(k);

            if (firstply != secondply)
            {
                firstply.SetListen(secondply, true);
            }
        }

        delete ents;
    }

    delete players;
}