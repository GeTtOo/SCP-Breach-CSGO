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

methodmap Utils < StringMap {
    
    public static JSON_OBJECT JSONRead(char[] path)
    {
        BuildPath(Path_SM, path, PLATFORM_MAX_PATH, path);
        
        if (FileExists(path))
        {
            char buffer[65536];
            Handle file = OpenFile(path,"r+");
            ReadFileString(file, buffer, sizeof(buffer));
            delete file;

            return view_as<JSON_OBJECT>(json_decode(buffer));
        }
        else
            return new JSON_OBJECT();
    }
    
    public static void JSONWrite(char[] path, JSON_OBJECT json)
    {
        BuildPath(Path_SM, path, PLATFORM_MAX_PATH, path);

        char buffer[65536];
        Handle file = OpenFile(path,"w+");
        json_encode(json, buffer, sizeof(buffer));
        WriteFileString(file, buffer, true);
        delete file;
    }

    public static JSON_OBJECT ReadMapConfig(char[] mapname, char[] filename)
    {
        char path[256];
        Format(path, sizeof(path),"configs/scp/%s/%s.json", mapname, filename);
        return Utils.JSONRead(path);
    }

    public static JSON_OBJECT ReadCurMapConfig(char[] filename)
    {
        char mapname[128];
        GetCurrentMap(mapname, sizeof(mapname));
        return Utils.ReadMapConfig(mapname, filename);
    }
}