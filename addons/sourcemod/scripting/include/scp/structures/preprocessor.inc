#include <json>

methodmap PreProcessor < Base {

    public PreProcessor(JSON_Object input) {
        StringMapSnapshot sinput = input.Snapshot();
        char key[32];
        sinput.GetKey(0, key, sizeof(key));

        delete sinput;
    }

    public int Random(int min, int max) { return GetRandomInt(min, max); }

    public float RandomFloat(float min, float max) { return GetRandomFloat(min, max); }

    public void RandomString(ArrayList list, char[] rndval) { list.GetString(0, rndval, 128); }
}