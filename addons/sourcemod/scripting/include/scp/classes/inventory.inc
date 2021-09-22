/*----------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Player inventory---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap InvItem < Base {

    public InvItem(char[] class) {
        InvItem self = view_as<InvItem>(new Base());
        self.SetString("entclass", class);
        return self;
    }

    property bool active {
        public set(bool val) { this.SetBool("active", val); }
        public get() { return this.GetBool("active", true); }
    }

    property bool disabled {
        public set(bool val) { this.SetBool("disabled", val); }
        public get() { return this.GetBool("disabled", false); }
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
            Entity item = list.Get(i);
            item.GetClass(itemname, sizeof(itemname));

            if (item.meta != null) {
                ArrayList checklist = item.meta.GetArrayList(val);

                if (checklist != null) {
                    for (int k=0; k < checklist.Length; k++)
                        if (checklist.Get(k) == check)
                            return true;
                }
                else
                {
                    if (item.meta.GetInt(val) == check)
                        return true;
                }
            }
        }

        return false;
    }

    public bool Add(char[] entclass) {
        ArrayList inv = this.GetArrayList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
            EntityMeta entdata = gamemode.meta.GetEntity(entclass);
    
            if (entdata != null)
            {
                Entity ent = new Entity();
                ent.meta = entdata;
                ent.spawned = false;
                ent.SetString("class", entclass);
                Base ents;
                gamemode.GetValue("ents", ents);
                ents.GetArrayList("entities").Push(ent);
                inv.Push(ent);

                return true;
            }
        }
        
        return false;
    }

    public bool Pickup(Entity item) {
        ArrayList inv = this.GetArrayList("inventory");

        if (inv.Length <= gamemode.config.invsize) {
            inv.Push(item);
            return true;
        }
        
        return false;
    }

    public bool Remove(char[] entname) {
        ArrayList inv = this.GetArrayList("inventory");
        char entclass[32];

        for (int i=0; i < inv.Length; i++) {
            Entity item = inv.Get(i);
            item.GetClass(entclass, sizeof(entclass));

            if (StrEqual(entclass, entname)) {
                inv.Erase(i);
                return true;
            }
        }
        
        return false;
    }

    public Entity Get(int index = 0) {
        return this.GetArrayList("inventory").Get(index);
    }

    public Entity Drop(int index = 0) {
        ArrayList inv = this.GetArrayList("inventory");

        Entity itm = inv.Get(index);
        inv.Erase(index);

        return itm;
    }

    public void Clear() {
        ArrayList items = this.GetArrayList("inventory");
        ArrayList ents = view_as<Base>(gamemode).GetBase("ents").GetArrayList("entities");

        for (int i=0; i < items.Length; i++) {
            Entity item = items.Get(i);
            ents.Erase(ents.FindValue(item));
        }
    }
}