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
#include <ripext>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] Discord Logger",
    author = "GeTtOo, Andrey::Dono",
    description = "Discord logger for SCP Breach modification",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

JSON_OBJECT config;

public void SCP_OnLoad() {
    config = Utils.ReadCurMapConfig("discord");
}

public void SCP_OnUnload() {
    delete config;
}

public void SCP_OnLog(LogType type, Player &admin, const char[] logtext)
{
    char strtype[32];
    switch(type)
    {
        case ERROR:     { strtype = "error";   }
        case Warning:   { strtype = "warning"; }
        case Info:      { strtype = "info";    }
        case Admin:     { strtype = "admin";   }
        case Debug:     { strtype = "debug";   }
    }
    
    if (!config.HasKey(strtype)) return;

    JSON_OBJECT tconfig = config.Get(strtype);

    JSONObject point = new JSONObject();

    char username[64];
    tconfig.GetString("username", username, sizeof(username));
    point.SetString("username", username);
    
    JSONArray embeds = new JSONArray();
    JSONObject log = new JSONObject();

    if (type != Admin)
    {
        char title[64];
        tconfig.GetString("title", title, sizeof(title));
        log.SetString("title", title);
    }
    else
    {
        char name[32], steamid[32], title[128];
        tconfig.GetString("title", title, sizeof(title));
        admin.GetName(name, sizeof(name));
        admin.GetAuth(steamid, sizeof(steamid));
        ReplaceString(title, sizeof(title), "{name}", name);
        ReplaceString(title, sizeof(title), "{steamid}", steamid);
        log.SetString("title", title);
    }

    log.SetInt("color", tconfig.GetInt("color"));
    log.SetString("description", logtext);
    
    JSONObject logfooter = new JSONObject();

    char timestring[32];
    FormatTime(timestring, sizeof(timestring), "%H:%M:%S-%d.%m.%Y");
    logfooter.SetString("text", timestring);

    log.Set("footer", logfooter);
    embeds.Push(log);
    point.Set("embeds", embeds);

    char hookurl[256];
    tconfig.GetString("hookurl", hookurl, sizeof(hookurl));
    view_as<HTTPRequest>(new HTTPRequest(hookurl)).Post(point, OnReceived);

    delete logfooter;
    delete log;
    delete embeds;
    delete point;
}

public void OnReceived(HTTPResponse resp, any val)
{
    //PrintToChatAll("%i %s", resp.Status, resp.Data);
}