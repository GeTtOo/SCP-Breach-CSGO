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
    name = "[SCP] Voice",
    author = "Andrey::Dono",
    description = "Voice plugin for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

methodmap IntercomController < Base {

    public IntercomController() {
        IntercomController controller = view_as<IntercomController>(new Base());
        controller.CreateArrayList("monitors");
        return controller;
    }

    property ArrayList monitors {
        public get() { return this.GetArrayList("monitors"); }
    }

    property TypeWorldText wtid {
        public get() { return view_as<TypeWorldText>(gamemode.plconfig.GetInt("wtid", 8)); }
    }

    property bool IsBroadcast {
        public set(bool value) { this.SetBool("bc", value); }
        public get() { return this.GetBool("bc"); }
    }

    property Player curply {
        public set(Player value) { this.SetBase("curply", value); }
        public get() { return view_as<Player>(this.GetBase("curply")); }
    }

    property bool ready {
        public set(bool value) { this.SetBool("ready", value); }
        public get() { return this.GetBool("ready"); }
    }

    property float relcdtime {
        public set(float value) { this.SetFloat("relcdtime", value); }
        public get() { return this.GetFloat("relcdtime"); }
    }

    public void UpdateText(char[] text, int size = 0) {
        ArrayList displays = worldtext.GetAll(this.wtid);

        if (displays.Length > 0)
            for (int i=0; i < displays.Length; i++) {
                WorldText display = displays.Get(i);

                display.SetText(text);
                if (size != 0)
                    display.SetSize(size);
            }

        delete displays;
    }

    public void Ready() {
        this.ready = true;
        this.UpdateText("Ready");
    }

    public void StrartTransmission(Player speaker) {
        char sound[128];
        gamemode.plconfig.GetObject("sound").GetString("start", sound, sizeof(sound));
        gamemode.mngr.PlaySoundToAll(sound, SNDCHAN_VOICE);

        gamemode.timer.Create("Intercom_transmission_active", gamemode.plconfig.GetInt("transmissiontime", 8) * 1000, 1, "TransmissionStop");

        this.UpdateText("~Live");

        this.curply = speaker;
        this.IsBroadcast = true;
        this.ready = false;
    }

    public void EndTransmission() {
        int transmissiontime = gamemode.plconfig.GetInt("cooldown", 8);
        gamemode.timer.Create("Intercom_cd_release", transmissiontime * 1000, 1, "VoiceRelease");
        this.relcdtime = GetGameTime() + float(transmissiontime);

        gamemode.timer.Create("Intercom_cd_countdown", 100, 0, "DisplayCounterUpdate");

        char sound[128];
        gamemode.plconfig.GetObject("sound").GetString("start", sound, sizeof(sound));
        gamemode.mngr.StopSoundAll(sound, SNDCHAN_VOICE);

        gamemode.plconfig.GetObject("sound").GetString("stop", sound, sizeof(sound));
        gamemode.mngr.PlaySoundToAll(sound, SNDCHAN_VOICE);

        this.curply = null;
        this.IsBroadcast = false;
    }

    public void CooldownRelease() {
        gamemode.timer.RemoveByName("Intercom_cd_countdown");
        this.Ready();
    }

    public void DisplayInit() {
        int entId = 0;
        while ((entId = FindEntityByClassname(entId, "prop_dynamic")) != -1) {
            if (!IsValidEntity(entId)) continue;

            char ModelName[256];
            GetEntPropString(entId, Prop_Data, "m_ModelName", ModelName, sizeof(ModelName));

            if (StrEqual(ModelName, "models/props/coop_autumn/surveillance_monitor/surveillance_monitor_32.mdl"))
            {
                this.monitors.Push(new Entity(entId));
                gamemode.log.Debug("Intercom display registered. Entity id: %i", entId);
            }
        }

        for (int i=0; i < this.monitors.Length; i++) {
            Entity display = this.monitors.Get(i);
            
            Vector dp = (display.GetAng() - new Angle(0.0,180.0,0.0)).Up((display.GetAng() - new Angle(0.0,180.0,0.0)).Right((display.GetAng() - new Angle(0.0,180.0,0.0)).Forward(display.GetPos(), 0.0), -13.5), -5.0);
            Angle da = display.GetAng() - new Angle(0.0,180.0,0.0);

            worldtext.Create(dp, da, this.wtid).SetSize(8).SetColor(new Colour(126,190,42));
        }
        
        gamemode.log.Debug("All displays successfully initialized");
    }
    
    public void Reset() {
        this.ready = false;
        this.IsBroadcast = false;
        this.relcdtime = 0.0;
        this.monitors.Clear();
    }

    public void Dispose() {
        gamemode.timer.RemoveByName("Intercom_transmission_active");
        gamemode.timer.RemoveByName("Intercom_cd_release");
        gamemode.timer.RemoveByName("Intercom_cd_countdown");
        delete this.monitors;
        delete this;
    }
}

