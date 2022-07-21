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
    name = "[SCP] 079",
    author = "Andrey::Dono",
    description = "SCP-079 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

methodmap Camera < Base {

    public Camera(Entity ent) {
        Camera cam = view_as<Camera>(new Base());
        
        cam.SetHandle("ent", ent);

        return cam;
    }

    property Entity ent {
        public set(Entity val) { this.SetHandle("ent", val); }
        public get() { return view_as<Entity>(this.GetHandle("ent")); }
    }

    property bool lock {
        public set(bool val) { this.SetBool("locked", val); }
        public get() { return this.GetBool("locked"); }
    }
}

methodmap Controller < Base {
    
    public Controller(Player ply) {
        Controller self = view_as<Controller>(new Base());
        self.SetHandle("player", ply);
        self.CreateArrayList("camlist");

        return self;
    }

    property Player ply {
        public get() { return view_as<Player>(this.GetHandle("player")); }
    }

    property Camera curcam {
        public set(Camera val) { this.SetHandle("curcam", val); }
        public get() { return view_as<Camera>(this.GetHandle("curcam")); }
    }

    property bool lock {
        public set(bool val) { this.SetBool("locked", val); }
        public get() { return this.GetBool("locked"); }
    }

    property ArrayList camlist {
        public get() { return this.GetArrayList("camlist"); }
    }

    public void Init() {
        int entId = 0;
        while ((entId = FindEntityByClassname(entId, "prop_dynamic")) != -1) {
            if (!IsValidEntity(entId)) continue;

            char ModelName[128];
            GetEntPropString(entId, Prop_Data, "m_ModelName", ModelName, sizeof(ModelName));

            if (StrEqual(ModelName, "models/freeman/cctv_camera_fisheye.mdl"))
            {
                this.camlist.Push(new Camera(new Entity(entId)));
            }
        }

        SetEntityMoveType(this.ply.id, MOVETYPE_NONE);
        this.ply.model.SetRenderMode(RENDER_NONE);
        this.ply.model.scale = 0.01;

        this.ply.SetProp("m_iHideHUD", 4112); // 1<<12|1<<4 || 2^12 + 2^4

        ClientCommand(this.ply.id, "r_screenoverlay models/scp/camera_effect");
    }

    public void Set(Camera camera) {
        Angle ang = camera.ent.GetAng();
        ang.x += 25.0;
        ang.z = 0.0;
        this.ply.SetPos(camera.ent.GetPos() - new Vector(0.0,0.0,75.0), ang);
        this.curcam = camera;
    }

    public void Next() {
        this.curcam = this.camlist.Get((this.camlist.FindValue(this.curcam) + 1) > this.camlist.Length - 1  ? 0 : this.camlist.FindValue(this.curcam) + 1);
        this.Set(this.curcam);
    }

    public void Prev() {
        this.curcam = this.camlist.Get((this.camlist.FindValue(this.curcam) - 1) < 0 ? this.camlist.Length - 1 : this.camlist.FindValue(this.curcam) - 1);
        this.Set(this.curcam);
    }

    public void Dispose() {
        delete this.camlist;
    }
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply.class.Is("079"))
    {
        Controller controller = new Controller(ply);
        
        controller.Init();
        controller.curcam = controller.camlist.Get(0);
        
        ply.SetHandle("079_controller", controller);
        
        //char timername[32];
        //Format(timername, sizeof(timername), "SCP-079_timeupd_", ply.id);
        //timer.Create(timername, 1000, 0, "TimeUpdate", ply);
        //TimeUpdate(ply);
    }
}

public void SCP_OnPlayerClear(Player &ply) {
    if (ply && ply.class && ply.class.Is("079"))
    {
        view_as<Controller>(ply.GetHandle("079_controller")).Dispose();

        ply.HideOverlay();
    }

    //char timername[32];
    //Format(timername, sizeof(timername), "SCP-079_timeupd_", ply.id);
    //timer.RemoveByName(timername);
}

public void SCP_OnPlayerSetupOverlay(Player &ply) {
    if (ply && ply.class && ply.class.Is("079"))
        ply.ShowOverlay("eternity/overlays/079");
}

public void SCP_OnInput(Player &ply, int buttons)
{
    if (ply.class.Is("079") && ply.IsAlive())
    {
        Controller controller = view_as<Controller>(ply.GetHandle("079_controller"));

        if (!controller.lock)
        {
            Camera curcam = controller.curcam;

            if (buttons & IN_ATTACK) // 2^0 +attack
                controller.Next();
            else if (buttons & IN_ATTACK2) // 2^11 +attack2
                controller.Prev();

            if (controller.curcam != curcam) {
                controller.lock = true;
                ply.TimerSimple(250, "CameraUnlock", ply);
            }
        }
    }
}

public void SCP_OnCallAction(Player &ply) {
    if (ply.class.Is("079"))
        ActionsMenu(ply);
}

public void CameraUnlock(Player ply) {
    view_as<Controller>(ply.GetHandle("079_controller")).lock = false;
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