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

methodmap PlayerStorage < Base {

    public PlayerStorage(Base ply) {
        PlayerStorage storage = view_as<PlayerStorage>(new Base());

        storage.SetHandle("player", ply);
        
        char steamid[32];
        GetClientAuthId(ply.GetInt("id"), AuthId_Steam2, steamid, sizeof(steamid), true);
        storage.SetString("steamid", steamid);
        
        return storage;
    }

    property JSON_OBJECT store {
        public set(JSON_OBJECT data) { this.SetHandle("store", data); }
        public get() { return view_as<JSON_OBJECT>(this.GetHandle("store")); }
    }

    public void LoadOrCreate() {
        char path[256], steamid[32];

        this.GetString("steamid", steamid, sizeof(steamid));
        BuildPath(Path_SM, path, sizeof(path), "data/scp/player/%s.json", steamid);
        
        if (FileExists(path))
        {
            char buffer[65536];
            Handle file = OpenFile(path,"r+");
            ReadFileString(file, buffer, sizeof(buffer));
            delete file;

            this.store = view_as<JSON_OBJECT>(json_decode(buffer));
        }
        else
            this.store = new JSON_OBJECT();
    }

    public void Save() {
        char path[256], steamid[32];

        this.GetString("steamid", steamid, sizeof(steamid));
        BuildPath(Path_SM, path, sizeof(path), "data/scp/player/%s.json", steamid);

        char buffer[65536];
        Handle file = OpenFile(path,"w+");
        json_encode(this.store, buffer, sizeof(buffer));
        WriteFileString(file, buffer, true);
        delete file;
    }

    public bool SetInt(char[] index, int val) { this.store.SetInt(index, val); }
    public bool SetFloat(char[] index, float val) { this.store.SetFloat(index, val); }
    public bool SetBool(char[] index, bool val) { this.store.SetBool(index, val); }

    public int GetInt(char[] index, int defval = 0) { return this.store.GetInt(index, defval); }
    public float GetFloat(char[] index, float defval = 0.0) { return this.store.GetFloat(index, defval); }
    public bool GetBool(char[] index, bool defval = false) { return this.store.GetBool(index, defval); }

    public bool RemoveValue(char[] index) { return this.store.Remove(index); }
}