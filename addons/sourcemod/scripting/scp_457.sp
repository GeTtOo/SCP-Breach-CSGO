#include <sourcemod>
#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
	name = "[SCP] 457",
	author = "GeTtOo, Andrey::Dono",
	description = "SCP-457",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerSpawn(Client &ply)
{
    if(ply.class.Is("457"))
    {
        ply.SetRenderMode(RENDER_NONE);
        
        Entity effect = (new Entity()).Create("info_particle_system");

        if(IsValidEdict(effect.id))
        {   
            effect.SetPos(ply.GetPos())
            .SetKV("targetname", "tf2particle")
            .SetKV("effect_name", "env_fire_large")
            .Spawn();
            
            SetVariantString("!activator");
            effect.Input("SetParent", ply)
            .Input("Start")
            .Activate();

            ply.SetBase("457_effect", effect);
        }
    }
}

public Action SCP_OnTakeDamage(Client &vic, Client &atk, float &damage, int &damagetype)
{
    if(atk.class.Is("457") && atk.id != vic.id)
    {
        IgniteEntity(vic.id, float(gamemode.config.pl.GetInt("ignitetime", 20)));
    }

    if(vic.class.Is("457") && damagetype == DMG_BURN)
    {
        return Plugin_Handled;
    }

    return Plugin_Continue;
}

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("457") && ply.InGame())
    {
        ply.SetRenderMode(RENDER_NORMAL);

        Entity ragdoll = ply.ragdoll;
        
        if (ragdoll)
            ragdoll.Remove();

        Entity effect = view_as<Entity>(ply.GetBase("457_effect"));
        
        if (effect)
            effect.Remove();
    }
}