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

methodmap Animation < Entity {
    
    public Animation(Entity ent = null, char[] path = "") {
        Animation self = view_as<Animation>(new Entity());
        self.SetBool("spawned", false);

        if (ent)
        {
            self.Set("parent", ent);
            if (strlen(path))
                self.SetString("animsource", path);
            else
            {
                char modelname[256];
                ent.model.GetPath(modelname, sizeof(modelname));
                self.SetString("animsource", modelname);
            }
        }

        return self;
    }

    property Entity parent {
        public get() { return this.Get("parent"); }
    }

    public Animation Play(char[] name = "")
    {
        this.parent.SetMoveType(MOVETYPE_NONE);
        this.Create("prop_dynamic");

        char tname[32];
        FormatEx(tname, sizeof(tname), "anim-%i", this.id);
        this.SetTargetName(tname);

        char modelpath[256];
        this.GetString("animsource", modelpath, sizeof(modelpath));

        this.model.SetPath(modelpath);
        this.SetKV("solid", "0");
        this.SetKV("rendermode", "10");
        
        this.Activate();
        this.Spawn();
        
        this.SetPos(this.parent.GetPos(), this.parent.GetAng());

        SetVariantString(tname);
        this.parent.Input("SetParent");

        int effects = this.parent.GetProp("m_fEffects");
        effects |= 721;
        this.parent.SetProp("m_fEffects", effects);
        
        SetVariantString(name);
        this.Input("SetAnimation");
    }

    public Animation Stop()
    {
        this.parent.Input("ClearParent", this.id, this.id);
        this.SetKV("OnUser1", "!self,Kill,,1.0,-1");
        this.Input("FireUser1");

        this.parent.SetMoveType(MOVETYPE_WALK);
    }

    public void Dispose()
    {
        this.Stop();
        this.model.Dispose();
        delete this;
    }
}