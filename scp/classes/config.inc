/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Configuration-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

#include "scp/classes/doors"

methodmap Config < JSON_Object {
    
    public Config() {
        char path[PLATFORM_MAX_PATH];
        Config self;
        BuildPath(Path_SM, path, PLATFORM_MAX_PATH, "configs/scp/config.json");
        if (FileExists(path)) {
            char buffer[8192];
            Handle file = OpenFile(path,"r+");
            ReadFileString(file, buffer, sizeof(buffer));
            self = view_as<Config>(json_decode(buffer));
        } else {
            self = view_as<Config>(new JSON_Object());
        }

        //self.SetObject("Doors", view_as<Doors>(new Doors()));

        return self;
    }
    
    public void DefaultGlobalClass(char[] gclass, int max_size) {
        this.GetString("DefaultGlobalClass", gclass, max_size);
    }

    public void DefaultClass(char[] class, int max_size) {
        this.GetString("DefaultClass", class, max_size);
    }

    public void SetDoorRules(char[] mapName) {
        char path[PLATFORM_MAX_PATH];
        JSON_Object obj;
        Format(path, PLATFORM_MAX_PATH, "%s%s%s", "configs/scp/doors/", mapName, ".json");
        BuildPath(Path_SM, path, PLATFORM_MAX_PATH, path);
        if (FileExists(path)) {
            char buffer[8192];
            Handle file = OpenFile(path,"r+");
            ReadFileString(file, buffer, sizeof(buffer));
            obj = json_decode(buffer);
        } else {
            obj = new JSON_Object();
        }
        this.SetObject("Doors", view_as<Doors>(obj));
    }

    property Doors doors { 
        public get() { return view_as<Doors>(this.GetObject("Doors")); }
    }

    property float NukeTime {
        public set(float time) { this.SetFloat("NukeTime", time); }
        public get() { return this.GetFloat("NukeTime"); }
    }

    property bool debug {
        public get() { return this.GetBool("debug"); }
    }
}