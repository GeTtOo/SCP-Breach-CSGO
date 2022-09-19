//////////////////////////////////////////////////////////////////////////////
//
//                               Load Configs 
//
//////////////////////////////////////////////////////////////////////////////

public void LoadAndPrecacheFileTable()
{
    PrecacheSound("weapons/c4/c4_exp_deb1.wav");
    Handle hFile = OpenFile("addons/sourcemod/configs/scp/downloads.txt", "r");
    
    if(hFile)
    {
        int size = 0;
        char buffer[PLATFORM_MAX_PATH];
        
        while(!IsEndOfFile(hFile) && ReadFileLine(hFile, buffer, sizeof(buffer)))
        {
            if(TrimString(buffer) > 2 && IsCharAlpha(buffer[0]))
            {
                AddFileToDownloadsTable(buffer);
                size = strlen(buffer);

                if(StrContains(buffer, ".mdl", false) == (size - 4))
                {
                    PrecacheModel(buffer);
                }
                else if(StrContains(buffer, ".wav", false) == (size - 4) || StrContains(buffer, ".mp3", false) == (size - 4))
                {
                    strcopy(buffer, sizeof(buffer), buffer[6]);
                    Format(buffer, sizeof(buffer), "%s%s", "*/", buffer);
                    FakePrecacheSound(buffer);
                }
            }
        }

        delete hFile;
    }
    else
    {
        LogError("Can't find downloads.txt");
    }
}

public void LoadMetaData()
{
    LoadModels();
    LoadEntities();
}

public void LoadEntities()
{
    JSON_OBJECT entities = Utils.ReadCurMapConfig("entities");
    StringMapSnapshot sents = entities.Snapshot();

    int keylen;
    for (int i = 0; i < sents.Length; i++)
    {
        keylen = sents.KeyBufferSize(i);
        char[] entclass = new char[keylen];
        sents.GetKey(i, entclass, keylen);
        
        gamemode.meta.RegisterEntity(entclass, view_as<EntityMeta>(GetSubMeta(entities.Get(entclass))));
    }

    delete sents;

    entities.Dispose();
}

public Base GetSubMeta(JSON_OBJECT obj)
{
    StringMapSnapshot sobj = obj.Snapshot();
    Base data = new Base();
        
    for (int k=0; k < sobj.Length; k++)
    {
        int kl = sobj.KeyBufferSize(k);
        char[] keyname = new char[kl];
        sobj.GetKey(k, keyname, kl);

        switch(obj.GetType(keyname))
        {
            case view_as<JSON_TYPE>(String): {
                char str[128];
                obj.GetString(keyname, str, sizeof(str));
                data.SetString(keyname, str);
            }
            case Int: { data.SetInt(keyname, obj.GetInt(keyname)); }
            case view_as<JSON_TYPE>(Float): { data.SetFloat(keyname, obj.GetFloat(keyname)); }
            case Bool: { data.SetBool(keyname, obj.GetBool(keyname)); }
            case Object: {
                JSON_OBJECT subobj = obj.Get(keyname);
                if (!subobj.IsArray)
                {
                    data.SetBase(keyname, GetSubMeta(subobj));
                    continue;
                }
                
                ArrayList list = new ArrayList();

                for (int v=0; v < view_as<JSON_ARRAY>(subobj).Length; v++)
                    list.Push(view_as<JSON_ARRAY>(subobj).GetInt(v));

                data.SetArrayList(keyname, list);
            }
        }
    }

    delete sobj;

    return data;
}

public void LoadModels()
{
    JSON_ARRAY modelsdata = gamemode.config.Get("meta").GetArr("models");

    for (int i=0; i < modelsdata.Length; i++)
    {
        JSON_OBJECT mdlmeta = view_as<JSON_OBJECT>(modelsdata.Get(i));

        char id[32], path[128];
        mdlmeta.GetString("path", path, sizeof(path));
        mdlmeta.GetString("id", id, sizeof(id));
        
        JSON_ARRAY mdlbg = mdlmeta.GetArr("bginf");
        
        ModelMeta modeldata = new ModelMeta();
        ArrayList groups = new ArrayList();
        
        for (int k=0; k < mdlbg.Length; k++)
            groups.Push(mdlbg.GetInt(k));
        
        modeldata.Path(path);
        modeldata.SetArrayList("bg", groups);

        gamemode.meta.RegisterModel(id, modeldata);
    }

    modelsdata.Dispose();
}

public void SetupMapRegions()
{
    JSON_ARRAY regions = gamemode.config.regions;

    for (int i=0; i < regions.Length; i++) 
    {
        JSON_OBJECT region = view_as<JSON_OBJECT>(regions.Get(i));
        Vector pos = region.GetVector("pos");
        char radius[5], name[128];
        IntToString(region.GetInt("radius"),radius,sizeof(radius));
        region.GetString("ltag",name,sizeof(name));
        
        Format(name, sizeof(name), "%T", name, LANG_SERVER);

        Entity ent = ents.Create("info_map_region").SetPos(pos);
        DispatchKeyValue(ent.id,"radius",radius);
        DispatchKeyValue(ent.id,"token",name);
        ent.Spawn();
    }
}

public void SpawnItemsOnMap()
{
    JSON_OBJECT spawnmap = gamemode.config.spawnmap;
    StringMapSnapshot snapshot = spawnmap.Snapshot();

    for (int i=0; i < snapshot.Length; i++) 
    {
        int itemlen = snapshot.KeyBufferSize(i);
        char[] item = new char[itemlen];
        snapshot.GetKey(i, item, itemlen);
        
        JSON_ARRAY rawDataArr = view_as<JSON_ARRAY>(spawnmap.Get(item));

        for (int v=0; v < rawDataArr.Length; v++) 
        {
            JSON_OBJECT data = view_as<JSON_OBJECT>(rawDataArr.Get(v));

            if (data.IsArray)
            {
                data = view_as<JSON_OBJECT>(view_as<JSON_ARRAY>(data).Get(GetRandomInt(0, view_as<JSON_ARRAY>(data).Length - 1)));
                
                ents.Create(item)
                .SetPos(data.GetVector("vec"), data.GetAngle("ang"))
                .Spawn();
                
                continue;
            }
            
            StringMapSnapshot sdata = data.Snapshot();

            int random = GetRandomInt(1,100);
            int count = 0;
            for (int k=0; k < sdata.Length; k++)
            {
                int chancekeylen = sdata.KeyBufferSize(k);
                char[] strchance = new char[chancekeylen];
                sdata.GetKey(k, strchance, chancekeylen);

                int chance = StringToInt(strchance);

                if (chance != 0)
                {
                    count += chance;
                    if (count >= random) {
                        data = data.Get(strchance);

                        ents.Create(item)
                        .SetPos(data.GetVector("vec"), data.GetAngle("ang"))
                        .Spawn();
                        
                        delete sdata;
                        break;
                    }
                }
            }

            delete sdata;

            if (count != 0)
                break;

            if (GetRandomInt(1, 100) <= data.GetInt("chance"))
                ents.Create(item)
                .SetPos(data.GetVector("vec"), data.GetAngle("ang"))
                .Spawn();
        }
    }

    delete snapshot;
}