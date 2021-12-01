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

#include <adminmenu>

enum TypeAdminAction
{
    SHOW_PLAYER_CLASS = 0,
    RESPAWN_PLAYER,
    TELEPORT,
    REINFORCE,
    //MOVE_TO_SPEC,
    MOVE_TO_ADMIN_ZONE,
    IGNORE_DOOR_ACCESS,
    GIVE_PLAYER_ITEM,
    ROUND_RESTART,
    DESTROY_SITE
}

methodmap AdminAction < Base
{
    public AdminAction(Client ply)
    {
        AdminAction menu = view_as<AdminAction>(new Base());
        menu.SetValue("admin", ply);
        return menu;
    }

    property Client admin 
    {
        public set(Client ply)  { this.SetValue("admin", ply); }
        public get()            { Client ply; return this.GetValue("admin", ply) ? ply : null; }
    }

    property Client target 
    {
        public set(Client ply)  { this.SetValue("target", ply); }
        public get()            { Client ply; return this.GetValue("target", ply) ? ply : null; }
    }

    property int action 
    {
        public set(int action)  { this.SetInt("action", action); }
        public get()            { return this.GetInt("action"); }
    }

    public void ShowPlayerClass()
    {
        ArrayList clients = Clients.GetAll();
        Client ply;

        for(int i = 0; i < clients.Length; i++)
        {
            ply = clients.Get(i);

            if(IsClientExist(ply.id))
            {
                if(IsClientInSpec(ply.id))
                {
                    PrintToChat(this.admin.id, " \x07[SCP] \x01%N: \x04%t", ply.id, "Spectator");
                }
                else if(ply.IsAlive())
                {
                    if (ply.class != null)
                    {
                        char team[32], subclass[32];

                        ply.Team(team, sizeof(team));
                        ply.class.Name(subclass, sizeof(subclass));
                        
                        PrintToChat(this.admin.id, " \x07[SCP] \x01%N: \x07%s: %s", ply.id, team, subclass);
                    }
                    else
                    {
                        PrintToChat(this.admin.id, " \x07[SCP] \x01%N: \x07Не инициализирован", ply.id);
                    }
                }
                else
                {
                    PrintToChat(this.admin.id, " \x07[SCP] \x01%N: \x03%t", ply.id, "Dead");
                }
            }
        }

        gamemode.log.Admin("The player %L viewed the list of live players", this.admin.id);
    }

    public void IgnoreDoorAccess()
    {
        if(!this.admin.fullaccess)
        {
            PrintToChat(this.admin.id, " \x07[SCP] \x01%t \x06%t", "Door access", "Enable");
            gamemode.log.Admin("%L: %t - %t", this.admin.id, "Door access", "Enable");
            this.admin.fullaccess = true;
        }
        else
        {
            PrintToChat(this.admin.id, " \x07[SCP] \x01%t \x06%t", "Door access", "Disable");
            gamemode.log.Admin("%L: %t - %t", this.admin.id, "Door access", "Disable");
            this.admin.fullaccess = false;
        }
    }

    public void PlayerTeleport()
    {
        if(this.target && IsPlayerAlive(this.target.id) && !IsClientInSpec(this.target.id))
        {
            Handle hTrace;
            float eyePos[3], angPos[3], targetPos[3];
            GetClientEyePosition(this.admin.id, eyePos);
            GetClientEyeAngles(this.admin.id, angPos);

            hTrace = TR_TraceRayFilterEx(eyePos, angPos, MASK_SOLID, RayType_Infinite, GetLookPos_Filter, this.admin.id);
            TR_GetEndPosition(targetPos, hTrace);
            CloseHandle(hTrace);

            TeleportEntity(this.target.id, targetPos, NULL_VECTOR, NULL_VECTOR);

            gamemode.log.Admin("%L teleport player %L ", this.admin.id, this.target.id);
        }
        else
        {
            PrintToChat(this.admin.id, " \x07[SCP] \x01%t", "Player unavailable");
        }
    }
}

