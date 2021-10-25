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
/*-------------------------------------------------World Text controller------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

enum TypeWorldText {
    All = 0,
    Text,
    NukeMonitor
}

methodmap WorldText < Entity {
    
    public WorldText(int id) {
        WorldText self = view_as<WorldText>(new Entity(id));
        return self;
    }

    property TypeWorldText type {
        public set(TypeWorldText val) { this.SetInt("type", view_as<int>(val)); }
        public get() { return view_as<TypeWorldText>(this.GetInt("type", 0)); }
    }
    
    public WorldText SetText(char[] text) {
        this.SetPropString("m_szText", text);
        return this;
    }
    
    public WorldText SetSize(any size) {
        this.SetPropFloat("m_flTextSize", size);
        return this;
    }

    public WorldText SetColor(Colour col) {
        this.SetProp("m_textColor", (col.b << 16) + (col.g << 8) + col.r);
        delete col;
        return this;
    }
}

methodmap WorldTextSingleton < Base {
    
    public WorldTextSingleton() {
        WorldTextSingleton self = view_as<WorldTextSingleton>(new Base());
        self.CreateArrayList("wtlist");
        return self;
    }
    
    property ArrayList list {
        public get() { return this.GetArrayList("wtlist"); }
    }

    public WorldText Create(Vector vec, Angle ang, TypeWorldText type = Text) {
        WorldText wt = new WorldText(CreateEntityByName("point_worldtext"));

        wt.type = type;
        wt.SetPos(vec, ang);

        this.list.Push(wt);

        return wt;
    }

    public WorldText Get(int id) {
        return this.list.Get(id);
    }

    public ArrayList GetAll(TypeWorldText type = Text) {
        ArrayList list = this.list;
        ArrayList sortedlist = new ArrayList();

        for (int i=0; i < list.Length; i++) {
            WorldText wt = list.Get(i);

            if (wt != null && (wt.type == type || type == All))
                sortedlist.Push(wt);
        }

        return sortedlist;
    }

    public void Clear() {
        this.list.Clear();
    }
}