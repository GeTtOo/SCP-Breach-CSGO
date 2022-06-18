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
/*-----------------------------------------------------Progress bar-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap ProgressBar < Base {

    public ProgressBar(Base ply) {
        ProgressBar self = view_as<ProgressBar>(new Base());
        self.SetValue("ply", ply);
        self.SetInt("st", FindSendPropInfo("CBaseEntity", "m_flSimulationTime"));
        self.SetInt("pbst", FindSendPropInfo("CCSPlayer", "m_flProgressBarStartTime"));
        self.SetInt("pbd", FindSendPropInfo("CCSPlayer", "m_iProgressBarDuration"));
        self.SetInt("buaip", FindSendPropInfo("CCSPlayer", "m_iBlockingUseActionInProgress"));
        return self;
    }

    property Base ply {
        public get() { Base val; return this.GetValue("ply", val) ? val : null; }
    }

    property int clientID {
        public get() { return this.ply.GetInt("id"); } 
    }

    property bool active {
        public set(bool val) { this.SetBool("active", val); }
        public get() { return this.GetBool("active"); }
    }

    public void timername(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("timername", buffer) : this.GetString("timername", buffer, max_size); }

    public Tmr Start(int time, char[] funcName = "") {
        
        int it = time / 1000;
        float ft = float(it);
        float gm = GetGameTime();

        SetEntDataFloat(this.clientID, this.GetInt("st"), gm + ft);
        SetEntData(this.clientID, this.GetInt("pbd"), it, 4, true);
        SetEntDataFloat(this.clientID, this.GetInt("pbst"), gm - (it - ft), true);
        SetEntData(this.clientID, this.GetInt("buaip"), 0, 4, true);

        char timername[64];
        Format(timername, sizeof(timername), "ply-%i|progress-bar", this.clientID);

        this.timername(timername);

        this.active = true;

        if (!StrEqual(funcName, ""))
            return timer.Create(timername, time, 1, funcName, this.ply);

        return null;
    }

    public void Stop(bool tmrdest = true) {
        char timername[64];
        this.timername(timername, sizeof(timername));

        SetEntDataFloat(this.clientID, this.GetInt("pbst"), 0.0, true);
        SetEntData(this.clientID, this.GetInt("pbd"), 0, 1, true);
        
        if (tmrdest)
            timer.RemoveByName(timername);
        
        this.active = false;
    }

    public void Dispose() {
        delete this;
    }
}