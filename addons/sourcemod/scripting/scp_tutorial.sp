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
    name = "[SCP] Tutorial",
    author = "GeTtOo, Anfrey::Dono",
    description = "Tutorial for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

Class tutorial;

public void SCP_OnLoad() {
    LoadTranslations("scpcore.phrases");

    AddCommandListener(Command_Kill, "kill");
    AddCommandListener(Command_Tutorial, "say");
    
    ConVar ptc = FindConVar("sv_disable_immunity_alpha");
    ptc.SetInt(1, true, true);
    delete ptc;
    
    tutorial = view_as<Class>(new Base());

    tutorial.Name("Tutorial");
    tutorial.SetInt("health", 1337);
    tutorial.SetInt("armor", 228);
}

public void SCP_OnUnload() {
    RemoveCommandListener(Command_Kill, "kill");
    RemoveCommandListener(Command_Tutorial, "say");
    
    delete tutorial;
}

public void SCP_OnRoundStart() {
    gamemode.timer.Create("TutorialPlayerRespawn", 1000, 0, "Tutorial_Respawn");
}

public void SCP_OnPlayerJoin(Player &ply)
{
    if (!ply.store.GetBool("tutorial", false))
        ply.ready = false;
}

public void SCP_PrePlayerSpawn(Player &ply) {
    if (!ply.store.GetBool("tutorial", false))
    {
        ply.SetPropFloat("m_flNextDecalTime", 2.0);
        ply.Team("None");
        ply.class = tutorial;
        //ply.SetupBaseStats();
        
        ply.TimerSimple(250, "TeleportToTutorialRoom", ply);
        
        PlayAmbientTheme(ply);
        if (!ply.GetHandle("tutor_sound")) ply.SetHandle("tutor_sound", ply.TimerSimple(180000, "PlayAmbientTheme", ply));
    }
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor) {
    if (vic && vic.class && vic.class.Is("Tutorial"))
        return Plugin_Handled;
    
    return Plugin_Continue;
}

public void Tutorial_Respawn() {
    ArrayList players = player.GetAll();

    for (int i=0; i < players.Length; i++)
    {
        Player ply = players.Get(i);

        if (!ply.store.GetBool("tutorial", false) && GetClientTeam(ply.id) > 1)
        {
            ply.Team("None");
            ply.class = tutorial;
            ply.Spawn();
        }
    }

    delete players;
}

public void PlayAmbientTheme(Player ply)
{
    char mapname[32];
    GetCurrentMap(mapname, sizeof(mapname));
    if (StrEqual(mapname, "workshop/2424265786/scp_site101"))
        ply.PlaySound("eternity/map/purrple-cat-edge-of-the-universe.wav", _, 25);

    ply.SetHandle("tutor_sound", ply.TimerSimple(180000, "PlayAmbientTheme", ply));
}

public void TeleportToTutorialRoom(Player ply)
{
    char lang[3];
    ply.GetLangInfo(lang, sizeof(lang));

    JSON_OBJECT pos = gamemode.plconfig.GetObject("positions").GetObject("en");
    
    if (gamemode.plconfig.GetObject("positions").HasKey(lang))
        pos = gamemode.plconfig.GetObject("positions").GetObject(lang);

    ply.SetPos(pos.GetVector("vec"), pos.GetAngle("ang"));
    ply.model.SetRenderMode(RENDER_TRANSCOLOR);
    ply.model.SetRenderColor(new Colour(255, 255, 255, 100));
}

public Action Command_Kill(int client, const char[] command, int argc)
{
    Player ply = player.GetByID(client);

    if (ply && ply.class && ply.class == tutorial)
    {
        PrintToConsole(client, "Самоуйбиство за класс Tutorial запрещено!");
        return Plugin_Handled;
    }

    return Plugin_Continue;
}

public Action Command_Tutorial(int client, const char[] command, int argc)
{
    Player ply = player.GetByID(client);
    
    char arg1[32];

    GetCmdArg(1, arg1, sizeof(arg1));

    if (StrEqual(arg1, "!game_ready") || StrEqual(arg1, "/game_ready"))
    {
        ply.ready = true;
        ply.model.SetRenderMode(RENDER_TRANSCOLOR);
        ply.model.SetRenderColor(new Colour(255, 255, 255, 255));
        ply.SetPropFloat("m_flNextDecalTime", 0.0);

        ply.store.SetBool("tutorial", true);
        PrintToChat(ply.id, " \x07[SCP]\x05 Вы успешно завершили туториал! Приятной игры.", ply.id);

        gamemode.timer.Remove(view_as<Tmr>(ply.GetHandle("tutor_sound")));
        ply.RemoveValue("tutor_sound");
        ply.StopSound("eternity/map/purrple-cat-edge-of-the-universe.wav");

        if (gamemode.mngr.RoundTime < 180)
        {
            char team[32], class[32];
            gamemode.config.DefaultGlobalClass(team, sizeof(team));
            gamemode.config.DefaultClass(class, sizeof(class));
            ply.Team(team);
            ply.class = gamemode.team(team).class(class);
            
            ply.UpdateClass();

            gamemode.mngr.GameCheck();
        }
        else
        {
            ply.Kill();
            
            if (ply.ragdoll) //Fix check if valid
            {
                ents.Remove(ply.ragdoll);
                ply.ragdoll = null;
            }
        }

        char name[32];
        ply.GetName(name, sizeof(name));
        gamemode.log.Info("Игрок %s прошёл туториал!", name);

        return Plugin_Stop;
    }

    if (StrEqual(arg1, "!trs") || StrEqual(arg1, "/trs"))
    {
        ply.ready = false;
        ply.store.SetBool("tutorial", false);
        ply.Kill();
        PrintToChat(ply.id, " \x07[SCP]\x05 Туториал сброшен!", ply.id);

        return Plugin_Stop;
    }

    return Plugin_Continue;
}