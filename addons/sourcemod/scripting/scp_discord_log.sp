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
    name = "[SCP] Discord logger",
    author = "GeTtOo, Andrey::Dono",
    description = "Discord logger",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

JSONObject point;

public void SCP_OnLoad() {
    point = new JSONObject();
    point.SetString("username", "SCP Breach Log");
}

public void SCP_OnUnload() {
    delete point;
}

public void SCP_OnLog(LogType type, const char[] timestring, const char[] formatMessage)
{
    char normal[256] = "https://discord.com/api/webhooks/962365953386090536/Yr6RDFAsNA-pg0SaIfb0FrYoIH4ZNUmXWL3sftT--3bTgJf4auPDMfjHygDJMOW4QlT_";
    char admin[256] = "https://discord.com/api/webhooks/977635645725999156/yWms0E12KAnE-BelrUdAhPt9YIRpqCTIlUX-FRlFuhJQHRs_PjZILy3o-NVId7MjSbf0";
    
    char buffer[256];

    switch (type)
    {
        case ERROR:
        {
            FormatEx(buffer, sizeof(buffer), "```md\n[%s][ERROR] %s```", timestring, formatMessage);
            point.SetString("content", buffer);
            view_as<HTTPRequest>(new HTTPRequest(admin)).Post(point, OnReceived);
        }
        case Warning:
        {
            FormatEx(buffer, sizeof(buffer), "```md\n[%s][Warning] %s```", timestring, formatMessage);
            point.SetString("content", buffer);
            view_as<HTTPRequest>(new HTTPRequest(admin)).Post(point, OnReceived);
        }
        case Info:
        {
            FormatEx(buffer, sizeof(buffer), "```md\n[%s][Info] %s```", timestring, formatMessage);
            point.SetString("content", buffer);
            view_as<HTTPRequest>(new HTTPRequest(normal)).Post(point, OnReceived);
        }
        case Admin:
        {
            FormatEx(buffer, sizeof(buffer), "```md\n[%s][Admin] %s```", timestring, formatMessage);
            point.SetString("content", buffer);
            view_as<HTTPRequest>(new HTTPRequest(admin)).Post(point, OnReceived);
        }
        case Debug:
        {
            FormatEx(buffer, sizeof(buffer), "```md\n[%s][Debug] %s```", timestring, formatMessage);
            point.SetString("content", buffer);
            view_as<HTTPRequest>(new HTTPRequest(admin)).Post(point, OnReceived);
        }
    }
}

public void OnReceived(HTTPResponse resp, any val)
{
    //PrintToChatAll("%i %s", resp.Status, resp.Data);
}