/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Round Manager-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Manager < StringMap {

    public Manager() {
        return view_as<Manager>(new StringMap());
    }

    property bool IsNuked {
        public set(bool value) { this.SetValue("nuked", value); }
        public get() { bool value; this.GetValue("nuked", value) ? value : false; }
    }

    property bool RoundComplete {
        public set(bool value) { this.SetValue("roundcomplete", value); }
        public get() { bool value; this.GetValue("roundcomplete", value) ? value : false; }
    }
}