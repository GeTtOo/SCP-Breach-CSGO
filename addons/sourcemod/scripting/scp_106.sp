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
    name = "[SCP] 106",
    author = "Andrey::Dono",
    description = "SCP-106 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerJoin(Player &ply) {

}

public void SCP_OnPlayerClear(Player &ply) {
    SDKUnhook(ply.id, SDKHook_StartTouch, CheckSurface);
    delete view_as<Vector>(ply.GetHandle("106_tp"));
    delete view_as<Vector>(ply.GetHandle("106_tp_ang"));
    delete view_as<Vector>(ply.GetHandle("106_temp_pos"));
    delete view_as<Vector>(ply.GetHandle("106_temp_ang"));
    ply.RemoveValue("106_tp");
    ply.RemoveValue("106_tp_ang");
    ply.RemoveValue("106_tplock");
    ply.RemoveValue("106_temp_pos");
    ply.RemoveValue("106_temp_ang");
    ply.RemoveValue("106_inpd");
    ply.RemoveValue("106_tplock");
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply && ply.class && ply.class.Is("106"))
    {
        SDKHook(ply.id, SDKHook_StartTouch, CheckSurface);
    }
}

public void Scp_OnRoundEnd()
{
    ArrayList players = player.GetAll();

    for (int i=0; i < players.Length; i++)
    {
        Player ply = players.Get(i);
        
        if (ply && ply.class && ply.class.Is("106"))
            SDKUnhook(ply.id, SDKHook_StartTouch, CheckSurface);
    }

    delete players;
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor) {
    if (vic.class.Is("106"))
    {
        switch (damagetype)
        {
            case DMG_GENERIC:
            {
                if (vic.health < vic.class.health) ((vic.health + 2) >= vic.class.health) ? (vic.health = vic.class.health) : (vic.health += 2);
                return Plugin_Handled;
            }
            default:
            {
                damage /= 20.0;
                return Plugin_Changed;
            }
        }
    }

    if (vic.GetBool("106_inp") && damagetype == DMG_CRUSH)
    {
        vic.SetPos(new Vector(-4306.0, 683.0, 2297.0), new Angle(0.0, 0.0, 0.0));
        vic.SetHandle("106_tp", new Vector(-11110.0, 520.0, -435.0));
        vic.SetHandle("106_tp_ang", new Angle(0.0, 0.0, 0.0));
        SmoothTP(vic);
        vic.RemoveValue("106_inp");
        return Plugin_Handled;
    }

    if (!atk || !atk.class) return Plugin_Continue;

    if (atk.class.Is("106") && vic.IsClass("player"))
    {
        vic.SetHandle("106_tp", new Vector(-4306.0, 683.0, 2297.0));
        vic.SetHandle("106_tp_ang", new Angle(0.0, 0.0, 0.0));
        SmoothTP(vic);
        vic.SetBool("106_inp", true);
        return Plugin_Handled;
    }

    return Plugin_Continue;
}

public void CheckSurface(int client, int entity) {

    char className[32];
    GetEntityClassname(entity, className, sizeof(className));
    
    if (StrEqual(className, "prop_dynamic"))
    {
        int doorid = GetEntPropEnt(entity, Prop_Data, "m_hMoveParent");
        Entity door = (doorid != -1) ? new Entity(doorid) : null;

        if (door && (door.IsClass("func_door") || door.IsClass("prop_door_rotating"))) // door.IsClass("func_door_rotating")
        {

            Player ply = player.GetByID(client);
            
            if (!ply.GetArrayList("106_sdw")) // Smooth door walk
            {
                ArrayList smoothtp = new ArrayList();

                Vector doorforward;
                if (!door.IsClass("prop_door_rotating"))
                {
                    Vector doordir = door.GetPropVector("m_vecMoveDir", Prop_Data);
                    doorforward = (FloatAbs(doordir.x) < 0.3) ? new Vector(1.0,0.0,0.0) : new Vector(0.0,1.0,0.0); //Don't judge me for that :d
                    delete doordir;
                }
                else
                {
                    Angle doorang = door.GetPropAngle("m_angRotation");
                    doorforward = (RoundToCeil(doorang.y) % 180 < 45) ? new Vector(1.0,0.0,0.0) : new Vector(0.0,1.0,0.0);
                    delete doorang;
                }

                Vector target = ply.GetPos() - doorforward.Clone() * doorforward.DotProduct(ply.GetPos() - door.GetPos());

                for (float i=0.1; i < 2.4; i+=0.1) smoothtp.Push(ply.GetPos().Lerp(target.Clone(), i));

                delete target;

                ply.SetArrayList("106_sdw", smoothtp);

                char timername[64];
                FormatEx(timername, sizeof(timername), "SCP-106-DoorWalk-%i", ply.id);
                gamemode.timer.Create(timername, 10, 24, "DoorWalk", ply);
            }

            door.Dispose();

        }
    }
}

public void DoorWalk(Player ply)
{
    ArrayList list = ply.GetArrayList("106_sdw");

    if (list && list.Length > 1)
    {
        Vector vec = list.Get(0);
        ply.SetPos(vec);
        list.Erase(0);
    }
    else
    {
        delete list;
        char timername[64];
        FormatEx(timername, sizeof(timername), "SCP-106-DoorWalk-%i", ply.id);
        gamemode.timer.RemoveByName(timername);
        ply.RemoveValue("106_sdw");
    }
}

