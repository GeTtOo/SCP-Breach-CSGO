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
        timer.SetBool("removed", false);

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

    property bool removed {
        public set(bool val) { this.SetBool("removed", val); }
        public get() { return this.GetBool("removed"); }
    }

    property float nextinvoke {
        public set(float time) { this.SetFloat("nextinvoke", time); }
        public get() { return this.GetFloat("nextinvoke"); }
    }
}

methodmap Timers < Base {

    public Timers() {
        Timers self = view_as<Timers>(new Base());
        self.CreateList("timers");
        return self;
    }

    public Tmr Create(char[] name, int delay, int repeations, char[] funcname, any args = false) {
        Tmr timer = new Tmr(GetMyHandle(), name, (float(delay) / float(1000)), repeations, funcname, args);
        this.GetList("timers").Push(timer);
        return timer;
    }

    public bool Remove(char[] name) {
        ArrayList list = this.GetList("timers");
        
        for (int i=0; i < list.Length; i++) {
            char timername[64];
            view_as<Tmr>(list.Get(i)).name(timername, sizeof(timername));
            if (StrEqual(timername, name)) {
                list.Erase(i);
                return true;
            }
        }
        
        return false;
    }

    public void PluginClear() {
        ArrayList list = this.GetList("timers");
        for (int i=0; i < list.Length; i++)
            if (view_as<Tmr>(list.Get(i)).ph == GetMyHandle())
                list.Erase(i);
    }
    
    public void Simple(int delay, char[] funcname, any args = false) {
        char timername[64], pname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
        IntToString(GetTime(), timername, sizeof(timername));
        Format(timername, sizeof(timername), "%s_%s", pname, timername);
        this.Create(timername, delay, 1, funcname, args);
    }

    public void Update() {
        ArrayList list = this.GetList("timers");

        for (int i=0; i < list.Length; i++) {
            Tmr timer = list.Get(i);
            if (!timer.removed && GetGameTime() >= timer.nextinvoke) {
                char funcname[64];

                timer.func(funcname, sizeof(funcname));

                Call_StartFunction(timer.ph, GetFunctionByName(timer.ph, funcname));
                if (timer.args)
                    Call_PushCell(timer.args);
                Call_Finish();
                
                if (timer.repeations == 1)
                {
                    timer.removed = true;
                    //list.Erase(i);
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
}