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

    property int percent {
        public get() { return this.GetInt("percent"); }
    }

    property bool priority {
        public get() { return this.GetBool("priority"); }
    }

    property bool escape {
        public get() { return this.GetBool("escape"); }
    }

    public bool Model(char[] buffer, int max_size) {
        this.GetString("model", buffer, max_size);
    }

    public bool HandsModel(char[] buffer, int max_size) {
        this.GetString("handsmodel", buffer, max_size);
    }

    property int health {
        public get() { return this.GetInt("health"); }
    }

    property int armor {
        public get() { return this.GetInt("armor"); }
    }

    property float speed {
        public get() { return this.GetFloat("speed"); }
    }

    public bool weapon(char[] buffer, int max_size) {
        this.GetString("weapon", buffer, max_size);
    }
}