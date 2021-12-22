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
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 079",
    author = "Andrey::Dono",
    description = "SCP-079 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

ArrayList CamList;

methodmap Controller < Base {
    
    property Entity current {
        public set(Entity val) { this.SetValue("current", val); }
        public get() { Entity val; this.GetValue("current", val); return val; }
    }

    property bool locked {
        public set(bool val) { this.SetBool("locked", val); }
        public get() { return this.GetBool("locked"); }
    }

    public void Set(Entity camera) {
        Player ply;
        this.GetValue("player", ply);

        float CameraAngle[3];
        camera.GetAng().GetArr(CameraAngle);
        ply.SetPos(camera.GetPos() - new Vector(0.0,0.0,75.0), new Angle(CameraAngle[0] + 25, CameraAngle[1], 0.0));
        this.current = camera;
    }

    public void Next() {
        this.current = CamList.Get((CamList.FindValue(this.current) + 1) > CamList.Length - 1  ? 0 : CamList.FindValue(this.current) + 1);
        this.Set(this.current);
    }

    public void Prev() {
        this.current = CamList.Get((CamList.FindValue(this.current) - 1) < 0 ? CamList.Length - 1 : CamList.FindValue(this.current) - 1);
        this.Set(this.current);
    }

    public Controller(Player ply) {
        Controller self = view_as<Controller>(new Base());
        self.SetValue("player", ply);
        self.SetValue("current", CamList.Get(0));
        self.Set(self.current);

        return self;
    }
}

public void SCP_OnRoundStart() {
    CamList = new ArrayList();

    int entId = 0;
    while ((entId = FindEntityByClassname(entId, "prop_dynamic")) != -1) {
        if (!IsValidEntity(entId)) continue;

        char ModelName[128];
        GetEntPropString(entId, Prop_Data, "m_ModelName", ModelName, sizeof(ModelName));

        if (StrEqual(ModelName, "models/freeman/cctv_camera_fisheye.mdl"))
            CamList.Push(new Entity(entId));
    }
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply.class.Is("079")) {
        SetEntityMoveType(ply.id, MOVETYPE_NONE);
        SetEntityRenderMode(ply.id, RENDER_NONE);
        SetEntProp(ply.id, Prop_Send, "m_iHideHUD", 4112); // 1<<12|1<<4 || 2^12 + 2^4
        SetEntPropFloat(ply.id, Prop_Send, "m_flModelScale", 0.01);
        ply.SetValue("camcontrol", new Controller(ply));
        ClientCommand(ply.id, "r_screenoverlay models/scp/camera_effect");
        char timername[32];
        Format(timername, sizeof(timername), "SCP-079_timeupd_", ply.id);
        gamemode.timer.Create(timername, 1000, 0, "TimeUpdate", ply);
        TimeUpdate(ply);
    }
}

public void TimeUpdate(Player ply) {
    char date[32];

    FormatTime(date, sizeof(date), "%H:%M:%S");
    SetHudTextParams(0.028, 0.888, 1.0, 200, 200, 200, 190);
    ShowHudText(ply.id, 52, date);

    FormatTime(date, sizeof(date), "%d/%m/%y");
    SetHudTextParams(0.025, 0.915, 1.0, 200, 200, 200, 190);
    ShowHudText(ply.id, 53, date);
}

public void SCP_OnPlayerReset(Player &ply) {
    Controller Camera;
    ply.GetValue("camcontrol", Camera);
    delete Camera;

    char timername[32];
    Format(timername, sizeof(timername), "SCP-079_timeupd_", ply.id);
    gamemode.timer.Remove(timername);

    ClientCommand(ply.id, "r_screenoverlay off");
}

public void SCP_OnPlayerDeath(Player &ply) {
    Controller Camera;
    ply.GetValue("camcontrol", Camera);
    delete Camera;

    char timername[32];
    Format(timername, sizeof(timername), "SCP-079_timeupd_", ply.id);
    gamemode.timer.Remove(timername);

    ClientCommand(ply.id, "r_screenoverlay off");
}

public void SCP_OnInput(Player &ply, int buttons)
{
    if (ply.class.Is("079") && IsPlayerAlive(ply.id))
    {
        Controller Camera;
        ply.GetValue("camcontrol", Camera);

        if (!Camera.locked)
        {
            Entity curcam = Camera.current;

            if (buttons & IN_ATTACK) // 2^0 +attack
                Camera.Next();
            else if (buttons & IN_ATTACK2) // 2^11 +attack2
                Camera.Prev();

            if (Camera.current != curcam) {
                Camera.locked = true;
                gamemode.timer.Simple(250, "CameraUnlock", ply);
            }
        }
    }
}

public void CameraUnlock(Player ply) {
    Controller Camera;
    ply.GetValue("camcontrol", Camera);
    Camera.locked = false;
}

public void SCP_OnCallActionMenu(Player &ply) {
    if (ply.class.Is("079"))
        ActionsMenu(ply);
}

public void ActionsMenu(Player ply) {
    Menu hndl = new Menu(ActionsMenuHandler);

    hndl.SetTitle("SCP-079 Actions");

    hndl.AddItem("1", "Блокировать двери", ITEMDRAW_DEFAULT);
    hndl.AddItem("1", "Отключить свет в секторе", ITEMDRAW_DEFAULT);
    
    hndl.Display(ply.id, 30);
}

public int ActionsMenuHandler(Menu hMenu, MenuAction action, int client, int item) 
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select) 
    {
        PrintToChat(client, "Test");
    }
}