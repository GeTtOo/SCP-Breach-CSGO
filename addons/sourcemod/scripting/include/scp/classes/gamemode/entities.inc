methodmap EntConfig < JSON_OBJECT {
    
    public void name(char[] buffer, int max_size) {
        this.GetString("name", buffer, max_size);
    }

    public void model(char[] buffer, int max_size) {
        this.GetString("model", buffer, max_size);
    }

    public void mass(char[] buffer, int max_size) {
        this.GetString("mass", buffer, max_size);
    }

    property int spawnflags {
        public get() { return this.GetInt("spawnflags"); }
    }
}

methodmap Entities < JSON_OBJECT {

    public Entities(char[] mapname) {
        return view_as<Entities>(ReadConfig(mapname, "entities"));
    }

    public EntConfig get(char[] name) { return view_as<EntConfig>(this.GetObject(name)); }
}