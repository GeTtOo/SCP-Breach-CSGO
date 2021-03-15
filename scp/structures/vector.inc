methodmap Vector < StringMap {

    public Vector(float x, float y, float z) {
        Vector vec = view_as<Vector>(new StringMap());

        vec.SetValue("x", x);
        vec.SetValue("y", y);
        vec.SetValue("z", z);
        
        return vec;
    }

    property float x {
        public get() { float x; return this.GetValue("x", x) ? x : 0.0; }
    }

    property float y {
        public get() { float y; return this.GetValue("y", y) ? y : 0.0; }
    }

    property float z {
        public get() { float z; return this.GetValue("z", z) ? z : 0.0; }
    }

    public void GetArr(float[3] vec) {
        vec[0] = this.x;
        vec[1] = this.y;
        vec[2] = this.z;
    }
}

methodmap Angle < Vector {}