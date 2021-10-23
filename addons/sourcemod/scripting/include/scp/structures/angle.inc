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