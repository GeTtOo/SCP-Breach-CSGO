# Player

Представление сущности игрока внутри мода.

------------------

## Описание основных параметров класса

### Свойства:  
| Название       | Описание |
| ------------- | ---------|
| `int lang` | Возвращает идентификатор языка клиента. |
| `int health` | Свойство управляющее количеством жизней. |
| `int armor` | Свойство управляющее количеством брони. |
| `float speed` | Свойство управляющее скоростью передвижения. |
| `float multipler` | Свойство-множитель скорости передвижения. |
| `bool IsSCP` | Свойство указывающее является ли игрок SCP-объектом. |
| `bool fullaccess` | Предоставляет клиенту возможность открывать любые двери на карте. |
| `bool FirstSpawn` | Возвращает значение отвечающее за то первое ли это появление игрока на карте. |
| `Class class` | Ссылка на игровой класс. |
| `Entity ragdoll` | Ссылка на рагдолл. |
| `Inventory inv` | Ссылка на инвентарь. |
| `ProgressBar progress` | Ссылка на инструмент контроля progressbar. |

### Методы:  
| Название       | Описание |
| ------------- | ---------|
| `bool GetName(char[] buffer, int max_size)` | Возвращает имя игрока. |
| `bool GetAuth(char[] buffer, int max_size, AuthIdType type)` | Возвращает SteamID (Флаги см. SM). |
| `bool IsAlive()` | Возвращает статус игрока (Жив/Мёртв). |
| `bool IsAdmin()` | Является ли игрок администратором. |
| `bool InGame()` | В игре ли игрок. |
| `Vector EyePos()` | Возвращает позицию глаз игрока. |
| `Angle GetAng()` | Возвращает углы позиционирования игрока в мире. |
| `void Team(char[] buffer, int max_size )` | Устанавливает/Возвращает команду игрока. |
| `void PrintNotify(const char[] format, any ...)` | Вывести игроку сообщение на экран. |
| `void PrintWarning(const char[] format, any ...)` | Вывести игроку сообщение-предупреждение на экран. |
| `void ShowOverlay(char[] name)` | Показать оверлей. |
| `void HideOverlay()` | Скрыть оверлей. |
| `void SetListen(Player ply, bool islisten)` | Устанавливает возможность слышимости другого игрока. |
| `void PlaySound(char[] path, int channel, int level, int entity)` | Воспроизвести звук. |
| `void StopSound(char[] path, int channel)` | Остановить воспроизведение звука. |
| `int Give(char[] item)` | Выдать оружие. |
| `void DropWeapons()` | Выбросить всё оружие в мир. |
| `void RestrictWeapons()` | Отобрать всё оружие. |
| `bool Check(char[] val, int check)` | Проверить bool значение переменной в данном объекте. |
| `Entity CreateRagdoll()` | Создаёт рагдолл. |
| `void Kill()` | Убивает игрока. |
| `void SilenceKill()` | Убивает игрока без эффектов. |
| `void Kick(char[] reason)` | Выгоняет данного игрока с сервера. |
| `void SetupBaseStats(Class class)` | Устанавливает базовые характеристики. |
| `void Setup()` | Устанавливает все параметры из класса игрока. |
| `void Spawn()` | Возрождает игрока. |
| `void UpdateClass()` | Обновляет класс игроку. |
| `void Dispose()` | Деструктор объекта. (Необходим для вычистки данного объекта из памяти сервера) |