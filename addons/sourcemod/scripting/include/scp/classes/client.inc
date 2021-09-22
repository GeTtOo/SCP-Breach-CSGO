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

    property Vector deathpos {
        public set(Vector val) { this.RemoveValue("deathpos"); this.SetVector("deathpos", val); }
        public get() { return view_as<Vector>(this.GetVector("deathpos", null)); }
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

    public void SetModel(char[] model) { SetEntityModel(this.id, model); }

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

    public void TimerSimple(int delay, char[] funcname, any args = false) {
        char timername[64], pname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
        FormatEx(timername, sizeof(timername), "%s_plyid-%i_%i_%i", pname, this.id, GetTime(), GetRandomInt(1, 1000));
        gamemode.timer.Create(timername, delay, 1, funcname, args);
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

    //public void SetCollisionGroup(int group) { gamemode.mngr.SetCollisionGroup(this.id, group); }

    public void Kill() { ForcePlayerSuicide(this.id); }

    public void SilenceKill() {
        this.RestrictWeapons();
        this.inv.Clear();
        this.Kill();
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
                    this.Give(weapon);
                }
                else
                {
                    JSON_ARRAY multigive = view_as<JSON_ARRAY>(this.class.weapons.GetObject(i));

                    for (int k=0; k < multigive.GetInt(1); k++)
                    {
                        char weapon[32];
                        multigive.GetString(0, weapon, sizeof(weapon));
                        this.Give(weapon);
                    }
                }
            }
        
        if (this.class.doors != null)
            for (int i=0; i < this.class.doors.Length; i++)
                AcceptEntityInput(this.class.doors.GetInt(i), "Open");

        if (this.class.HasKey("model")) {
            char playerModel[128];
            this.class.Model(playerModel, sizeof(playerModel));
            this.SetModel(playerModel);

            if (this.class.HasKey("bodygroup"))
                if (view_as<int>(this.class.GetKeyType("bodygroup")) == 1) {
                    this.SetProp("m_nBody", this.class.GetInt("bodygroup", 0));
                }
                else
                {
                    ArrayList modelMetaData = new ArrayList();
                    
                    JSON_ARRAY metaArr = gamemode.config.meta;

                    for (int i=0; i < metaArr.Length; i++) {
                        JSON_OBJECT metaobj = view_as<JSON_OBJECT>(metaArr.GetObject(i));
                        
                        char modelName[128];
                        metaobj.GetString("model", modelName, sizeof(modelName));

                        if (StrEqual(playerModel, modelName)) {
                            JSON_ARRAY bgdata = metaobj.GetArray("bgdata");

                            for (int k=0; k < bgdata.Length; k++) {
                                modelMetaData.Push(bgdata.GetInt(k));
                            }
                        }
                    }

                    JSON_ARRAY sbgarr = this.class.GetArray("bodygroup");

                    int bodyGroupAddress = 0;
                    
                    for (int groupIndex = 0; groupIndex < sbgarr.Length; groupIndex++) {
                        int groupValue;
                        
                        if (view_as<int>(sbgarr.GetKeyType(groupIndex)) == 1)
                            groupValue = sbgarr.GetInt(groupIndex);
                        else
                            groupValue = GetRandomInt(sbgarr.GetArray(groupIndex).GetInt(0), sbgarr.GetArray(groupIndex).GetInt(1));

                        int idx = 1;
                        if (groupValue != 0) {
                            for (int k=0; k < groupIndex; k++)
                            idx += idx * modelMetaData.Get(k);

                            bodyGroupAddress += idx * groupValue;
                        }
                    }

                    this.SetProp("m_nBody", bodyGroupAddress);
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
        this.inv.Clear();
        this.Setup();
    }
}