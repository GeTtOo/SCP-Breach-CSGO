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
/*--------------------------------------------------------Doors---------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

methodmap Door < JSON_OBJECT {
    
    public Door(Door door) {
        return view_as<Door>(door);
    }
    
    property int id {
        public get() { return this.GetInt("id"); }
    }

    property int access {
        public get() { return this.GetInt("access"); }
    }

    property bool scp {
        public get() { return this.GetBool("scp"); }
    }
}

methodmap Doors < JSON_OBJECT {
    public StringMapSnapshot GetAll() { return this.Snapshot(); }

    public Door Get(char[] id) { return view_as<Door>(this.GetObject(id)); }
}