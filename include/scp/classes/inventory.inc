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

    public Inventory(int id) {
        Inventory self = view_as<Inventory>(new StringMap());
        self.SetValue("id", id);
        self.SetValue("inventory", new ArrayList());
        return self;
    }

    public void Add(Entity ent) {
        ArrayList inv;
        this.GetValue("inventory", inv);

        char class[32];
        ent.GetClass(class, sizeof(class));

        Item itm = new Item(class);

        inv.Push(itm);
    }

    public bool TryAdd(Entity ent) {
        ArrayList inv;
        this.GetValue("inventory", inv);

        if (inv.Length <= 4) {
            char class[32], name[32];
            ent.GetClass(class, sizeof(class));
            ent.name(name, sizeof(name));

            Item itm = new Item(class);
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
        this.SetValue("inventory", inv);
    }

    public void Display() {
        Menu InvMenu = new Menu(InventoryHandler);

        InvMenu.SetTitle("Инвентарь");
        
        ArrayList inv;
        this.GetValue("inventory", inv);

        if (inv.Length)
            for (int i=0; i < inv.Length; i++) {
                char itemid[8], itemName[32];

                IntToString(i, itemid, sizeof(itemid));
                view_as<Item>(inv.Get(i, 0)).name(itemName, sizeof(itemName));
                
                InvMenu.AddItem(itemid, itemName, ITEMDRAW_DEFAULT);
            }
        else
            InvMenu.AddItem("item1", "Предметов нет", ITEMDRAW_DISABLED);

        int id;
        this.GetValue("id", id);

        InvMenu.Display(id, 30);
    }
}