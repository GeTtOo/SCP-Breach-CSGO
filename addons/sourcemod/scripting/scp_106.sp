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

#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 106",
    author = "Andrey::Dono",
    description = "SCP-106 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public Action SoundHandler(int clients[MAXPLAYERS], int &numClients, char sample[PLATFORM_MAX_PATH], int &entity, int &channel, float &volume, int &level, int &pitch, int &flags, char soundEntry[PLATFORM_MAX_PATH], int& seed)
{
	if (0 < entity <= MaxClients)
	{
		if (StrContains(sample, "physics") != -1 || StrContains(sample, "footsteps") != -1)
		{
			Player ply = player.GetByID(entity);
			
			if (ply && ply.class && ply.class.Is("106"))
			{
				char sound[128];
				JSON_ARRAY sarr = gamemode.plconfig.GetObject("sound").GetArray("steps");
				sarr.GetString(GetRandomInt(0, sarr.Length - 1), sound, sizeof(sound));
				ply.PlayAmbient(sound);
				//EmitSound(clients, numClients, sound, entity, channel, level, flags, volume, pitch);
				
				return Plugin_Stop;
			}
		}
	}

	return Plugin_Continue;
}

public void SCP_OnLoad() {
    AddNormalSoundHook(SoundHandler);
    HookEntityOutput("trigger_hurt", "OnStartTouch", OnTriggerActivated);
}

public void SCP_OnUnload() {
    RemoveNormalSoundHook(SoundHandler);
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply.class.Is("106")) SDKHook(ply.id, SDKHook_StartTouch, CheckSurface);
}

public void SCP_OnPlayerClear(Player &ply) {
    if (ply.class && ply.class.Is("106"))
    {
        SDKUnhook(ply.id, SDKHook_StartTouch, CheckSurface);
        if (ply.GetHandle("106_tmrpdfo")) gamemode.timer.Remove(view_as<Tmr>(ply.GetHandle("106_tmrpdfo")));
        delete ply.GetHandle("106_tp_vec");
        delete ply.GetHandle("106_tp_ang");
        ply.RemoveValue("106_tp_vec");
        ply.RemoveValue("106_tp_ang");
        ply.RemoveValue("106_tplock");
        ply.RemoveValue("106_inpd");
        ply.RemoveValue("106_lock");
        ply.RemoveValue("106_tmrpdfo");
    }
    
    if (ply.class && !ply.IsSCP) ply.RemoveValue("106_inpd");
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor) {
    if (vic.class.Is("106"))
    {
        switch (damagetype)
        {
            case DMG_GENERIC:
            {
                if (vic.health < vic.class.health) ((vic.health + 2) >= vic.class.health) ? (vic.health = vic.class.health) : (vic.health += 2);
                return Plugin_Handled;
            }
            case DMG_CRUSH:
            {
                if (vic.GetBool("106_inpd") && !vic.GetBool("106_tplock"))
                {
                    vic.SetPos(gamemode.plconfig.GetVector("pocket") - new Vector(0.0, 0.0, 100.0), new Angle(0.0, 0.0, 0.0));
                    int idposrnd = GetRandomInt(0, gamemode.plconfig.GetObject("pocketout").GetArray("scp").Length - 1);
                    Vector vec = view_as<JSON_OBJECT>(gamemode.plconfig.GetObject("pocketout").GetArray("scp").GetObject(idposrnd)).GetVector("vec");
                    Angle ang = view_as<JSON_OBJECT>(gamemode.plconfig.GetObject("pocketout").GetArray("scp").GetObject(idposrnd)).GetAngle("ang");
                    SCP_106_TeleportToPos(vic, vec - new Vector(0.0, 0.0, 64.0), ang);
                    vic.SetBool("106_inpd", false);
                    
                    if (vic.GetHandle("106_tmrpdfo")) gamemode.timer.Remove(view_as<Tmr>(vic.GetHandle("106_tmrpdfo")));

                    return Plugin_Handled;
                }
                else if (damage > 25000.0) damage = 1000.0;

                return Plugin_Changed;
            }
            default:
            {
                if (!gamemode.nuke.IsNuked)
                    damage /= 7.5;
                else
                    damage /= 1.5;

                return Plugin_Changed;
            }
        }
    }

    if (vic.GetBool("106_inpd") && damagetype == DMG_CRUSH)
    {
        if (GetRandomInt(1, 100) < gamemode.plconfig.GetInt("escapechanceofpocket", 25))
        {
            vic.SetPos(gamemode.plconfig.GetVector("pocket") - new Vector(0.0, 0.0, 100.0), new Angle(0.0, 0.0, 0.0));
            int idposrnd = GetRandomInt(0, gamemode.plconfig.GetObject("pocketout").GetArray("people").Length - 1);
            Vector vec = view_as<JSON_OBJECT>(gamemode.plconfig.GetObject("pocketout").GetArray("people").GetObject(idposrnd)).GetVector("vec");
            Angle ang = view_as<JSON_OBJECT>(gamemode.plconfig.GetObject("pocketout").GetArray("people").GetObject(idposrnd)).GetAngle("ang");
            SCP_106_TeleportToPos(vic, vec - new Vector(0.0, 0.0, 64.0), ang);
            vic.SetBool("106_inpd", false);
            return Plugin_Handled;
        }
        
        return Plugin_Continue;
    }

    if (!atk || !atk.class) return Plugin_Continue;

    if (atk.class.Is("106") && vic.IsClass("player") && !vic.GetBool("106_inpd"))
    {
        if (gamemode.nuke.IsNuked)
        {
            damage = 5000.0;
            return Plugin_Changed;
        }
        
        SCP_106_TeleportToPos(vic, gamemode.plconfig.GetVector("pocket") - new Vector(0.0, 0.0, 64.0), new Angle(0.0, 0.0, 0.0));
        SpawnBlob(vic);
        vic.SetBool("106_inpd", true);
        return Plugin_Handled;
    }

    return Plugin_Continue;
}

