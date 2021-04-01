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

        delete vec2;

        return this;
    }

    public Vector Minus(Vector vec2) {
        this.x -= vec2.x;
        this.y -= vec2.y;
        this.z -= vec2.z;

        delete vec2;

        return this;
    }
}

stock Vector operator+(Vector first, Vector second) {
    Vector result = new Vector(first.x + second.x, first.y + second.y, first.z + second.z);
    delete first; delete second;
    return result;
}

stock Vector operator-(Vector first, Vector second) {
    Vector result = new Vector(first.x - second.x, first.y - second.y, first.z - second.z);
    delete first; delete second;
    return result;
}

methodmap Angle < Vector {
    
    public Angle(float x, float y, float z) {
        Angle ang = view_as<Angle>(new StringMap());

        ang.SetValue("x", x);
        ang.SetValue("y", y);
        ang.SetValue("z", z);
        
        return ang;
    }
}