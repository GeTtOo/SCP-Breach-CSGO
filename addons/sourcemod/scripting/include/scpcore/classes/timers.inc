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
/*----------------------------------------------------Advanced Timers---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Tmr < Base {

    public Tmr(Handle ph, char[] name, float delay, int repeations, char[] func, ArrayList args) {
        Tmr tmr = view_as<Tmr>(new StringMap());
        tmr.SetValue("pluginhandle", ph);
        tmr.SetString("name", name);
        tmr.SetFloat("delay", delay);
        tmr.SetInt("repeations", repeations);
        tmr.SetString("func", func);
        tmr.SetValue("args", args);

        tmr.SetFloat("nextinvoke", GetGameTime() + delay);

        return tmr;
    }

    property Handle ph {
        public get() { return this.GetHandle("pluginhandle"); }
    }

    public void name(char[] name, int max_len) {
        this.GetString("name", name, max_len);
    }

    property float delay {
        public get() { return this.GetFloat("delay"); }
    }

    property int repeations {
        public set(int count) { this.SetInt("repeations", count); }
        public get() { return this.GetInt("repeations"); }
    }

    public void func(char[] name, int max_len) {
        this.GetString("func", name, max_len);
    }

    property any args {
        public get() { Handle args; return this.GetValue("args", args) ? args : null; }
    }

    property float nextinvoke {
        public set(float time) { this.SetFloat("nextinvoke", time); }
        public get() { return this.GetFloat("nextinvoke"); }
    }

    property bool active {
        public set(bool val) { this.SetBool("active", val); }
        public get() { return this.GetBool("active"); }
    }

    public int GetTimeLeft() { return RoundToCeil(this.nextinvoke - GetGameTime()); }

    public void Dispose()
    {
        delete this;
    }
}

methodmap Timers < Base {

    public Timers() {
        Timers self = view_as<Timers>(new Base());
        self.CreateArrayList("timers");
        return self;
    }

    property ArrayList list {
        public native get();
    }

    public native Tmr HideCreate(Handle ph, char[] name, float delay, int repeations, char[] funcname, any args);

    public Tmr Create(char[] name, int delay, int repeations, char[] funcname = "", any args = false) {
        return this.HideCreate(GetMyHandle(), name, (float(delay) / float(1000)), repeations, funcname, args);
    }

    public bool IsAlive(Tmr tmr) { return (this.list.FindValue(tmr) != -1) ? true : false; }

    public bool Remove(Tmr tmr) {
        for (int i=0; i < this.list.Length; i++) {
            if (this.list.Get(i) == tmr) {
                tmr.active = false;
                return true;
            }
        }

        return false;
    }

    public bool RemoveByName(char[] name) {
        for (int i=0; i < this.list.Length; i++) {
            char timername[64];
            Tmr tmr = this.list.Get(i);
            tmr.name(timername, sizeof(timername));
            if (StrEqual(timername, name)) {
                tmr.active = false;
                return true;
            }
        }

        return false;
    }

    public void RemoveIsContains(char[] name) {
        for (int i=0; i < this.list.Length; i++) {
            char timername[64];
            Tmr tmr = this.list.Get(i);
            tmr.name(timername, sizeof(timername));
            if (StrContains(timername, name) != -1) {
                tmr.active = false;
            }
        }
    }

    public void ClearAll() {
        for (int i=0; i < this.list.Length; i++) view_as<Tmr>(this.list.Get(i)).Dispose();
        this.list.Clear();
    }

    public void PluginClear() {
        if (this.list.Length != 0)
            for (int i=0; i < this.list.Length; i++)
                if (view_as<Tmr>(this.list.Get(i)).ph == GetMyHandle())
                {
                    this.list.Erase(i);
                    i--;
                }
    }
    
    public Tmr Simple(int delay, char[] funcname = "", any args = false) {
        char timername[64], pname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
        FormatEx(timername, sizeof(timername), "%s_%i_%i", pname, GetTime(), GetRandomInt(1, 1000000));
        return this.Create(timername, delay, 1, funcname, args);
    }

    public void Update()
    {
        for (int i=0; i < this.list.Length; i++)
        {
            Tmr tmr = this.list.Get(i);

            if (!tmr.GetBool("active"))
            {
                delete tmr;
                this.list.Erase(i); i--;
                continue;
            }

            if (tmr.nextinvoke <= GetGameTime())
            {

                char funcname[64];
                tmr.func(funcname, sizeof(funcname));

                if (strlen(funcname) != 0)
                {
                    Call_StartFunction(tmr.ph, GetFunctionByName(tmr.ph, funcname));
                    if (tmr.args)
                        Call_PushCell(tmr.args);
                    Call_Finish();
                }

                //if (!this.IsAlive(timer)) continue; // Dirty fix invalid handle error...

                if (tmr.repeations == 1)
                {
                    tmr.active = false;
                }
                else if (tmr.repeations > 1)
                {
                    tmr.nextinvoke += tmr.delay;
                    tmr.repeations--;
                }
                else if (tmr.repeations == 0)
                    tmr.nextinvoke += tmr.delay;
            }
        }
    }

    public void Dispose() {
        this.ClearAll();
        delete this.list;
        this.RemoveValue("timers");
    }
}