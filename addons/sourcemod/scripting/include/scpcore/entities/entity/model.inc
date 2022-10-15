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
/*-----------------------------------------------------Model class------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#pragma semicolon 1
#pragma newdecls required

#include <sdktools>

methodmap Mdl < Base {

    public Mdl(int entid, char[] path = "") {
        Mdl self = view_as<Mdl>(new Base());
        self.SetInt("entid", entid);
        self.SetString("path", path);

        return self;
    }

    property int entid {
        public set(int id) { this.SetInt("entid", id); }
        public get() { return this.GetInt("entid"); }
    }

    property float scale {
        public set(float val) { SetEntPropFloat(this.entid, Prop_Send, "m_flModelScale", val); }
        public get() { return GetEntPropFloat(this.entid, Prop_Send, "m_flModelScale"); }
    }

    property ModelMeta mdlmeta {
        public set(ModelMeta value) { this.SetValue("mdlmeta", value); }
        public get() { ModelMeta meta; return this.GetValue("mdlmeta", meta) ? meta : null; }
    }

    property ArrayList bglist {
        public set(ArrayList list) { ArrayList oldlist = this.GetArrayList("bglist"); if (oldlist) delete oldlist; this.SetArrayList("bglist", list); }
        public get() { return this.GetArrayList("bglist"); }
    }

    public Mdl SetPath(char[] modelName) { SetEntityModel(this.entid, modelName); return this; }

    public Mdl GetPath(char[] modelName, int max_size) { GetEntPropString(this.entid, Prop_Data, "m_ModelName", modelName, max_size); return this; }

    public void SetById(char[] modelid) {
        this.mdlmeta = gamemode.meta.GetModel(modelid);

        char path[256];
        this.mdlmeta.Path(path, sizeof(path));
        this.SetPath(path);

        ArrayList list = new ArrayList();
        for (int i=0; i < this.mdlmeta.bg.Length; i++)
            list.Push(0);
        this.bglist = list;
    }

    public void SetBodyGroup(char[] name, int idx) {
        this.bglist.Set(this.mdlmeta.GetBgId(name), this.mdlmeta.GetBgIdx(name, idx));

        int bgmath = 0;
        for (int i=0; i < this.bglist.Length; i++)
            bgmath += this.bglist.Get(i);

        SetEntProp(this.entid, Prop_Send, "m_nBody", bgmath);
    }

    public void SetSkin(int skin) { SetEntProp(this.entid, Prop_Send, "m_nSkin", skin); }

    public int GetSkin() { return GetEntProp(this.entid, Prop_Send, "m_nSkin"); }

    public Mdl SetRenderMode(RenderMode mode) { SetEntityRenderMode(this.entid, mode); return this; }

    public Mdl SetRenderColor(Colour clr) { SetEntityRenderColor(this.entid, clr.r, clr.g, clr.b, clr.a); delete clr; return this; }

    public void Dispose() {
        delete this.GetArrayList("bglist");
        delete this;
    }
}