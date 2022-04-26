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

methodmap List < StringMap {

    public List() { return view_as<List>(CreateTrie()); }

    public bool Set(char[] idx, any value) { return this.SetValue(idx, value); }
    public any Get(char[] idx) { any value; return this.GetValue(idx, value) ? value : false; }

    public ArrayList GetKeys() {
        ArrayList list = new ArrayList(32);
        StringMapSnapshot snap = this.Snapshot();

        int keylength;
        for (int i=0; i < snap.Length; i++) {
            keylength = snap.KeyBufferSize(i);
            char[] keyname = new char[keylength];
            snap.GetKey(i, keyname, keylength);
            list.PushString(keyname);
        }

        delete snap;
        return list;
    }
}

methodmap Base < StringMap {
    
    public Base() { return view_as<Base>(CreateTrie()); }
    
    public bool SetBase(char[] index, Base val) { return this.SetValue(index, val); }
    public bool SetHandle(char[] index, Handle val) { return this.SetValue(index, val); }
    public bool SetInt(char[] index, int val) { return this.SetValue(index, val); }
    public bool SetFloat(char[] index, float val) { return this.SetValue(index, val); }
    public bool SetBool(char[] index, bool val) { return this.SetValue(index, val); }
    public bool SetVector(char[] index, Base val) { return this.SetValue(index, val); }
    public bool SetAngle(char[] index, Base val) { return this.SetValue(index, val); }
    public bool SetList(char[] index, List val) { return this.SetValue(index, val); }
    public bool CreateList(char[] index) { return this.SetValue(index, new List()); }
    public bool SetArrayList(char[] index, ArrayList val) { return this.SetValue(index, val); }
    public bool CreateArrayList(char[] index, int blocksize = 1, int size = 0) { return this.SetValue(index, new ArrayList(blocksize, size)); }

    public Base GetBase(char[] index, Base defval = null) { Base val; return this.GetValue(index, val) ? val : defval; }
    public Handle GetHandle(char[] index, Handle defval = null) { Handle val; return this.GetValue(index, val) ? val : defval; }
    public int GetInt(char[] index, int defval = 0) { int val; return this.GetValue(index, val) ? val : defval; }
    public float GetFloat(char[] index, float defval = 0.0) { float val; return this.GetValue(index, val) ? val : defval; }
    public bool GetBool(char[] index, bool defval = false) { bool val; return this.GetValue(index, val) ? val : defval; }
    public Base GetVector(char[] index, Base defval = null) { Base val; return this.GetValue(index, val) ? val : defval; }
    public Base GetAngle(char[] index, Base defval = null) { Base val; return this.GetValue(index, val) ? val : defval; }
    public List GetList(char[] index) { List list; return this.GetValue(index, list) ? list : null; }
    public ArrayList GetArrayList(char[] index) { ArrayList list; return this.GetValue(index, list) ? list : null; }

    public bool RemoveValue(char[] index) { return this.Remove(index); }

    property int Length {
        public get() { return GetTrieSize(this); }
    }

    public void Dispose() {
        StringMapSnapshot snap = this.Snapshot();

        int kl = 0;
        for (int i=0; i < snap.Length; i++) {
            kl = snap.KeyBufferSize(i);
            char[] key = new char[kl];
            snap.GetKey(i, key, kl);
            delete this.GetBase(key);
        }

        delete snap;
        delete this;
    }
}