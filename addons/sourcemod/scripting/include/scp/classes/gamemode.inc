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
/*------------------------------------------------------Game Mode-------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include "scp\classes\json\config"
#include "scp\classes\json\class"

methodmap Classes < JSON_OBJECT {
    public Class get(char[] name) { Class class = view_as<Class>(this.GetObject(name)); class.Name(name); return class; }
}

methodmap GTeam < JSON_OBJECT {

    property int percent {
        public get() { return this.GetInt("percent"); }
    }

    property bool priority {
        public get() { return this.GetBool("priority"); }
    }

    property bool reinforce {
        public get() { return this.GetBool("reinforce"); }
    }

    property bool randompick {
        public get() { return this.GetBool("randompick"); }
    }

    property Classes classes {
        public get() { return view_as<Classes>(this.GetObject("classes")); }
    }

    public Class class(char[] name) {
        return this.classes.get(name);
    }

    public StringMapSnapshot GetClassNames() {
        return this.GetObject("classes").Snapshot();
    }

    public ArrayList GetClassList(bool filter = true) {
        ArrayList list = new ArrayList(32);
        StringMapSnapshot snap = this.GetClassNames();
        int keylength;
        for (int i=0; i < snap.Length; i++) {
            keylength = snap.KeyBufferSize(i);
            char[] className = new char[keylength];
            snap.GetKey(i, className, keylength);
            if (json_is_meta_key(className)) continue;
            if (filter && (this.class(className).percent == 0 || !this.class(className).autospawn)) continue;
            list.PushString(className);
        }
        delete snap;
        return list;
    }
    
    public Class GetClassById(int id) {
        ArrayList list = this.GetClassList();

        char className[32];
        list.GetString(id / 2, className, sizeof(className));
        delete list;

        return this.class(className);
    }

    public Class GetRandomClass() {
        ArrayList list = this.GetClassList();

        char className[32];
        list.GetString(GetRandomInt(0, list.Length - 1), className, sizeof(className));
        delete list;

        return this.class(className);
    }
}

methodmap Teams < JSON_OBJECT {
    
    public Teams() {
        return view_as<Teams>(Utils.ReadCurMapConfig("classes"));
    }

    public void Add(char[] name, Class class) {
        this.SetObject(name, class);
    }

    public GTeam get(char[] name) { return view_as<GTeam>(this.GetObject(name)); }
}

methodmap GameMode < Base {
    
    public GameMode() {
        GameMode self = view_as<GameMode>(new JSON_OBJECT());
        self.SetValue("Teams", new Teams());
        self.SetValue("Config", new Config());
        self.SetValue("Meta", new Meta());
        self.SetValue("Timers", new Timers());
        return self;
    }

    property Config config {
        public native get();
    }

    property JSON_OBJECT plconfig {
        public get() {
            char pname[64];
            GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
            if (!StrEqual(pname, "[SCP] GameMode"))
            {
                JSON_OBJECT plconfig = this.config.GetObject(pname);
                if (!plconfig)
                {
                    plconfig = view_as<JSON_OBJECT>(CreateTrie());
                    //this.config.SetObject(pname, view_as<JSON_OBJECT>(CloneHandle(plconfig, FindPluginByFile("scp_core.sp"))));
                    //delete plconfig;
                    this.config.SetObject(pname, view_as<JSON_OBJECT>(plconfig));
                }
                return plconfig;
            }
            return view_as<JSON_OBJECT>(null);
        }
    }

    property Meta meta {
        public native get();
    }

    property Manager mngr {
        public native get();
    }

    property NuclearWarhead nuke {
        public native get();
    }

    property Timers timer {
        public native get();
    }

    property Logger log {
        public native get();
    }

    public native ArrayList GetTeamList(bool filter = true);
    
    property int lang {
        public get() { return GetServerLanguage(); }
    }

    public native GTeam team(char[] name);

    public void Dispose()
    {
        Teams teams;
        this.GetValue("Teams", teams);
        teams.Cleanup();
        delete teams;
        teams = null;
        this.RemoveValue("Teams");
        
        view_as<Config>(this.GetHandle("Config")).Dispose();
        this.RemoveValue("Config");

        view_as<Meta>(this.GetHandle("Meta")).Dispose();
        this.RemoveValue("Meta");

        view_as<Timers>(this.GetHandle("Timers")).Dispose();
        this.RemoveValue("Timers");

        delete this;
    }
}