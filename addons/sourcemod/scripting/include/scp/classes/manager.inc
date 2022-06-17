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
        mngr.CreateArrayList("escapezones");
        return mngr;
    }

    property int serverlang {
        public get() { return GetServerLanguage(); }
    }
    
    property bool IsWarmup {
        public get() { return (GameRules_GetProp("m_bWarmupPeriod")) ? true : false; }
    }

    property int rst {
        public set(int val) { this.SetInt("rst", val); }
        public get() { return this.GetInt("rst"); }
    }

    property int RoundTime {
        public get() { return GetTime() - this.rst; }
    }

    property bool RoundComplete {
        public set(bool value) { this.SetBool("roundcomplete", value); }
        public get() { return this.GetBool("roundcomplete"); }
    }

    property bool RoundLock {
        public set(bool value) { this.SetBool("roundlock", value); }
        public get() { return this.GetBool("roundlock", false); }
    }

    public void CheckLimitEntities() { if (ents.list.Length > 500) ents.SpawnedClear(); }

    public void GetServerLangInfo(char[] name, int max_size) { GetLanguageInfo(this.serverlang, name, max_size); }

    public void PlaySoundToAll(char[] path, int channel) {
        EmitSoundToAll(path, _, channel);
    }

    public void PlayTranslatedSoundToAll(char[] metapath)
    {
        ArrayList players = player.GetAll();

        for (int i=0; i < players.Length; i++) view_as<Player>(players.Get(i)).PlayTranslatedSound(metapath);

        delete players;
    }

    public void PlayNonCheckSoundToAll(char[] path) {
        ArrayList players = player.GetAll();

        for (int i=0; i < players.Length; i++)
        {
            Player ply = players.Get(i);

            ply.PlayNonCheckSound(path);
        }

        delete players;
    }

    public void PlayTranslatedNonCheckSoundToAll(char[] metapath)
    {
        ArrayList players = player.GetAll();

        for (int i=0; i < players.Length; i++) view_as<Player>(players.Get(i)).PlayTranslatedNonCheckSound(metapath);

        delete players;
    }

    public void StopSoundAll(char[] path, int channel) {
        ArrayList players = player.GetAll();

        for (int i=0; i < players.Length; i++)
        {
            Player ply = players.Get(i);

            StopSound(ply.id, channel, path);
        }

        delete players;
    }

    public void PlayAmbient(char[] path, Entity ent) {
        float vecarr[3];
        Vector vec = ent.GetPos();
        vec.GetArr(vecarr);
        delete vec;
        EmitAmbientSound(path, vecarr, ent.id);
    }

    public void PlayTranslatedAmbient(char[] metapath, char[] langcode, Entity ent) {
        float vecarr[3];
        Vector vec = ent.GetPos();
        vec.GetArr(vecarr);
        delete vec;

        char path[256], patchcheck[256];

        Utils.TransformPath(metapath, path, langcode);

        FormatEx(patchcheck, sizeof(patchcheck), "sound/%s", path);
        if (!FileExists(patchcheck, true)) Utils.TransformPath(metapath, path);

        Format(path, sizeof(path), "*/%s", path);
        EmitAmbientSound(path, vecarr, ent.id);
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
    
    public bool IsEscapeZone(int id) { return (this.GetArrayList("escapezones").FindValue(id) != -1) ? true : false; }

    public bool CreateEscapeZoneList()
    {
        ArrayList teams = gamemode.GetTeamList();

        for (int i = 0; i < teams.Length; i++)
        {
            char teamname[32];
            teams.GetString(i, teamname, sizeof(teamname));
            
            GTeam team = gamemode.team(teamname);

            ArrayList classes = team.GetClassList();
            
            for (int v = 0; v < classes.Length; v++)
            {
                char classname[32];
                classes.GetString(v, classname, sizeof(classname));

                Class class = team.class(classname);

                if (class.escape && !this.IsEscapeZone(class.escape.trigger))
                    this.GetArrayList("escapezones").Push(class.escape.trigger);
            }

            delete classes;
        }

        delete teams;
    }

    public bool CheckTeamStatus(char[] buffer, int max_size) {
        ArrayList GlobalTeams = new ArrayList(32);
        ArrayList players = player.GetAll();
        int tpc[64], pr = 0;

        for (int i=0; i < players.Length; i++)
        {
            Player ply = players.Get(i);
            
            if (!ply || !ply.class) continue;
            
            if (ply.ready) pr++;

            char plyTeamName[32];
            ply.Team(plyTeamName, sizeof(plyTeamName));

            if (StrEqual(plyTeamName, "Dead") || StrEqual(plyTeamName, "None")) continue;

            int idt = GlobalTeams.FindString(plyTeamName);

            if (idt == -1)
            {
                idt = GlobalTeams.PushString(plyTeamName);
                tpc[idt] = 1;
            }
            else
                tpc[idt]++;
        }

        int teamAlive = 0;
        char buf[32];

        for (int i = 0; i < GlobalTeams.Length; i++) {
            char lbuf[32];
            GlobalTeams.GetString(i, lbuf, sizeof(lbuf));

            if (tpc[i] > 0) {
                teamAlive++;
                buf = lbuf;
            }
        }

        if (pr == 1 && player.Alive() > 1) teamAlive = 0;

        delete GlobalTeams;
        delete players;

        if (teamAlive == 1) {
            Format(buffer, max_size, buf);
            return true;
        }

        return false;
    }

    public void EndGame(char[] winteam) {
        this.RoundComplete = true;
        
        if(StrEqual(winteam, "nobody"))
            PrintToChatAll(" \x07[SCP] \x01%t", "No One Lives");
        else if(StrEqual(winteam, "restart"))
            PrintToChatAll(" \x07[SCP] \x01%t", "Round restart");
        else if(StrEqual(winteam, "restartbyadmin"))
            PrintToChatAll(" \x07[SCP] \x01%t", "Round restart by admin");
        else
        {
            PrintToChatAll(" \x07[SCP] \x01%t", "Team Win", winteam);

            char winoverlay[256];
            gamemode.team(winteam).GetString("winoverlay", winoverlay, sizeof(winoverlay));
            
            Player ply;
            ArrayList players = player.GetAll();
            
            for(int i = 0; i < players.Length; i++)
            {
                ply = players.Get(i);

                char path[256], patchcheck[256], langcode[3];
                
                ply.GetLangInfo(langcode, sizeof(langcode));
                Utils.TransformPath(winoverlay, path, langcode);

                FormatEx(patchcheck, sizeof(patchcheck), "materials/%s.vmt", path);
                if (!FileExists(patchcheck, true)) Utils.TransformPath(winoverlay, path);
                
                char overlay[256];
                Format(overlay, sizeof(overlay), "r_screenoverlay %s", path);
                ply.ExecCommand(overlay);
            }

            delete players;
        }

        CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_Draw, false);
    }

    public void GameCheck() {
        if (this.RoundLock || this.RoundComplete) return;
        
        char winteam[32];
        if(player.InGame() > 1 && player.Alive() > 0 && this.CheckTeamStatus(winteam, sizeof(winteam)))
            this.EndGame(winteam);
        else if (player.InGame() > 0 && player.Alive() == 0)
            this.EndGame("nobody");
    }

    public bool CombatReinforcement(char[] teamName) {
        bool status = false;
        ArrayList players = player.GetAll();
        ArrayList deadPlayers = new ArrayList();

        for (int i=0; i < players.Length; i++) {
            Player ply = players.Get(i);

            if (!ply.IsAlive())
                deadPlayers.Push(ply);
        }

        deadPlayers.Sort(Sort_Random, Sort_Integer);

        GTeam team = gamemode.team(teamName);
        ArrayList classes = team.GetClassList();

        int baseclasschance = 0;
        char baseclass[32];

        for (int v = 0; v < classes.Length; v++)
        {
            char classname[32];
            classes.GetString(v, classname, sizeof(classname));

            Class class = team.class(classname);
            int classCount = deadPlayers.Length * class.percent / 100;
            classCount = (classCount != 0 || !class.priority) ? classCount : 1;

            if (class.percent > baseclasschance)
                baseclass = classname;

            for (int scc = 1; scc <= classCount; scc++)
            {
                int id = deadPlayers.Length - 1;
                if (id < 0) break;
                Player ply = deadPlayers.Get(id);
                deadPlayers.Erase(id);
                ply.Team(teamName);
                ply.class = class;
                ply.Spawn();

                status = true;

                if (gamemode.config.debug)
                    gamemode.log.Info("[Class] %s setup on player: %i", classname, ply.id);
            }
        }

        for (int i=0; i < deadPlayers.Length; i++)
        {
            Player ply = deadPlayers.Get(i);

            ply.Team(teamName);
            ply.class = team.class(baseclass);
            ply.Spawn();

            status = true;
        }

        if (status)
        {
            char metapath[128], path[128], patchcheck[128], langcode[3];

            gamemode.team(teamName).GetString("reinforcesound", metapath, sizeof(metapath));
            
            for (int i=0; i < players.Length; i++)
            {
                Player ply = players.Get(i);

                ply.GetLangInfo(langcode, sizeof(langcode));
                Utils.TransformPath(metapath, path, langcode);
                Format(patchcheck, sizeof(patchcheck), "sound/%s", path);

                if (FileExists(patchcheck, true)) ply.PlayNonCheckSound(path);
            }
        }

        delete players;
        delete deadPlayers;
        delete classes;

        return status;
    }

    public void GetSpecInfo(Player ply, Player target)
    {
        if (!ply || !target || !target.class || ply == target) return;
        
        char playername[64], team[32], class[32];

        target.GetName(playername, sizeof(playername));
        target.Team(team, sizeof(team));
        target.class.Name(class, sizeof(class));

        char invstr[160];

        for (int i=0; i < target.inv.list.Length; i++)
        {
            InvItem item = target.inv.Get(i);

            char itemclass[64], itemname[64];
            item.GetClass(itemclass, sizeof(itemclass));
            FormatEx(itemname, sizeof(itemname), "%T", itemclass, ply.id);
            
            Format(invstr, sizeof(invstr), "%s%i. %s\n", invstr, i+1, itemname);
        }

        char fullinfo[512];

        Format(fullinfo, sizeof(fullinfo), "      %s: %s - %s\n-------------------------------------------------\n%s", playername, team, class, invstr);

        ply.PrintNotify(fullinfo);
    }

    public void Dispose()
    {
        delete this.GetArrayList("escapezones");
        delete this;
    }
}