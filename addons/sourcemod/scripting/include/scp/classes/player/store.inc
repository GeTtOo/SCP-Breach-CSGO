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
/*------------------------------------------------Player storage system-------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <clientprefs>

methodmap PlayerStorage < Base {

    public PlayerStorage(Base ply) {
        PlayerStorage storage = view_as<PlayerStorage>(new Base());

        storage.SetHandle("player", ply);
        
        return storage;
    }

    public bool SetString(char[] index, char[] val)
    {
        char plname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, plname, sizeof(plname));
        Cookie cookie = new Cookie(index, plname, CookieAccess_Protected);
        cookie.Set(this.GetBase("player").GetInt("id"), val);
        delete cookie;
    }
    public bool SetInt(char[] index, int val) { char buf[32]; IntToString(val, buf, sizeof(buf)); this.SetString(index, buf); }
    public bool SetFloat(char[] index, float val) { char buf[32]; FloatToString(val, buf, sizeof(buf)); this.SetString(index, buf); }
    public bool SetBool(char[] index, bool val) { char buf[32]; IntToString(val, buf, sizeof(buf)); this.SetString(index, buf); }

    public bool GetString(char[] index, char[] val, int max_size)
    {
        char plname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, plname, sizeof(plname));
        Cookie cookie = new Cookie(index, plname, CookieAccess_Protected);
        cookie.Get(this.GetBase("player").GetInt("id"), val, max_size);
        delete cookie;
    }
    public int GetInt(char[] index, int defval = 0) { char buf[32]; this.GetString(index, buf, sizeof(buf)); return StringToInt(buf); }
    public float GetFloat(char[] index, float defval = 0.0) { char buf[32]; this.GetString(index, buf, sizeof(buf)); return StringToFloat(buf); }
    public bool GetBool(char[] index, bool defval = false) { char buf[32]; this.GetString(index, buf, sizeof(buf)); return view_as<bool>(StringToInt(buf)); }
}