methodmap AdminMenuSingleton < Base
{
    public AdminMenuSingleton()
    {
        AdminMenuSingleton menu = view_as<AdminMenuSingleton>(CreateTrie());
        menu.SetValue("admins", new ArrayList());
        return menu;
    }

    public void Add(Client ply)
    {
        this.GetArrayList("admins").Push(new AdminAction(ply));
    }

    public AdminAction Get(int client)
    {
        Client ply = Clients.Get(client);
        ArrayList list = this.GetArrayList("admins");
        
        for(int i = 0; i < list.Length; i++)
        {
            if(view_as<AdminAction>(list.Get(i)).admin == ply)
            {
                return view_as<AdminAction>(list.Get(i));
            }
        }

        return view_as<AdminAction>(null);
    }
}

AdminMenuSingleton AdminMenu;

public void DisplayAdminMenu(int client)
{
    if(IsClientExist(client))
    {
        char buffer[128];
        Menu hMenu = new Menu(MenuHandler_ScpAdminMenu);
        
        FormatEx(buffer, sizeof(buffer), "%T", "Admin menu title", client);
        hMenu.SetTitle(buffer);

        FormatEx(buffer, sizeof(buffer), "%T", "Show class", client);
        hMenu.AddItem("item1", buffer, ITEMDRAW_DEFAULT);
        FormatEx(buffer, sizeof(buffer), "%T", "Respawn", client);
        hMenu.AddItem("item2", buffer, ITEMDRAW_DEFAULT);
        FormatEx(buffer, sizeof(buffer), "%T", "Teleport", client);
        hMenu.AddItem("item3", buffer, ITEMDRAW_DEFAULT);
        FormatEx(buffer, sizeof(buffer), "%T", "Reinforce", client);
        hMenu.AddItem("item4", buffer, ITEMDRAW_DEFAULT);
        /*FormatEx(buffer, sizeof(buffer), "%T", "Move to spec", client);
        hMenu.AddItem("item4", buffer, ITEMDRAW_DEFAULT);*/
        FormatEx(buffer, sizeof(buffer), "%T", "Talk", client);
        hMenu.AddItem("item5", buffer, ITEMDRAW_DEFAULT);
        FormatEx(buffer, sizeof(buffer), "%T", "Door access", client);
        hMenu.AddItem("item6", buffer, ITEMDRAW_DEFAULT);
        FormatEx(buffer, sizeof(buffer), "%T", "Give item", client);
        hMenu.AddItem("item7", buffer, ITEMDRAW_DEFAULT);
        FormatEx(buffer, sizeof(buffer), "%T", "Restart", client);
        hMenu.AddItem("item8", buffer, ITEMDRAW_DEFAULT);
        FormatEx(buffer, sizeof(buffer), "%T", "Explode", client);
        hMenu.AddItem("item9", buffer, ITEMDRAW_DEFAULT);

        hMenu.Display(client, 30);
    }
}

public int MenuHandler_ScpAdminMenu(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select)
    {
        switch(item)
        {
            case SHOW_PLAYER_CLASS:
            {
                AdminMenu.Get(client).ShowPlayerClass();
            }
            case REINFORCE:
            {
                RenderReinforceMenu(client);
            }
            case IGNORE_DOOR_ACCESS:
            {
                AdminMenu.Get(client).IgnoreDoorAccess();
            }
            case ROUND_RESTART:
            {
                gamemode.mngr.RoundComplete = true;
                CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TargetBombed, false);
                PrintToChatAll(" \x07[SCP] \x01%t", "Rount restart");
                gamemode.log.Admin("Round restart by %L", client);
            }
            case DESTROY_SITE:
            {
                gamemode.nuke.ready = true;
                gamemode.nuke.active = true;
                gamemode.nuke.Activate();
                gamemode.log.Admin("Site destroy by %L", client);
            }
            default:
            {
                AdminMenu.Get(client).action = item;
                DisplayTargetMenu(client);
            }
        }
    }
}

