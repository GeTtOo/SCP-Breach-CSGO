methodmap Colour < Base {

    public Colour(int r = 0, int g = 0, int b = 0, int a = 255) {
        Colour col = view_as<Colour>(new Base());

        col.SetValue("r", r);
        col.SetValue("g", g);
        col.SetValue("b", b);
        col.SetValue("a", a);
        
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

    property int a {
        public set(int a) { this.SetInt("a", a); }
        public get() { return this.GetInt("a"); }
    }

    public Colour Clone() {
        return new Colour(this.r, this.g, this.b, this.a);
    }

    public void GetArr(int[] clr) {
        clr[0] = this.r;
        clr[1] = this.g;
        clr[2] = this.b;
        clr[3] = this.a;
    }
}