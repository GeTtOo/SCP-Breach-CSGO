/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Round Manager-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Manager < Base {

    public Manager(Teams teams) {
        Manager mngr = view_as<Manager>(new StringMap());
        mngr.SetValue("teams", teams);
        return mngr;
    }

    property int PlayerCollisionGroup {
        public set(int value) { this.SetInt("playercollisiongroup", value); }
        public get() { return this.GetInt("playercollisiongroup"); }
    }

    property bool IsNuked {
        public set(bool value) { this.SetBool("nuked", value); }
        public get() { return this.GetBool("nuked"); }
    }

    property bool RoundComplete {
        public set(bool value) { this.SetBool("roundcomplete", value); }
        public get() { return this.GetBool("roundcomplete"); }
    }

    property int DeadPlayers {
        public set(int val) { this.SetInt("deadplayers", val); }
        public get() { return this.GetInt("deadplayers", 0); }
    }

    property Teams teams {
        public get() {
            Teams teams;
            this.GetValue("teams", teams);
            return teams;
        }
    }

    public GTeam team(char[] name) {
        Teams teams;
        this.GetValue("teams", teams);

        return teams.get(name);
    }

    public bool CheckTeamStatus(char[] buffer, int max_size) {
        Teams teams;
        this.GetValue("teams", teams);
        
        StringMapSnapshot snapshot = teams.Snapshot();

        int TeamAlive = 0;
        char winTeam[32];
        
        for (int i=0; i < snapshot.Length; i++) {
            int teamlen = snapshot.KeyBufferSize(i);
            char[] teamname = new char[teamlen];
            snapshot.GetKey(i, teamname, teamlen);
            if (json_is_meta_key(teamname)) continue;

            GTeam team = teams.get(teamname);

            if (team.count > 0 && !StrEqual(teamname, "Dead")) {
                TeamAlive++;
                Format(winTeam, sizeof(winTeam), teamname);
            }
        }
        
        if (TeamAlive == 1) {
            Format(buffer, max_size, winTeam);
            return true;
        }

        return false;
    }

    public void Reset() {
        Teams teams;
        this.GetValue("teams", teams);
        
        StringMapSnapshot snapshot = teams.Snapshot();
        
        for (int i=0; i < snapshot.Length; i++) {
            int teamlen = snapshot.KeyBufferSize(i);
            char[] teamname = new char[teamlen];
            snapshot.GetKey(i, teamname, teamlen);
            if (json_is_meta_key(teamname)) continue;
            
            teams.get(teamname).count = 0;
        }

        this.DeadPlayers = 0;
    }
}