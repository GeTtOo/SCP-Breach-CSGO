methodmap Vector < Base {

    public Vector(float x, float y, float z) {
        Vector vec = view_as<Vector>(new StringMap());

        vec.SetValue("x", x);
        vec.SetValue("y", y);
        vec.SetValue("z", z);
        
        return vec;
    }

    property float x {
        public set(float x) { this.SetFloat("x", x); }
        public get() { return this.GetFloat("x"); }
    }

    property float y {
        public set(float y) { this.SetFloat("y", y); }
        public get() { return this.GetFloat("y"); }
    }

    property float z {
        public set(float z) { this.SetFloat("z", z); }
        public get() { return this.GetFloat("z"); }
    }

    public void GetArr(float[3] vec) {
        vec[0] = this.x;
        vec[1] = this.y;
        vec[2] = this.z;
    }

    public Vector Plus(Vector vec2) {
        this.x += vec2.x;
        this.y += vec2.y;
        this.z += vec2.z;

        return this;
    }

    public Vector Minus(Vector vec2) {
        this.x -= vec2.x;
        this.y -= vec2.y;
        this.z -= vec2.z;

        return this;
    }
}

methodmap Angle < Vector {}