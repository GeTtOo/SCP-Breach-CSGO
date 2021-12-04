/**
 * =============================================================================
 * Copyright (C) 2021 Eternity team (Andrey::Dono, GeTtOo).
 * =============================================================================
 *
 * This file is part of the SCP Breach CS:GO.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 **/

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
    
    property bool IsWarmup {
        public get() { return (GameRules_GetProp("m_bWarmupPeriod")) ? true : false; }
    }

    property bool RoundComplete {
        public set(bool value) { this.SetBool("roundcomplete", value); }
        public get() { return this.GetBool("roundcomplete"); }
    }

    property bool RoundLock {
        public set(bool value) { this.SetBool("roundlock", value); }
        public get() { return this.GetBool("roundlock", false); }
    }

    public void CheckLimitEntities() {
        if (Ents.list.Length > 300)
            Ents.SpawnedClear();
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
        {
            PrintToChatAll(" \x07[SCP] \x01%t", "Team Win", winteam);

            char overlay[128];
            Format(overlay, sizeof(overlay), "win_%s", winteam);
            
            Client ply;
            ArrayList players = Clients.GetAll();
            
            for(int i = 0; i < players.Length - 1; i++)
            {
                ply = players.Get(i);
                ply.ShowOverlay(overlay);
            }

            delete players;
        }

        CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TargetBombed, false);
    }

    public void GameCheck() {
        if (!this.RoundLock)
        {
            if(Clients.Alive() != 0 && Clients.InGame() != 0)
            {
                char winTeam[32];
                if (gamemode.mngr.CheckTeamStatus(winTeam, sizeof(winTeam)))
                    this.EndGame(winTeam);
            }
            /*else if (Clients.InGame() == 0)
            {
                ServerCommand("mp_restartgame 1");
                ServerCommand("mp_warmup_start");
            }*/
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
        
        ArrayList classes = team.GetClassList();

        if (team.randompick)
        {
            for (int scc = 1; scc <= deadPlayers.Length; scc++)
            {
                int id = deadPlayers.Length - 1;
                if (id < 0) break;
                
                int classid = GetRandomInt(0, classes.Length - 1);

                char classname[32];
                classes.GetString(classid, classname, sizeof(classname));
                classes.Erase(classid);

                Class class = team.class(classname);

                Client player = deadPlayers.Get(id);
                deadPlayers.Erase(id);
                player.Team(teamName);
                player.class = class;
                player.Spawn();

                if (gamemode.config.debug)
                    gamemode.log.Info("[Class] %s random setup on player: %i", classname, player.id);
            }
        }
        else
        {
            for (int v = 0; v < classes.Length; v++)
            {
                char classname[32];
                classes.GetString(v, classname, sizeof(classname));

                Class class = team.class(classname);

                int classCount = deadPlayers.Length * class.percent / 100;
                classCount = (classCount != 0 || !class.priority) ? classCount : 1;

                /*if (gamemode.config.debug)
                    gamemode.log.Info("[Class] %s trying setup on %i players", classname, classCount);*/

                for (int scc = 1; scc <= classCount; scc++)
                {
                    int id = deadPlayers.Length - 1;
                    if (id < 0) break;
                    Client player = deadPlayers.Get(id);
                    deadPlayers.Erase(id);
                    player.Team(teamName);
                    player.class = class;
                    player.Spawn();

                    if (gamemode.config.debug)
                        gamemode.log.Info("[Class] %s setup on player: %i", classname, player.id);
                }
            }
        }

        delete classes;
    }
}