public Action OnTriggerActivated(const char[] output, int caller, int activator, float delay)
{
    char triggername[32];
    GetEntPropString(caller, Prop_Data, "m_iName", triggername, sizeof(triggername));

    if (StrEqual(triggername, "scp_106_trigger") && !player.GetByID(activator).IsSCP)
    {
        ArrayList players = player.GetAll();
    
        for (int i=0; i < player.Length; i++)
        {
            Player ply = players.Get(i);

            if (ply && ply.class && ply.class.Is("106") && !ply.GetBool("106_lock"))
            {
                ply.TimerSimple(10000, "SCP_106_Locking", ply);
                ply.SetBool("106_lock", true);
            }
        }

        delete players;

        return Plugin_Continue;
    }
    else
    {
        return Plugin_Handled;
    }
}

public void CheckSurface(int client, int entity)
{
    char className[32];
    GetEntityClassname(entity, className, sizeof(className));
    
    if (StrEqual(className, "prop_dynamic"))
    {
        int doorid = GetEntPropEnt(entity, Prop_Data, "m_hMoveParent");
        Entity door = (doorid != -1 && !DoorIsBlock(doorid)) ? new Entity(doorid) : null;

        if (door && (door.IsClass("func_door") || door.IsClass("prop_door_rotating"))) // door.IsClass("func_door_rotating")
        {

            Player ply = player.GetByID(client);
            
            if (!ply.GetArrayList("106_sdw")) // Smooth door walk
            {
                ArrayList smoothtp = new ArrayList();

                Vector doorforward;
                if (!door.IsClass("prop_door_rotating"))
                {
                    Vector doordir = door.GetPropVector("m_vecMoveDir", Prop_Data);
                    doorforward = (FloatAbs(doordir.x) < 0.3) ? new Vector(1.0,0.0,0.0) : new Vector(0.0,1.0,0.0); //Don't judge me for that :d
                    delete doordir;
                }
                else
                {
                    Angle doorang = door.GetPropAngle("m_angRotation");
                    doorforward = (RoundToCeil(doorang.y) % 180 < 45) ? new Vector(1.0,0.0,0.0) : new Vector(0.0,1.0,0.0);
                    delete doorang;
                }

                Vector target = ply.GetPos() - doorforward.Clone() * doorforward.DotProduct(ply.GetPos() - door.GetPos());

                for (float i=0.1; i <= 2.2; i+=0.1) smoothtp.Push(ply.GetPos().Lerp(target.Clone(), i));

                delete target;

                ply.SetArrayList("106_sdw", smoothtp);

                char timername[64];
                FormatEx(timername, sizeof(timername), "SCP-106-DoorWalk-%i", ply.id);
                gamemode.timer.Create(timername, 10, 24, "DoorWalk", ply);
            }

            door.Dispose();

        }
    }
}

