/*----------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------World Text controller------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

enum TypeWorldText {
    All = 0,
    Text,
    NukeMonitor
}

methodmap WorldText < Entity {
    
    public WorldText(int id) {
        WorldText self = view_as<WorldText>(new Entity(id));
        return self;
    }

    property TypeWorldText type {
        public set(TypeWorldText val) { this.SetInt("type", view_as<int>(val)); }
        public get() { return view_as<TypeWorldText>(this.GetInt("type", 0)); }
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
        self.CreateArrayList("wtlist");
        return self;
    }
    
    property ArrayList list {
        public get() { return this.GetArrayList("wtlist"); }
    }

    public WorldText Create(Vector vec, Angle ang, TypeWorldText type = Text) {
        WorldText wt = new WorldText(CreateEntityByName("point_worldtext"));

        wt.type = type;
        wt.SetPos(vec, ang);

        this.list.Push(wt);

        return wt;
    }

    public WorldText Get(int id) {
        return this.list.Get(id);
    }

    public ArrayList GetAll(TypeWorldText type = Text) {
        ArrayList list = this.list;
        ArrayList sortedlist = new ArrayList();

        for (int i=0; i < list.Length; i++) {
            WorldText wt = list.Get(i);

            if (wt != null && (wt.type == type || type == All))
                sortedlist.Push(wt);
        }

        return sortedlist;
    }

    public void Clear() {
        this.list.Clear();
    }
}