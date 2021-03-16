methodmap EntConfig < JSON_Object {
    
    public void model(char[] buffer, int max_size) {
        this.GetString("model", buffer, max_size);
    }

    public void mass(char[] buffer, int max_size) {
        this.GetString("mass", buffer, max_size);
    }
}

methodmap Entities < JSON_Object {

    public Entities(char[] mapname) {
        return view_as<Entities>(ReadConfig(mapname, "entities"));
    }

    public EntConfig get(char[] name) { return view_as<EntConfig>(this.GetObject(name)); }
}