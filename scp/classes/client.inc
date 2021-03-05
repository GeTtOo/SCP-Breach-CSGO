/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Client < StringMap {

    public Client(int id) {
        Client self = view_as<Client>(new StringMap());
        self.SetValue("id", id);
        self.SetValue("haveClass", false);

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

    property int Health {
        public set(int health) { SetEntityHealth(this.id, health); }
        public get() { return GetClientHealth(this.id); }
    }

    property int Armor {
        public set(int armor) { SetEntProp(this.id, Prop_Send, "m_ArmorValue", armor); }
        public get() { return GetEntProp(this.id, Prop_Send, "m_ArmorValue"); }
    }

    property bool haveClass {
        public set(bool value) { this.SetValue("haveClass", value); }
        public get() { bool value; return this.GetValue("haveClass", value) ? value : false; }
    }

    public void class(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("class", buffer);
        else
            this.GetString("class", buffer, max_size);
    }

    public void subclass(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("subclass", buffer);
        else
            this.GetString("subclass", buffer, max_size);
    }

    public void Kick(char[] reason) {
        KickClient(this.id, reason);
    }

    public void GetPos(float[3] pos) { GetClientAbsOrigin(this.GetInt("id"), pos); }
}

methodmap ClientSingleton < StringMap {

    public ClientSingleton() {
        ClientSingleton self = view_as<ClientSingleton>(new StringMap());
        Client clients[MAXPLAYERS + 1];
        self.SetArray("Clients", clients, sizeof(clients));
        
        return self;
    }
    
    public Client Get(int id) {
        Client clients[MAXPLAYERS + 1];
        this.GetArray("Clients", clients, sizeof(clients));
        if (id <= MAXPLAYERS)
            return view_as<Client>(clients[id]);
        else
            return null;
    }

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

    public int InGame() {
        int client = 0;
        while (IsClientInGame(client) && GetClientTeam(client) > 1)
            client++;
        return client;
    }

    public Client GetRandom() {
        Client clients[MAXPLAYERS + 1];
        this.GetArray("Clients", clients, sizeof(clients));
        int random = GetRandomInt(1, this.InGame());
        return view_as<Client>(clients[random]);
    }

    public Client GetRandomWithoutClass() {
        Client client = this.GetRandom();
        while (view_as<int>(client) != 0 && client.haveClass)
            client = this.GetRandom();

        return view_as<Client>(client);
    }
}