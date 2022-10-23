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
/*----------------------------------------------------Ambient class-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Ambient < Entity {
    
    public Ambient(Entity ent = null, int volume = 10, float radius = 800.0, char[] spath = "") {
        Ambient self = view_as<Ambient>(new Entity());
        self.SetBool("spawned", false);

        if (ent)
        {
            char target[32];
            ent.GetString("targetname", target, sizeof(target));

            self.Create("ambient_generic");

            self.SetKV("SourceEntityName", target);
            self.SetKV("message", spath);
            self.SetKVInt("health", volume);
            self.SetKVFloat("radius", radius);

            self.SetInt("volume", volume);
            self.SetFloat("radius", radius);
        }

        return self;
    }
    
    property int volume {
        public set(int val) { this.SetInt("volume", val); this.SetKVInt("health", val); }
        public get() { return this.GetInt("volume", 10); }
    }

    property float radius {
        public set(float val) { this.SetFloat("radius", val); this.SetKVFloat("radius", val); }
        public get() { return this.GetFloat("radius", 800.0); }
    }

    public Ambient SetSource(char[] target) { this.SetKV("SourceEntityName", target); }

    public Ambient SetSourceEnt(Entity ent)
    {
        char target[32];
        ent.GetString("targetname", target, sizeof(target));
        this.SetKV("SourceEntityName", target);
    }

    public Ambient SetSound(char[] path)
    {
        this.SetString("sound", path);
        this.SetKV("message", path);
    }

    public Ambient Play(char[] path = "")
    {
        if (strlen(path) > 0) this.SetSound(path);

        if (!this.spawned)
        {
            this.Spawn();
            this.Activate();
            this.SetBool("spawned", true);
        }

        this.Input("PlaySound");
    }

    public Ambient Stop() { this.Input("StopSound"); }
}