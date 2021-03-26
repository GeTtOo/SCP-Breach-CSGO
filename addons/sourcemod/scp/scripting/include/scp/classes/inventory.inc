/*----------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Player inventory---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Item < StringMap {

    public Item(char[] class) {
        Item self = view_as<Item>(new StringMap());
        self.SetString("class", class);
        return self;
    }

    public Item SetEntClass(char[] class) {
        this.SetString("class", class);
        return this;
    }

    public Item GetEntClass(char[] class, int max_len) {
        this.GetString("class", class, max_len);
        return this;
    }
    
    public void name(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("name", buffer);
        else
            this.GetString("name", buffer, max_size);
    }
}

methodmap Inventory < StringMap {

    public Inventory() {
        Inventory self = view_as<Inventory>(new StringMap());
        self.SetValue("inventory", new ArrayList());
        return self;
    }

    public void Add(char[] entclass) {
        ArrayList inv;
        this.GetValue("inventory", inv);

        EntConfig entcfg = gamemode.entities.get(entclass);

        char name[32];
        entcfg.name(name, sizeof(name));

        Item itm = new Item(entclass);
        itm.name(name);

        inv.Push(itm);
    }

    public bool TryAdd(char[] entclass) {
        ArrayList inv;
        this.GetValue("inventory", inv);

        if (inv.Length <= 4) {
            EntConfig entcfg = gamemode.entities.get(entclass);

            char name[32];
            entcfg.name(name, sizeof(name));

            Item itm = new Item(entclass);
            itm.name(name);

            inv.Push(itm);
            return true;
        }
        else
            return false;
    }

    public Item Get(int id) {
        ArrayList inv;
        this.GetValue("inventory", inv);

        Item itm = view_as<Item>(inv.Get(id, 0));
        inv.Erase(id);
        this.SetValue("inventory", inv);

        return itm;
    }

    public void Clear() {
        ArrayList inv;
        this.GetValue("inventory", inv);
        inv.Clear();
    }
}