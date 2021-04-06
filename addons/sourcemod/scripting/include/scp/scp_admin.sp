#include <adminmenu>

const SHOW_PLAYER_CLASS   = 0;
const RESPAWN_PLAYER      = 1;
const TELEPORT            = 2;
const MOVE_TO_SPEC        = 3;
const MOVE_TO_ADMIN_ZONE  = 4;
const IGNORE_DOOR_ACCESS  = 5;
const GIVE_PLAYER_ITEM    = 6;
const ROUND_RESTART       = 7;
const DESTROY_SITE        = 8;


methodmap AdminAction < Base
{
    public AdminAction(Client ply)
    {
        AdminAction menu = view_as<AdminAction>(CreateTrie());
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
        Client ply;

        for(int target = 0; target < MAXPLAYERS; target++)
        {
            if(IsClientExist(target))
            {
                if(IsClientInSpec(target))
                {
                    PrintToChat(this.admin.id, " \x07[SCP] \x01%N: \x04Наблюдатель", target);
                }
                else if(IsPlayerAlive(target))
                {
                    char team[32], subclass[32];

                    ply = Clients.Get(target);
                    ply.Team(team, sizeof(team));
                    ply.class.Name(subclass, sizeof(subclass));
                    
                    PrintToChat(this.admin.id, " \x07[SCP] \x01%N: \x07%s: %s", target, team, subclass);
                }
                else
                {
                    PrintToChat(this.admin.id, " \x07[SCP] \x01%N: \x03Мертв", target);
                }
            }
        }
    }

    public void IgnoreDoorAccess()
    {
        if(!this.admin.fullaccess)
        {
            PrintToChat(this.admin.id, " \x07[SCP] \x01Игнорирование карт доступа \x06включено");
            this.admin.fullaccess = true;
        }
        else
        {
            PrintToChat(this.admin.id, " \x07[SCP] \x01Игнорирование карт доступа \x06отключено");
            this.admin.fullaccess = false;
        }
    }

    public void PlayerTeleport()
    {
        if(this.target && IsPlayerAlive(this.target.id) && !IsCleintInSpec(this.target.id))
        {
            Handle hTrace;
            float eyePos[3], angPos[3], targetPos[3];
            GetClientEyePosition(this.admin.id, eyePos);
            GetClientEyeAngles(this.admin.id, angPos);

            hTrace = TR_TraceRayFilterEx(eyePos, angPos, MASK_SOLID, RayType_Infinite, GetLookPos_Filter, this.admin.id);
            TR_GetEndPosition(targetPos, hTrace);
            CloseHandle(hTrace);

            TeleportEntity(this.target.id, targetPos, NULL_VECTOR, NULL_VECTOR);
        }
        else
        {
            PrintToChat(this.admin.id, " \x07[SCP] \x01Игрок вышел/умер/находиться в наблюдателях");
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
        this.GetList("admins").Push(new AdminAction(ply));
    }

    public AdminAction Get(int client)
    {
        Client ply = Clients.Get(client);
        ArrayList list = this.GetList("admins");
        
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

void DisplayAdminMenu(int client)
{
    if(IsClientExist(client))
    {
        Menu hMenu = new Menu(MenuHandler_ScpAdminMenu);
        hMenu.SetTitle("Меню администратора");

        hMenu.AddItem("item1", "Просмотреть классы игроков", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item2", "Возродить игрока", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item3", "Телепортировать игрока", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item4", "Переместить в наблюдатели", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item5", "Провести беседу с игроком", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item6", "Игнорирование карт доступа", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item7", "Выдать предмет", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item8", "Перезапустить раунд", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item9", "Взорвать комплекс", ITEMDRAW_DEFAULT);

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
            case IGNORE_DOOR_ACCESS:
            {
                AdminMenu.Get(client).IgnoreDoorAccess();
            }
            case ROUND_RESTART:
            {
                gamemode.mngr.RoundComplete = true;
                CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TargetBombed, false);
                PrintToChatAll(" \x07[SCP] \x01Раунд перезапущен администратором");
            }
            case DESTROY_SITE:
            {
                SCP_NukeActivation();
            }
            default:
            {
                AdminMenu.Get(client).action = item;
                DisplayTargetMenu(client);
            }
        }
    }
}

void DisplayTargetMenu(int client)
{
    Menu hMenu = new Menu(MenuHandler_ScpAdminMenuTarget);
    hMenu.SetTitle("Выберите игрока:");

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
            PrintToChat(client, " \x07[SCP] \x01Игрок больше не доступен");
        }
        else if (!CanUserTarget(client, target))
		{
			PrintToChat(client, " \x07[SCP] \x01Невозможно найти игрока");
		}
        else
		{
            AdminMenu.Get(client).target = Clients.Get(target);
            
            switch(AdminMenu.Get(client).action)
            {
                case RESPAWN_PLAYER:
                {
                    ChangePlayerClass(client);
                }
                case TELEPORT:
                {
                    AdminMenu.Get(client).PlayerTeleport();
                }
                case MOVE_TO_SPEC:
                {
                    if(IsClientExist(target))
                    {
                        ChangeClientTeam(target, 1);
                    }
                }
                case MOVE_TO_ADMIN_ZONE:
                {
                    if(IsClientExist(target) && IsPlayerAlive(target) && !IsCleintInSpec(target))
                    {
                        float pos[3];
                        pos[0] = gamemode.config.AdminRoom.x;
                        pos[1] = gamemode.config.AdminRoom.y;
                        pos[2] = gamemode.config.AdminRoom.z;
                        TeleportEntity(target, pos, NULL_VECTOR, NULL_VECTOR);
                    }
                }
                case GIVE_PLAYER_ITEM:
                {
                    PrintToChat(client, "┬─┬ ノ( ゜-゜ノ)");
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

void ChangePlayerClass(int client)
{
    int teamKeylen, classKeylen; 
    
    Menu hMenu = new Menu(MenuHandler_ScpChangeClass);
    hMenu.SetTitle("Выберите класс:");

    StringMapSnapshot teamSnap = gamemode.GetTeamNames();

    for(int i = 0; i < teamSnap.Length; i++)
    {
        teamKeylen = teamSnap.KeyBufferSize(i);
        char[] teamName = new char[teamKeylen];
        teamSnap.GetKey(i, teamName, teamKeylen);

        if(json_is_meta_key(teamName))
            continue;
        
        GTeam team = gamemode.team(teamName);
        StringMapSnapshot classSnap = team.GetClassNames();

        for(int x = 0; x < classSnap.Length; x++)
        {
            classKeylen = classSnap.KeyBufferSize(x);
            char[] className = new char[classKeylen];
            classSnap.GetKey(x, className, classKeylen);

            if(json_is_meta_key(className))
                continue;

            hMenu.AddItem(teamName, className, ITEMDRAW_DEFAULT);
        }
    }

    hMenu.Display(client, 30);
}

public int MenuHandler_ScpChangeClass(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select)
    {
        if(AdminMenu.Get(client).target && !IsCleintInSpec(AdminMenu.Get(client).target.id))
        {
            char team[32], class[32];
            hMenu.GetItem(item, team, sizeof(team), _, class, sizeof(class));

            if(IsPlayerAlive(AdminMenu.Get(client).target.id))
                AdminMenu.Get(client).target.Kill();
            
            AdminMenu.Get(client).target.Team(team);
            AdminMenu.Get(client).target.class = gamemode.team(team).class(class);
            AdminMenu.Get(client).target.Spawn();
        }
    }
}
