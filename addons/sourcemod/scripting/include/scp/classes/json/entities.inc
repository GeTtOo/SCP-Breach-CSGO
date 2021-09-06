methodmap EntConfig < JSON_OBJECT {

    property float mass {
        public get() { return this.GetFloat("mass"); }
    }

    property int bodygroup {
        public get() { return this.GetInt("bodygroup"); }
    }

    property int skin {
        public get() { return this.GetInt("skin"); }
    }

    property int spawnflags {
        public get() { return this.GetInt("spawnflags"); }
    }
    
    public void name(char[] buffer, int max_size) {
        this.GetString("name", buffer, max_size);
    }

    public void model(char[] buffer, int max_size) {
        this.GetString("model", buffer, max_size);
    }
}

methodmap Entities < JSON_OBJECT {

    public Entities(char[] mapname) {
        return view_as<Entities>(ReadConfig(mapname, "entities"));
    }

    public EntConfig get(char[] name) { return view_as<EntConfig>(this.GetObject(name)); }
}