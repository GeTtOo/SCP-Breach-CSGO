#pragma semicolon 1
#pragma newdecls required

#include <json>

methodmap Class < JSON_OBJECT {

    public void Name(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("name", buffer);
        else
            this.GetString("name", buffer, max_size);
    }

    public Vector GetPos() {
        return this.GetVector("pos");
    }

    public Angle GetAng() {
        return view_as<Angle>(this.GetVector("ang"));
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

    property JSON_ARRAY doors {
        public get() { return view_as<JSON_ARRAY>(this.GetObject("doors")); }
    }

    property JSON_ARRAY items {
        public get() { return view_as<JSON_ARRAY>(this.GetObject("items")); }
    }

    property JSON_ARRAY weapons {
        public get() { return view_as<JSON_ARRAY>(this.GetObject("weapons")); }
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