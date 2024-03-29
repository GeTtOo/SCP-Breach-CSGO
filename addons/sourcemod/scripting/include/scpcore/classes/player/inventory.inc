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

    property Tmr tmr {
        public set(Tmr value) { this.SetHandle("timer", value); }
        public get() {
            if (!timer.IsAlive(view_as<Tmr>(this.GetHandle("timer")))) this.RemoveValue("timer");
            return view_as<Tmr>(this.GetHandle("timer"));
        }
    }

    public Tmr CooldownStart(int time, char[] funcname = "", any args = false) {
        char itemclass[64], timername[64];
        this.GetClass(itemclass, sizeof(itemclass));
        FormatEx(timername, sizeof(timername), "%s-cd|time-%i|seed-%i", itemclass, GetTime(), GetRandomInt(1, 1000000));
        this.tmr = timer.Create(timername, time, 1, funcname, args);
        return this.tmr;
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

    property ArrayList list {
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

    public native Entity Give(char[] entclass);

    public bool Pickup(Entity item) {
        ArrayList inv = this.GetArrayList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
            inv.Push(item);
            return true;
        }
        
        return false;
    }

    public native bool Remove(InvItem item);

    public InvItem Get(int index = 0) {
        if (this.GetArrayList("inventory").Length > index)
            return this.GetArrayList("inventory").Get(index);
        
        return null;
    }

    public InvItem GetByClass(char[] classname) {
        ArrayList list = this.GetArrayList("inventory");

        for (int i=0; i < list.Length; i++) {
            char itemname[32];
            Entity item = list.Get(i);
            item.GetClass(itemname, sizeof(itemname));

            if (StrEqual(itemname, classname))
                return view_as<InvItem>(item);
        }

        return null;
    }

    public native InvItem Drop(InvItem item);

    public native InvItem DropByIdx(int index = 0);

    public native void DropAll();

    public void Clear() {
        this.GetArrayList("inventory").Clear();
    }

    public native void FullClear();

    public void Dispose()
    {
        this.FullClear();
        delete this.GetArrayList("inventory");
        delete this;
    }
}