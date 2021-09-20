methodmap List < StringMap {

    public List() { return view_as<List>(CreateTrie()); }

    public bool Set(char[] idx, any value) { return this.SetValue(idx, value); }
    public any Get(char[] idx) { any value; return this.GetValue(idx, value) ? value : false; }
}

methodmap Base < StringMap {
    
    public Base() { return view_as<Base>(CreateTrie()); }
    
    public bool SetInt(char[] index, int val) { return this.SetValue(index, val); }
    public bool SetFloat(char[] index, float val) { return this.SetValue(index, val); }
    public bool SetBool(char[] index, bool val) { return this.SetValue(index, val); }
    public bool SetVector(char[] index, Base val) { return this.SetValue(index, val); }
    public bool SetList(char[] index, List val) { return this.SetValue(index, val); }
    public bool CreateList(char[] index) { return this.SetValue(index, new List()); }
    public bool SetArrayList(char[] index, ArrayList val) { return this.SetValue(index, val); }
    public bool CreateArrayList(char[] index, int blocksize = 1, int size = 0) { return this.SetValue(index, new ArrayList(blocksize, size)); }

    public int GetInt(char[] index, int defval = 0) { int val; return this.GetValue(index, val) ? val : defval; }
    public float GetFloat(char[] index, float defval = 0.0) { float val; return this.GetValue(index, val) ? val : defval; }
    public bool GetBool(char[] index, bool defval = false) { bool val; return this.GetValue(index, val) ? val : defval; }
    public Base GetVector(char[] index, Base defval = null) { Base val; return this.GetValue(index, val) ? val : defval; }
    public List GetList(char[] index) { List list; return this.GetValue(index, list) ? list : null; }
    public ArrayList GetArrayList(char[] index) { ArrayList list; return this.GetValue(index, list) ? list : null; }

    public bool RemoveValue(char[] index) { return this.Remove(index); }
}