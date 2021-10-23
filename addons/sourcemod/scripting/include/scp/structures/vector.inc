methodmap Vector < Base {

    public Vector(float x = 0.0, float y = 0.0, float z = 0.0) {
        Vector vec = view_as<Vector>(new Base());

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

    public void SetArr(float[3] vec) {
        this.x = vec[0];
        this.y = vec[1];
        this.z = vec[2];
    }

    public void GetArr(float[3] vec) {
        vec[0] = this.x;
        vec[1] = this.y;
        vec[2] = this.z;
    }

    public void GetArrD(float[3] vec) {
        vec[0] = this.x;
        vec[1] = this.y;
        vec[2] = this.z;

        delete this;
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

    public Vector Clone() {
        return new Vector(this.x, this.y, this.z);
    }

    public float DotProduct(Vector vec2) {
        float vecarr1[3], vecarr2[3];
        this.GetArr(vecarr1);
        vec2.GetArr(vecarr2);

        float dotproduct = GetVectorDotProduct(vecarr1, vecarr2);

        delete this;
        delete vec2;

        return dotproduct;
    }

    public float Length() {
        float vecarr1[3];
        this.GetArr(vecarr1);
        
        float length = GetVectorLength(vecarr1);
        
        delete this;

        return length;
    }

    public Vector GetFromPoint(Vector vec) {
        float vecarr1[3], vecarr2[3], vecarr3[3];
        this.GetArr(vecarr1);
        vec.GetArr(vecarr2);

        MakeVectorFromPoints(vecarr1, vecarr2, vecarr3);
        
        delete this;
        delete vec;

        return new Vector(vecarr3[0], vecarr3[1], vecarr3[2]);
    }

    public Vector Normalize() {
        float vecarr[3];
        this.GetArr(vecarr);
        NormalizeVector(vecarr, vecarr);
        this.SetArr(vecarr);

        return this;
    }

    public Vector Scale(float scale) {
        float vecarr[3];
        this.GetArr(vecarr);
        ScaleVector(vecarr, scale);
        this.SetArr(vecarr);

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

stock bool operator>(Vector first, Vector second) {
    bool result = false;
    if ((first.x > second.x) && (first.y > second.y) && (first.z > second.z))
        result = true;
    delete first; delete second;
    return result;
}

stock bool operator<(Vector first, Vector second) {
    bool result = false;
    if ((first.x < second.x) && (first.y < second.y) && (first.z < second.z))
        result = true;
    delete first; delete second;
    return result;
}

methodmap Angle < Vector {
    
    public Angle(float x, float y, float z) {
        Angle ang = view_as<Angle>(new Base());

        ang.SetValue("x", x);
        ang.SetValue("y", y);
        ang.SetValue("z", z);
        
        return ang;
    }

    public Vector Forward(Vector startpos, float scale) {
        float startposarr[3], angarr[3], forwardarr[3], endposarr[3];
        startpos.GetArr(startposarr);
        this.GetArr(angarr);
        GetAngleVectors(angarr, forwardarr, NULL_VECTOR, NULL_VECTOR);
        ScaleVector(forwardarr, scale);
        AddVectors(startposarr, forwardarr, endposarr);

        delete this;
        delete startpos;

        return new Vector(endposarr[0], endposarr[1], endposarr[2]);
    }

    public Vector Right(Vector startpos, float scale) {
        float startposarr[3], angarr[3], rightarr[3], endposarr[3];
        startpos.GetArr(startposarr);
        this.GetArr(angarr);
        GetAngleVectors(angarr, NULL_VECTOR, rightarr, NULL_VECTOR);
        ScaleVector(rightarr, scale);
        AddVectors(startposarr, rightarr, endposarr);

        delete this;
        delete startpos;

        return new Vector(endposarr[0], endposarr[1], endposarr[2]);
    }

    public Vector Up(Vector startpos, float scale) {
        float startposarr[3], angarr[3], uparr[3], endposarr[3];
        startpos.GetArr(startposarr);
        this.GetArr(angarr);
        GetAngleVectors(angarr, NULL_VECTOR, NULL_VECTOR, uparr);
        ScaleVector(uparr, scale);
        AddVectors(startposarr, uparr, endposarr);

        delete this;
        delete startpos;

        return new Vector(endposarr[0], endposarr[1], endposarr[2]);
    }
}

stock Angle operator+(Angle first, Angle second) {
    Angle result = new Angle(first.x + second.x, first.y + second.y, first.z + second.z);
    delete first; delete second;
    return result;
}

stock Angle operator-(Angle first, Angle second) {
    Angle result = new Angle(first.x - second.x, first.y - second.y, first.z - second.z);
    delete first; delete second;
    return result;
}