IntercomController Intercom;

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_USE, "radio", "OnUse"); // @arg1 Player, @arg2 Entity
}

public void SCP_OnLoad()
{
    Intercom = new IntercomController();
}

public void SCP_OnUnload()
{
    gamemode.timer.RemoveByName("AdvancedVoice");
    Intercom.Dispose();
}

public void SCP_OnRoundStart() {
    ResetListening();

    gamemode.timer.Create("AdvancedVoice", 250, 0, "VoiceLogicHandler");
    gamemode.log.Debug("Voice channels starting update");

    Intercom.DisplayInit();
    Intercom.Ready();
}

public void SCP_OnButtonPressed(Player &ply, int doorid) {
    if (gamemode.plconfig.GetInt("buttonid") == doorid && Intercom.ready && !ply.IsSCP) Intercom.StrartTransmission(ply);
}

public void OnUse(Player &ply, Entity &ent)
{
    ent.SetBool("active", !ent.GetBool("active", true));
    ply.PrintNotify((ent.GetBool("active")) ? "Рация включена" : "Рация выключена");
}

public void TransmissionStop()
{
    Intercom.EndTransmission();
}

public void VoiceRelease()
{
    Intercom.CooldownRelease();
}

public void DisplayCounterUpdate() {
    char time[32], time2[10];
    float cursec = Intercom.relcdtime - GetGameTime();
    int min = RoundToNearest(cursec) / 60;
    int sec = RoundFloat(cursec) % 60;
    FloatToString(FloatFraction(cursec), time2, sizeof(time2));
    Format(time, sizeof(time), (min < 10 ) ? ((sec < 10 ) ? "0%i:0%i" : "0%i:%i") : ((sec < 10 ) ? "%i:0%i" : "%i:%i"),  RoundToNearest(cursec) / 60, RoundFloat(cursec) % 60);
    Intercom.UpdateText(time);
}

public void ResetListening()
{
    ArrayList players = player.GetAll();
    Player firstply;
    Player secondply;

    for (int i=0; i < players.Length; i++)
    {
        firstply = players.Get(i);
        for (int k=0; k < players.Length; k++)
        {
            secondply = players.Get(k);
            if (firstply != secondply)
                firstply.SetListen(secondply, false);
        }
    }

    delete players;

    gamemode.log.Debug("Channels reseted...");
}

public void VoiceLogicHandler()
{
    ArrayList players = player.GetAll();
    Player listener;
    Player speaker;

    for (int i=0; i < players.Length; i++)
    {
        listener = players.Get(i);

        for (int k=0; k < players.Length; k++)
        {
            speaker = players.Get(k);

            if ((Intercom.IsBroadcast && Intercom.curply == speaker) || (speaker.IsSCP && listener.IsSCP) || ((speaker.inv.Have("radio") && speaker.inv.GetByClass("radio").GetBool("active", true)) && listener.inv.Have("radio")) || (!speaker.IsAlive() && !listener.IsAlive())) // SCP can hear other SCP players with no range limits
            {
                if (!listener.GetListen(speaker))
                    listener.SetListen(speaker, true);
            }
            else
            {
                if (listener.GetListen(speaker))
                    listener.SetListen(speaker, false);
            }
        }

        //if (!listener.IsAlive()) continue;

        float distance = float(gamemode.plconfig.GetInt("distance", 500));

        char filter[1][32] = {"player"};
        ArrayList entities = ents.FindInBox(listener.GetPos() - new Vector(distance, distance, distance), listener.GetPos() + new Vector(distance, distance, distance), filter, sizeof(filter));

        for (int k=0; k < entities.Length; k++)
        {
            speaker = entities.Get(k);

            if (!listener.IsAlive() || (!speaker.IsSCP) && (speaker.IsAlive()))
            {
                if (!listener.GetListen(speaker))
                    listener.SetListen(speaker, true);
            }
        }

        delete entities;
    }

    delete players;
}