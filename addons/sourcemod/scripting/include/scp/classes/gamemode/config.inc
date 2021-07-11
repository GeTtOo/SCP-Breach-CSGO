/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Configuration-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

#include "scp/classes/gamemode/doors"

methodmap Config < JSON_OBJECT {
    
    public Config(char[] mapname) {
        Config self = view_as<Config>(ReadConfig(mapname, "config"));

        self.SetObject("Doors", view_as<Doors>(ReadConfig(mapname, "doors")));
        self.SetObject("SpawnMap", view_as<JSON_OBJECT>(ReadConfig(mapname, "spawn_map")));
        self.SetObject("Regions", view_as<JSON_ARRAY>(ReadConfig(mapname, "regions")));

        return self;
    }

    public void Add(char[] name, any value) {
        this.SetValue(name, value);
    }
    
    public void DefaultGlobalClass(char[] team, int max_size) {
        this.GetString("DefaultGlobalClass", team, max_size);
    }

    public void DefaultClass(char[] class, int max_size) {
        this.GetString("DefaultClass", class, max_size);
    }

    property Doors doors { 
        public get() { return view_as<Doors>(this.GetObject("Doors")); }
    }

    property JSON_OBJECT spawnmap { 
        public get() { return view_as<JSON_OBJECT>(this.GetObject("SpawnMap")); }
    }

    property JSON_ARRAY regions { 
        public get() { return view_as<JSON_ARRAY>(this.GetObject("Regions")); }
    }

    property int invsize {
        public get() { return this.GetInt("invsize") - 1; } //Index started at 0
    }

    property float NukeTime {
        public set(float time) { this.SetFloat("NukeTime", time); }
        public get() { return this.GetFloat("NukeTime"); }
    }

    property float NukeKillPos {
        public set(float level) { this.SetFloat("NukeKillPos", level); }
        public get() { return this.GetFloat("NukeKillPos"); }
    }

    public void NukeSound(char[] buffer, int max_size) {
        this.GetString("NukeSound", buffer, max_size);
    }

    property int NukeButtonID {
        public get() { return this.GetBool("NukeButtonID"); }
    }
    
    property Vector AdminRoom {
        public get() {
            return this.GetVector("AdminRoom");
        }
    }

    property bool debug {
        public get() { return this.GetBool("debug"); }
    }

    property bool ff {
        public get() { return this.GetBool("ff"); }
    }
}