/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Tmr < Base {

    public Tmr(char[] action, int seconds, bool repeat, Handle hndl, Handle value) {
        Tmr timer = view_as<Tmr>(new StringMap());
        timer.SetString("action", action);
        timer.SetInt("seconds", seconds);
        timer.SetBool("repeat", repeat);
        timer.SetValue("ph", hndl);
        timer.SetValue("value", value);

        return timer;
    }
    
    public void action(char[] name, int max_len) {
        this.GetString("action", name, max_len);
    }

    property int seconds {
        public get() { return this.GetInt("seconds"); }
    }

    property bool repeat {
        public get() { return this.GetBool("repeat"); }
    }

    property Handle ph {
        public get() { Handle ph; return this.GetValue("ph", ph) ? ph : null; }
    }

    property any value {
        public get() { Handle value; return this.GetValue("value", value) ? value : null; }
    }
}

methodmap Timers < Base {

    public Timers() {
        Timers self = view_as<Timers>(new StringMap());
        self.SetValue("timers", new ArrayList());
        return self;
    }

    public void Add(char[] action, int seconds, bool repeat, Handle value) {
        ArrayList timers;
        this.GetValue("timers", timers);
        
        timers.Push(new Tmr(action, GetTime() + seconds, repeat, GetMyHandle(), value));
    }

    public void Update() {
        ArrayList timers;
        this.GetValue("timers", timers);

        for (int i=0; i < timers.Length; i++) {
            Tmr timer = view_as<Tmr>(timers.Get(i, 0));
            if (GetTime() >= timer.seconds) {
                char name[32];
                timer.action(name, sizeof(name));
                Call_StartFunction(timer.ph, GetFunctionByName(timer.ph, name));
                Call_PushCell(timer.value);
                Call_Finish();
                
                if (!timer.repeat)
                    timers.Erase(i);
            }
        }
    }
}