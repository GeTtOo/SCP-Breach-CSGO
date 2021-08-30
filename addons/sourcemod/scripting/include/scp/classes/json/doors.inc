/*----------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------Doors---------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#include <json>

methodmap Door < JSON_OBJECT {
    
    public Door(Door door) {
        return view_as<Door>(door);
    }
    
    property int id {
        public get() { return this.GetInt("id"); }
    }

    property int access {
        public get() { return this.GetInt("access"); }
    }

    property bool scp {
        public get() { return this.GetBool("scp"); }
    }
}

methodmap Doors < JSON_OBJECT {
    public StringMapSnapshot GetAll() { return this.Snapshot(); }

    public Door Get(char[] id) { return view_as<Door>(this.GetObject(id)); }
}