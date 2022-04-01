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
/*---------------------------------------------------Player inventory---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap InvItem < Entity {

    property bool disabled {
        public set(bool value) { this.SetBool("disabled", value); }
        public get() { return this.GetBool("disabled"); }
    }

    property float cdr {
        public set(float value) { this.SetFloat("cdr", value); }
        public get() { return this.GetFloat("cdr"); }
    }

    public Tmr CooldownStart(int time, char[] funcname, any arg) {
        this.cdr = GetGameTime() + float(time / 1000);
        return this.TimerSimple(time, funcname, arg);
    }
}

methodmap Inventory < Base {

    public Inventory(Base ply) {
        Inventory inv = view_as<Inventory>(new Base());
        inv.SetValue("ply", ply);
        inv.CreateArrayList("inventory");
        return inv;
    }

    property Base ply {
        public get() { Base val; return this.GetValue("ply", val) ? val : null; }
    }

    property ArrayList items {
        public set(ArrayList val) { this.SetArrayList("inventory", val); }
        public get() { return this.GetArrayList("inventory"); }
    }

    public bool Have(char[] entclass) {
        ArrayList list = this.GetArrayList("inventory");

        for (int i=0; i < list.Length; i++) {
            char itemname[32];
            Entity item = list.Get(i);
            item.GetClass(itemname, sizeof(itemname));

            if (StrEqual(itemname, entclass))
                return true;
        }

        return false;
    }

    public bool Check(char[] val, int check) {
        ArrayList list = this.GetArrayList("inventory");

        for (int i=0; i < list.Length; i++) {
            char itemname[32];
            Entity item = list.Get(i);
            item.GetClass(itemname, sizeof(itemname));

            if (item.meta != null) {
                ArrayList checklist = item.meta.GetArrayList(val);

                if (checklist != null) {
                    for (int k=0; k < checklist.Length; k++)
                        if (checklist.Get(k) == check)
                            return true;
                }
                else
                {
                    if (item.meta.GetInt(val) == check)
                        return true;
                }
            }
        }

        return false;
    }

    public bool Give(char[] entclass) {
        ArrayList inv = this.GetArrayList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
            EntityMeta entdata = gamemode.meta.GetEntity(entclass);
    
            if (entdata != null)
            {
                Entity ent = new Entity();
                ent.meta = entdata;
                ent.spawned = false;
                ent.SetString("class", entclass);

                any data[2];
                data[0] = ent.id;
                data[1] = ent;
                
                gamemode.GetBase("entities").GetArrayList("entities").PushArray(data);
                inv.Push(ent);

                return true;
            }
            else
            {
                gamemode.log.Warning("Can't give item: %s to the player id: %i. (Not registered in entities list)", entclass, this.ply.GetInt("id"));
            }
        }
        else
        {
            gamemode.log.Info("Can't give item: %s to the player id: %i. (Not enough space in inventory)", entclass, this.ply.GetInt("id"));
        }
        
        return false;
    }

    public bool Pickup(Entity item) {
        ArrayList inv = this.GetArrayList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
            inv.Push(item);
            return true;
        }
        
        return false;
    }

    public bool Remove(char[] entname) {
        ArrayList inv = this.GetArrayList("inventory");
        char entclass[32];

        for (int i=0; i < inv.Length; i++) {
            Entity item = inv.Get(i);
            item.GetClass(entclass, sizeof(entclass));
            delete item;

            if (StrEqual(entclass, entname)) {
                inv.Erase(i);
                return true;
            }
        }
        
        return false;
    }

    public InvItem Get(int index = 0) {
        return this.GetArrayList("inventory").Get(index);
    }

    public InvItem Drop(int index = 0) {
        ArrayList inv = this.GetArrayList("inventory");

        InvItem itm = inv.Get(index);
        inv.Erase(index);

        return itm;
    }

    public void Clear() {
        this.GetArrayList("inventory").Clear();
    }

    public native void FullClear();
}