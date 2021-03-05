methodmap Class < JSON_Object {
    
    public Class(JSON_Object class) {
        Class self = view_as<Class>(class);
        return self;
    }

    property int percent {
        public get() { return this.GetInt("percent"); }
    }

    property bool priority {
        public get() { return this.GetBool("priority"); }
    }

    public bool model(char[] buffer, int max_size) {
        this.GetString("model", buffer, max_size);
    }

    property int health {
        public get() { return this.GetInt("health"); }
    }

    property int armor {
        public get() { return this.GetInt("armor"); }
    }

    public bool weapon(char[] buffer, int max_size) {
        this.GetString("weapon", buffer, max_size);
    }
}

methodmap Classes < JSON_Object {
    
    public Classes(JSON_Object class_arr) {
        Classes self = view_as<Classes>(class_arr);
        return self;
    }

    public Class get(char[] name) { return view_as<Class>(this.GetObject(name)); }
}

methodmap GlobalClass < JSON_Object {
    
    public GlobalClass(JSON_Object globalclass) {
        GlobalClass self = view_as<GlobalClass>(globalclass);
        return self;
    }

    property int percent {
        public get() { return this.GetInt("percent"); }
    }

    property bool priority {
        public get() { return this.GetBool("priority"); }
    }

    property Classes classes {
        public get() { return view_as<Classes>(this.GetObject("Classes")); }
    }

    public Class class(char[] name) {
        return view_as<Classes>(this.GetObject("Classes")).get(name);
    }

    public StringMapSnapshot GetClassNames() {
        return this.GetObject("Classes").Snapshot();
    }
}

methodmap GlobalClasses < JSON_Object {
    
    public GlobalClasses() {
        char path[PLATFORM_MAX_PATH];
        GlobalClasses self;
        BuildPath(Path_SM, path, PLATFORM_MAX_PATH, "configs/scp/classes.json");
        if (FileExists(path)) {
            char buffer[8192];
            Handle file = OpenFile(path,"r+");
            ReadFileString(file, buffer, sizeof(buffer));
            self = view_as<GlobalClasses>(json_decode(buffer));
        } else {
            self = view_as<GlobalClasses>(new JSON_Object());
        }

        return self;
    }

    public void Add(char[] name, Class class) {
        this.SetObject(name, class);
    }

    public GlobalClass get(char[] name) { return view_as<GlobalClass>(this.GetObject(name)); }
}

methodmap Config < JSON_Object {
    
    public Config() {
        char path[PLATFORM_MAX_PATH];
        Config config;
        BuildPath(Path_SM, path, PLATFORM_MAX_PATH, "configs/scp/config.json");
        if (FileExists(path)) {
            char buffer[8192];
            Handle file = OpenFile(path,"r+");
            ReadFileString(file, buffer, sizeof(buffer));
            config = view_as<Config>(json_decode(buffer));
        } else {
            config = view_as<Config>(new JSON_Object());
        }

        return config;
    }
    
    public void DefaultGlobalClass(char[] gclass, int max_size) {
        this.GetString("DefaultGlobalClass", gclass, max_size);
    }

    public void DefaultClass(char[] class, int max_size) {
        this.GetString("DefaultClass", class, max_size);
    }

    property float NukeTime {
        public set(float time) { this.SetFloat("NukeTime", time); }
        public get() { return this.GetFloat("NukeTime"); }
    }
}

methodmap GameMode < JSON_Object {
    
    public GameMode() {
        GameMode self = view_as<GameMode>(new JSON_Object());
        self.SetObject("GlobalClasses", view_as<GlobalClasses>(new GlobalClasses()));
        self.SetObject("Config", view_as<Config>(new Config()));
        return self;
    }

    property Config config {
        public get() { return view_as<Config>(this.GetObject("Config")); }
    }

    public GlobalClass gclass(char[] name) {
        return view_as<GlobalClasses>(this.GetObject("GlobalClasses")).get(name);
    }

    public StringMapSnapshot GetGlobalClassNames() {
        return this.GetObject("GlobalClasses").Snapshot();
    }
}