/*----------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------Nuclear warhead manager------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#define NUKE_EXPLOSION_SOUND "weapons/c4/c4_exp_deb1.wav"

methodmap NuclearWarhead < Base {

    public NuclearWarhead() {
        NuclearWarhead controller = view_as<NuclearWarhead>(new Base());
        controller.CreateArrayList("monitors");
        return controller;
    }

    property ArrayList monitors {
        public set(ArrayList value) { this.SetArrayList("monitors", value); }
        public get() { return this.GetArrayList("monitors"); }
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

    public void Update(char[] text, int size = 0) {
        ArrayList displays = WT.GetAll(NukeMonitor);

        if (displays.Length > 0)
            for (int i=0; i < displays.Length; i++) {
                WorldText display = displays.Get(i);

                display.SetText(text);
                if (size != 0)
                    display.SetSize(size);
            }

        delete displays;
    }

    public void Init() {
        this.Update("Disabled");
    }

    public void Ready() {
        this.ready = true;
        this.Update(" Ready");
    }

    public void Activate() {
        char sound[128];
        gamemode.config.nuke.sound(sound, sizeof(sound));

        gamemode.mngr.PlaySoundToAll(sound, 240);

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
        gamemode.timer.Create("SCP_ALPHA_WARHEAD_COUNTDOWN", 100, 0, "SCP_AlphaWarheadCountdown");
        //else
            //gamemode.timer.Create("SCP_ALPHA_WARHEAD_COUNTDOWN", 10, RoundToNearest(this.resumetime) * 100000, "SCP_AlphaWarheadCountdown");

        this.active = true;

        for (int i=0; i < this.monitors.Length; i++) {
            Entity display = this.monitors.Get(i);
            
            display.SetProp("m_nSkin", (gamemode.lang == 22) ? 1 : 3);
        }
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
        gamemode.mngr.StopSoundAll(sound, 240);

        gamemode.config.nuke.cancelsound(sound, sizeof(sound));
        gamemode.mngr.PlaySoundToAll(sound, 250);

        this.Update("Canceled");
    }

    public void AddCounterToDisplay() {
        int entId = 0;
        while ((entId = FindEntityByClassname(entId, "prop_dynamic")) != -1) {
            if (!IsValidEntity(entId)) continue;

            char ModelName[128];
            GetEntPropString(entId, Prop_Data, "m_ModelName", ModelName, sizeof(ModelName));

            if (StrEqual(ModelName, "models/eternity/map/monitor.mdl"))
                this.monitors.Push(new Entity(entId));
        }

        for (int i=0; i < this.monitors.Length; i++) {
            Entity display = this.monitors.Get(i);
            
            Vector dp = (display.GetAng() - new Angle(10.0,90.0,0.0)).Up((display.GetAng() - new Angle(10.0,90.0,0.0)).Right((display.GetAng() - new Angle(10.0,90.0,0.0)).Forward(display.GetPos(), -8.0), -37.5), -17.0);
            Angle da = display.GetAng() - new Angle(10.0,90.0,0.0);

            WT.Create(dp, da, NukeMonitor).SetSize(15).SetColor(new Colour(180,0,0));
        }
    }

    public void SpawnDisplay() {
        
        JSON_ARRAY spawnlist = gamemode.config.nuke.spawnlist;

        for (int i=0; i < spawnlist.Length; i++) {
            JSON_OBJECT data = view_as<JSON_OBJECT>(spawnlist.GetObject(i));

            WT.Create(data.GetVector("pos"), data.GetAngle("ang"), NukeMonitor).SetSize(data.GetInt("size")).SetColor(data.GetColor("color")).SetText("Disabled");
        }
        
        this.AddCounterToDisplay();
    }
    
    public void Controller(int btnId) {
        if (gamemode.config.nuke.buttons.ready == btnId && !this.ready && !this.IsNuked)
            this.Ready();
        else if (gamemode.config.nuke.buttons.active == btnId && this.ready && !this.active && !this.IsNuked)
            this.Activate();
        else if (gamemode.config.nuke.buttons.cancel == btnId && this.ready && this.active)
            this.Cancel();
    }
    
    public void Reset() {
        this.IsNuked = false;
        this.ready = false;
        this.active = false;
        this.resumetime = 0.0;
        this.detonationtime = 0.0;
        this.monitors.Clear();
    }
}

public void SCP_AlphaWarheadCountdown() {
    char time[32], time2[10], time3[3];
    float cursec = gamemode.nuke.detonationtime - GetGameTime();
    int min = RoundToNearest(cursec) / 60;
    int sec = RoundFloat(cursec) % 60;
    FloatToString(FloatFraction(cursec), time2, sizeof(time2));
    strcopy(time3, sizeof(time3), time2[2]);
    Format(time, sizeof(time), (min < 10 ) ? ((sec < 10 ) ? "0%i:0%i:%s" : "0%i:%i:%s") : ((sec < 10 ) ? "%i:0%i:%s" : "%i:%i:%s"),  RoundToNearest(cursec) / 60, RoundFloat(cursec) % 60, time3);
    gamemode.nuke.Update(time);

    //if ((gamemode.nuke.detonationtime - GetGameTime()) < 0.1)
        //gamemode.nuke.Update("Site destroyed");
}

public void ClosingDoorBeforeNukeExplode() {
    int ent;
    while((ent = FindEntityByClassname(ent, "func_door")) != -1)
    {
        char name[16];
        GetEntPropString(ent, Prop_Data, "m_iName", name, sizeof(name));
        
        if(StrContains(name, "DoorGate", false) != -1)
        {
            AcceptEntityInput(ent, "Close");
            AcceptEntityInput(ent, "Lock");
        }
    }
}

public void NukeExplode() {

    gamemode.nuke.IsNuked = true;

    for(int i=0; i < MAXPLAYERS; i++)
    {
        Client ply = Clients.Get(i);

        if(ply != null && ply.IsAlive())
        {
            Vector pos = ply.GetPos();

            if(pos.z <= gamemode.config.nuke.killpos)
            {
                ply.Kill();
            }
            else
            {
                EmitSoundToClient(ply.id, NUKE_EXPLOSION_SOUND);
                gamemode.mngr.Shake(ply.id, 5, 10, 50);
            }

            delete pos;
        }
    }

    gamemode.timer.Remove("SCP_ALPHA_WARHEAD_COUNTDOWN");

    gamemode.nuke.Update("Site destroyed", 8);

    gamemode.mngr.EndGame("nuke");
}