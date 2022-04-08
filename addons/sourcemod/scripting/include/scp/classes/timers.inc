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
        Tmr timer = view_as<Tmr>(new StringMap());
        timer.SetValue("pluginhandle", ph);
        timer.SetString("name", name);
        timer.SetFloat("delay", delay);
        timer.SetInt("repeations", repeations);
        timer.SetString("func", func);
        timer.SetValue("args", args);

        timer.SetFloat("nextinvoke", GetGameTime() + delay);

        return timer;
    }

    property Handle ph {
        public get() { Handle ph; return this.GetValue("pluginhandle", ph) ? ph : null; }
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
}

methodmap Timers < Base {

    public Timers() {
        Timers self = view_as<Timers>(new Base());
        self.CreateArrayList("timers");
        return self;
    }

    public Tmr Create(char[] name, int delay, int repeations, char[] funcname, any args = false) {
        Tmr timer = new Tmr(GetMyHandle(), name, (float(delay) / float(1000)), repeations, funcname, args);
        this.GetArrayList("timers").Push(timer);
        return timer;
    }

    public bool IsAlive(Tmr timer) { return (this.GetArrayList("timers").FindValue(timer) != -1) ? true : false; }

    public bool Remove(char[] name) {
        ArrayList list = this.GetArrayList("timers");
        
        for (int i=0; i < list.Length; i++) {
            char timername[64];
            Tmr timer = list.Get(i);
            timer.name(timername, sizeof(timername));
            if (StrEqual(timername, name)) {
                list.Erase(i);
                delete timer;
                return true;
            }
        }

        return false;
    }

    public bool RemoveIsContains(char[] name) {
        ArrayList list = this.GetArrayList("timers");
        
        for (int i=0; i < list.Length; i++) {
            char timername[64];
            view_as<Tmr>(list.Get(i)).name(timername, sizeof(timername));
            if (StrContains(timername, name) != -1) {
                list.Erase(i);
            }
        }
        
        return false;
    }

    public void ClearAll() {
        this.GetArrayList("timers").Clear();
    }

    public void PluginClear() {
        ArrayList list = this.GetArrayList("timers");
        if (list.Length != 0)
            for (int i=0; i < list.Length; i++)
                if (view_as<Tmr>(list.Get(i)).ph == GetMyHandle())
                    list.Erase(i);
    }
    
    public Tmr Simple(int delay, char[] funcname, any args = false) {
        char timername[64], pname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
        FormatEx(timername, sizeof(timername), "%s_%i_%i", pname, GetTime(), GetRandomInt(1, 1000));
        return this.Create(timername, delay, 1, funcname, args);
    }

    public void Update() {
        ArrayList list = this.GetArrayList("timers");

        for (int i=0; i < list.Length; i++) {
            Tmr timer = list.Get(i);
            if (GetGameTime() >= timer.nextinvoke) {

                char funcname[64];
                timer.func(funcname, sizeof(funcname));

                Call_StartFunction(timer.ph, GetFunctionByName(timer.ph, funcname));
                if (timer.args)
                    Call_PushCell(timer.args);
                Call_Finish();

                if (!this.IsAlive(timer)) continue; // Dirty fix invalid handle error...

                if (timer.repeations == 1)
                {
                    list.Erase(i);
                    delete timer;
                }
                else if (timer.repeations > 1) {
                    timer.nextinvoke += timer.delay;
                    timer.repeations--;
                }
                else if (timer.repeations == 0)
                    timer.nextinvoke += timer.delay;
            }
        }
    }

    public void Dispose() {
        this.ClearAll();
        delete this.GetArrayList("timers");
        this.RemoveValue("timers");
    }
}