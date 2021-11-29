# Entity

Основное представление базовой сущности внутри гейммода.

------------------

## Описание основных параметров класса

### Свойства:  
| Название       | Описание |
| ------------- | ---------|
| `EntityMeta meta` | Содержит ссылки на функции при происхождении, а так же внутренние параметры. |
| `ModelMeta mdlmeta` | Содержит дополнительную информацию о модели (BodyGroups). |
| `int id` | Уникальный идентификатор сущности в игре. |
| `bool spawned` | Содержит информацию о состоянии сущности в мире. |
| `ArrayList bglist` | Содержит текущий список bodygroups. |

### Методы:  
| Название       | Описание |
| ------------- | ---------|
| `Entity Create(char[] entclass)` | Содержит ссылки на функции при происхождении. |
| `bool HasProp(char[] name, PropType type)` | Содержит ссылки на функции при происхождении. |
| `Entity SetKV(char[] name, char[] value)` | Содержит ссылки на функции при происхождении. |
| `Entity SetProp(char[] name, any value, PropType type, int element)` | Содержит ссылки на функции при происхождении. |
| `Entity SetPropFloat(char[] name, any value, PropType type)` | Содержит ссылки на функции при происхождении. |
| `Entity SetPropString(char[] name, char[] value, PropType type)` | Содержит ссылки на функции при происхождении. |
| `Entity SetPropEnt(char[] name, Entity ent, PropType type)` | Содержит ссылки на функции при происхождении. |
| `int GetProp(char[] name, PropType type, int element)` | Содержит ссылки на функции при происхождении. |
| `float GetPropFloat(char[] name, PropType type)` | Содержит ссылки на функции при происхождении. |
| `int GetPropString(char[] name, char[] value, int max_size, PropType type)` | Содержит ссылки на функции при происхождении. |
| `Entity GetPropEnt(char[] name, PropType type, int element)` | Содержит ссылки на функции при происхождении. |
| `Entity Input(char[] input, Entity activator, Entity caller)` | Содержит ссылки на функции при происхождении. |
| `void SetModel(char[] modelName)` | Содержит ссылки на функции при происхождении. |
| `void SetModelById(char[] modelid)` | Содержит ссылки на функции при происхождении. |
| `void GetModel(char[] modelName, int max_size)` | Содержит ссылки на функции при происхождении. |
| `void SetBodyGroup(char[] name, int idx)` | Содержит ссылки на функции при происхождении. |
| `void SetSkin(int skin)` | Содержит ссылки на функции при происхождении. |
| `void SetRenderMode(RenderMode mode)` | Содержит ссылки на функции при происхождении. |
| `void SetMoveType(MoveType type)` | Содержит ссылки на функции при происхождении. |
| `void TimerSimple(int delay, char[] funcname, any args)` | Содержит ссылки на функции при происхождении. |
| `Entity SetClass(char[] name)` | Содержит ссылки на функции при происхождении. |
| `void GetClass(char[] name, int max_size)` | Содержит ссылки на функции при происхождении. |
| `bool IsClass(char[] equalClass)` | Содержит ссылки на функции при происхождении. |
| `Vector GetPos()` | Содержит ссылки на функции при происхождении. |
| `Angle GetAng()` | Содержит ссылки на функции при происхождении. |
| `Entity SetPos(Vector vec, Angle ang)` | Содержит ссылки на функции при происхождении. |
| `Entity Push(Entity ent, float force)` | Содержит ссылки на функции при происхождении. |
| `Entity ReversePush(Vector vec, float force)` | Содержит ссылки на функции при происхождении. |
| `Entity SetHook(SDKHookType type, SDKHookCB cb)` | Содержит ссылки на функции при происхождении. |
| `Entity RemoveHook(SDKHookType type, SDKHookCB cb)` | Содержит ссылки на функции при происхождении. |
| `Entity Spawn()` | Содержит ссылки на функции при происхождении. |
| `Entity Activate()` | Содержит ссылки на функции при происхождении. |
| `void Dispose()` | Содержит ссылки на функции при происхождении. |
| `void WorldRemove()` | Содержит ссылки на функции при происхождении. |
| `void Remove()` | Содержит ссылки на функции при происхождении. |