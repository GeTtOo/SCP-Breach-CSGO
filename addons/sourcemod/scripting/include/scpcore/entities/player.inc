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
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
#pragma semicolon 1
#pragma newdecls required

#include <sdktools>
#include <cstrike>

#include "scp/classes/player/store"
#include "scp/classes/player/inventory"
#include "scp/classes/player/progress"
#include "scp/classes/player/statuseffect"

methodmap Player < Entity {

    public Player(int id) {
        Player ply = view_as<Player>(new Entity(id));
        ply.SetBase("storage", new PlayerStorage(ply));
        ply.SetBase("inventory", new Inventory(ply));
        ply.SetBase("progress", new ProgressBar(ply));
        ply.SetBase("sel", new StatusEffectList(ply));
        ply.SetBool("spawned", false);

        return ply;
    }

    property int lang {
        public get() { return GetClientLanguage(this.id); }
    }

    property int health {
        public set(int val) { SetEntityHealth(this.id, val); }
        public get() { return GetClientHealth(this.id); }
    }

    property int armor {
        public set(int val) { this.SetProp("m_ArmorValue", val); }
        public get() { return this.GetProp("m_ArmorValue"); }
    }

    property float speed {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed"); }
    }

    property float multipler {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue"); }
    }

    property Class class {
        public set(Class val) { this.SetValue("class", val); }
        public get() { Class val; return this.GetValue("class", val) ? val : null; }
    }

    property bool IsSCP {
        public get() { char team[32]; this.GetString("team", team, sizeof(team)); return StrEqual(team, "SCP"); }
    }

    property bool fullaccess {
        public set(bool card) { this.SetBool("fullaccess", card); }
        public get() { return this.GetBool("fullaccess", false); }
    }

    property bool ready {
        public set(bool val) { this.SetBool("ready", val); }
        public get() { return this.GetBool("ready", true); }
    }

    property bool FirstSpawn {
        public set(bool val) { this.SetBool("firstspawn", val); }
        public get() { return this.GetBool("firstspawn", true); }
    }

    property Entity ragdoll {
        public set(Entity val) { this.RemoveValue("ragdoll"); this.SetValue("ragdoll", val); }
        public get() { Entity val; return this.GetValue("ragdoll", val) ? val : null; }
    }

    property PlayerStorage store {
        public set(PlayerStorage store) { this.SetHandle("storage", store); }
        public get() { return view_as<PlayerStorage>(this.GetHandle("storage")); }
    }

    property Inventory inv {
        public set(Inventory inv) { this.SetHandle("inventory", inv); }
        public get() { return view_as<Inventory>(this.GetHandle("inventory")); }
    }

    property ProgressBar progress {
        public set(ProgressBar pb) { this.SetHandle("progress", pb); }
        public get() { return view_as<ProgressBar>(this.GetHandle("progress")); }
    }

    property StatusEffectList se {
        public set(StatusEffectList val) { this.SetHandle("sel", val); }
        public get() { return view_as<StatusEffectList>(this.GetHandle("sel")); }
    }

    public void GetLangInfo(char[] name, int max_size) { GetLanguageInfo(this.lang, name, max_size); }

    public bool GetName(char[] buffer, int max_size = 0) { return GetClientName(this.id, buffer, max_size); }

    public bool GetAuth(char[] buffer, int max_size = 0, AuthIdType type = AuthId_Steam2) { return GetClientAuthId(this.id, type, buffer, max_size, true); }

    public bool IsAlive() { return IsPlayerAlive(this.id); }

    public bool IsAdmin(AdminFlag flag = Admin_Generic) { return GetAdminFlag(GetUserAdmin(this.id), flag); }

    public bool InGame() { return IsClientInGame(this.id); }

    public Vector EyePos() { float vecarr[3]; GetClientEyePosition(this.id, vecarr); return new Vector(vecarr[0], vecarr[1], vecarr[2]); }

    public Angle GetAng() { float angarr[3]; GetClientEyeAngles(this.id, angarr); return new Angle(angarr[0], angarr[1], angarr[2]); }

    public void SetHandsModel(char[] model) { SetEntPropString(this.id, Prop_Send, "m_szArmsModel", model); }

    public void Team(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("team", buffer) : this.GetString("team", buffer, max_size); }

    public void ExecCommand(char[] format, any ...) {
        int len = strlen(format) + 255;
        char[] data = new char[len];
        VFormat(data, len, format, 3);
        ClientCommand(this.id, data);
    }

    public void PrintNotify(const char[] format, any ...) {
        int len = strlen(format) + 8192;
        char[] formattedText = new char[len];
        VFormat(formattedText, len, format, 3);
        PrintHintText(this.id, "%s", formattedText);
    }

    public void PrintWarning(const char[] format, any ...) {
        int len = strlen(format) + 8192;
        char[] formattedText = new char[len];
        VFormat(formattedText, len, format, 3);
        PrintCenterText(this.id, "%s", formattedText);
    }

    public void ShowOverlay(char[] metapath) {
        char path[256], patchcheck[256], langcode[3];
                
        this.GetLangInfo(langcode, sizeof(langcode));
        Utils.TransformPath(metapath, path, langcode);

        FormatEx(patchcheck, sizeof(patchcheck), "materials/%s.vmt", path);
        if (!FileExists(patchcheck, true)) Utils.TransformPath(metapath, path);
        
        char overlay[272];
        Format(overlay, sizeof(overlay), "r_screenoverlay %s", path);
        this.ExecCommand(overlay);
    }

    public void HideOverlay() {
        this.ExecCommand("r_screenoverlay none");
    }

    public void SetListen(Player ply, bool islisten)
    {
        SetListenOverride(this.id, ply.id, islisten ? Listen_Yes : Listen_No);
    }

    public bool GetListen(Player ply)
    {
        return (GetListenOverride(this.id, ply.id) == Listen_Yes) ? true : false;
    }

    public void PlaySound(char[] path, int channel = 1, int level = 100, int entity = 0) {
        if (!this.ready) return;

        int clients[1];
        clients[0] = this.id;

        EmitSound(clients, sizeof(clients), path, (entity == 0) ? this.id : entity, channel, level);
    }

    public void PlayTranslatedSound(char[] metapath)
    {
        char path[256], patchcheck[256], langcode[3];

        this.GetLangInfo(langcode, sizeof(langcode));
        Utils.TransformPath(metapath, path, langcode);

        FormatEx(patchcheck, sizeof(patchcheck), "sound/%s", path);
        if (!FileExists(patchcheck, true)) Utils.TransformPath(metapath, path);

        Format(path, sizeof(path), "*/%s", path);

        this.PlaySound(path, SNDCHAN_ITEM);
    }

    public void PlayNonCheckSound(char[] path) {
        if (!this.ready) return;

        char soundcommand[256];
        FormatEx(soundcommand, sizeof(soundcommand), "playgamesound %s", path);
        this.ExecCommand(soundcommand);
    }

    public void PlayTranslatedNonCheckSound(char[] metapath)
    {
        char path[256], patchcheck[256], langcode[3];

        this.GetLangInfo(langcode, sizeof(langcode));
        Utils.TransformPath(metapath, path, langcode);

        FormatEx(patchcheck, sizeof(patchcheck), "sound/%s", path);
        if (!FileExists(patchcheck, true)) Utils.TransformPath(metapath, path);

        Format(path, sizeof(path), "%s", path);

        this.PlayNonCheckSound(path);
    }

    public void PlayAmbient(char[] path)
    {
        float vecarr[3];
        Vector vec = this.EyePos();
        vec.GetArr(vecarr);
        delete vec;
        EmitAmbientSound(path, vecarr, this.id);
    }

    public void StopSound(char[] path, int channel = 1) {
        StopSound(this.id, channel, path);
    }

    public void GetCurWeapon(char[] name, int max_size = 0) { GetClientWeapon(this.id, name, max_size); }

    public native Entity Give(char[] item);

    public native void DropWeapons();

    public native void RestrictWeapons();

    public bool Check(char[] val, int check) {
        ArrayList checklist = this.GetArrayList(val);
        if (checklist) {
            for (int k=0; k < checklist.Length; k++)
                if (checklist.Get(k) == check)
                    return true;
        }
        else
        {
            if (this.GetInt(val) == check)
                return true;
        }

        return false;
    }
    
    public Entity CreateRagdoll() {
        Entity ragdoll;
        
        ragdoll = this.GetPropEnt("m_hRagdoll");

        if (ragdoll && ragdoll.id != -1)
            ragdoll.Input("Kill");

        ragdoll.Dispose();

        ragdoll = new Entity();

        char model[128];
        this.model.GetPath(model, sizeof(model));

        ragdoll.meta = new EntityMeta();
        ragdoll.meta.model(model);
        ragdoll.meta.bodygroup = this.GetProp("m_nBody");
        ragdoll.meta.skin = this.GetProp("m_nSkin");
        ragdoll.meta.spawnflags = 4;

        ragdoll.Create("prop_ragdoll");
        ragdoll.Spawn();
        ragdoll.SetPos(this.GetPos(), this.GetAng());

        ragdoll.SetBool("IsSCP", this.IsSCP);
        char team[32];
        this.Team(team, sizeof(team));
        ragdoll.SetString("team", team);
        ragdoll.SetHandle("class", this.class);

        return ragdoll;
    }

    //public void SetCollisionGroup(int group) { gamemode.mngr.SetCollisionGroup(this.id, group); }

    public void TakeDamage(Player ply = null, float damage = 25.0, any dmgtype = DMG_GENERIC) {
        SDKHooks_TakeDamage(this.id, (ply) ? ply.id : 0, (ply) ? ply.id : 0, damage, dmgtype);
    }

    public void Kill() {
        this.DropWeapons();
        ForcePlayerSuicide(this.id);
    }

    public void SilenceKill() {
        this.RestrictWeapons();
        this.inv.FullClear();
        this.Kill();
    }

    public void Kick(char[] reason) { KickClient(this.id, reason); }

    public void SetupBaseStats() {
        this.health = this.class.health;
        (this.class.armor != 0)  ? (this.armor = this.class.armor) : (this.armor = 0);
        (this.class.helmet) ? this.SetProp("m_bHasHelmet", 1) : this.SetProp("m_bHasHelmet", 0);
        this.speed = this.class.speed;
        this.multipler = this.class.multipler;
    }

    public native void Setup();

    public native void SetupModel();

    public void Spawn() {
        this.spawned = true;
        if (!this.IsAlive())
            CS_RespawnPlayer(this.id);
    }

    public void UpdateClass() {
        this.Spawn();
        this.RestrictWeapons();
        this.inv.FullClear();
        
        this.SetupBaseStats();
        this.SetupModel();
        this.Setup();
    }

    public void Dispose() {
        this.inv.Dispose();
        this.progress.Dispose();
        this.se.Dispose();
        this.model.Dispose();
        delete this;
    }
}