public void SmoothTP(Player ply)
{
    if (ply.GetHandle("106_tp") && !ply.GetArrayList("106_stpin"))
    {
        ArrayList smoothtp = new ArrayList();

        for (int i=1; i < 70; i++)
        {
            Vector vec = ply.GetPos();
            vec.z -= i;
            smoothtp.Push(vec);
        }

        ply.SetArrayList("106_stpin", smoothtp);

        char timername[64];
        FormatEx(timername, sizeof(timername), "SCP-106-Smooth-Tp-%i", ply.id);
        gamemode.timer.Create(timername, 10, 70, "HandlerSmoothTPIn", ply);
        
        ply.SetBool("106_tplock", true);
    }
}

public void HandlerSmoothTPIn(Player ply)
{
    ArrayList list = ply.GetArrayList("106_stpin");

    if (list && list.Length > 1)
    {
        Vector vec = list.Get(0);
        ply.SetPos(vec);
        list.Erase(0);
    }
    else
    {
        delete list;
        char timername[64];
        FormatEx(timername, sizeof(timername), "SCP-106-Smooth-Tp-%i", ply.id);
        gamemode.timer.RemoveByName(timername);
        ply.RemoveValue("106_stpin");
        
        ply.SetPos(view_as<Vector>(ply.GetHandle("106_tp")).Clone(), view_as<Angle>(view_as<Angle>(ply.GetHandle("106_tp_ang")).Clone()));

        ArrayList smoothtp = new ArrayList();

        for (int i=1; i < 70; i++)
        {
            Vector vec = ply.GetPos(); 
            vec.z += i;
            smoothtp.Push(vec);
        }

        ply.SetArrayList("106_stpout", smoothtp);

        FormatEx(timername, sizeof(timername), "SCP-106-Smooth-Tp-%i", ply.id);
        gamemode.timer.Create(timername, 10, 70, "HandlerSmoothTPOut", ply);
    }
}

public void HandlerSmoothTPOut(Player ply)
{
    ArrayList list = ply.GetArrayList("106_stpout");

    if (list && list.Length > 1)
    {
        Vector vec = list.Get(0);
        ply.SetPos(vec.Clone());
        list.Erase(0);
    }
    else
    {
        delete list;
        char timername[64];
        FormatEx(timername, sizeof(timername), "SCP-106-Smooth-Tp-%i", ply.id);
        gamemode.timer.RemoveByName(timername);
        ply.RemoveValue("106_stpout");
        
        if (ply.GetHandle("106_temp_pos"))
        {
            delete view_as<Vector>(ply.GetHandle("106_tp"));
            delete view_as<Angle>(ply.GetHandle("106_tp_ang"));
            ply.RemoveValue("106_tp");
            ply.SetHandle("106_tp", ply.GetHandle("106_temp_pos"));
            ply.RemoveValue("106_tp_ang");
            ply.SetHandle("106_tp_ang", ply.GetHandle("106_temp_ang"));
            ply.RemoveValue("106_temp_pos");
            ply.RemoveValue("106_temp_ang");
        }
        
        ply.SetBool("106_tplock", false);
    }
}

public void SCP_OnCallAction(Player &ply) {
    if (ply.class.Is("106"))
        ActionsMenu(ply);
}

public void ActionsMenu(Player ply) {
    Menu hndl = new Menu(ActionsMenuHandler);

    hndl.SetTitle("SCP-106 Actions");

    hndl.AddItem("1", "Set point", (!ply.GetBool("106_inpd")) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    hndl.AddItem("2", "Move to point", (ply.GetHandle("106_tp") && !ply.GetBool("106_tplock")) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    hndl.AddItem("3", "Move to a pocket dimension", (ply.GetHandle("106_tp") && !ply.GetBool("106_tplock") && !ply.GetBool("106_inpd")) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    
    hndl.Display(ply.id, 30);
}

public int ActionsMenuHandler(Menu hMenu, MenuAction action, int client, int idx) 
{
    switch (action)
    {
        case MenuAction_Select:
        {
            Player ply = player.GetByID(client);
            
            switch (idx)
            {
                case 0:
                {
                    if (ply.GetPropEntId("m_hGroundEntity") == 0)
                    {
                        if (ply.GetHandle("106_tp"))
                        {
                            delete view_as<Vector>(ply.GetHandle("106_tp"));
                            delete view_as<Angle>(ply.GetHandle("106_tp_ang"));
                            ply.RemoveValue("106_tp");
                            ply.RemoveValue("106_tp_ang");
                        }
                        
                        ply.SetHandle("106_tp", ply.GetPos() - new Vector(0.0, 0.0, 68.0));
                        ply.SetHandle("106_tp_ang", ply.GetAng());
                    }
                }
                case 1:
                {
                    if (ply.GetPropEntId("m_hGroundEntity") == 0)
                    {
                        SmoothTP(ply);
                        
                        ply.SetBool("106_inpd", false);
                    }
                }
                case 2:
                {
                    if (ply.GetPropEntId("m_hGroundEntity") == 0)
                    {
                        ply.SetHandle("106_temp_pos", ply.GetHandle("106_tp"));
                        ply.SetHandle("106_temp_ang", ply.GetHandle("106_tp_ang"));
                        ply.SetHandle("106_tp", new Vector(-4306.0, 683.0, 2297.0));
                        ply.SetHandle("106_tp_ang", new Angle(0.0, 0.0, 0.0));
                        SmoothTP(ply);

                        ply.SetBool("106_inpd", true);
                    }
                }
            }
        }
    }

    return 0;
}