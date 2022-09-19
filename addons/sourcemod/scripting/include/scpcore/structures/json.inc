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

#include <json>

enum JSON_TYPE {
    Invalid = -1,
    String = 0,
    Int,
    Int64,
    Float,
    Bool,
    Object
}

methodmap JSON_ARRAY < JSON_Array {

    public JSON_ARRAY() { return view_as<JSON_ARRAY>(new JSON_Object()); }

    public JSON_TYPE GetType(int index) {
        return view_as<JSON_TYPE>(this.GetKeyType(index));
    }

    public Vector GetVector(int index) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.Get(index));
        if (pos == null || pos.Length == 0) return null;
        return new Vector(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Angle GetAngle(int index) {
        JSON_ARRAY ang = view_as<JSON_ARRAY>(this.Get(index));
        if (ang == null || ang.Length == 0) return null;
        return new Angle(float(ang.GetInt(0)),float(ang.GetInt(1)),float(ang.GetInt(2)));
    }

    public Colour GetColor(int index) {
        JSON_ARRAY col = view_as<JSON_ARRAY>(this.Get(index));
        if (col == null || col.Length == 0) return null;
        return new Colour(col.GetInt(0),col.GetInt(1),col.GetInt(2));
    }

    public JSON_Object Get(int index) { return view_as<JSON_Object>(this.GetObject(index)); } 
    public JSON_ARRAY GetArr(int index) { return view_as<JSON_ARRAY>(this.GetObject(index)); }
    
    public void Dispose() {
        this.Cleanup();
    }
}

methodmap JSON_OBJECT < JSON_Object {

    public JSON_OBJECT() { return view_as<JSON_OBJECT>(new JSON_Object()); }

    public JSON_TYPE GetType(char[] name) {
        return view_as<JSON_TYPE>(this.GetKeyType(name));
    }

    public Vector GetVector(char[] name) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.Get(name));
        if (pos == null || pos.Length == 0) return null;
        return new Vector(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Angle GetAngle(char[] name) {
        JSON_ARRAY ang = view_as<JSON_ARRAY>(this.Get(name));
        if (ang == null || ang.Length == 0) return null;
        return new Angle(float(ang.GetInt(0)),float(ang.GetInt(1)),float(ang.GetInt(2)));
    }

    public Colour GetColor(char[] name) {
        JSON_ARRAY col = view_as<JSON_ARRAY>(this.Get(name));
        if (col == null || col.Length == 0) return null;
        return new Colour(col.GetInt(0),col.GetInt(1),col.GetInt(2));
    }

    public JSON_OBJECT Get(char[] name) { return view_as<JSON_OBJECT>(this.GetObject(name)); }
    public JSON_ARRAY GetArr(char[] name) { return view_as<JSON_ARRAY>(this.GetObject(name)); }

    public void Dispose() {
        this.Cleanup();
    }
}