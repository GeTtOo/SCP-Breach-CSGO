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
/*--------------------------------------------------Status effect base--------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap StatusEffect < Base {

    public StatusEffect(Handle ply, char[] name, int time) {
        StatusEffect effect = view_as<StatusEffect>(new Base());
        effect.SetHandle("player", ply);
        StatusEffectMeta sem = gamemode.meta.GetStatusEffect(name);
        effect.SetHandle("meta", sem);
        effect.SetString("name", name);
        effect.SetInt("time", time);
        effect.SetInt("count", RoundToCeil(float(time) * sem.rate));
        effect.SetFloat("nextinvoke", GetGameTime());

        return effect;
    }

    property Handle ply {
        public get() { return this.GetHandle("player"); }
    }

    property bool inited {
        public set(bool val) { this.SetBool("inited", val); }
        public get() { return this.GetBool("inited"); }
    }

    property StatusEffectMeta meta {
        public set(StatusEffectMeta val) { this.SetValue("meta", val); }
        public get() { return view_as<StatusEffectMeta>(this.GetHandle("meta")); }
    }

    property int time {
        public set(int val) { this.SetInt("time", val); }
        public get() { return this.GetInt("time"); }
    }

    property int count {
        public set(int val) { this.SetInt("count", val); }
        public get() { return this.GetInt("count"); }
    }

    property float nextinvoke {
        public set(float time) { this.SetFloat("nextinvoke", time); }
        public get() { return this.GetFloat("nextinvoke"); }
    }

    public void name(char[] name, int max_len) { this.GetString("name", name, max_len); }

    public void Dispose() {
        delete this;
    }
}

methodmap StatusEffectSingleton < Base {
    
    public StatusEffectSingleton()
    {
        StatusEffectSingleton ses = view_as<StatusEffectSingleton>(new Base());
        ses.CreateArrayList("list");

        return ses;
    }

    property ArrayList list {
        public native get();
    }

    public bool IsHave(Handle ply, char[] name)
    {
        for (int i=0; i < this.list.Length; i ++)
        {
            StatusEffect se = this.list.Get(i);
            
            char sen[64];
            se.name(sen, sizeof(sen));

            if (se.ply == ply && StrEqual(name, sen))
            {
                return true;
            }
        }

        return false;
    }

    public void Create(Handle ply, char[] name, int time)
    {
        if (!this.IsHave(ply, name))
            this.list.Push(new StatusEffect(ply, name, time));
    }

    public StatusEffect Get(Handle ply, char[] name)
    {
        for (int i=0; i < this.list.Length; i ++)
        {
            StatusEffect se = this.list.Get(i);
            
            char sen[64];
            se.name(sen, sizeof(sen));

            if (se.ply == ply && StrEqual(name, sen))
            {
                return se;
            }
        }

        return null;
    }

    public bool Remove(Handle ply, char[] name)
    {
        for (int i=0; i < this.list.Length; i ++)
        {
            StatusEffect se = this.list.Get(i);
            
            char sen[64];
            se.name(sen, sizeof(sen));

            if (se.ply == ply && StrEqual(name, sen))
            {
                char funcname[64];
                
                se.meta.end.name(funcname, sizeof(funcname));
                Format(funcname, sizeof(funcname), "%s_Force", funcname);

                if (GetFunctionByName(se.meta.end.hndl, funcname) != INVALID_FUNCTION)
                {
                    Call_StartFunction(se.meta.end.hndl, GetFunctionByName(se.meta.end.hndl, funcname));
                    Call_PushCell(se.ply);
                    Call_Finish();
                }

                this.list.Erase(i);
                delete se;
                return true;
            }
        }

        return false;
    }

    public void ClearAll()
    {
        this.list.Clear();
    }

    public void Update()
    {
        for (int i=0; i < this.list.Length; i++)
        {
            StatusEffect se = this.list.Get(i);
            
            if (GetGameTime() >= se.nextinvoke)
            {
                char funcname[64];
            
                if (!se.inited)
                {
                    if (se.meta.init)
                    {
                        se.meta.init.name(funcname, sizeof(funcname));
                        
                        Call_StartFunction(se.meta.init.hndl, GetFunctionByName(se.meta.init.hndl, funcname));
                        Call_PushCell(se.ply);
                        Call_Finish();
                    }
                    
                    se.inited = true;
                }

                if (se.count > 0)
                {
                    if (se.meta.update)
                    {
                        se.meta.update.name(funcname, sizeof(funcname));
                        
                        Call_StartFunction(se.meta.update.hndl, GetFunctionByName(se.meta.update.hndl, funcname));
                        Call_PushCell(se.ply);
                        Call_Finish();
                    }

                    se.count--;
                    se.nextinvoke += (1.0 / se.meta.rate);

                    if (se.count == 0)
                    {
                        if (se.meta.end)
                        {
                            se.meta.end.name(funcname, sizeof(funcname));
                            
                            Call_StartFunction(se.meta.end.hndl, GetFunctionByName(se.meta.end.hndl, funcname));
                            Call_PushCell(se.ply);
                            Call_Finish();
                        }
                        
                        char name[64];
                        se.name(name, sizeof(name));
                        this.Remove(se.ply, name);
                    }
                }
            }
        }
    }
}