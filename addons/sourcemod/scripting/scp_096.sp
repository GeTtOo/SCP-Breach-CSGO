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
    name = "[SCP] 096",
    author = "Andrey::Dono, GeTtOo",
    description = "SCP-096 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

enum State
{
    Calm = 0,
    Panic,
    Rage,
    Cooldown
}

methodmap ClassManager < Base {

    public ClassManager(Player ply) {
        ClassManager self = view_as<ClassManager>(new Base());
        
        self.Set("player", ply);
        self.Set("ambient", new Ambient(ply));
        self.Set("animation", new Animation(ply));
        self.Set("state", Calm);
        self.CreateArrayList("targets");

        char timername[64];
        Format(timername, sizeof(timername), "scp-096-%i", ply.id);
        self.Set("timer", timer.Create(timername, 250, 0, "CheckState", self));

        return self;
    }
    
    property Player ply {
        public get() { return this.Get("player"); }
    }

    property Ambient ambient {
        public get() { return this.Get("ambient"); }
    }

    property Animation anim {
        public get() { return this.Get("animation"); }
    }

    property State state {
        public set(State val) { this.Set("state", val); }
        public get() { return this.Get("state"); }
    }

    property bool lockstate {
        public set(bool val) { this.SetBool("lockstate", val); }
        public get() { return this.GetBool("lockstate", false); }
    }

    property ArrayList targets {
        public get() { return this.GetArrayList("targets"); }
    }

    property Tmr timer {
        public get() { return this.Get("timer"); }
    }

    property Tmr ragetmr {
        public set(Tmr val) { this.Set("ragetmr", val); }
        public get() { return this.Get("ragetmr"); }
    }

    property Tmr sndtmr {
        public set(Tmr val) { this.Set("sndtmr", val); }
        public get() { return this.Get("sndtmr"); }
    }

    public ClassManager Init()
    {
        this.ply.ExecCommand("thirdperson");
        this.ply.ExecCommand("cam_idealdist 50");
        StartCalmState(this);
        return this;
    }

    public void Dispose()
    {
        this.ply.RemoveHook(SDKHook_StartTouch, OnDoorTouched);
        this.ply.ExecCommand("firstperson");
        timer.Remove(this.timer);
        timer.Remove(this.sndtmr);
        this.ambient.Remove();
        delete this.targets;
        delete this;
    }
}

bool scpfound = false;

public void SCP_OnRoundEnd()
{
    if (scpfound) return;

    ArrayList players = player.GetAll();

    for (int i=0; i < players.Length; i++)
    {
        if (!scpfound) break;
        view_as<Player>(players.Get(i)).RemoveHook(SDKHook_SetTransmit, OnEntityTransmit);
    }

    delete players;
    
    scpfound = false;
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply.class.Is("096"))
    {
        if (!scpfound)
        {
            scpfound = true;

            ArrayList players = player.GetAll();

            for (int i=0; i < players.Length; i++)
            {
                if (!scpfound) break;
                if (ply == players.Get(i)) break;
                view_as<Player>(players.Get(i)).SetHook(SDKHook_SetTransmit, OnEntityTransmit);
            }

            delete players;
        }
        
        ply.Set("096_mngr", (new ClassManager(ply)).Init());
    }
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply.class && ply.class.Is("096"))
    {
        view_as<ClassManager>(ply.Get("096_mngr")).Dispose();
        ply.RemoveValue("096_mngr");
    }

    ArrayList players = player.GetAll();

    for (int i=0; i < players.Length; i++)
    {
        if (view_as<Player>(players.Get(i)).HasKey("096_mngr"))
        {
            ArrayList targets = view_as<ClassManager>(view_as<Player>(players.Get(i)).Get("096_mngr")).targets;

            int idx = targets.FindValue(ply);
            if (idx != -1)
                targets.Erase(idx);
        }
    }

    delete players;
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor)
{
	if (!atk || !atk.class) return Plugin_Continue;

	if(atk.class.Is("096"))
	{
        if (view_as<ClassManager>(atk.Get("096_mngr")).state == Rage)
            damage += 10000.0;
        else
            damage = 0.0;

        return Plugin_Changed;
	}

	return Plugin_Continue;
}

