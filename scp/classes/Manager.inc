/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Round Manager-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Manager < JSON_Object {

    public Manager() {
        return view_as<Manager>(new JSON_Object());
    }

    property int PlayerCollisionGroup {
        public set(int value) { this.SetInt("playercollisiongroup", value); }
        public get() { return this.GetInt("playercollisiongroup"); }
    }

    property bool IsNuked {
        public set(bool value) { this.SetBool("nuked", value); }
        public get() { return this.GetBool("nuked"); }
    }

    property bool RoundComplete {
        public set(bool value) { this.SetBool("roundcomplete", value); }
        public get() { return this.GetBool("roundcomplete"); }
    }
}