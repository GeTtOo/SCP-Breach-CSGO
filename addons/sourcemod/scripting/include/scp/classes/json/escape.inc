#pragma semicolon 1
#pragma newdecls required

#include <json>

methodmap EscapeInfo < JSON_OBJECT {

    property int trigger {
        public get() { return this.GetInt("trigger"); }
    }

    property bool savepos {
        public get() { return this.GetBool("savepos"); }
    }

    public bool team(char[] buffer, int max_size) {
        return this.GetString("team", buffer, max_size);
    }

    public bool class(char[] buffer, int max_size) {
        return this.GetString("class", buffer, max_size);
    }
}