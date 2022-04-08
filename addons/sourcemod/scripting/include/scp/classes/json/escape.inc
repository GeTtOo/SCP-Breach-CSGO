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

methodmap EscapeInfo < JSON_OBJECT {

    property int trigger {
        public set(int val) { this.SetInt("trigger", val); }
        public get() { return this.GetInt("trigger"); }
    }

    property bool savepos {
        public set(bool val) { this.SetBool("savepos", val); }
        public get() { return this.GetBool("savepos"); }
    }

    public void team(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("team", buffer) : this.GetString("team", buffer, max_size); }

    public void class(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("class", buffer) : this.GetString("class", buffer, max_size); }

    public void wp(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("wp", buffer) : this.GetString("wp", buffer, max_size); }
}