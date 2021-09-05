#pragma semicolon 1
#pragma newdecls required

#include <json>

methodmap NukeConfig < JSON_OBJECT {

    property int time {
        public get() { return this.GetInt("time"); }
    }

    property bool nukefullreset {
        public get() { return this.GetBool("nukefullreset"); }
    }

    property float killpos {
        public get() { return float(this.GetInt("killpos")); }
    }

    property int readybtn {
        public get() { return this.GetInt("readybtn"); }
    }

    property int activebtn {
        public get() { return this.GetInt("activebtn"); }
    }

    property int cancelbtn {
        public get() { return this.GetInt("cancelbtn"); }
    }

    property JSON_ARRAY spawnlist {
        public get() { return view_as<JSON_ARRAY>(this.GetObject("spawnlist")); }
    }

    public void sound(char[] buffer, int max_size) {
        this.GetString("sound", buffer, max_size);
    }

    public void cancelsound(char[] buffer, int max_size) {
        this.GetString("cancelsound", buffer, max_size);
    }
}