/*----------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------Game Mode-------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

#include "scp/classes/json/config"
#include "scp/classes/json/class"
#include "scp/classes/json/entities"
#include "scp/classes/timers"

methodmap Classes < JSON_OBJECT {
    public Class get(char[] name) { Class class = view_as<Class>(this.GetObject(name)); class.Name(name); return class; }
}

methodmap GTeam < JSON_OBJECT {

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
        return this.classes.get(name);
    }

    public StringMapSnapshot GetClassNames() {
        return this.GetObject("Classes").Snapshot();
    }
}

methodmap Teams < JSON_OBJECT {
    
    public Teams(char[] mapname) {
        return view_as<Teams>(ReadConfig(mapname, "classes"));
    }

    public void Add(char[] name, Class class) {
        this.SetObject(name, class);
    }

    public GTeam get(char[] name) { return view_as<GTeam>(this.GetObject(name)); }
}

methodmap GameMode < JSON_OBJECT {
    
    public GameMode(char[] mapname) {
        GameMode self = view_as<GameMode>(new JSON_OBJECT());
        self.SetValue("Teams", new Teams(mapname));
        self.SetValue("Entities", new Entities(mapname));
        self.SetValue("Config", new Config(mapname));
        self.SetValue("Timers", new Timers());
        return self;
    }

    property Config config {
        public native get();
    }

    property Manager mngr {
        public native get();
    }

    property Timers timer {
        public native get();
    }

    property Logger log {
        public native get();
    }

    public native GTeam team(char[] name);

    property Entities entities {
        public native get();
    }

    public StringMapSnapshot GetTeamNames() {
        return this.GetObject("Teams").Snapshot();
    }
}

public JSON_OBJECT ReadConfig(char[] mapname, char[] filename) {
    char path[PLATFORM_MAX_PATH];
    JSON_OBJECT obj;
    Format(path, sizeof(path), "%s%s%s%s%s", "configs/scp/", mapname, "/", filename, ".json");
    BuildPath(Path_SM, path, PLATFORM_MAX_PATH, path);
    if (FileExists(path)) {
        char buffer[65536];
        Handle file = OpenFile(path,"r+");
        ReadFileString(file, buffer, sizeof(buffer));
        obj = view_as<JSON_OBJECT>(json_decode(buffer));
    } else {
        obj = new JSON_OBJECT();
    }

    return obj;
}