public void DisplayTargetMenu(int client)
{
    char buffer[64];
    Menu hMenu = new Menu(MenuHandler_ScpAdminMenuTarget);
    
    FormatEx(buffer, sizeof(buffer), "%T", "Select target", client);
    hMenu.SetTitle(buffer);

    AddTargetsToMenu2(hMenu, client, COMMAND_FILTER_NO_BOTS|COMMAND_FILTER_CONNECTED);

    hMenu.Display(client, 30);
}

public int MenuHandler_ScpAdminMenuTarget(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if(action == MenuAction_Select)
    {
        char info[32], name[32];
        int userid, target;

        hMenu.GetItem(item, info, sizeof(info), _, name, sizeof(name));
        userid = StringToInt(info);

        if((target = GetClientOfUserId(userid)) == 0)
        {
            PrintToChat(client, " \x07[SCP] \x01%t", "Target unavailable");
        }
        else if (!CanUserTarget(client, target))
		{
			PrintToChat(client, " \x07[SCP] \x01%t", "Cant find");
		}
        else
		{
            AdminMenu.Get(client).target = Clients.Get(target);
            
            switch(AdminMenu.Get(client).action)
            {
                case RESPAWN_PLAYER:
                {
                    RenderTeamMenu(client);
                }
                case TELEPORT:
                {
                    RenderTeleportMenu(client);
                }
                /*case MOVE_TO_SPEC:
                {
                    if(IsClientExist(target))
                    {
                        ChangeClientTeam(target, 1);
                    }
                }*/
                case MOVE_TO_ADMIN_ZONE:
                {
                    if(AdminMenu.Get(client).target && IsPlayerAlive(target) && !IsClientInSpec(target))
                    {
                        AdminMenu.Get(client).target.SetPos(gamemode.config.AdminRoom);
                    }
                }
                case GIVE_PLAYER_ITEM:
                {
                    RenderGiveMenu(client);
                }
                default:
                {
                    delete hMenu;
                }
            }
        }
    }
}

public bool GetLookPos_Filter(int entity, int contentsMask, any data)
{
    return data != entity;
}

public void RenderTeamMenu(int client)
{
    char buffer[64];
    
    Menu hMenu = new Menu(MenuHandler_GetTeams);
    FormatEx(buffer, sizeof(buffer), "%T", "Select team", client);
    hMenu.SetTitle(buffer);

    ArrayList teams = gamemode.GetTeamList(false);

    for(int i = 0; i < teams.Length; i++)
    {
        char teamname[32];
        teams.GetString(i, teamname, sizeof(teamname));

        hMenu.AddItem(teamname, teamname, ITEMDRAW_DEFAULT);
    }

    hMenu.Display(client, 30);
}

public void RenderClassMenu(int client, char[] teamName)
{
    char buffer[64];
    
    Menu hMenu = new Menu(MenuHandler_GetClass);
    FormatEx(buffer, sizeof(buffer), "%T", "Select class", client);
    hMenu.SetTitle(buffer);

    ArrayList classes = gamemode.team(teamName).GetClassList(false);

    for(int x = 0; x < classes.Length; x++)
    {
        char classname[32];
        classes.GetString(x, classname, sizeof(classname));

        hMenu.AddItem(teamName, classname, ITEMDRAW_DEFAULT);
    }

    hMenu.Display(client, 30);
}

public int MenuHandler_GetTeams(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select)
    {
        if(AdminMenu.Get(client).target && !IsClientInSpec(AdminMenu.Get(client).target.id))
        {
            char team[32];
            hMenu.GetItem(item, team, sizeof(team));

            RenderClassMenu(client, team);
        }
    }
}

