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
| `Entity Create(char[] entclass)` | Создаёт объект сущности. |
| `Entity SetKV(char[] name, char[] value)` | Устанавливает пару Key/Value (до создания в мире). |
| `bool HasProp(char[] name, PropType type)` | Проверяет наличие свойства. |
| `Entity SetProp(char[] name, any value, PropType type, int element)` | Устанавливает значение типа int. |
| `Entity SetPropFloat(char[] name, any value, PropType type)` | Устанавливает значение типа float. |
| `Entity SetPropString(char[] name, char[] value, PropType type)` | Устанавливает значение типа string. |
| `Entity SetPropEnt(char[] name, Entity ent, PropType type)` | Устанавливает значение типа Entity. |
| `int GetProp(char[] name, PropType type, int element)` | Получает значение типа int. |
| `float GetPropFloat(char[] name, PropType type)` | Получает значение типа float. |
| `int GetPropString(char[] name, char[] value, int max_size, PropType type)` | Получает значение типа string. |
| `Entity GetPropEnt(char[] name, PropType type, int element)` | Получает значение типа Entity. |
| `Entity Input(char[] input, Entity activator, Entity caller)` | Вызывает input у объекта. |
| `void SetModel(char[] modelName)` | Устанавливает модель. |
| `void SetModelById(char[] modelid)` | Устанавливает модель по внутреннему id (устанавливается в meta разделе конфигурационного файла гейммода). |
| `void GetModel(char[] modelName, int max_size)` | Получает модель. |
| `void SetBodyGroup(char[] name, int idx)` | Устанавливает bodygroup у текущей модели объекта. |
| `void SetSkin(int skin)` | Устанавливает skin у текущей модели объекта. |
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