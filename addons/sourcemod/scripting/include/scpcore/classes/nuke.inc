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

/*----------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------Nuclear warhead manager------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

enum AlphaWarhead {
    Disabled = 0,
    Ready,
    Activated,
    Canceled,
    Nuked
}

methodmap NuclearWarhead < Base {

    public NuclearWarhead() {
        NuclearWarhead controller = view_as<NuclearWarhead>(new Base());
        controller.SetHandle("fw", CreateGlobalForward("SCP_OnAlphaWarhead", ET_Event, Param_Cell));
        controller.CreateArrayList("monitors");
        return controller;
    }

    property Handle fw {
        public get() { return this.GetHandle("fw"); }
    }

    property ArrayList monitors {
        public set(ArrayList value) { this.SetArrayList("monitors", value); }
        public get() { return this.GetArrayList("monitors"); }
    }

    property bool IsNuked {
        public set(bool value) { this.SetBool("nuked", value); }
        public get() { return this.GetBool("nuked"); }
    }

    property bool ready {
        public set(bool value) { this.SetBool("ready", value); }
        public get() { return this.GetBool("ready"); }
    }

    property bool active {
        public set(bool value) { this.SetBool("active", value); }
        public get() { return this.GetBool("active"); }
    }

    property bool block {
        public set(bool value) { this.SetBool("block", value); }
        public get() { return this.GetBool("block"); }
    }

    property float detonationtime {
        public set(float value) { this.SetFloat("detonationtime", value); }
        public get() { return this.GetFloat("detonationtime"); }
    }

    property float resumetime {
        public set(float value) { this.SetFloat("resumetime", value); }
        public get() { return this.GetFloat("resumetime"); }
    }

    public void Update(char[] text, int size = 0) {
        ArrayList displays = worldtext.GetAll(NukeMonitor);

        if (displays.Length > 0)
            for (int i=0; i < displays.Length; i++) {
                WorldText display = displays.Get(i);

                display.SetText(text);
                if (size != 0)
                    display.SetSize(size);
            }

        delete displays;
    }

    public void Init() {
        this.Update("Disabled");
    }

    public void Ready() {
        if (!this.ready)
        {
            this.ready = true;
            this.Update(" Ready");

            Call_StartForward(this.fw);
            Call_PushCell(Ready);
            Call_Finish();
        }
        else
        {
            this.ready = false;
            this.Update("Disabled");

            Call_StartForward(this.fw);
            Call_PushCell(Disabled);
            Call_Finish();
        }
    }

    public void Activate() {
        timer.Create("SCP_ALPHA_WARHEAD_CALL_EXPLODE", gamemode.config.nuke.time * 1000, 1, "NukeExplode");
        timer.Create("SCP_ALPHA_WARHEAD_DOOR_CONTROLLER", (gamemode.config.nuke.time - 10) * 1000, 1, "ClosingDoorBeforeNukeExplode");

        char path[256];
        gamemode.config.nuke.sound.start(path, sizeof(path));
        gamemode.mngr.PlayTranslatedSoundToAll(path);

        int ent;
        
        while((ent = FindEntityByClassname(ent, "func_door")) != -1)
        {
            char name[16];
            GetEntPropString(ent, Prop_Data, "m_iName", name, sizeof(name));
            
            if (gamemode.config.nuke.immunedoors.IndexOf(ent) != -1) continue;
            
            AcceptEntityInput(ent, "Open");
        }

        this.detonationtime = GetGameTime() + float(gamemode.config.nuke.time);

        //if (this.resumetime == 0.0)
        timer.Create("SCP_ALPHA_WARHEAD_COUNTDOWN", 100, 0, "SCP_AlphaWarheadCountdown");
        //else
            //timer.Create("SCP_ALPHA_WARHEAD_COUNTDOWN", 10, RoundToNearest(this.resumetime) * 100000, "SCP_AlphaWarheadCountdown");

        this.active = true;

        for (int i=0; i < this.monitors.Length; i++) {
            Entity display = this.monitors.Get(i);
            
            display.SetProp("m_nSkin", (gamemode.lang == 22) ? 1 : 3);
        }

        Call_StartForward(this.fw);
        Call_PushCell(Activated);
        Call_Finish();
    }

    public void Cancel() {
        //this.resumetime = this.detonationtime - GetGameTime();
        this.active = false;
        this.ready = false;
        
        timer.RemoveByName("SCP_ALPHA_WARHEAD_CALL_EXPLODE");
        timer.RemoveByName("SCP_ALPHA_WARHEAD_DOOR_CONTROLLER");
        timer.RemoveByName("SCP_ALPHA_WARHEAD_COUNTDOWN");
        timer.RemoveByName("SCP_ALPHA_WARHEAD_AUTO_COUNTDOWN");

        char path[256];
        gamemode.config.nuke.sound.stop(path, sizeof(path));
        gamemode.mngr.PlayTranslatedSoundToAll(path);

        this.Update("Canceled");

        Call_StartForward(this.fw);
        Call_PushCell(Canceled);
        Call_Finish();
    }

    public void AddCounterToDisplay() {
        int entId = 0;
        while ((entId = FindEntityByClassname(entId, "prop_dynamic")) != -1) {
            if (!IsValidEntity(entId)) continue;

            char ModelName[128];
            GetEntPropString(entId, Prop_Data, "m_ModelName", ModelName, sizeof(ModelName));

            if (StrEqual(ModelName, "models/eternity/map/monitor.mdl"))
                this.monitors.Push(new Entity(entId));
        }

        for (int i=0; i < this.monitors.Length; i++) {
            Entity display = this.monitors.Get(i);

            display.SetProp("m_nSkin", (gamemode.lang == 22) ? 0 : 2);
            
            Vector dp = (display.GetAng() - new Angle(10.0,90.0,0.0)).GetUpVectorScaled((display.GetAng() - new Angle(10.0,90.0,0.0)).GetRightVectorScaled((display.GetAng() - new Angle(10.0,90.0,0.0)).GetForwardVectorScaled(display.GetPos(), -8.0), -37.5), -17.0);
            Angle da = display.GetAng() - new Angle(10.0,90.0,0.0);

            worldtext.Create(dp, da, NukeMonitor).SetSize(15).SetColor(new Colour(190,0,20));
        }
    }

    public void SpawnDisplay() {
        
        JSON_ARRAY spawnlist = gamemode.config.nuke.spawnlist;

        for (int i=0; i < spawnlist.Length; i++) {
            JSON_OBJECT data = view_as<JSON_OBJECT>(spawnlist.Get(i));

            worldtext.Create(data.GetVector("pos"), data.GetAngle("ang"), NukeMonitor).SetSize(data.GetInt("size")).SetColor(data.GetColor("color")).SetText("Disabled");
        }
        
        this.AddCounterToDisplay();

        Call_StartForward(this.fw);
        Call_PushCell(Disabled);
        Call_Finish();
    }
    
    public void Controller(int btnId) {
        if (gamemode.config.nuke.buttons.ready == btnId && !this.active && !this.IsNuked)
            this.Ready();
        else if (gamemode.config.nuke.buttons.active == btnId && this.ready && !this.active && !this.IsNuked)
            this.Activate();
        else if (gamemode.config.nuke.buttons.cancel == btnId && this.ready && this.active && !this.block)
            this.Cancel();
    }
    
    public void AutoStart(int seconds)
    {
        timer.Create("SCP_ALPHA_WARHEAD_AUTO_COUNTDOWN", seconds * 1000, 1, "SCP_AlphaWarheadAutoStart");
    }

    public void Reset() {
        this.IsNuked = false;
        this.ready = false;
        this.active = false;
        this.block = false;
        this.resumetime = 0.0;
        this.detonationtime = 0.0;
        for (int i=0; i < this.monitors.Length; i++) view_as<Entity>(this.monitors.Get(i)).Dispose();
        this.monitors.Clear();
    }

    public void Dispose()
    {
        delete this.fw;
        
        for (int i=0; i < this.monitors.Length; i++) view_as<Entity>(this.monitors.Get(i)).Dispose();
        this.monitors.Clear();
        delete this.monitors;
        delete this;
    }
}

public void SCP_AlphaWarheadCountdown() {
    char time[32], time2[10], time3[3];
    float cursec = gamemode.nuke.detonationtime - GetGameTime();
    int min = RoundToNearest(cursec) / 60;
    int sec = RoundFloat(cursec) % 60;
    FloatToString(FloatFraction(cursec), time2, sizeof(time2));
    strcopy(time3, sizeof(time3), time2[2]);
    Format(time, sizeof(time), (min < 10 ) ? ((sec < 10 ) ? "0%i:0%i:%s" : "0%i:%i:%s") : ((sec < 10 ) ? "%i:0%i:%s" : "%i:%i:%s"),  RoundToNearest(cursec) / 60, RoundFloat(cursec) % 60, time3);
    gamemode.nuke.Update(time);

    //if ((gamemode.nuke.detonationtime - GetGameTime()) < 0.1)
        //gamemode.nuke.Update("Site destroyed");
}

public void SCP_AlphaWarheadAutoStart()
{
    gamemode.nuke.ready = true;
    gamemode.nuke.Activate();
    gamemode.nuke.block = true;
}

public void ClosingDoorBeforeNukeExplode() {
    int ent;
    while((ent = FindEntityByClassname(ent, "func_door")) != -1)
    {
        char name[16];
        GetEntPropString(ent, Prop_Data, "m_iName", name, sizeof(name));
        
        if(StrContains(name, "DoorGate", false) != -1)
        {
            AcceptEntityInput(ent, "Close");
            AcceptEntityInput(ent, "Lock");
        }
    }
}

public void NukeExplode() {

    gamemode.nuke.IsNuked = true;

    for(int i=0; i < MAXPLAYERS; i++)
    {
        Player ply = player.GetByID(i);

        if(ply != null && ply.IsAlive())
        {
            Vector pos = ply.GetPos();

            if(pos.z <= gamemode.config.nuke.killpos)
            {
                ply.TakeDamage(_, 50000.0, DMG_BLAST);
            }
            else
            {
                EmitSoundToClient(ply.id, "weapons/c4/c4_exp_deb1.wav");
                gamemode.mngr.Shake(ply.id, 5, 10, 50);
            }

            delete pos;
        }
    }

    timer.RemoveByName("SCP_ALPHA_WARHEAD_COUNTDOWN");
    timer.RemoveByName("SCP_ALPHA_WARHEAD_AUTO_COUNTDOWN");

    gamemode.nuke.Update("Site destroyed", 8);

    PrintToChatAll(" \x07[SCP] \x01%t", "Site destroyed");
    gamemode.log.Info("%t", "Site destroyed");

    Call_StartForward(gamemode.nuke.fw);
    Call_PushCell(Nuked);
    Call_Finish();

    //gamemode.mngr.EndGame("nuke");
}