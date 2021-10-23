#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 106",
    author = "Andrey::Dono",
    description = "SCP-106 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerJoin(Client &ply) {

}

public void SCP_OnPlayerClear(Client &ply) {
    SDKUnhook(ply.id, SDKHook_StartTouch, CheckSurface);
}

public void SCP_OnPlayerSpawn(Client &ply) {
    if (ply != null && ply.class != null && ply.class.Is("106"))
    {
        SDKHook(ply.id, SDKHook_StartTouch, CheckSurface);
    }
}

public void Scp_OnRoundEnd()
{
    for (int i=0; i < Clients.Length; i++)
    {
        Client ply = Clients.Get(i);
        
        if (ply != null && ply.class != null && ply.class.Is("106"))
            SDKUnhook(ply.id, SDKHook_StartTouch, CheckSurface);
    }
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    
}

public void CheckSurface(int client, int entity) {

    char className[32];
    GetEntityClassname(entity, className, sizeof(className));
    
    if (StrEqual(className, "prop_dynamic"))
    {
        int entid = GetEntPropEnt(entity, Prop_Data, "m_hMoveParent");
        Entity model = (entid != -1) ? new Entity(entid) : null;

        char dcn[32];

        model.GetClass(dcn, sizeof(dcn));

        if (StrEqual(dcn, "func_door") || StrEqual(dcn, "func_door_rotating") || StrEqual(dcn, "prop_door_rotating")) {
            if (gamemode.config.debug)
                PrintToChat(client, "Ded touched door. (ID: %i)", entity);

            int entidq = GetEntPropEnt(model.id, Prop_Data, "m_hMoveChild");
            Entity idpad = (entidq != -1) ? new Entity(entidq) : null;

            Client ply = Clients.Get(client);

            char t[32];
            FormatEx(t, sizeof(t), "Test_1234_%i", ply.id);
            
            ply.SetPos(ply.GetPos().GetFromPoint(idpad.GetPos()).Normalize().Scale(70.0));

            //ply.SetPos(ply.GetAng().Forward(ply.GetPos(), 70.0));
        }
    }
}

public void Test(Entity ply)
{
    ply.SetPos(ply.GetAng().Forward(ply.GetPos(), float(5)));
}