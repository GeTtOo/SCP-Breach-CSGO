/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Round Manager-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include "scp\classes\manager\nuke.inc"

methodmap Manager < Base {

    public Manager() {
        Manager mngr = view_as<Manager>(new Base());
        mngr.SetValue("nuke", new NuclearWarhead());
        return mngr;
    }

    property NuclearWarhead nuke {
        public get() { NuclearWarhead nuke; return this.GetValue("nuke", nuke) ? nuke : null; }
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