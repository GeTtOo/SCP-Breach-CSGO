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
/*-----------------------------------------------------Entity class-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#pragma semicolon 1
#pragma newdecls required

#include <sdktools>

methodmap StatusEffectList < Base {

    public StatusEffectList(Base ply) {
        StatusEffectList sel = view_as<StatusEffectList>(new Base());
        sel.SetBase("ply", ply);

        return sel;
    }

    property Base ply {
        public get() { return this.GetBase("ply"); }
    }

    property ArrayList list {
        public get() { return this.GetArrayList("selist"); }
    }

    public void Create(char[] name, int time)
    {
        statuseffect.Create(this.ply, name, time);
    }

    public bool Remove(char[] name)
    {
        statuseffect.Remove(this.ply, name);
    }
    
    public void ClearAll()
    {
        statuseffect.ClearAllOnPlayer(this.ply);
    }

    public void Dispose()
    {
        delete this;
    }
}