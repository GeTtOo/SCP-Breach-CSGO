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

//////////////////////////////////////////////////////////////////////////////
//
//                                 Menu
//
//////////////////////////////////////////////////////////////////////////////

public void InventoryDisplay(Player ply)
{
    Menu InvMenu = new Menu(InventoryHandler);
    InvMenu.OptionFlags = MENUFLAG_NO_SOUND;

    char bstr[128];

    FormatEx(bstr, sizeof(bstr), "%T", "Inventory", ply.id);
    InvMenu.SetTitle(bstr);
    
    ArrayList inv = ply.inv.list;

    if (inv.Length)
    {
        for (int i=0; i < inv.Length; i++)
        {
            char itemid[8], itemclass[32];

            IntToString(i, itemid, sizeof(itemid));
            view_as<InvItem>(inv.Get(i)).GetClass(itemclass, sizeof(itemclass));

            FormatEx(bstr, sizeof(bstr), "%T", itemclass, ply.id);
            InvMenu.AddItem(itemid, bstr, ITEMDRAW_DEFAULT);
        }
    }
    else
    {
        ply.PrintNotify("%t", "Inventory empty");
    }

    InvMenu.ExitButton = true;
    InvMenu.Display(ply.id, 30);
}

public int InventoryHandler(Menu hMenu, MenuAction action, int client, int idx)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select) 
    {
        Player ply = player.GetByID(client);
        InvItem item = ply.inv.Get(idx);
        
        if (!item) return 0;

        char sound[256];
        gamemode.config.sound.GetString("menuselect", sound, sizeof(sound));
        ply.PlayNonCheckSound(sound);

        char class[32];
        item.GetClass(class, sizeof(class));

        Menu InvItmMenu = new Menu(InventoryItemHandler, MenuAction_DrawItem | MenuAction_DisplayItem | MenuAction_Select | MenuAction_End);
        InvItmMenu.OptionFlags = MENUFLAG_NO_SOUND;

        char bstr[128], itemid[3];

        FormatEx(bstr, sizeof(bstr), "%T", class, ply.id);
        IntToString(idx, itemid, sizeof(itemid));
        
        InvItmMenu.SetTitle(bstr);
        InvItmMenu.AddItem(itemid, "use");
        InvItmMenu.AddItem(itemid, "info");
        InvItmMenu.AddItem(itemid, "drop");

        InvItmMenu.ExitButton = true;
        InvItmMenu.Display(ply.id, 30);
    }

    return 0;
}

