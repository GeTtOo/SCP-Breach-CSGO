/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
#pragma semicolon 1
#pragma newdecls required

#include <sdktools>
#include <cstrike>
#include <json>

#include "scp/classes/inventory"

methodmap Client < Entity {

    public Client(int id) {
        Client client = view_as<Client>(new Entity(id));
        client.SetValue("active", false);
        client.SetValue("haveсlass", false);
        client.SetValue("inventory", new Inventory());

        return client;
    }

    property int health {
        public set(int val) { SetEntityHealth(this.id, val); }
        public get() { return GetClientHealth(this.id); }
    }

    property int armor {
        public set(int val) { SetEntProp(this.id, Prop_Send, "m_ArmorValue", val); }
        public get() { return GetEntProp(this.id, Prop_Send, "m_ArmorValue"); }
    }

    property float speed {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed"); }
    }

    property float multipler {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue"); }
    }

    property bool haveclass {
        public set(bool value) { this.SetBool("haveсlass", value); }
        public get() { return this.GetBool("haveсlass", false); }
    }

    property Class class {
        public set(Class val) { this.SetValue("class", val); }
        public get() { Class val; return this.GetValue("class", val) ? val : null; }
    }

    property bool IsSCP {
        public get() { char team[32]; this.GetString("team", team, sizeof(team)); return StrEqual(team, "SCP"); }
    }

    property int access {
        public set(int card) { this.SetInt("access", card); }
        public get() { return this.GetInt("access", 0); }
    }

    property bool fullaccess {
        public set(bool card) { this.SetBool("fullaccess", card); }
        public get() { return this.GetBool("fullaccess", false); }
    }

    property bool spawned {
        public set(bool val) { this.SetBool("spawned", val); }
        public get() { return this.GetBool("spawned", false); }
    }

    property Inventory inv {
        public set(Inventory inv) { this.SetValue("inventory", inv); }
        public get() { Inventory inv; return this.GetValue("inventory", inv) ? inv : null; }
    }

    public void SetupBaseStats(Class class) {
        this.health = class.health;
        this.armor = class.armor;
        this.speed = class.speed;
        this.multipler = class.multipler;
    }

    public bool GetName(char[] buffer, int max_size = 0) { return GetClientName(this.id, buffer, max_size); }

    public bool IsAlive() { return IsPlayerAlive(this.id); }

    public Vector EyePos() { float vecarr[3]; GetClientEyePosition(this.id, vecarr); return new Vector(vecarr[0], vecarr[1], vecarr[2]); }

    public Angle EyeAngles() { float angarr[3]; GetClientEyeAngles(this.id, angarr); return new Angle(angarr[0], angarr[1], angarr[2]); }

    public void SetModel(char[] model) { SetEntityModel(this.id, model); }

    public void SetHandsModel(char[] model) { SetEntPropString(this.id, Prop_Send, "m_szArmsModel", model); }

    public void Team(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("team", buffer) : this.GetString("team", buffer, max_size); }

    public int Give(char[] item) { return GivePlayerItem(this.id, item); }

    public void RestrictWeapons() {
        int m_hMyWeapons_size = GetEntPropArraySize(this.id, Prop_Send, "m_hMyWeapons");
        int item;

        for(int index = 0; index < m_hMyWeapons_size; index++)
        { 
            item = GetEntPropEnt(this.id, Prop_Send, "m_hMyWeapons", index);

            if(item != -1)
            { 
                RemovePlayerItem(this.id, item);
                AcceptEntityInput(item, "Kill");
            } 
        }
    }

    //public void SetCollisionGroup(int group) { gamemode.mngr.SetCollisionGroup(this.id, group); }

    public void Kill() { ForcePlayerSuicide(this.id); }

    public void Kick(char[] reason) { KickClient(this.id, reason); }

    public void Setup() {
        this.SetupBaseStats(this.class);

        if (this.class.GetPos() != null)
            this.SetPos(this.class.GetPos(), this.class.GetAng());

        if (this.class.items != null)
            for (int i=0; i < this.class.items.Length; i++) {
                char entclass[32];
                this.class.items.GetString(i, entclass, sizeof(entclass));
                this.inv.Add(entclass);
            }

        if (this.class.weapons != null)
            for (int i=0; i < this.class.weapons.Length; i++) {
                if (view_as<int>(this.class.weapons.GetKeyType(i)) != 4) {
                    char weapon[32];
                    this.class.weapons.GetString(i, weapon, sizeof(weapon));
                    this.Give(weapon);
                }
                else
                {
                    JSON_ARRAY multigive = view_as<JSON_ARRAY>(this.class.weapons.GetObject(i));

                    for (int k=0; k < multigive.GetInt(1); k++)
                    {
                        char weapon[32];
                        multigive.GetString(0, weapon, sizeof(weapon));
                        this.Give(weapon);
                    }
                }
            }
        
        if (this.class.doors != null)
            for (int i=0; i < this.class.doors.Length; i++)
                AcceptEntityInput(this.class.doors.GetInt(i), "Open");

        if (this.class.HasKey("model")) {
            char playerModel[128];
            this.class.Model(playerModel, sizeof(playerModel));
            this.SetModel(playerModel);

            SetEntProp(this.id, Prop_Send, "m_nBody", this.GetInt("bodygroup"));
            SetEntProp(this.id, Prop_Send, "m_nSkin", this.GetInt("skin"));
        }

        if (this.class.HasKey("hands")) {
            char handsModel[128];
            this.class.Model(handsModel, sizeof(handsModel));
            this.SetHandsModel(handsModel);
        }
    }

    public void Spawn() {
        if (!IsPlayerAlive(this.id))
            CS_RespawnPlayer(this.id);
    }
}