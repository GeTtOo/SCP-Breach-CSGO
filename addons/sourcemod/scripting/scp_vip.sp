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
    name = "[SCP] Vip",
    author = "Andrey::Dono",
    description = "VIP plugin for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/author/plugin"
};

public void SCP_OnLoad() {
    AddCommandListener(Command_VIP, "say");
}

public void SCP_OnUnload() {
    RemoveCommandListener(Command_VIP, "say");
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (GetAdminFlag(GetUserAdmin(ply.id), Admin_Custom5))
    {
        if (ply.class.Is("D Class") && ply.store.GetBool("vip_replace_dclass"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Medic");
            ply.SetupBaseStats();
        }
        else if (ply.class.Is("Scientist") && ply.store.GetBool("vip_replace_scientist"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Dr.Bright");
            ply.SetupBaseStats();
        }
        else if (ply.class.Is("Cadet") && ply.store.GetBool("vip_replace_cadet"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Sapper");
            ply.SetupBaseStats();
        }
        else if (ply.class.Is("Lieutenant") && ply.store.GetBool("vip_replace_lieutenant"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Lieutenant-Medic");
            ply.SetupBaseStats();
        }
    }
}

public void SCP914_OnModify(Player &ply, int &ruinechance, int &modifychance) {
    if (GetAdminFlag(GetUserAdmin(ply.id), Admin_Custom5))
    {
        if ((ruinechance != -1))
            if ((ruinechance - 25) >= 0)
                ruinechance -= 25;
            else
                ruinechance = 0;
                
        if ((modifychance + 25) <= 100)
            modifychance += 25;
        else
            modifychance = 100;
    }
}

public Action Command_VIP(int client, const char[] command, int argc)
{
    Player ply = player.GetByID(client);
    
    char arg1[32];

    GetCmdArg(1, arg1, sizeof(arg1));

    if (StrEqual(arg1, "!vip") || StrEqual(arg1, "/vip") || StrEqual(arg1, "vip"))
    {
        if (GetAdminFlag(GetUserAdmin(ply.id), Admin_Custom5))
            VIP_Menu_Render(ply);
        else
            PrintToChat(ply.id, "У вас нет доступа к данной команде!");

        return Plugin_Stop;
    }

    return Plugin_Continue;
}

public void VIP_Menu_Render(Player ply)
{
    Menu hndl = new Menu(VIPMenuHandler, MenuAction_Select);

    hndl.SetTitle("Меню VIP | Баланс: 0 Doge");

    hndl.AddItem("0", "Замена класса", ITEMDRAW_DEFAULT);
    hndl.AddItem("1", "Вероятности SCP-914", ITEMDRAW_DISABLED);
    
    hndl.Display(ply.id, 30);
}

public int VIPMenuHandler(Menu hMenu, MenuAction action, int arg1, int idx) 
{
    switch (action)
    {
        case MenuAction_Select:
        {
            Player ply = player.GetByID(arg1);
            
            switch (idx)
            {
                case 0:
                VIP_Menu_Change_Class_Render(ply);
            }
        }
    }

    return 0;
}

public void VIP_Menu_Change_Class_Render(Player ply)
{
    Menu hndl = new Menu(VIP_Menu_Change_Class_Handler, MenuAction_DisplayItem | MenuAction_Select | MenuAction_End);

    hndl.SetTitle("Меню VIP");

    hndl.AddItem("1", "", ITEMDRAW_DEFAULT);
    hndl.AddItem("2", "", ITEMDRAW_DEFAULT);
    hndl.AddItem("3", "", ITEMDRAW_DEFAULT);
    hndl.AddItem("4", "", ITEMDRAW_DEFAULT);
    
    hndl.Display(ply.id, 30);
}

public int VIP_Menu_Change_Class_Handler(Menu hMenu, MenuAction action, int arg1, int idx) 
{
    switch (action)
    {
        case MenuAction_DisplayItem:
        {
            Player ply = player.GetByID(arg1);

            char buffer[128];

            switch (idx)
            {
                case 0:
                {
                    FormatEx(buffer, sizeof(buffer), "D Class => Medic | %s", (ply.store.GetBool("vip_replace_dclass")) ? "Вкл" : "Выкл");
                    return RedrawMenuItem(buffer);
                }
                case 1:
                {
                    FormatEx(buffer, sizeof(buffer), "Scientist => Dr.Bright | %s", (ply.store.GetBool("vip_replace_scientist")) ? "Вкл" : "Выкл");
                    return RedrawMenuItem(buffer);
                }
                case 2:
                {
                    FormatEx(buffer, sizeof(buffer), "Cadet => Sapper | %s", (ply.store.GetBool("vip_replace_cadet")) ? "Вкл" : "Выкл");
                    return RedrawMenuItem(buffer);
                }
                case 3:
                {
                    FormatEx(buffer, sizeof(buffer), "Lieutenant => Lieutenant-Medic | %s", (ply.store.GetBool("vip_replace_lieutenant")) ? "Вкл" : "Выкл");
                    return RedrawMenuItem(buffer);
                }
            }
        }
        case MenuAction_Select:
        {
            Player ply = player.GetByID(arg1);
            
            switch (idx)
            {
                case 0:
                    ply.store.SetBool("vip_replace_dclass", !ply.store.GetBool("vip_replace_dclass"));
                case 1:
                    ply.store.SetBool("vip_replace_scientist", !ply.store.GetBool("vip_replace_scientist"));
                case 2:
                    ply.store.SetBool("vip_replace_cadet", !ply.store.GetBool("vip_replace_cadet"));
                case 3:
                    ply.store.SetBool("vip_replace_lieutenant", !ply.store.GetBool("vip_replace_lieutenant"));
            }
            
            VIP_Menu_Change_Class_Render(ply);
        }
        case MenuAction_End:
        {
            //if (arg1 == MenuEnd_Selected)
            delete hMenu;
        }
    }

    return 0;
}