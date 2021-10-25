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
/*----------------------------------------------------Configuration-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

#include "scp/classes/json/doors"
#include "scp/classes/json/nuke"

methodmap Config < JSON_OBJECT {
    
    public Config(char[] mapname) {
        Config self = view_as<Config>(ReadConfig(mapname, "config"));

        self.SetObject("Doors", view_as<Doors>(ReadConfig(mapname, "doors")));
        self.SetObject("SpawnMap", view_as<JSON_OBJECT>(ReadConfig(mapname, "spawnlist")));
        self.SetObject("Regions", view_as<JSON_ARRAY>(ReadConfig(mapname, "regions")));

        return self;
    }

    public void Add(char[] name, any value) {
        this.SetValue(name, value);
    }

    public Base GetBase(char[] name) { return view_as<Base>(this.GetObject(name)); }
    
    public void DefaultGlobalClass(char[] team, int max_size) {
        this.GetString("DefaultGlobalClass", team, max_size);
    }

    public void DefaultClass(char[] class, int max_size) {
        this.GetString("DefaultClass", class, max_size);
    }

    property Doors doors {
        public get() { return view_as<Doors>(this.GetObject("Doors")); }
    }

    property JSON_OBJECT spawnmap {
        public get() { return view_as<JSON_OBJECT>(this.GetObject("SpawnMap")); }
    }

    property JSON_ARRAY regions {
        public get() { return view_as<JSON_ARRAY>(this.GetObject("Regions")); }
    }
    
    property JSON_ARRAY meta {
        public get() { return this.GetArray("meta"); }
    }

    property int invsize {
        public get() { return this.GetInt("invsize") - 1; } //Index started at 0
    }

    property int tsto {
        public get() { return this.GetInt("tsto"); }
    }

    property int psars {
        public get() { return this.GetInt("psars"); }
    }

    property JSON_OBJECT reinforce {
        public get() { return this.GetObject("reinforce"); }
    }

    property NukeConfig nuke {
        public get() { NukeConfig nuke; return this.GetValue("nuke", nuke) ? nuke : null; }
    }
    
    property Vector AdminRoom {
        public get() {
            return this.GetVector("AdminRoom");
        }
    }

    property bool usablecards {
        public get() { return this.GetBool("usablecards"); }
    }

    property bool debug {
        public get() { return this.GetBool("debug"); }
    }

    property bool ff {
        public get() { return this.GetBool("ff"); }
    }
}