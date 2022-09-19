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

enum InventoryEvents {
    ON_TOUCH = 0,
    ON_PICKUP,
    ON_USE,
    ON_DROP
}

enum StatusEffectEvents {
    INIT = 0,
    UPDATE,
    END
}

methodmap OnUseMeta < Base {

    public OnUseMeta() {
        return view_as<OnUseMeta>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap OnPickupMeta < Base {

    public OnPickupMeta() {
        return view_as<OnPickupMeta>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap OnTouchMeta < Base {

    public OnTouchMeta() {
        return view_as<OnTouchMeta>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap OnDropMeta < Base {

    public OnDropMeta() {
        return view_as<OnDropMeta>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap ItemSoundMeta < Base {

    public ItemSoundMeta() {
        return view_as<ItemSoundMeta>(new Base());
    }

    public bool touch(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("touch", buffer) : this.GetString("touch", buffer, max_size); }

    public bool pickup(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("pickup", buffer) : this.GetString("pickup", buffer, max_size); }

    public bool use(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("use", buffer) : this.GetString("use", buffer, max_size); }

    public bool drop(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("drop", buffer) : this.GetString("drop", buffer, max_size); }
}

methodmap ItemMenuMeta < Base {

    public ItemMenuMeta() {
        return view_as<ItemMenuMeta>(new Base());
    }

    public bool use(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("use", buffer) : this.GetString("use", buffer, max_size); }

    public bool cd(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("cd", buffer) : this.GetString("cd", buffer, max_size); }

    public bool info(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("info", buffer) : this.GetString("info", buffer, max_size); }

    public bool drop(char[] buffer, int max_size = 0) { return (max_size == 0) ? this.SetString("drop", buffer) : this.GetString("drop", buffer, max_size); }
}

methodmap EntityMeta < Base {

    public EntityMeta() {
        EntityMeta meta = view_as<EntityMeta>(new Base());
        return meta;
    }
    
    property float mass {
        public set(float val) { this.SetFloat("mass", val); }
        public get() { return this.GetFloat("mass"); }
    }

    property int bodygroup {
        public set(int val) { this.SetInt("bodygroup", val); }
        public get() { return this.GetInt("bodygroup"); }
    }

    property int skin {
        public set(int val) { this.SetInt("skin", val); }
        public get() { return this.GetInt("skin"); }
    }

    property int spawnflags {
        public set(int val) { this.SetInt("spawnflags", val); }
        public get() { return this.GetInt("spawnflags", 4356); }
    }

    property bool usable {
        public set(bool val) { this.SetBool("usable", val); }
        public get() { return this.GetBool("usable"); }
    }

    property bool SCPCanUse {
        public set(bool val) { this.SetBool("scpcanuse", val); }
        public get() { return this.GetBool("scpcanuse"); }
    }

    property int cd {
        public set(int val) { this.SetInt("cooldown", val); }
        public get() { return this.GetInt("cooldown"); }
    }

    property OnTouchMeta ontouch {
        public set(OnTouchMeta val) { this.SetValue("ontouch", val); }
        public get() { OnTouchMeta val; return this.GetValue("ontouch", val) ? val : null; }
    }

    property OnPickupMeta onpickup {
        public set(OnPickupMeta val) { this.SetValue("onpickup", val); }
        public get() { OnPickupMeta val; return this.GetValue("onpickup", val) ? val : null; }
    }

    property OnUseMeta onuse {
        public set(OnUseMeta val) { this.SetValue("onuse", val); }
        public get() { OnUseMeta val; return this.GetValue("onuse", val) ? val : null; }
    }

    property OnDropMeta ondrop {
        public set(OnDropMeta val) { this.SetValue("ondrop", val); }
        public get() { OnDropMeta val; return this.GetValue("ondrop", val) ? val : null; }
    }

    property ItemSoundMeta sound {
        public set(ItemSoundMeta val) { this.SetValue("sound", val); }
        public get() { ItemSoundMeta val; return this.GetValue("sound", val) ? val : null; }
    }

    property ItemMenuMeta menu {
        public set(ItemMenuMeta val) { this.SetValue("menu", val); }
        public get() { ItemMenuMeta val; return this.GetValue("menu", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }

    public void model(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("model", buffer) : this.GetString("model", buffer, max_size); }
    
    public void Dispose() {
        if (this.ontouch) delete this.ontouch.hndl;
        delete this.ontouch;
        if (this.onpickup) delete this.onpickup.hndl;
        delete this.onpickup;
        if (this.onuse) delete this.onuse.hndl;
        delete this.onuse;
        if (this.ondrop) delete this.ondrop.hndl;
        delete this.ondrop;
        delete this.sound;
        delete this.menu;
        delete this;
    }
}

methodmap ModelMeta < Base {

    public ModelMeta() {
        return view_as<ModelMeta>(new Base());
    }
    
    property ArrayList bg {
        public get() { return this.GetArrayList("bg"); }
    }

    public void Path(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("path", buffer) : this.GetString("path", buffer, max_size); }

    public int GetBgIdx(char[] name, int idx) {
        int bgidx = 1;
        char bgnames[11][12] = {"base","head","eyes","helmet","mask",/*"flag",*/"rank","body","belt","legs","pl","pr"};

        for (int i=0; i < this.bg.Length; i++)
        {
            if (StrEqual(bgnames[i], name)) break;
            
            int id = this.bg.Get(i);
            
            if (id == 0) continue;

            bgidx += bgidx * id;
            
            if (StrEqual(bgnames[i], name))
            {
                break;
            }
        }

        return bgidx * idx;
    }

    public int GetBgId(char[] name) {
        int id = 0;
        char bgnames[11][12] = {"base","head","eyes","helmet","mask",/*"flag",*/"rank","body","belt","legs","pl","pr"};
        for (int i=0; i < this.bg.Length; i++) {
            if (StrEqual(bgnames[i], name)) break;
            id++;
        }

        return id;
    }

    public void Dispose() {
        delete this.bg;
        delete this;
    }
}

methodmap SEInit < Base {

    public SEInit() {
        return view_as<SEInit>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap SEUpdate < Base {

    public SEUpdate() {
        return view_as<SEUpdate>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap SEEnd < Base {

    public SEEnd() {
        return view_as<SEEnd>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap StatusEffectMeta < Base {

    public StatusEffectMeta(char[] name, float rate) {
        StatusEffectMeta sem = view_as<StatusEffectMeta>(new Base());
        sem.SetString("name", name);
        sem.SetFloat("rate", rate);
        
        return sem;
    }

    property float rate {
        public set(float val) { this.SetFloat("rate", val); }
        public get() { return this.GetFloat("rate"); }
    }

    property SEInit init {
        public set(SEInit val) { this.SetValue("init", val); }
        public get() { SEInit val; return this.GetValue("init", val) ? val : null; }
    }

    property SEUpdate update {
        public set(SEUpdate val) { this.SetValue("update", val); }
        public get() { SEUpdate val; return this.GetValue("update", val) ? val : null; }
    }

    property SEEnd end {
        public set(SEEnd val) { this.SetValue("end", val); }
        public get() { SEEnd val; return this.GetValue("end", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }

    public void Dispose() {
        if (this.init) delete this.init.hndl;
        delete this.init;
        if (this.update) delete this.update.hndl;
        delete this.update;
        if (this.end) delete this.end.hndl;
        delete this.end;
        delete this;
    }
}

methodmap Meta < Base {

    public Meta() {
        Meta meta = view_as<Meta>(new Base());
        meta.CreateBase("entities");
        meta.CreateBase("models");
        meta.CreateBase("statuseffects");
        return meta;
    }

    public void RegisterEntity(char[] entclass, EntityMeta entmeta) { this.GetBase("entities").SetHandle(entclass, entmeta); }

    public EntityMeta GetEntity(char[] entclass) { return view_as<EntityMeta>(this.GetBase("entities").GetHandle(entclass)); }

    public void RegEntEvent(InventoryEvents eid, char[] entclass, char[] funcname, char[] sound = "") {
        EntityMeta entdata = this.GetEntity(entclass);

        if (strlen(sound) > 0 && !entdata.GetHandle("sound")) entdata.sound = new ItemSoundMeta();
        
        switch (eid) {
            case ON_TOUCH: {
                entdata.ontouch = new OnTouchMeta();

                entdata.ontouch.hndl = GetMyHandle();
                entdata.ontouch.name(funcname);

                if (strlen(sound) > 0) entdata.sound.touch(sound);
            }
            case ON_PICKUP: {
                entdata.onpickup = new OnPickupMeta();

                entdata.onpickup.hndl = GetMyHandle();
                entdata.onpickup.name(funcname);

                if (strlen(sound) > 0) entdata.sound.pickup(sound);
            }
            case ON_USE: {
                entdata.onuse = new OnUseMeta();

                entdata.onuse.hndl = GetMyHandle();
                entdata.onuse.name(funcname);

                if (strlen(sound) > 0) entdata.sound.use(sound);
            }
            case ON_DROP: {
                entdata.ondrop = new OnDropMeta();

                entdata.ondrop.hndl = GetMyHandle();
                entdata.ondrop.name(funcname);

                if (strlen(sound) > 0) entdata.sound.drop(sound);
            }
        }
    }

    public void RegisterModel(char[] modelid, ModelMeta mdlmeta) { this.GetBase("models").SetHandle(modelid, mdlmeta); }

    public ModelMeta GetModel(char[] modelid) { return view_as<ModelMeta>(this.GetBase("models").GetHandle(modelid)); }

    public void RegisterStatusEffect(char[] sen, float rate = 1.0, StatusEffectMeta sem = null) { this.GetBase("statuseffects").SetHandle(sen, (sem) ? sem : new StatusEffectMeta(sen, rate)); }

    public StatusEffectMeta GetStatusEffect(char[] sen) { return view_as<StatusEffectMeta>(this.GetBase("statuseffects").GetHandle(sen)); }

    public void RegStatusEffectEvent(StatusEffectEvents eid, char[] sen, char[] funcname) {
        StatusEffectMeta sem = this.GetStatusEffect(sen);
        
        switch (eid) {
            case INIT: {
                sem.init = new SEInit();

                sem.init.hndl = GetMyHandle();
                sem.init.name(funcname);
            }
            case UPDATE: {
                sem.update = new SEUpdate();

                sem.update.hndl = GetMyHandle();
                sem.update.name(funcname);
            }
            case END: {
                sem.end = new SEEnd();

                sem.end.hndl = GetMyHandle();
                sem.end.name(funcname);
            }
        }
    }

    public void Dispose()
    {
        ArrayList entskeys = this.GetBase("entities").GetKeys();
        ArrayList modelskeys = this.GetBase("models").GetKeys();
        ArrayList sekeys = this.GetBase("statuseffects").GetKeys();

        char keyname[64];
        for (int i=0; i < entskeys.Length; i++)
        {
            entskeys.GetString(i, keyname, sizeof(keyname));
            view_as<EntityMeta>(this.GetBase("entities").GetHandle(keyname)).Dispose();
        }

        for (int i=0; i < modelskeys.Length; i++)
        {
            modelskeys.GetString(i, keyname, sizeof(keyname));
            view_as<EntityMeta>(this.GetBase("models").GetHandle(keyname)).Dispose();
        }

        for (int i=0; i < sekeys.Length; i++)
        {
            sekeys.GetString(i, keyname, sizeof(keyname));
            view_as<EntityMeta>(this.GetBase("statuseffects").GetHandle(keyname)).Dispose();
        }

        delete entskeys;
        delete modelskeys;
        delete sekeys;

        this.GetBase("entities").Dispose();
        this.GetBase("models").Dispose();
        this.GetBase("statuseffects").Dispose();

        delete this;
    }
}