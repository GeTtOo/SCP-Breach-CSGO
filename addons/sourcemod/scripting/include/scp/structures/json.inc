#include <json>

methodmap JSON_ARRAY < JSON_Array {

    public JSON_ARRAY() { return view_as<JSON_ARRAY>(CreateTrie()); }
    
    property JSON_Array Super {
        public get() { return view_as<JSON_Array>(this); }
    }
    
    public Vector GetVector(int index) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(index));
        if (pos == null || pos.Length == 0) return null;
        return new Vector(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Angle GetAngle(int index) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(index));
        if (pos == null || pos.Length == 0) return null;
        return new Angle(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Colour GetColor(int index) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(index));
        if (pos == null || pos.Length == 0) return null;
        return new Colour(pos.GetInt(0),pos.GetInt(1),pos.GetInt(2));
    }

    public JSON_ARRAY GetArray(int index) { return view_as<JSON_ARRAY>(this.Super.GetObject(index)); }
}

methodmap JSON_OBJECT < JSON_Object {

    public JSON_OBJECT() { return view_as<JSON_OBJECT>(CreateTrie()); }

    property JSON_Object Super {
        public get() { return view_as<JSON_Object>(this); }
    }

    public Vector GetVector(char[] name) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(name));
        if (pos == null || pos.Length == 0) return null;
        return new Vector(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Angle GetAngle(char[] name) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(name));
        if (pos == null || pos.Length == 0) return null;
        return new Angle(float(pos.GetInt(0)),float(pos.GetInt(1)),float(pos.GetInt(2)));
    }

    public Colour GetColor(char[] name) {
        JSON_ARRAY pos = view_as<JSON_ARRAY>(this.GetObject(name));
        if (pos == null || pos.Length == 0) return null;
        return new Colour(pos.GetInt(0),pos.GetInt(1),pos.GetInt(2));
    }

    public JSON_OBJECT GetObject(char[] name) { return view_as<JSON_OBJECT>(this.Super.GetObject(name)); }
    public JSON_ARRAY GetArray(char[] name) { return view_as<JSON_ARRAY>(this.Super.GetObject(name)); }
}