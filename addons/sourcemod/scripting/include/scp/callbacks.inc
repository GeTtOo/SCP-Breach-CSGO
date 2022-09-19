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

//////////////////////////////////////////////////////////////////////////////
//
//                                Callbacks
//
//////////////////////////////////////////////////////////////////////////////

public Action CB_EntUse(int entity, int client)
{
    Player ply = player.GetByID(client);
    Entity ent = ents.Get(entity);

    if (ent.meta)
    {
        bool canpickup = true;

        Call_StartForward(OnPlayerPickupItemForward);
        Call_PushCellRef(ply);
        Call_PushCellRef(ent);
        Call_Finish(canpickup);

        if (!canpickup || ply.IsSCP && !ent.meta.SCPCanUse) return Plugin_Handled;
        
        if (ent.meta.onpickup)
        {
            char funcname[32];
            ent.meta.onpickup.name(funcname, sizeof(funcname));

            Call_StartFunction(ent.meta.onpickup.hndl, GetFunctionByName(ent.meta.onpickup.hndl, funcname));
            Call_PushCellRef(ply);
            Call_PushCellRef(ent);
            Call_Finish(canpickup);
        }

        if (canpickup)
            if (ply.inv.Pickup(ent))
            {
                ent.WorldRemove();
                ents.IndexUpdate(ent);
            }
            else
            {
                ply.PrintWarning("%t", "Inventory full");
            }
    }

    return Plugin_Continue;
}

public void CB_EntTouch(int firstentity, int secondentity)
{
    Entity ent1 = ents.Get(firstentity), ent2 = ents.Get(secondentity);

    if (ent1.meta && ent1.meta.ontouch && ent2)
    {
        char funcname[32];
        ent1.meta.ontouch.name(funcname, sizeof(funcname));

        Call_StartFunction(ent1.meta.ontouch.hndl, GetFunctionByName(ent1.meta.ontouch.hndl, funcname));
        Call_PushCellRef(ent2);
        Call_PushCellRef(ent1);
        Call_Finish();
    }
}

public Action SoundHandler(int clients[MAXPLAYERS], int &numClients, char sample[PLATFORM_MAX_PATH], int &entity, int &channel, float &volume, int &level, int &pitch, int &flags, char soundEntry[PLATFORM_MAX_PATH], int& seed)
{
	if (0 < entity <= MaxClients)
	{
		if (StrContains(sample, "physics") != -1 || StrContains(sample, "footsteps") != -1)
		{
			Player ply = player.GetByID(entity);
			
			if (ply && ply.class && ply.class.sound && ply.class.sound.HasKey("footsteps"))
			{
                JSON_ARRAY soundarr = ply.class.sound.GetArr("footsteps");

                int idx = GetRandomInt(0, ply.class.sound.GetArr("footsteps").Length - 1);
                char sound[128];

                if (soundarr.GetType(idx) == Object)
                {
                    if (timer.IsAlive(view_as<Tmr>(ply.GetHandle("footstepcd"))))
                        return Plugin_Stop;
                    else
                        ply.RemoveValue("footstepcd");

                    ply.SetHandle("footstepcd", ply.TimerSimple(soundarr.GetArr(idx).GetInt(1)));
                    soundarr.GetArr(idx).GetString(0, sound, sizeof(sound));
                }
                else
                    soundarr.GetString(idx, sound, sizeof(sound));

                ply.PlayAmbient(sound);

                return Plugin_Stop;
            }
		}
	}

	return Plugin_Continue;
}