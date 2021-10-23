#pragma semicolon 1
#pragma newdecls required

#include <json>
#include <scp/classes/json/escape>

methodmap Class < JSON_OBJECT {

    public void Name(char[] buffer, int max_size = 0) {
        if (max_size == 0)
            this.SetString("name", buffer);
        else
            this.GetString("name", buffer, max_size);
    }

    public JSON_OBJECT GetVecAng() {
        JSON_OBJECT posobj = this.GetObject("pos");
        
        if (posobj != null)
            if (!posobj.IsArray)
                return posobj;
            else
            {
                JSON_ARRAY posarr = view_as<JSON_ARRAY>(posobj);
                for (int i=0; i < posarr.Length; i++)
                {
                    JSON_OBJECT pos = view_as<JSON_OBJECT>(posarr.GetObject(i));
                    if (!pos.GetBool("lock"))
                        return pos;
                }
            }
        
        return null;
    }

    property int percent {
        public get() { return this.GetInt("percent", 0); }
    }

    property bool priority {
        public get() { return this.GetBool("priority", false); }
    }

    property bool autospawn {
        public get() { return this.GetBool("autospawn", true); }
    }

    property int health {
        public get() { return this.GetInt("health", 100); }
    }

    property int armor {
        public get() { return this.GetInt("armor", 0); }
    }

    property bool helmet {
        public get() { return this.GetBool("helmet", false); }
    }

    property float speed {
        public get() { return float(this.GetInt("speed", 260)); }
    }

    property float multipler {
        public get() { return this.GetFloat("multipler", 1.0); }
    }

    property bool fists {
        public set(bool fists) { this.SetBool("fists", fists); }
        public get() { return this.GetBool("fists", true); }
    }

    property EscapeInfo escape {
        public get() { return view_as<EscapeInfo>(this.GetObject("escape")); }
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

    public bool Is(char[] equalClass) {
        char thisClass[32]; this.GetString("name", thisClass, sizeof(thisClass)); return StrEqual(thisClass, equalClass);
    }

    public bool Model(char[] buffer, int max_size) {
        this.GetString("model", buffer, max_size);
    }

    public bool ModelId(char[] buffer, int max_size) {
        this.GetString("modelid", buffer, max_size);
    }

    public bool HandsModel(char[] buffer, int max_size) {
        this.GetString("handsmodel", buffer, max_size);
    }

    public bool weapon(char[] buffer, int max_size) {
        this.GetString("weapon", buffer, max_size);
    }

    public bool overlay(char[] buffer, int max_size) { (max_size == 0) ? this.SetString("overlay", buffer) : this.GetString("overlay", buffer, max_size); }
}