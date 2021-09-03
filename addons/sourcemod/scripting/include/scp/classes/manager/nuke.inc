/*----------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------Nuclear warhead manager------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#define NUKE_EXPLOSION_SOUND "weapons/c4/c4_exp_deb1.wav"

methodmap NuclearWarhead < Base {

    public NuclearWarhead() {
        return view_as<NuclearWarhead>(new Base());
    }

    property bool IsNuked {
        public set(bool value) { this.SetBool("nuked", value); }
        public get() { return this.GetBool("nuked"); }
    }

    property bool ready {
        public set(bool value) { this.SetBool("ready", value); }
        public get() { return this.GetBool("ready"); }
    }

    property bool active {
        public set(bool value) { this.SetBool("active", value); }
        public get() { return this.GetBool("active"); }
    }

    property float detonationtime {
        public set(float value) { this.SetFloat("detonationtime", value); }
        public get() { return this.GetFloat("detonationtime"); }
    }

    property float resumetime {
        public set(float value) { this.SetFloat("resumetime", value); }
        public get() { return this.GetFloat("resumetime"); }
    }

    public void Update(char[] text) {
        ArrayList displays = WT.GetAll(2);

        if (displays.Length > 0)
            for (int i=0; i < displays.Length; i++) {
                WorldText display = displays.Get(i);

                display.SetText(text);
            }

        delete displays;
    }

    public void Init() {
        this.Update("Disabled");
    }

    public void Ready() {
        this.ready = true;
        this.Update("Ready");
    }

    public void Activate() {
        char sound[128];
        gamemode.config.nuke.sound(sound, sizeof(sound));

        EmitSoundToAll(sound,_,240);

        gamemode.timer.Create("SCP_ALPHA_WARHEAD_CALL_EXPLODE", gamemode.config.nuke.time * 1000, 1, "NukeExplode");
        gamemode.timer.Create("SCP_ALPHA_WARHEAD_DOOR_CONTROLLER", (gamemode.config.nuke.time - 10) * 1000, 1, "ClosingDoorBeforeNukeExplode");

        int ent;
        
        while((ent = FindEntityByClassname(ent, "func_door")) != -1)
        {
            char name[16];
            GetEntPropString(ent, Prop_Data, "m_iName", name, sizeof(name));
            
            if(StrContains(name, "DoorGate", false) == -1)
            {
                AcceptEntityInput(ent, "Open");
            }
        }

        this.detonationtime = GetGameTime() + float(gamemode.config.nuke.time);

        //if (this.resumetime == 0.0)
        gamemode.timer.Create("SCP_ALPHA_WARHEAD_COUNTDOWN", 10, gamemode.config.nuke.time * 100000, "SCP_AlphaWarheadCountdown");
        //else
            //gamemode.timer.Create("SCP_ALPHA_WARHEAD_COUNTDOWN", 10, RoundToNearest(this.resumetime) * 100000, "SCP_AlphaWarheadCountdown");

        this.active = true;
    }

    public void Cancel() {
        //this.resumetime = this.detonationtime - GetGameTime();
        this.active = false;
        this.ready = false;
        
        gamemode.timer.Remove("SCP_ALPHA_WARHEAD_CALL_EXPLODE");
        gamemode.timer.Remove("SCP_ALPHA_WARHEAD_DOOR_CONTROLLER");
        gamemode.timer.Remove("SCP_ALPHA_WARHEAD_COUNTDOWN");

        char sound[128];
        gamemode.config.nuke.sound(sound, sizeof(sound));

        StopSound(1,240,sound);
    }

    public void SpawnDisplay() {
        
        JSON_ARRAY spawnlist = gamemode.config.nuke.spawnlist;

        for (int i=0; i < spawnlist.Length; i++) {
            JSON_OBJECT data = view_as<JSON_OBJECT>(spawnlist.GetObject(i));

            WT.Create(data.GetVector("pos"), data.GetAngle("ang"), 2).SetSize(data.GetInt("size")).SetColor(data.GetColor("color")).SetText("Disabled");
        }
        
    }
    
    public void Controller(int btnId) {
        if (gamemode.config.nuke.readybtn == btnId && !this.ready)
            this.Ready();
        else if (gamemode.config.nuke.activebtn == btnId && this.ready && !this.active)
            this.Activate();
        else if (gamemode.config.nuke.cancelbtn == btnId && this.ready && this.active)
            this.Cancel();
    }
}