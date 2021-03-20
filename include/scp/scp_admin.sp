#include <adminmenu>

int g_PlayerAdminAction[MAXPLAYERS+1];

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
    if (action == MenuAction_Select)
    {
        if (IsClientExist(client))
        {
            
            
            switch(item)
            {
                case 0:
                {
                    Client ply;

                    for(int target = 0; target < MAXPLAYERS; target++)
                    {
                        if(IsClientExist(target))
                        {
                            if(IsClientInSpec(target))
                            {
                                PrintToChat(client, " \x07[SCP] \x01%N: \x04Наблюдатель", target);
                            }
                            else if(IsPlayerAlive(target))
                            {
                                char gclass[32], subclass[32];

                                ply = Clients.Get(target);
                                ply.gclass(gclass, sizeof(gclass));
                                ply.class.Name(subclass, sizeof(subclass));
                                
                                PrintToChat(client, " \x07[SCP] \x01%N: \x07%s: %s", target, gclass, subclass);
                            }
                            else
                            {
                                PrintToChat(client, " \x07[SCP] \x01%N: \x03Мертв", target);
                            }
                        }
                    }
                }
                // Respawn Players
                case 1:
                {
                    g_PlayerAdminAction[client] = 1;
                    DisplayTargetMenu(client);
                }
                // Teleport player
                case 2:
                {
                    g_PlayerAdminAction[client] = 2;
                    DisplayTargetMenu(client);
                }
                // Move to spec
                case 3:
                {
                    g_PlayerAdminAction[client] = 3;
                    DisplayTargetMenu(client);
                }
                // Move player to admin zone
                case 4:
                {
                    g_PlayerAdminAction[client] = 4;
                    DisplayTargetMenu(client);
                }
                // Ignore door access
                case 5:
                {
                    Client ply = Clients.Get(client);
                    
                    if(!ply.FullAccess)
                    {
                        PrintToChat(client, " \x07[SCP] \x01Игнорирование карт доступа \x06включено");
                        ply.FullAccess = true;
                    }
                    else
                    {
                        PrintToChat(client, " \x07[SCP] \x01Игнорирование карт доступа \x06отключено");
                        ply.FullAccess = false;
                    }
                }
                // Give player item
                case 6:
                {
                    g_PlayerAdminAction[client] = 6;
                    DisplayTargetMenu(client);
                }
                // Round restart
                case 7:
                {
                    gamemode.mngr.RoundComplete = true;
                    CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TargetBombed, false);
                    PrintToChatAll(" \x07[SCP] \x01Раунд перезапущен администратором");
                }
                // Destroy site
                case 8:
                {
                    SCP_NukeActivation();
                }
                default:
                {
                    delete hMenu;
                }
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
    if(action == MenuAction_Select)
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
            switch(g_PlayerAdminAction[client])
            {
                // Respawn Player
                case 1:
                {
                    PrintToChat(client, "(╯°□°）╯︵ ┻━┻");
                }
                // Teleport Player
                case 2:
                {
                    if(IsClientExist(target) && IsPlayerAlive(target) && !IsCleintInSpec(target))
                    {
                        Handle hTrace;
                        float eyePos[3], angPos[3], targetPos[3];
                        GetClientEyePosition(client, eyePos);
                        GetClientEyeAngles(client, angPos);

                        hTrace = TR_TraceRayFilterEx(eyePos, angPos, MASK_SOLID, RayType_Infinite, GetLookPos_Filter, client);
                        TR_GetEndPosition(targetPos, hTrace);
                        CloseHandle(hTrace);

                        TeleportEntity(target, targetPos, NULL_VECTOR, NULL_VECTOR);
                        LogAction(client, target, "\"%L\" телепортировал игрока \"%L\"", client, target);
                    }
                    else
                    {
                        PrintToChat(client, " \x07[SCP] \x01Игрок вышел/умер/находиться в наблюдателях");
                    }
                }
                // Move to spec
                case 3:
                {
                    if(IsClientExist(target))
                    {
                        ChangeClientTeam(target, 1);
                    }
                }
                // Move player to admin zone
                case 4:
                {
                    if(IsClientExist(target) && IsPlayerAlive(target) && !IsCleintInSpec(target))
                    {
                        // ВОТ ТУТА ИЗ КОНФИГА ИЛИ ПОД ДВЕРЬ НАСРУ!
                        float pos[3];
                        TeleportEntity(target, pos, NULL_VECTOR, NULL_VECTOR);
                    }
                }
                case 6:
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