public int InventoryItemHandler(Menu hMenu, MenuAction action, int client, int idx)
{
    switch (action)
    {
        case MenuAction_DrawItem:
        {
            Player ply = player.GetByID(client);
            
            switch (idx)
            {
                case 0:
                {
                    char itemid[3];
                    hMenu.GetItem(idx, itemid, sizeof(itemid));

                    InvItem item = ply.inv.Get(StringToInt(itemid));
                    
                    return (item.meta.onuse && !item.tmr) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
                }
                case 1:
                {
                    char itemid[3];
                    hMenu.GetItem(idx, itemid, sizeof(itemid));

                    InvItem item = ply.inv.Get(StringToInt(itemid));

                    char class[64];
                    item.GetClass(class, sizeof(class));
                    Format(class, sizeof(class), "%s_info", class);
                    
                    return (TranslationPhraseExists(class)) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
                }
            }
        }
        case MenuAction_DisplayItem:
        {
            Player ply = player.GetByID(client);

            char itemid[3];
            hMenu.GetItem(idx, itemid, sizeof(itemid));

            InvItem item = ply.inv.Get(StringToInt(itemid));

            char str[128];
            switch (idx)
            {
                case 0:
                {
                    if (!item.tmr)
                    {
                        if (item.meta.menu && item.meta.menu.use(str, sizeof(str)))
                        {
                            if (TranslationPhraseExists(str)) FormatEx(str, sizeof(str), "%T", str, ply.id);
                        }
                        else FormatEx(str, sizeof(str), "%T", "Item use", ply.id);
                    }
                    else
                    {
                        if (item.meta.menu && item.meta.menu.cd(str, sizeof(str)))
                        {
                            if (TranslationPhraseExists(str)) FormatEx(str, sizeof(str), "%T", str, ply.id);
                        }
                        else FormatEx(str, sizeof(str), "%T", "Item cooldown", ply.id);

                        Utils.GetTimeString(str, sizeof(str), item.tmr.GetTimeLeft());
                    }
                }
                case 1:
                {
                    if (item.meta.menu && item.meta.menu.info(str, sizeof(str)))
                    {
                        if (TranslationPhraseExists(str)) FormatEx(str, sizeof(str), "%T", str, ply.id);
                    }
                    else FormatEx(str, sizeof(str), "%T", "Item info", ply.id);
                }
                case 2:
                {
                    if (item.meta.menu && item.meta.menu.drop(str, sizeof(str)))
                    {
                        if (TranslationPhraseExists(str)) FormatEx(str, sizeof(str), "%T", str, ply.id);
                    }
                    else FormatEx(str, sizeof(str), "%T", "Item drop", ply.id);
                }
            }

            return RedrawMenuItem(str);
        }
        case MenuAction_Select:
        {
            Player ply = player.GetByID(client);
            
            char itemid[3];
            hMenu.GetItem(idx, itemid, sizeof(itemid));

            switch (idx)
            {
                case 0:
                {
                    InvItem item = ply.inv.Get(StringToInt(itemid));
                    
                    if (item)
                    {
                        char sound[256];
                        if (item.meta.sound && item.meta.sound.use(sound, sizeof(sound)))
                            ply.PlayNonCheckSound(sound);
                        else
                        {
                            gamemode.config.sound.GetString("menuselect", sound, sizeof(sound));
                            ply.PlayNonCheckSound(sound);
                        }

                        char funcname[32];
                        item.meta.onuse.name(funcname, sizeof(funcname));

                        Call_StartFunction(item.meta.onuse.hndl, GetFunctionByName(item.meta.onuse.hndl, funcname));
                        Call_PushCellRef(ply);
                        Call_PushCellRef(item);
                        Call_Finish();
                    }
                }
                case 1:
                {
                    InvItem item = ply.inv.Get(StringToInt(itemid));
                    
                    if (item)
                    {
                        char sound[256];
                        gamemode.config.sound.GetString("menuselect", sound, sizeof(sound));
                        ply.PlayNonCheckSound(sound);

                        char classdata[64], itemaname[64];
                        item.GetClass(classdata, sizeof(classdata));
                        FormatEx(itemaname, sizeof(itemaname), "%T", classdata, ply.id);
                        Format(classdata, sizeof(classdata), "%s_info", classdata);

                        PrintToChat(ply.id, "%s", itemaname);
                        PrintToChat(ply.id, "------------------------");

                        char text[8192];
                        char exptext[20][1024];
                        FormatEx(text, sizeof(text), "%T", classdata, ply.id);
                        ExplodeString(text, "<br>", exptext, 20, 1024);

                        int i=0;
                        while (strlen(exptext[i]) != 0)
                        {
                            PrintToChat(ply.id, exptext[i]);
                            i++;
                        }
                            
                        PrintToChat(ply.id, "------------------------");
                    }
                }
                case 2:
                {
                    InvItem item = ply.inv.DropByIdx(StringToInt(itemid));

                    if (item)
                    {
                        char sound[128];
                        if (item.meta.sound && item.meta.sound.drop(sound, sizeof(sound)))
                            ply.PlayNonCheckSound(sound);
                        else
                        {
                            gamemode.config.sound.GetString("menuselect", sound, sizeof(sound));
                            ply.PlayNonCheckSound(sound);
                        }
                    }
                }
            }
        }
        case MenuAction_End:
        {
            delete hMenu;
        }
    }

    return 0;
}