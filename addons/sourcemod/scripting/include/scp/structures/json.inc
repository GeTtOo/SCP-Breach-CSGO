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

methodmap JSON_ARRAY < JSON_Array {

    public JSON_ARRAY() { return view_as<JSON_ARRAY>(CreateTrie()); }
    
    property JSON_Array Super {
        public get() { return view_as<JSON_Array>(this); }
    }
    
    public Vector GetVector(int index) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(index));
        if (pos == null || pos.Length == 0) return null;
        return new Vector(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Angle GetAngle(int index) {
        JSON_ARRAY ang = view_as<JSON_ARRAY>(this.GetObject(index));
        if (ang == null || ang.Length == 0) return null;
        return new Angle(float(ang.GetInt(0)),float(ang.GetInt(1)),float(ang.GetInt(2)));
    }

    public Colour GetColor(int index) {
        JSON_ARRAY col = view_as<JSON_ARRAY>(this.GetObject(index));
        if (col == null || col.Length == 0) return null;
        return new Colour(col.GetInt(0),col.GetInt(1),col.GetInt(2));
    }

    public JSON_ARRAY GetArray(int index) { return view_as<JSON_ARRAY>(this.Super.GetObject(index)); }
}

methodmap JSON_OBJECT < JSON_Object {

    public JSON_OBJECT() { return view_as<JSON_OBJECT>(CreateTrie()); }

    property JSON_Object Super {
        public get() { return view_as<JSON_Object>(this); }
    }

    public Vector GetVector(char[] name) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(name));
        if (pos == null || pos.Length == 0) return null;
        return new Vector(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Angle GetAngle(char[] name) {
        JSON_ARRAY ang = view_as<JSON_ARRAY>(this.GetObject(name));
        if (ang == null || ang.Length == 0) return null;
        return new Angle(float(ang.GetInt(0)),float(ang.GetInt(1)),float(ang.GetInt(2)));
    }

    public Colour GetColor(char[] name) {
        JSON_ARRAY col = view_as<JSON_ARRAY>(this.GetObject(name));
        if (col == null || col.Length == 0) return null;
        return new Colour(col.GetInt(0),col.GetInt(1),col.GetInt(2));
    }

    public JSON_OBJECT GetObject(char[] name) { return view_as<JSON_OBJECT>(this.Super.GetObject(name)); }
    public JSON_ARRAY GetArray(char[] name) { return view_as<JSON_ARRAY>(this.Super.GetObject(name)); }
}