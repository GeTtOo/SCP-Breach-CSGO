/*----------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Player inventory---------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap InvItem < Entity {

    property bool disabled {
        public set(bool value) { this.SetBool("disabled", value); }
        public get() { return this.GetBool("disabled"); }
    }

    property float cdr {
        public set(float value) { this.SetFloat("cdr", value); }
        public get() { return this.GetFloat("cdr"); }
    }

    public void StartCooldown(int time, char[] funcname, any arg) {
        this.TimerSimple(time, funcname, arg);
        this.cdr = GetGameTime() + float(time / 1000);
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

    public void FullClear() {
        ArrayList items = this.GetArrayList("inventory");
        ArrayList ents = view_as<Base>(gamemode).GetBase("ents").GetArrayList("entities");

        for (int i=0; i < items.Length; i++) {
            InvItem item = items.Get(i);
            ents.Erase(ents.FindValue(item));
        }
    }
}