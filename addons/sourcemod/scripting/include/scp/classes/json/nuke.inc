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

#pragma semicolon 1
#pragma newdecls required

#include <json>

methodmap NukeButtons < JSON_OBJECT {

    property int ready {
        public get() { return this.GetInt("ready"); }
    }

    property int active {
        public get() { return this.GetInt("active"); }
    }

    property int cancel {
        public get() { return this.GetInt("cancel"); }
    }
}

methodmap NukeConfig < JSON_OBJECT {

    property int time {
        public get() { return this.GetInt("time"); }
    }

    property int ast {
        public get() { return this.GetInt("ast", 900); }
    }

    property bool autostart {
        public get() { return this.GetBool("autostart", false); }
    }

    property bool nukefullreset {
        public get() { return this.GetBool("nukefullreset"); }
    }

    property float killpos {
        public get() { return float(this.GetInt("killpos")); }
    }

    property NukeButtons buttons {
        public get() { return view_as<NukeButtons>(this.GetObject("buttons")); }
    }

    property JSON_ARRAY doors {
        public get() { return view_as<JSON_ARRAY>(this.GetObject("doors")); }
    }

    property JSON_ARRAY spawnlist {
        public get() { return view_as<JSON_ARRAY>(this.GetObject("spawnlist")); }
    }

    public void sound(char[] buffer, int max_size) {
        this.GetString("sound", buffer, max_size);
    }

    public void cancelsound(char[] buffer, int max_size) {
        this.GetString("cancelsound", buffer, max_size);
    }
}