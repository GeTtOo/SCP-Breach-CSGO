/*----------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------Game Mode-------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

#include "scp/classes/gamemode/config"
#include "scp/classes/gamemode/class"
#include "scp/classes/gamemode/entities"

methodmap Classes < JSON_Object {
    public Class get(char[] name) { Class class = view_as<Class>(this.GetObject(name)); class.Name(name); return class; }
}

methodmap GTeam < JSON_Object {

    property int percent {
        public get() { return this.GetInt("percent"); }
    }

    property bool priority {
        public get() { return this.GetBool("priority"); }
    }

    property Classes classes {
        public get() { return view_as<Classes>(this.GetObject("Classes")); }
    }

    property int count {
        public set(int val) { this.SetInt("count", val); }
        public get() { return this.GetInt("count", 0); }
    }

    public Class class(char[] name) {
        return this.classes.get(name);
    }

    public StringMapSnapshot GetClassNames() {
        return this.GetObject("Classes").Snapshot();
    }
}

methodmap Teams < JSON_Object {
    
    public Teams(char[] mapname) {
        return view_as<Teams>(ReadConfig(mapname, "classes"));
    }

    public void Add(char[] name, Class class) {
        this.SetObject(name, class);
    }

    public GTeam get(char[] name) { return view_as<GTeam>(this.GetObject(name)); }
}

#include "scp/classes/gamemode/manager"

methodmap GameMode < JSON_Object {
    
    public GameMode(char[] mapname) {
        GameMode self = view_as<GameMode>(new JSON_Object());
        self.SetObject("Teams", new Teams(mapname));
        self.SetObject("Entities", new Entities(mapname));
        self.SetObject("Config", new Config(mapname));
        self.SetValue("Manager", new Manager(view_as<Teams>(self.GetObject("Teams"))));
        return self;
    }

    property Config config {
        public native get();
    }

    property Manager mngr {
        public native get();
    }

    public native GTeam team(char[] name);

    property Entities entities {
        public get() { return view_as<Entities>(this.GetObject("Entities")); }
    }

    public StringMapSnapshot GetGlobalClassNames() {
        return this.GetObject("Teams").Snapshot();
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