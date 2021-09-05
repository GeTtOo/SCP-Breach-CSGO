/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Round Manager-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Manager < Base {

    public Manager() {
        Manager mngr = view_as<Manager>(new Base());
        return mngr;
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
}