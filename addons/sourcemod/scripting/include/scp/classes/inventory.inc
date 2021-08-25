/*----------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Player inventory---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Item < Base {

    public Item(char[] class) {
        Item self = view_as<Item>(new Base());
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

methodmap Inventory < Base {

    public Inventory() {
        Inventory self = view_as<Inventory>(CreateTrie());
        self.SetValue("inventory", new ArrayList());
        return self;
    }

    property ArrayList items {
        public get() { return this.GetList("inventory"); }
    }

    public bool Check(char[] val, int check) {
        ArrayList list = this.GetList("inventory");

        for (int i=0; i < list.Length; i++) {
            char itemname[32];
            view_as<Item>(list.Get(i)).GetEntClass(itemname, sizeof(itemname));

            EntConfig entcfg = gamemode.entities.get(itemname);
            if (entcfg && entcfg != null) {
                if (view_as<int>(entcfg.GetKeyType(val)) == 4) {
                    JSON_ARRAY arrvalues = view_as<JSON_ARRAY>(entcfg.GetObject(val));

                    for (int k=0; k < arrvalues.Length; k++)
                        if (arrvalues.GetInt(k, 0) == check)
                            return true;
                }
                else
                {
                    if (entcfg.GetInt(val, 0) == check)
                        return true;
                }
            }
        }

        return false;
    }

    public bool Add(char[] entclass) {
        ArrayList inv = this.GetList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
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

    public Item Drop(int index = 0) {
        ArrayList inv = this.GetList("inventory");

        Item itm = inv.Get(index);
        inv.Erase(index);

        return itm;
    }

    public void Clear() {
        this.GetList("inventory").Clear();
    }
}