/*----------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Player inventory---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap InvItem < Base {

    public InvItem(char[] class) {
        InvItem self = view_as<InvItem>(new Base());
        self.SetString("entclass", class);
        return self;
    }

    public InvItem SetEntClass(char[] class) {
        this.SetString("entclass", class);
        return this;
    }

    public InvItem GetEntClass(char[] class, int max_len) {
        this.GetString("entclass", class, max_len);
        return this;
    }
}

methodmap Inventory < Base {

    public Inventory() {
        Inventory inv = view_as<Inventory>(new Base());
        inv.CreateArrayList("inventory");
        return inv;
    }

    property ArrayList items {
        public get() { return this.GetArrayList("inventory"); }
    }

    public bool Check(char[] val, int check) {
        ArrayList list = this.GetArrayList("inventory");

        for (int i=0; i < list.Length; i++) {
            char itemname[32];
            view_as<InvItem>(list.Get(i)).GetEntClass(itemname, sizeof(itemname));

            EntityMeta entdata = gamemode.meta.GetEntity(itemname);
            if (entdata != null) {
                ArrayList checklist = entdata.GetArrayList(val);

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
        ArrayList inv = this.GetArrayList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
            inv.Push(new InvItem(entclass));
            return true;
        }
        
        return false;
    }

    public bool Remove(char[] entname) {
        ArrayList inv = this.GetArrayList("inventory");
        char entclass[32];

        for (int i=0; i < inv.Length; i++) {
            InvItem item = inv.Get(i);
            item.GetEntClass(entclass, sizeof(entclass));

            if (StrEqual(entclass, entname)) {
                inv.Erase(i);
                return true;
            }
        }
        
        return false;
    }

    public InvItem Get(int index = 0) {
        return this.GetArrayList("inventory").Get(index);
    }

    public InvItem Drop(int index = 0) {
        ArrayList inv = this.GetArrayList("inventory");

        InvItem itm = inv.Get(index);
        inv.Erase(index);

        return itm;
    }

    public void Clear() {
        this.GetArrayList("inventory").Clear();
    }
}