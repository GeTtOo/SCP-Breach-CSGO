/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
#pragma semicolon 1
#pragma newdecls required

#include <sdktools>
#include <cstrike>
#include <json>

#include "scp/classes/inventory"

methodmap Client < Base {

    public Client(int id) {
        Client self = view_as<Client>(new StringMap());
        self.SetValue("id", id);
        self.SetValue("haveClass", false);
        self.SetValue("inventory", new Inventory());

        return self;
    }
    ///////////////////////////Logic in player object/////////////////////////////////

    property int id {
        public get() { return this.GetInt("id"); }
    }

    public bool GetName(char[] buffer, int max_size = 0) {
        return GetClientName(this.id, buffer, max_size);
    }

    public void SetModel(char[] model) {
        SetEntityModel(this.id, model);
    }

    public void SetHandsModel(char[] model) {
        SetEntPropString(this.id, Prop_Send, "m_szArmsModel", model);
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
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue"); }
    }

    property bool haveClass {
        public set(bool value) { this.SetBool("haveClass", value); }
        public get() { return this.GetBool("haveClass", false); }
    }

    public void team(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("team", buffer);
        else
            this.GetString("team", buffer, max_size);
    }

    property Class class {
        public set(Class val) { this.SetValue("class", val); }
        public get() { Class obj; return this.GetValue("class", obj) ? obj : null; }
    }

    property bool IsSCP {
        public get() { char team[32]; this.GetString("team", team, sizeof(team)); return StrEqual(team, "SCP"); }
    }

    property int access {
        public set(int card) { this.SetInt("access", card); }
        public get() { return this.GetInt("access", 0); }
    }

    property bool FullAccess {
        public set(bool card) { this.SetBool("fullaccess", card); }
        public get() { return this.GetBool("fullaccess", false); }
    }

    property Inventory inv {
        public set(Inventory inv) { this.SetValue("inventory", inv); }
        public get() { Inventory inv; return this.GetValue("inventory", inv) ? inv : null; }
    }

    public void Kick(char[] reason) {
        KickClient(this.id, reason);
    }

    public void SetPos(Vector vec) {
        float vectorArr[3];
        vec.GetArr(vectorArr);
        TeleportEntity(this.id, vectorArr, NULL_VECTOR, NULL_VECTOR);
        delete vec;
    }

    public Vector GetPos() { float pos[3]; GetEntPropVector(this.GetInt("id"), Prop_Send, "m_vecOrigin", pos); return new Vector(pos[0], pos[1], pos[2]); }

    public void Give(char[] item) { GivePlayerItem(this.id, item); }

    public void Spawn() {
        this.health = this.class.health;
        this.armor = this.class.armor;
        this.speed = this.class.speed;

        if (this.class.GetPos() != null)
            this.SetPos(this.class.GetPos());

        if (this.class.items != null)
            for (int i=0; i < this.class.items.Length; i++) {
                char entclass[32];
                this.class.items.GetString(i, entclass, sizeof(entclass));
                this.inv.TryAdd(entclass);
            }

        if (this.class.weapons != null)
            for (int i=0; i < this.class.weapons.Length; i++) {
                char weapon[32];
                this.class.weapons.GetString(i, weapon, sizeof(weapon));
                this.Give(weapon);
            }
        
        if (this.class.doors != null)
            for (int i=0; i < this.class.doors.Length; i++)
                AcceptEntityInput(this.class.doors.GetInt(i), "Open");

        char playerModel[128], playerHandsModel[128];

        this.class.Model(playerModel, sizeof(playerModel));
        this.class.HandsModel(playerHandsModel, sizeof(playerHandsModel));

        //this.SetModel(playerModel);
        //this.SetHandsModel(playerHandsModel);

        if (!IsPlayerAlive(this.id))
            CS_RespawnPlayer(this.id);
    }
}

methodmap ClientSingleton < StringMap {

    public ClientSingleton() {
        ClientSingleton self = view_as<ClientSingleton>(new StringMap());
        Client clients[MAXPLAYERS + 1];
        self.SetArray("Clients", clients, sizeof(clients));
        
        return self;
    }
    
    public native Client Get(int id);

    public Client Add(int id) {
        Client clients[MAXPLAYERS + 1];
        this.GetArray("Clients", clients, sizeof(clients));
        clients[id] = view_as<Client>(new Client(id));
        this.SetArray("Clients", clients, sizeof(clients));
    }

    public Client Remove(int id) {
        Client clients[MAXPLAYERS + 1];
        this.GetArray("Clients", clients, sizeof(clients));
        delete clients[id];
        this.SetArray("Clients", clients, sizeof(clients));
    }

    property int len { 
        public get() { return MAXPLAYERS + 1; }
    }

    public native int InGame();

    public native int Alive();

    public native Client GetRandom();

    public Client GetRandomWithoutClass() {
        Client client = this.GetRandom();
        int crutch = 0; // :c
        while (view_as<int>(client) != 0 && client.haveClass) {
            if (crutch == 500) break;
            client = this.GetRandom();
            crutch++;
        }

        return view_as<Client>(client);
    }
}