public void CheckState(ClassManager mngr)
{
    if (mngr.state == Cooldown) return;

    if (mngr.state == Rage && mngr.targets.Length == 0)
    {
        mngr.ply.RemoveHook(SDKHook_StartTouch, OnDoorTouched);

        mngr.ply.SetProp("m_iHideHUD", 1<<12);
        mngr.ply.speed = mngr.ply.class.speed;
        mngr.ply.multipler = mngr.ply.class.multipler;

        timer.Remove(mngr.sndtmr);
        mngr.Set("cooldown", mngr.ply.TimerSimple(25000, "CooldownEndState", mngr));
        mngr.ply.TimerSimple(5800, "StartCalmState", mngr);

        if (mngr.ragetmr) timer.Remove(mngr.ragetmr);

        mngr.ambient.Play("*/eternity/scp/096/tranquility.mp3");

        mngr.state = Cooldown;
        return;
    }

    char filter[1][32] = {"player"};
    ArrayList targets = ents.FindInCone(mngr.ply.EyePos(), mngr.ply.GetAng().GetForwardVectorScaled(mngr.ply.EyePos(), 2000.0), 90, filter, sizeof(filter));

    for (int i=0; i < targets.Length; i++)
    {
        Player target = targets.Get(i);

        if (mngr.state == Cooldown || mngr.targets.FindValue(target) != -1 || mngr.ply == target || target.IsSCP || !target.IsAlive()) continue;

        ArrayList checklist = ents.FindInPVS(target, 2000);

        if (checklist.FindValue(mngr.ply) != -1)
        {
            mngr.targets.Push(target);

            if (mngr.state == Calm) mngr.state = Panic;
        }

        delete checklist;
    }

    delete targets;

    if (mngr.lockstate) return;

    switch (mngr.state)
    {
        case Panic:
        {
            mngr.ply.SetProp("m_iHideHUD", 0);
            mngr.ply.speed = 0.1;
            timer.Remove(mngr.sndtmr);
            mngr.ambient.Play("*/eternity/scp/096/rage_start.mp3");
            mngr.ply.TimerSimple(5400, "StartRageState", mngr);
            mngr.lockstate = true;
        }
        case Rage:
        {
            mngr.ply.speed = 260.0;
            mngr.ply.multipler = 2.5;
            mngr.ply.SetHook(SDKHook_StartTouch, OnDoorTouched);
            mngr.lockstate = true;
            mngr.ambient.Play("*/eternity/scp/096/rage.mp3");

            char timername[64];
            Format(timername, sizeof(timername), "scp-096-loopsnd-%i", mngr.ply.id);
            mngr.sndtmr = timer.Create(timername, 10400, 0, "RepeatRageSound", mngr);
            mngr.ragetmr = mngr.ply.TimerSimple(45000, "RageClear", mngr);
        }
    }
}

public void StartRageState(ClassManager mngr)
{
    mngr.lockstate = false;
    mngr.state = Rage;
}

public void RageClear(ClassManager mngr)
{
    mngr.targets.Clear();
    mngr.ragetmr = null;
}

public void RepeatCalmSound(ClassManager mngr)
{
    mngr.ambient.Play("*/eternity/scp/096/crying.mp3");
}

public void RepeatRageSound(ClassManager mngr)
{
    mngr.ambient.Play("*/eternity/scp/096/rage.mp3");
}

public void StartCalmState(ClassManager mngr)
{
    mngr.ambient.Play("*/eternity/scp/096/crying.mp3");

    char timername[64];
    Format(timername, sizeof(timername), "scp-096-loopsnd-%i", mngr.ply.id);
    mngr.sndtmr = timer.Create(timername, 46000, 0, "RepeatCalmSound", mngr);
}

public void CooldownEndState(ClassManager mngr)
{
    mngr.state = Calm;
    mngr.lockstate = false;
    timer.Remove(mngr.Get("cooldown"));
}

public Action OnEntityTransmit(int entity, int client)
{
    Player ply = player.GetByID(client);
    Player target = player.GetByID(entity);

    if (entity != client && ply.IsAlive() && target.IsAlive() && ply.class.Is("096"))
    {
        ClassManager mngr = ply.Get("096_mngr");
        
        if (mngr.state == Rage && mngr.targets.FindValue(player.GetByID(entity)) == -1) return Plugin_Handled;
    }

    return Plugin_Continue;
}

public void OnDoorTouched(int client, int entity)
{
    ClassManager mngr = player.GetByID(client).Get("096_mngr");
    
    char classname[32];
    GetEntityClassname(entity, classname, sizeof(classname));

    if (!StrEqual(classname, "func_door")) return;
    
    int cid = GetEntPropEnt(entity, Prop_Data, "m_hMoveChild");

    if (cid != -1)
    {
        char sound[128];
        JSON_ARRAY nbs = gamemode.plconfig.Get("sound").GetArr("doorbroke");
        nbs.GetString(GetRandomInt(0, nbs.Length - 1), sound, sizeof(sound));

        float vecarr[3];
        GetEntPropVector(entity, Prop_Send, "m_vecOrigin", vecarr);
        EmitAmbientSound(sound, vecarr, entity);

        RemoveEntity(entity);
    }
    else
    {
        char filter[1][32] = {"prop_dynamic"};
        ArrayList entities = ents.FindInBox(mngr.ply.GetPos() - new Vector(100.0,100.0,100.0), mngr.ply.GetPos() + new Vector(100.0,100.0,100.0), filter, sizeof(filter));

        for (int i=0; i < entities.Length; i++)
        {
            Entity ent = entities.Get(i);

            char modelpath[128];
            ent.model.GetPath(modelpath, sizeof(modelpath));

            if (StrEqual(modelpath, "models/eternity/map/scp_gate.mdl"))
            {
                SetVariantString("096_Break");
                ent.Input("SetAnimation");
            }
            
            if (ents.list.FindValue(ent, 1) == -1)
                ent.Dispose();
        }

        delete entities;
    }
}

public void SCP_OnCallAction(Player &ply) {
    if (ply && ply.class && ply.class.Is("096"))
    {
        return;
    }
}