public void DoorWalk(Player ply)
{
    ArrayList list = ply.GetArrayList("106_sdw");

    if (list && list.Length > 0)
    {
        Vector vec = list.Get(0);
        ply.SetPos(vec);
        list.Erase(0);
    }
    else
    {
        delete list;
        char timername[64];
        FormatEx(timername, sizeof(timername), "SCP-106-DoorWalk-%i", ply.id);
        gamemode.timer.RemoveByName(timername);
        ply.RemoveValue("106_sdw");
    }
}

public void SCP_106_TeleportToPos(Player ply, Vector vec, Angle ang)
{
    ply.SetBool("106_tplock", true);
    
    int height = 65;

    ArrayList pointsin = new ArrayList();
    ArrayList pointsout = new ArrayList();

    for (int i=1; i <= 65; i++)
    {
        Vector vin = ply.GetPos();
        vin.z -= i;
        pointsin.Push(vin);

        Vector vout = vec.Clone();
        vout.z += i;
        pointsout.Push(vout);
    }
    
    delete vec;

    char timername[64];

    Base data = new Base();
    data.SetHandle("player", ply);
    data.SetArrayList("points", pointsin);
    data.SetArrayList("pointsout", pointsout);
    data.SetInt("pointslen", height);
    data.SetHandle("angle", ang);
    data.SetBool("fs", true);

    FormatEx(timername, sizeof(timername), "SCP-106-Teleport-%i", ply.id);
    gamemode.timer.Create(timername, 25, height * 2, "SCP_106_TeleportHandler", data);
}

public void SCP_106_TeleportHandler(Base data)
{
    Player ply = view_as<Player>(data.GetHandle("player"));
    ArrayList points = data.GetArrayList("points");
    int plen = data.GetInt("pointslen");

    if (points.Length == plen)
    {
        char sound[128];
        JSON_ARRAY sarr = gamemode.plconfig.GetObject("sound").GetArray("portal");
        sarr.GetString(GetRandomInt(0, sarr.Length - 1), sound, sizeof(sound));
        ply.PlayAmbient(sound);
    }

    if (points.Length == plen && !data.GetBool("fs"))
        ply.SetPos(_, view_as<Angle>(data.GetHandle("angle")));

    ply.SetPos(points.Get(0));
    points.Erase(0);

    if (points.Length == 0 && data.GetBool("fs"))
    {
        delete points;
        data.SetArrayList("points", data.GetArrayList("pointsout"));
        data.SetBool("fs", false);
    }

    if (points && points.Length == 0 && !data.GetBool("fs"))
    {
        delete points;
        delete data;
        ply.SetBool("106_tplock", false);
    }
}

public void SCP_OnCallAction(Player &ply) {
    if (ply.class.Is("106"))
        ActionsMenu(ply);
}

public void SCP_OnAlphaWarhead(AlphaWarhead status) {
    if (status == Nuked)
    {
        ArrayList players = player.GetAll();

        for (int i=0; i < players.Length; i++)
        {
            Player ply = players.Get(i);

            if (ply && ply.class && ply.class.Is("106"))
            {
                delete ply.GetHandle("106_tp_vec");
                delete ply.GetHandle("106_tp_ang");
                ply.RemoveValue("106_tp_vec");
                ply.RemoveValue("106_tp_ang");

                if (ply.GetBool("106_inpd"))
                {
                    if (ply.GetHandle("106_tmrpdfo")) gamemode.timer.Remove(view_as<Tmr>(ply.GetHandle("106_tmrpdfo")));
                    ply.RemoveValue("106_tmrpdfo");

                    SCP_106_TeleportToPos(ply, gamemode.plconfig.GetObject("exitposafternuke").GetVector("vec") - new Vector(0.0, 0.0, 64.0), gamemode.plconfig.GetObject("exitposafternuke").GetAngle("ang"));
                    ply.SetBool("106_inpd", false);
                }
            }
        }

        delete players;
    }
}

public void SpawnBlob(Player ply)
{
    /*Entity blob = new Entity();
    blob.Create("env_sprite_oriented");
    blob.SetKV("model", "materials/eternity/blob.vmt");
    blob.SetKV("classname", "env_sprite_oriented");
    blob.SetKV("rendercolor", "255 255 255");
    blob.SetKV("scale", "0.25");
    blob.SetKV("rendermode", "0");
    blob.SetKV("spawnflags", "1");

    blob.SetPos(ply.GetPos());
    blob.Spawn();
    blob.SetPos(_, new Angle(0.0, 180.0, 0.0));
    blob.Dispose();*/
    
    float pos[3];
    ply.GetPos().GetArrD(pos);

    TE_Start("BSP Decal");
    TE_WriteVector("m_vecOrigin", pos);
    TE_WriteNum("m_nEntity", 0);
    TE_WriteNum("m_nIndex", PrecacheDecal("eternity/blob"));
    TE_SendToAll();
}

