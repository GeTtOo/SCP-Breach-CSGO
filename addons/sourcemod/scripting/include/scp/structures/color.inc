methodmap Colour < Base {

    public Colour(int r = 0, int g = 0, int b = 0) {
        Colour col = view_as<Colour>(new Base());

        col.SetValue("r", r);
        col.SetValue("g", g);
        col.SetValue("b", b);
        
        return col;
    }

    property int r {
        public set(int r) { this.SetInt("r", r); }
        public get() { return this.GetInt("r"); }
    }

    property int g {
        public set(int g) { this.SetInt("g", g); }
        public get() { return this.GetInt("g"); }
    }

    property int b {
        public set(int b) { this.SetInt("b", b); }
        public get() { return this.GetInt("b"); }
    }

    public Colour Clone() {
        return new Colour(this.r, this.g, this.b);
    }
}