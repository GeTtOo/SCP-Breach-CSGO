/*----------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------Progress bar-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap ProgressBar < Base {

    public ProgressBar(Base ply) {
        ProgressBar self = view_as<ProgressBar>(new Base());
        self.SetValue("ply", ply);
        self.SetBool("stop", true);
        return self;
    }

    property Base ply {
        public get() { Base val; return this.GetValue("ply", val) ? val : null; }
    }

    property int clientID {
        public get() { return this.ply.GetInt("id"); } 
    }

    property bool stop {
        public set(bool val) { this.SetBool("stop", val); }
        public get() { return this.GetBool("stop"); }
    }

    public void timername(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("timername", buffer) : this.GetString("timername", buffer, max_size); }

    public void Start(int time, char[] funcName) {
        
        int it = time / 1000;
        float ft = float(it);
        float gm = GetGameTime();

        SetEntDataFloat(this.clientID, gamemode.config.GetInt("st"), gm + ft);
        SetEntData(this.clientID, gamemode.config.GetInt("pbd"), it, 4, true);
        SetEntDataFloat(this.clientID, gamemode.config.GetInt("pbst"), gm - (it - ft), true);
        SetEntData(this.clientID, gamemode.config.GetInt("buaip"), 0, 4, true);

        char timername[32];
        
        Format(timername, sizeof(timername), "progress_bar_ply_%i", this.clientID);
        gamemode.timer.Create(timername, time, 1, funcName, this.ply);

        this.timername(timername);

        Format(timername, sizeof(timername), "progress_bar_r_ply_%i", this.clientID);
        gamemode.timer.Create(timername, time, 1, "Remove", this);
    }

    public void Stop() {
        if (this.stop) {
            char timername[32];
            this.timername(timername, sizeof(timername));

            SetEntDataFloat(this.clientID, gamemode.config.GetInt("pbst"), 0.0, true);
            SetEntData(this.clientID, gamemode.config.GetInt("pbd"), 0, 1, true);
            
            gamemode.timer.Remove(timername);
        }
        if (!this.stop) this.stop = true;
    }
}

public void Remove(ProgressBar pb) {
    pb.Stop();
}