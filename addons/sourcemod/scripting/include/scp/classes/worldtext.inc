/*----------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------World Text controller------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap WorldText < Entity {
    
    public WorldText(int id) {
        WorldText self = view_as<WorldText>(new Entity(id));
        return self;
    }

    property int type {
        public set(int val) { this.SetInt("type", val); }
        public get() { return this.GetInt("type", 1); }
    }
    
    public WorldText SetText(char[] text) {
        this.SetPropString("m_szText", text);
        return this;
    }
    
    public WorldText SetSize(any size) {
        this.SetPropFloat("m_flTextSize", size);
        return this;
    }

    public WorldText SetColor(Colour col) {
        this.SetProp("m_textColor", (col.b << 16) + (col.g << 8) + col.r);
        delete col;
        return this;
    }
}

methodmap WorldTextSingleton < Base {
    
    public WorldTextSingleton() {
        WorldTextSingleton self = view_as<WorldTextSingleton>(new Base());
        self.CreateList("wtlist");
        return self;
    }
    
    property ArrayList list {
        public get() { return this.GetList("wtlist"); }
    }

    public WorldText Create(Vector vec, Angle ang, int type = 1) {
        WorldText wt = new WorldText(CreateEntityByName("point_worldtext"));

        wt.type = type;
        wt.SetPos(vec, ang);

        this.list.Push(wt);

        return wt;
    }

    public WorldText Get(int id) {
        return this.list.Get(id);
    }

    public ArrayList GetAll(int type = 0) {
        ArrayList list = this.list;
        ArrayList sortedlist = new ArrayList();

        for (int i=0; i < list.Length; i++) {
            WorldText wt = list.Get(i);

            if (wt != null && (wt.type == type || type == 0))
                sortedlist.Push(wt);
        }

        return sortedlist;
    }

    public void Clear() {
        this.list.Clear();
    }
}