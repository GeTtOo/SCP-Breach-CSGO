# Entity

Основное представление базовой сущности внутри гейммода.

------------------

## Описание основных параметров класса

### Свойства:  
| Название       | Описание |
| ------------- | ---------|
| `EntityMeta meta` | Содержит ссылки на функции при происхождении, а так же внутренние параметры. |
| `Mdl model` | Содержит ссылку на методы управления моделью объекта.  |
| `int id` | Уникальный идентификатор сущности в игре. |
| `bool spawned` | Содержит информацию о состоянии сущности в мире. |
| `ArrayList bglist` | Содержит текущий список bodygroups. |

### Методы:  
| Название       | Описание |
| ------------- | ---------|
| `Entity Create(char[] entclass)` | Создаёт объект. |
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
| `void SetMoveType(MoveType type)` | Устанавливает MoveType (см. флаги SM). |
| `void TimerSimple(int delay, char[] funcname, any args)` | Простой таймер привязанный к текущему объекту. |
| `Entity SetClass(char[] name)` | Устанавливает класс. |
| `void GetClass(char[] name, int max_size)` | Получает класс. |
| `bool IsClass(char[] equalClass)` | Выполняет проверку на класс. |
| `Vector GetPos()` | Получает позицию. |
| `Angle GetAng()` | Получает углы позиционирования. |
| `Entity SetPos(Vector vec, Angle ang)` | Установить позицию. |
| `Entity Push(Entity ent, float force)` | Толкает объект в направлении от указанного объекта. |
| `Entity ReversePush(Vector vec, float force)` | Толкает объект в направлении указанной позиции. |
| `Entity SetHook(SDKHookType type, SDKHookCB cb)` | Устанавливает хук. (Типы хуков смотри в SM). |
| `Entity RemoveHook(SDKHookType type, SDKHookCB cb)` | Снимает хук с объекта. |
| `Entity Spawn()` | Инициализирует объект в мире. |
| `Entity Activate()` | Активирует объект. (Необходимо для некотрых стандартных игровых сущностей) |
| `void Dispose()` | Деструктор объекта. (Необходим для вычистки данного объекта из памяти сервера) |
| `void WorldRemove()` | Удаляет объект из мира. |
| `void Remove()` | Удаляет объект из мира, а затем вызывает деструктор. |