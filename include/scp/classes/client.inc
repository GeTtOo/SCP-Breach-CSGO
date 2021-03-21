/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
#pragma semicolon 1
#pragma newdecls required

#include <sdktools>
#include <json>

#include "scp/classes/inventory"

methodmap Client < StringMap {

    public Client(int id) {
        Client self = view_as<Client>(new StringMap());
        self.SetValue("id", id);
        self.SetValue("haveClass", false);
        self.SetValue("inventory", new Inventory(id));

        return self;
    }

    public int GetInt(char[] index) { int buf; return this.GetValue(index, buf) ? buf : 0; }
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
        public set(bool value) { this.SetValue("haveClass", value); }
        public get() { bool value; return this.GetValue("haveClass", value) ? value : false; }
    }

    public void gclass(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("gclass", buffer);
        else
            this.GetString("gclass", buffer, max_size);
    }

    property Class class {
        public set(Class val) { this.SetValue("class", val); }
        public get() { Class obj; return this.GetValue("class", obj) ? obj : null; }
    }

    property bool IsSCP {
        public get() { char gclass[32]; this.GetString("gclass", gclass, sizeof(gclass)); return StrEqual(gclass, "SCP"); }
    }

    property int access {
        public set(int card) { this.SetValue("access", card); }
        public get() { int card; return this.GetValue("access", card) ? card : 0; }
    }

    property bool FullAccess {
        public set(bool card) { this.SetValue("fullaccess", card); }
        public get() { bool card; return this.GetValue("fullaccess", card) ? card : false; }
    }

    property Inventory inventory {
        public set(Inventory inv) { this.SetValue("inventory", inv); }
        public get() { Inventory inv; return this.GetValue("inventory", inv) ? inv : null; }
    }

    public void Kick(char[] reason) {
        KickClient(this.id, reason);
    }

    public Vector GetPos() { float pos[3]; GetEntPropVector(this.GetInt("id"), Prop_Send, "m_vecOrigin", pos); return new Vector(pos[0], pos[1], pos[2]); }

    public void Spawn() {
        Class class = this.class;

        this.health = class.health;
        this.armor = class.armor;
        this.speed = class.speed;

        char playerModel[128], playerHandsModel[128];

        class.Model(playerModel, sizeof(playerModel));
        class.HandsModel(playerHandsModel, sizeof(playerHandsModel));

        //this.SetModel(playerModel);
        //this.SetHandsModel(playerHandsModel);
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
        view_as<Client>(clients[id]) = view_as<Client>(new Client(id));
        this.SetArray("Clients", clients, sizeof(clients));
    }

    public Client Remove(int id) {
        Client clients[MAXPLAYERS + 1];
        this.GetArray("Clients", clients, sizeof(clients));
        delete view_as<Client>(clients[id]);
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