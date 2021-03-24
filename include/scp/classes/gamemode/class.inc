#pragma semicolon 1
#pragma newdecls required

#include <json>

methodmap Class < JSON_Object {

    public void Name(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("name", buffer);
        else
            this.GetString("name", buffer, max_size);
    }

    public Vector GetPos() {
        JSON_Array pos = view_as<JSON_Array>(this.GetObject("pos", new JSON_Array()));
        if (pos.Length == 0) return null;
        return new Vector(pos.GetFloat(0),pos.GetFloat(1),pos.GetFloat(2));
    }

    property int percent {
        public get() { return this.GetInt("percent"); }
    }

    property bool priority {
        public get() { return this.GetBool("priority", false); }
    }

    property bool escape {
        public get() { return this.GetBool("escape", false); }
    }

    property int health {
        public get() { return this.GetInt("health", 100); }
    }

    property int armor {
        public get() { return this.GetInt("armor", 0); }
    }

    property float speed {
        public get() { return this.GetFloat("speed", 1.0); }
    }

    property JSON_Array doors {
        public get() { return view_as<JSON_Array>(this.GetObject("doors")); }
    }

    property JSON_Array items {
        public get() { return view_as<JSON_Array>(this.GetObject("items")); }
    }

    property JSON_Array weapons {
        public get() { return view_as<JSON_Array>(this.GetObject("weapons")); }
    }

    public bool Model(char[] buffer, int max_size) {
    this.GetString("model", buffer, max_size);
    }

    public bool HandsModel(char[] buffer, int max_size) {
        this.GetString("handsmodel", buffer, max_size);
    }

    public bool weapon(char[] buffer, int max_size) {
        this.GetString("weapon", buffer, max_size);
    }
}