public int MenuHandler_GetClass(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select)
    {
        if(AdminMenu.Get(client).target && !IsClientInSpec(AdminMenu.Get(client).target.id))
        {
            char team[32], class[32];
            hMenu.GetItem(item, team, sizeof(team), _, class, sizeof(class));
            
            if(IsPlayerAlive(AdminMenu.Get(client).target.id))
                AdminMenu.Get(client).target.SilenceKill();

            AdminMenu.Get(client).target.Team(team);
            AdminMenu.Get(client).target.class = gamemode.team(team).class(class);
            AdminMenu.Get(client).target.Spawn();
            gamemode.log.Admin("%L spawn player %L", client, AdminMenu.Get(client).target.id);
        }
    }
}

public void RenderTeleportMenu(int client)
{
    int Keylen;
    char buffer[64];
    
    Menu hMenu = new Menu(MenuHandler_GetTeleportPoint);
    FormatEx(buffer, sizeof(buffer), "%T", "Select point", client);
    hMenu.SetTitle(buffer);

    StringMapSnapshot stp = gamemode.config.GetObject("teleport").Snapshot();

    for(int i = 0; i < stp.Length; i++)
    {
        Keylen = stp.KeyBufferSize(i);
        char[] pointName = new char[Keylen];
        stp.GetKey(i, pointName, Keylen);

        if(json_is_meta_key(pointName))
            continue;

        hMenu.AddItem(pointName, pointName, ITEMDRAW_DEFAULT);
    }

    delete stp;

    hMenu.Display(client, 30);
}

public int MenuHandler_GetTeleportPoint(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select)
    {
        if(AdminMenu.Get(client).target && !IsClientInSpec(AdminMenu.Get(client).target.id))
        {
            char tpname[32];
            hMenu.GetItem(item, tpname, sizeof(tpname));

            JSON_OBJECT pos = gamemode.config.GetObject("teleport").GetObject(tpname);

            AdminMenu.Get(client).target.SetPos(pos.GetVector("vec"), pos.GetAngle("ang"));
            gamemode.log.Admin("%L teleport %L", client, AdminMenu.Get(client).target.id);
        }
    }
}

public void RenderReinforceMenu(int client)
{
    char buffer[64];
    
    Menu hMenu = new Menu(MenuHandler_Reinforce);
    FormatEx(buffer, sizeof(buffer), "%T", "Select team", client);
    hMenu.SetTitle(buffer);

    ArrayList teams = gamemode.GetTeamList(false);

    for(int i = 0; i < teams.Length; i++)
    {
        char teamname[32];
        teams.GetString(i, teamname, sizeof(teamname));

        hMenu.AddItem(teamname, teamname, ITEMDRAW_DEFAULT);
    }

    hMenu.Display(client, 30);
}

public int MenuHandler_Reinforce(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select)
    {
        if(AdminMenu.Get(client).target && !IsClientInSpec(AdminMenu.Get(client).target.id))
        {
            char team[32];
            hMenu.GetItem(item, team, sizeof(team));

            gamemode.mngr.CombatReinforcement(team);
        }
    }
}

public void RenderGiveMenu(int client)
{
    char buffer[64];
    
    Menu hMenu = new Menu(MenuHandler_GiveMenu);
    FormatEx(buffer, sizeof(buffer), "%T", "Give Item", client);
    hMenu.SetTitle(buffer);

    ArrayList entlist = gamemode.meta.GetList("entities").GetKeys();

    for(int i = 0; i < entlist.Length; i++)
    {
        char entclass[32], entname[128];
        entlist.GetString(i, entclass, sizeof(entclass));
        FormatEx(entname, sizeof(entname), "%T", entclass, client);

        hMenu.AddItem(entclass, entname, ITEMDRAW_DEFAULT);
    }

    hMenu.Display(client, 30);
}

public int MenuHandler_GiveMenu(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select)
    {
        if(AdminMenu.Get(client).target && !IsClientInSpec(AdminMenu.Get(client).target.id))
        {
            char itemclass[32];
            hMenu.GetItem(item, itemclass, sizeof(itemclass));

            AdminMenu.Get(client).target.inv.Add(itemclass);
            gamemode.log.Admin("%L give %s item to %L ", client, itemclass, AdminMenu.Get(client).target.id);
        }
    }
}