public void ForcedTpFromPD(Player ply)
{
    SCP_106_TeleportToPos(ply, view_as<Vector>(ply.GetHandle("106_tp_vec")).Clone(), view_as<Angle>(view_as<Angle>(ply.GetHandle("106_tp_ang")).Clone()));
    ply.SetBool("106_inpd", false);
}

public void SCP_106_Locking(Player ply)
{
    SCP_106_TeleportToPos(ply, gamemode.plconfig.GetObject("cell").GetVector("vec")- new Vector(0.0,0.0,64.0), gamemode.plconfig.GetObject("cell").GetAngle("ang"));
}

public bool DoorIsBlock(int doorid)
{
    JSON_ARRAY blockdoors = gamemode.plconfig.GetArray("blockdoors");
    
    for (int i=0; i < blockdoors.Length; i++)
        if (blockdoors.GetInt(i) == doorid)
            return true;

    return false;
}

public void ActionsMenu(Player ply) {
    Menu hndl = new Menu(ActionsMenuHandler);

    hndl.SetTitle("SCP-106 Actions");

    hndl.AddItem("1", "Set point", (!ply.GetBool("106_lock") && !ply.GetBool("106_tplock") && !ply.GetBool("106_inpd")) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    hndl.AddItem("2", "Move to point", (ply.GetHandle("106_tp_vec") && !ply.GetBool("106_lock") && !ply.GetBool("106_tplock")) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    if (gamemode.plconfig.GetInt("timeforcedpocketexit") >= 0 && (!gamemode.plconfig.GetBool("pocketviponly") || (gamemode.plconfig.GetBool("pocketviponly") && ply.GetBool("IsVIP"))))
        hndl.AddItem("3", "Move to a pocket dimension", (ply.GetHandle("106_tp_vec") && !ply.GetBool("106_lock") && !ply.GetBool("106_tplock") && !ply.GetBool("106_inpd") && !gamemode.nuke.IsNuked) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    
    hndl.Display(ply.id, 30);
}

public int ActionsMenuHandler(Menu hMenu, MenuAction action, int client, int idx) 
{
    switch (action)
    {
        case MenuAction_Select:
        {
            Player ply = player.GetByID(client);
            
            if (ply.GetPropEntId("m_hGroundEntity") == 0)
                switch (idx)
                {
                    case 0:
                    {
                        delete ply.GetHandle("106_tp_vec");
                        delete ply.GetHandle("106_tp_ang");
                        
                        ply.SetHandle("106_tp_vec", ply.GetPos() - new Vector(0.0, 0.0, 64.0));
                        ply.SetHandle("106_tp_ang", ply.GetAng());

                        SpawnBlob(ply);
                    }
                    case 1:
                    {
                        SCP_106_TeleportToPos(ply, view_as<Vector>(ply.GetHandle("106_tp_vec")).Clone(), view_as<Angle>(view_as<Angle>(ply.GetHandle("106_tp_ang")).Clone()));
                        if (!ply.GetBool("106_inpd"))
                            SpawnBlob(ply);
                        else
                            if (ply.GetHandle("106_tmrpdfo")) gamemode.timer.Remove(view_as<Tmr>(ply.GetHandle("106_tmrpdfo")));

                        ply.SetBool("106_inpd", false);
                    }
                    case 2:
                    {
                        SCP_106_TeleportToPos(ply, gamemode.plconfig.GetVector("pocket") - new Vector(0.0, 0.0, 64.0), new Angle(0.0, 0.0, 0.0));
                        if (!ply.GetBool("106_inpd")) SpawnBlob(ply);
                        ply.SetBool("106_inpd", true);
                        
                        if (gamemode.plconfig.GetInt("timeforcedpocketexit") > 0)
                            ply.SetHandle("106_tmrpdfo", ply.TimerSimple(gamemode.plconfig.GetInt("timeforcedpocketexit") * 1000, "ForcedTpFromPD", ply));
                    }
                }
        }
    }

    return 0;
}