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
	name = "[SCP] Turret",
	author = "Andrey::Dono, GeTtOo",
	description = "Automatic turret",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_RegisterMetaData() {
	gamemode.meta.RegEntEvent(ON_USE, "turret", "TurretDeploy");
}

public void SCP_OnLoad() {
	PrecacheModel("models/props_survival/dronegun/dronegun.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib1.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib2.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib3.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib4.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib5.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib6.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib7.mdl");
	PrecacheModel("models/props_survival/dronegun/dronegun_gib8.mdl");
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor)
{
	char clsname[64];
	GetEntityClassname(inflictor, clsname, sizeof(clsname));

	if(StrEqual("env_gunfire", clsname))
		damage = float(vic.class.health / 100 * 15);

	return Plugin_Changed;
}

public void TurretDeploy(Player &ply, InvItem &ent)
{
	Entity turret = ents.Create("dronegun");
	
	Vector pos = ply.GetAng().GetForwardVectorScaled(ply.EyePos(), 75.0);
	Vector plypos = ply.GetPos();
	pos.z = plypos.z;
	delete plypos;

	turret.SetPos(pos);
	turret.Spawn();
	turret.SetProp("m_iHealth", 300);
	turret.SetCollisionGroup(2);

	ply.inv.Remove(ent);
}