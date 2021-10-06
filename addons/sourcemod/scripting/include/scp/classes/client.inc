/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
#pragma semicolon 1
#pragma newdecls required

#include <sdktools>
#include <cstrike>
#include <json>

#include "scp/classes/inventory"
#include "scp/classes/progress"

methodmap Client < Entity {

    public Client(int id) {
        Client client = view_as<Client>(new Entity(id));
        client.SetValue("active", false);
        client.SetValue("haveсlass", false);
        client.SetValue("inventory", new Inventory());
        client.SetValue("progress", new ProgressBar(client));

        return client;
    }

    property bool active {
        public set(bool value) { this.SetBool("active", value); }
        public get() { return this.GetBool("active"); }
    }

    property int lang {
        public get() { return GetClientLanguage(this.id); }
    }

    property int health {
        public set(int val) { SetEntityHealth(this.id, val); }
        public get() { return GetClientHealth(this.id); }
    }

    property int armor {
        public set(int val) { SetEntProp(this.id, Prop_Send, "m_ArmorValue", val); }
        public get() { return GetEntProp(this.id, Prop_Send, "m_ArmorValue"); }
    }

    property float speed {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed"); }
    }

    property float multipler {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue"); }
    }

    property bool haveclass {
        public set(bool value) { this.SetBool("haveсlass", value); }
        public get() { return this.GetBool("haveсlass", false); }
    }

    property Class class {
        public set(Class val) { this.SetValue("class", val); }
        public get() { Class val; return this.GetValue("class", val) ? val : null; }
    }

    property bool IsSCP {
        public get() { char team[32]; this.GetString("team", team, sizeof(team)); return StrEqual(team, "SCP"); }
    }

    property bool fullaccess {
        public set(bool card) { this.SetBool("fullaccess", card); }
        public get() { return this.GetBool("fullaccess", false); }
    }

    property bool FirstSpawn {
        public set(bool val) { this.SetBool("firstspawn", val); }
        public get() { return this.GetBool("firstspawn", true); }
    }

    property bool spawned {
        public set(bool val) { this.SetBool("spawned", val); }
        public get() { return this.GetBool("spawned", false); }
    }

    property ArrayList activebg {
        public set(ArrayList list) { this.SetArrayList("activebg", list); }
        public get() { return this.GetArrayList("activebg"); }
    }

    property Entity ragdoll {
        public set(Entity val) { this.RemoveValue("ragdoll"); this.SetValue("ragdoll", val); }
        public get() { Entity val; return this.GetValue("ragdoll", val) ? val : null; }
    }

    property Inventory inv {
        public set(Inventory inv) { this.SetValue("inventory", inv); }
        public get() { Inventory inv; return this.GetValue("inventory", inv) ? inv : null; }
    }

    property ProgressBar progress {
        public set(ProgressBar pb) { this.SetValue("progress", pb); }
        public get() { ProgressBar pb; return this.GetValue("progress", pb) ? pb : null; }
    }

    public void SetupBaseStats(Class class) {
        this.health = class.health;
        this.armor = class.armor;
        this.speed = class.speed;
        this.multipler = class.multipler;
    }

    public bool GetName(char[] buffer, int max_size = 0) { return GetClientName(this.id, buffer, max_size); }

    public bool IsAlive() { return IsPlayerAlive(this.id); }

    public Vector EyePos() { float vecarr[3]; GetClientEyePosition(this.id, vecarr); return new Vector(vecarr[0], vecarr[1], vecarr[2]); }

    public Angle GetAng() { float angarr[3]; GetClientEyeAngles(this.id, angarr); return new Angle(angarr[0], angarr[1], angarr[2]); }

    public void SetModel(char[] model) {
        SetEntityModel(this.id, model);
    }

    public void SetModelById(char[] modelid) {
        this.mdlmeta = gamemode.meta.GetModel(modelid);

        char path[128];
        this.mdlmeta.Path(path, sizeof(path));
        SetEntityModel(this.id, path);

        int len = this.mdlmeta.bg.Length;

        ArrayList list = new ArrayList();
        for (int i=0; i < len; i++)
            list.Push(0);
        this.activebg = list;
    }

    public void GetModel(char[] model, int max_size) { GetClientModel(this.id, model, max_size); }

    public void SetHandsModel(char[] model) { SetEntPropString(this.id, Prop_Send, "m_szArmsModel", model); }

    public void Team(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("team", buffer) : this.GetString("team", buffer, max_size); }

    public void PrintNotify(const char[] format, any ...) {
        int len = strlen(format) + 255;
        char[] formattedText = new char[len];
        VFormat(formattedText, len, format, 3);
        PrintHintText(this.id, "%s", formattedText);
    }

    public void PrintWarning(const char[] format, any ...) {
        int len = strlen(format) + 255;
        char[] formattedText = new char[len];
        VFormat(formattedText, len, format, 3);
        PrintCenterText(this.id, "%s", formattedText);
    }

    public void ShowOverlay(char[] name) {
        char overlay[128];
        Format(overlay, sizeof(overlay), "r_screenoverlay models/eternity/overlays/%s", name);
        ClientCommand(this.id, overlay);
    }

    public void HideOverlay() {
        ClientCommand(this.id, "r_screenoverlay none");
    }

    public void PlaySound(char[] path, int channel = 40, int level = 100, int entity = 0) {
        int clients[1];
        clients[0] = this.id;

        EmitSound(clients, sizeof(clients), path, (entity == 0) ? this.id : entity, channel, level);
    }

    public void StopSound(char[] path, int channel = 40) {
        StopSound(this.id, channel, path);
    }

    public int Give(char[] item) { return GivePlayerItem(this.id, item); }

    public void RestrictWeapons() {
        int m_hMyWeapons_size = GetEntPropArraySize(this.id, Prop_Send, "m_hMyWeapons");
        int item;

        for(int index = 0; index < m_hMyWeapons_size; index++)
        { 
            item = GetEntPropEnt(this.id, Prop_Send, "m_hMyWeapons", index);

            if(item != -1)
            { 
                RemovePlayerItem(this.id, item);
                AcceptEntityInput(item, "Kill");
            } 
        }
    }

    public bool Check(char[] val, int check) {
        ArrayList checklist = this.GetArrayList(val);
        if (checklist != null) {
            for (int k=0; k < checklist.Length; k++)
                if (checklist.Get(k) == check)
                    return true;
        }
        else
        {
            if (this.GetInt(val) == check)
                return true;
        }

        return false;
    }

    public void SetBodyGroup(char[] name, int idx) {
        char plymdl[128];
        this.GetModel(plymdl, sizeof(plymdl));

        this.activebg.Set(this.mdlmeta.GetBgId(name), this.mdlmeta.GetBgIdx(name, idx));

        int bgmath = 0;
        for (int i=0; i < this.activebg.Length; i++)
            bgmath += this.activebg.Get(i);

        this.SetProp("m_nBody", bgmath);
    }
    
    public Entity CreateRagdoll() {
        Entity ragdoll;
        
        ragdoll = this.GetPropEnt("m_hRagdoll");

        if (ragdoll != null && ragdoll.id != -1)
            ragdoll.Input("Kill");

        ragdoll.Dispose();

        ragdoll = new Entity();

        char model[128];
        this.GetModel(model, sizeof(model));

        ragdoll.meta = new EntityMeta();
        ragdoll.meta.model(model);
        ragdoll.meta.bodygroup = this.GetProp("m_nBody");
        ragdoll.meta.skin = this.GetProp("m_nSkin");
        ragdoll.meta.spawnflags = 4;

        ragdoll.Create("prop_ragdoll");
        ragdoll.Spawn();
        ragdoll.SetPos(this.GetPos(), this.GetAng());

        return ragdoll;
    }

    //public void SetCollisionGroup(int group) { gamemode.mngr.SetCollisionGroup(this.id, group); }

    public void Kill() { ForcePlayerSuicide(this.id); }

    public void SilenceKill() {
        this.RestrictWeapons();
        this.inv.FullClear();
        this.Kill();
    }

    public void Kick(char[] reason) { KickClient(this.id, reason); }

    public void Setup() {
        this.SetupBaseStats(this.class);

        JSON_OBJECT pos = this.class.GetVecAng();
        if (pos != null)
            this.SetPos(pos.GetVector("vec"), pos.GetAngle("ang"));

        if (this.class.items != null)
        {
            JSON_ARRAY items = this.class.items;

            for (int i=0; i < items.Length; i++) {
                if (view_as<int>(items.GetKeyType(i)) == 0)
                {
                    char entclass[32];
                    items.GetString(i, entclass, sizeof(entclass));
                    this.inv.Add(entclass);
                }
                else
                {
                    JSON_OBJECT itemsobj = view_as<JSON_OBJECT>(items.GetObject(i));
                    StringMapSnapshot sitems = itemsobj.Snapshot();
                    int keylen;
                    int random = GetRandomInt(1,100);
                    int count = 0;
                    for (int k=0; k < sitems.Length; k++) {
                        keylen = sitems.KeyBufferSize(k);
                        char[] chance = new char[keylen];
                        sitems.GetKey(k, chance, keylen);
                        if (json_is_meta_key(chance)) continue;

                        count += StringToInt(chance);
                        if (count >= random) {
                            char entclass[32];
                            itemsobj.GetString(chance, entclass, sizeof(entclass));
                            this.inv.Add(entclass);
                            break;
                        }
                    }
                }
            }
        }

        if (this.class.weapons != null)
            for (int i=0; i < this.class.weapons.Length; i++) {
                if (view_as<int>(this.class.weapons.GetKeyType(i)) != 4) {
                    char weapon[32];
                    this.class.weapons.GetString(i, weapon, sizeof(weapon));
                    Entity ent = new Entity(this.Give(weapon));
                    ent.SetString("class", weapon);

                    any data[2];
                    data[0] = ent.id;
                    data[1] = ent;
                    
                    Base ents;
                    gamemode.GetValue("ents", ents);
                    ents.GetArrayList("entities").PushArray(data);
                }
                else
                {
                    JSON_ARRAY multigive = view_as<JSON_ARRAY>(this.class.weapons.GetObject(i));

                    for (int k=0; k < multigive.GetInt(1); k++)
                    {
                        char weapon[32];
                        multigive.GetString(0, weapon, sizeof(weapon));
                        Entity ent = new Entity(this.Give(weapon));
                        ent.SetString("class", weapon);

                        any data[2];
                        data[0] = ent.id;
                        data[1] = ent;
                        
                        Base ents;
                        gamemode.GetValue("ents", ents);
                        ents.GetArrayList("entities").PushArray(data);
                    }
                }
            }
        
        if (this.class.doors != null)
            for (int i=0; i < this.class.doors.Length; i++)
                AcceptEntityInput(this.class.doors.GetInt(i), "Open");

        if (this.class.HasKey("modelid")) {
            char modelid[32];
            this.class.ModelId(modelid, sizeof(modelid));
            this.SetModelById(modelid);

            if (this.class.HasKey("bodygroup"))
                if (view_as<int>(this.class.GetKeyType("bodygroup")) == 1) {
                    this.SetProp("m_nBody", this.class.GetInt("bodygroup", 0));
                }
                else
                {
                    JSON_OBJECT pgi = this.class.GetObject("bodygroup");
                    StringMapSnapshot spgi = pgi.Snapshot();
                    
                    int keylen;
                    for (int i=0; i < spgi.Length; i++) {
                        keylen = spgi.KeyBufferSize(i);
                        char[] name = new char[keylen];
                        spgi.GetKey(i, name, keylen);
                        if (json_is_meta_key(name)) continue;
                        
                        if (view_as<int>(pgi.GetKeyType(name)) == 1)
                            this.SetBodyGroup(name, pgi.GetInt(name));
                        else
                            this.SetBodyGroup(name, GetRandomInt(pgi.GetArray(name).GetInt(0), pgi.GetArray(name).GetInt(1)));
                    }
                }

            this.SetProp("m_nSkin", this.class.GetInt("skin", 0));
        }

        if (this.class.HasKey("hands")) {
            char handsModel[128];
            this.class.Model(handsModel, sizeof(handsModel));
            this.SetHandsModel(handsModel);
        }

        char teamName[32], className[32];
        this.Team(teamName, sizeof(teamName));
        this.class.Name(className, sizeof(className));
        PrintToChat(this.id, " \x07[SCP] \x01%t", "Show class when player spawn", teamName, className);
    }

    public void Spawn() {
        if (!IsPlayerAlive(this.id))
            CS_RespawnPlayer(this.id);
    }

    public void UpdateClass() {
        if (!IsPlayerAlive(this.id))
            CS_RespawnPlayer(this.id);
        this.RestrictWeapons();
        this.inv.FullClear();
        this.Setup();
    }
}