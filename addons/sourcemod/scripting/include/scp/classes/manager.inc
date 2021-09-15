/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Round Manager-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Manager < Base {

    public Manager() {
        Manager mngr = view_as<Manager>(new Base());
        return mngr;
    }

    property int serverlang {
        public get() { return GetServerLanguage(); }
    }

    property int CollisionGroup {
        public set(int value) { this.SetInt("collisiongroup", value); }
        public get() { return this.GetInt("collisiongroup"); }
    }

    property bool RoundComplete {
        public set(bool value) { this.SetBool("roundcomplete", value); }
        public get() { return this.GetBool("roundcomplete"); }
    }

    public void SetCollisionGroup(int entity, int group) {
        SetEntData(entity, this.CollisionGroup, group, 4, true);
    }

    public void PlaySoundToAll(char[] path, int channel) {
        EmitSoundToAll(path, _, channel);
    }

    public void StopSoundAll(char[] path, int channel) {
        for (int i=0; i < MAXPLAYERS; i++) {
            Client ply = Clients.Get(i);

            if (ply != null)
                StopSound(ply.id, channel, path);
        }
    }

    public void PlayAmbient(char[] path, Client ply) {
        float vecarr[3];
        Vector vec = ply.EyePos();
        vec.GetArr(vecarr);
        delete vec;
        EmitAmbientSound(path, vecarr, ply.id);
    }

    public void Fade(int entid, int duration, int holdtime, Colour clr, int bits = 0x0001)
    {
        Handle umsg = StartMessageOne("Fade", entid, USERMSG_RELIABLE);
        PbSetInt(umsg, "duration", duration);
        PbSetInt(umsg, "hold_time", holdtime);
        PbSetInt(umsg, "flags", bits);
        int arr[4];
        clr.GetArr(arr);
        PbSetColor(umsg, "clr", arr);
        EndMessage();
    }

    public void Shake(int entid, int duration, int amplitude, int frequency)
    {
        Handle message = StartMessageOne("Shake", entid, USERMSG_RELIABLE);

        PbSetInt(message, "command", 0);
        PbSetFloat(message, "local_amplitude", float(amplitude));
        PbSetFloat(message, "frequency", float(frequency));
        PbSetFloat(message, "duration", float(duration));
        EndMessage();
    }

    public void PrintTeamStatus() {
        ArrayList GlobalTeams = new ArrayList(32);
        int tpc[64];

        for (int i=1; i <= Clients.Length; i++) {
            Client ply = Clients.Get(i);

            char plyTeamName[32];
            ply.Team(plyTeamName, sizeof(plyTeamName));

            int idt = GlobalTeams.FindString(plyTeamName);

            if (idt == -1) {
                idt = GlobalTeams.PushString(plyTeamName);
                tpc[idt] = 1;
            }
            else
            {
                tpc[idt]++;
            }
        }

        PrintToServer("----------");

        for (int i = 0; i < GlobalTeams.Length; i++) {
            char buf[32];
            GlobalTeams.GetString(i, buf, sizeof(buf));
            PrintToServer("Team: %s. (Count: %i)", buf, tpc[i]);
        }

        PrintToServer("----------");

        delete GlobalTeams;
    }
    
    public bool CheckTeamStatus(char[] buffer, int max_size) {
        ArrayList GlobalTeams = new ArrayList(32);
        int tpc[64];

        for (int i=1; i <= Clients.Length; i++) {
            Client ply = Clients.Get(i);

            char plyTeamName[32];
            ply.Team(plyTeamName, sizeof(plyTeamName));

            int idt = GlobalTeams.FindString(plyTeamName);

            if (idt == -1) {
                idt = GlobalTeams.PushString(plyTeamName);
                tpc[idt] = 1;
            }
            else
            {
                tpc[idt]++;
            }
        }

        int teamAlive = 0;
        char buf[32];

        for (int i = 0; i < GlobalTeams.Length; i++) {
            char lbuf[32];
            GlobalTeams.GetString(i, lbuf, sizeof(lbuf));

            if (tpc[i] > 0 && !StrEqual(lbuf, "Dead")) {
                teamAlive++;
                buf = lbuf;
            }
        }

        if (teamAlive == 1) {
            Format(buffer, max_size, buf);
            return true;
        }

        delete GlobalTeams;

        return false;
    }

    public void EndGame(char[] winteam) {
        gamemode.mngr.RoundComplete = true;
        
        if (StrEqual(winteam, "nuke"))
            PrintToChatAll(" \x07[SCP] \x01%t", "Site destroy");
        else if(StrEqual(winteam, "nobody"))
            PrintToChatAll(" \x07[SCP] \x01%t", "Site destroy");
        else
            PrintToChatAll(" \x07[SCP] \x01%t", "Team Win", winteam);

        CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TargetBombed, false);
    }

    public void GameCheck(Client ply) {
        if(Clients.Alive() == 0 && Clients.InGame() != 0)
        {
            this.EndGame("nuke");
        }
        else if(Clients.Alive() != 0)
        {
            if (ply != null)
            {
                char team[32];
                ply.Team(team, sizeof(team));
                
                char winTeam[32];
                if (gamemode.mngr.CheckTeamStatus(winTeam, sizeof(winTeam)))
                    this.EndGame(winTeam);

                PrintToServer(winTeam);
            }
        }
        else if (Clients.InGame() != 0)
        {
            this.EndGame("nobody");
        }
    }

    public void CombatReinforcement(char[] teamName) {
        ArrayList players = Clients.GetAll();
        ArrayList deadPlayers = new ArrayList();

        for (int i=0; i < players.Length; i++) {
            Client ply = players.Get(i);

            if (!ply.IsAlive())
                deadPlayers.Push(ply);
        }

        deadPlayers.Sort(Sort_Random, Sort_Integer);

        GTeam team = gamemode.team(teamName);
        StringMapSnapshot classNameS = team.GetClassNames();
        int classKeyLen, classCount = 0;

        for (int v = 0; v < classNameS.Length; v++)
        {
            classKeyLen = classNameS.KeyBufferSize(v);
            char[] classKey = new char[classKeyLen];
            classNameS.GetKey(v, classKey, classKeyLen);
            if (json_is_meta_key(classKey)) continue;

            Class class = team.class(classKey);

            classCount = deadPlayers.Length * class.percent / 100;
            classCount = (classCount != 0 || !class.priority) ? classCount : 1;

            for (int scc = 1; scc <= classCount; scc++)
            {
                int id = deadPlayers.Length - 1;
                if (id < 0) break;
                Client player = deadPlayers.Get(id);
                deadPlayers.Erase(id);
                player.Team(teamName);
                player.class = class;
                player.haveclass = true;
                
                player.UpdateClass();
            }
        }
    }
}