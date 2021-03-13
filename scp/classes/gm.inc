/*----------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------Game Mode-------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

#include "scp/classes/config"
#include "scp/classes/player_class"

methodmap Classes < JSON_Object {
    public Class get(char[] name) { Class class = view_as<Class>(this.GetObject(name)); class.Name(name); return class; }
}

methodmap GlobalClass < JSON_Object {

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
    
    public GlobalClasses(char[] mapname) {
        return view_as<GlobalClasses>(ReadConfig(mapname, "classes"));
    }

    public void Add(char[] name, Class class) {
        this.SetObject(name, class);
    }

    public GlobalClass get(char[] name) { return view_as<GlobalClass>(this.GetObject(name)); }
}

methodmap GameMode < JSON_Object {
    
    public GameMode(char[] mapname) {
        GameMode self = view_as<GameMode>(new JSON_Object());
        self.SetObject("GlobalClasses", new GlobalClasses(mapname));
        self.SetObject("Config", new Config(mapname));
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

public JSON_Object ReadConfig(char[] mapname, char[] filename) {
    char path[PLATFORM_MAX_PATH];
    JSON_Object obj;
    Format(path, sizeof(path), "%s%s%s%s%s", "configs/scp/", mapname, "/", filename, ".json");
    BuildPath(Path_SM, path, PLATFORM_MAX_PATH, path);
    if (FileExists(path)) {
        char buffer[8192];
        Handle file = OpenFile(path,"r+");
        ReadFileString(file, buffer, sizeof(buffer));
        obj = json_decode(buffer);
    } else {
        obj = new JSON_Object();
    }

    return obj;
}