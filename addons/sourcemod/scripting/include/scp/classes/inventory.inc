/*----------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Player inventory---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Item < Base {

    public Item(char[] class) {
        Item self = view_as<Item>(new Base());
        self.SetString("class", class);
        return self;
    }

    property bool usable {
        public set(bool val) { this.SetBool("usable", val); }
        public get() { return this.GetBool("usable"); }
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
        Inventory inv = view_as<Inventory>(new Base());
        inv.CreateList("inventory");
        return inv;
    }

    property ArrayList items {
        public get() { return this.GetList("inventory"); }
    }

    public bool Check(char[] val, int check) {
        ArrayList list = this.GetList("inventory");

        for (int i=0; i < list.Length; i++) {
            char itemname[32];
            view_as<Item>(list.Get(i)).GetEntClass(itemname, sizeof(itemname));

            EntityMeta entdata = gamemode.meta.GetEntity(itemname);
            if (entdata != null) {
                ArrayList checklist = entdata.GetList(val);

                if (checklist != null) {
                    for (int k=0; k < checklist.Length; k++)
                        if (checklist.Get(k) == check)
                            return true;
                }
                else
                {
                    if (entdata.GetInt(val) == check)
                        return true;
                }
            }
        }

        return false;
    }

    public bool Add(char[] entclass) {
        ArrayList inv = this.GetList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
            EntityMeta entdata = gamemode.meta.GetEntity(entclass);

            char name[32];
            entdata.name(name, sizeof(name));

            Item itm = new Item(entclass);
            itm.name(name);

            inv.Push(itm);
            return true;
        }
        else
            return false;
    }

    public Item Get(int index = 0) {
        return this.GetList("inventory").Get(index);
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