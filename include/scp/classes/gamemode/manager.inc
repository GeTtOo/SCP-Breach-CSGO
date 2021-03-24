/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Round Manager-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Team < Base {
    
    public Team(char[] name) {
        Team t = view_as<Team>(new StringMap());
        t.SetString("name", name);
        return t;
    }

    property int count {
        public set(int val) { this.SetInt("count", val); }
        public get() { return this.GetInt("count", 0); }
    }
    
}

methodmap Manager < Base {

    public Manager() {
        Manager mngr = view_as<Manager>(new StringMap());
        mngr.SetValue("teams", new ArrayList());
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
    
    public bool RegisterTeam(char[] name) {
        ArrayList teams;
        this.GetValue("teams", teams);
        teams.Push(new Team(name));
    }

    property ArrayList teams {
        public get() { ArrayList teams; return this.GetValue("teams", teams) ? teams : null; }
    }

    public Team TeamGet(char[] name) {
        ArrayList teams;
        this.GetValue("teams", teams);

        for (int i=0; i < teams.Length; i++) {
            char teamName[32];
            Team cur = teams.Get(i);
            cur.GetString("name", teamName, sizeof(teamName));

            if (StrEqual(teamName, name))
                return cur;
        }

        return null;
    }

    public bool CheckTeamStatus(char[] buffer, int max_size) {
        ArrayList teams;
        this.GetValue("teams", teams);

        int TeamAlive = 0;
        char winTeam[32];
        
        for (int i=0; i < teams.Length; i++) {
            Team team = teams.Get(i);
            if (team.count > 0) {
                TeamAlive++;
                team.GetString("name", winTeam, sizeof(winTeam));
            }
        }
        
        if (TeamAlive == 1) {
            Format(buffer, max_size, winTeam);
            return true;
        }

        return false;
    }

    public void Reset() {
        ArrayList teams;
        this.GetValue("teams", teams);
        teams.Clear();
    }

    
}