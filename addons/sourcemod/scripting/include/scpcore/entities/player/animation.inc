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
/*----------------------------------------------------Animation class-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap PlayerAnimation < Entity {
    
    public PlayerAnimation(Player ply) {
        PlayerAnimation self = view_as<PlayerAnimation>(new Entity());
        self.SetBool("spawned", false);

        self.Set("player", ply);

        return self;
    }

    property Player player {
        public get() { return this.Get("player"); }
    }

    public PlayerAnimation SetModel(char[] path) { this.SetString("custommodel", path); return this; }

    public PlayerAnimation Play(char[] name = "")
    {
        this.player.SetMoveType(MOVETYPE_NONE);
        this.player.model.SetRenderMode(RENDER_NONE);
        this.Create("prop_dynamic");

        char tname[32];
        FormatEx(tname, sizeof(tname), "anim-%i", this.id);
        this.SetTargetName(tname);

        char modelpath[256];
        if (this.HasKey("custommodel"))
            this.GetString("custommodel", modelpath, sizeof(modelpath));
        else
            this.player.model.GetPath(modelpath, sizeof(modelpath));

        this.model.SetPath(modelpath);
        this.SetKV("solid", "0");
        //this.SetKV("rendermode", "10");

        this.Activate();
        this.Spawn();
        
        this.SetPos(this.player.GetPos(), this.player.GetAng());

        SetVariantString(tname);
        this.player.Input("SetParent");

        //int effects = this.parent.GetProp("m_fEffects");
        //effects |= 721;
        //this.parent.SetProp("m_fEffects", effects);
        
        SetVariantString(name);
        this.Input("SetAnimation");
    }

    public PlayerAnimation Stop()
    {
        this.player.Input("ClearParent", this.id, this.id);
        this.SetKV("OnUser1", "!self,Kill,,0.0,-1");
        this.Input("FireUser1");

        this.player.SetMoveType(MOVETYPE_WALK);
        this.player.model.SetRenderMode(RENDER_NORMAL);
    }

    public void Dispose()
    {
        this.Stop();
        this.model.Dispose();
        delete this;
    }
}