/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Configuration-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

#include "scp/classes/gamemode/doors"

methodmap Config < JSON_Object {
    
    public Config(char[] mapname) {
        Config self = view_as<Config>(ReadConfig(mapname, "config"));

        self.SetObject("Doors", view_as<Doors>(ReadConfig(mapname, "doors")));
        self.SetObject("SpawnMap", view_as<JSON_Object>(ReadConfig(mapname, "spawn_map")));
        self.SetObject("Regions", view_as<JSON_Array>(ReadConfig(mapname, "regions")));

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

    property JSON_Object spawnmap { 
        public get() { return view_as<JSON_Object>(this.GetObject("SpawnMap")); }
    }

    property JSON_Array regions { 
        public get() { return view_as<JSON_Array>(this.GetObject("Regions")); }
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
    
    property Vector AdminRoomPos {
        public get() {
            JSON_Object rawPos = this.GetObject("AdminRoom");
            Vector pos = new Vector(rawPos.GetFloat("x"), rawPos.GetFloat("y"), rawPos.GetFloat("z"));
            return pos;
        }
    }

    property bool debug {
        public get() { return this.GetBool("debug"); }
    }
}