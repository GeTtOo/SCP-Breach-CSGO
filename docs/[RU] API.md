# API

Пожалуй, начнем нашу самую интересную часть документации. Здесь Вы узнаете, что такое боль.  
Далее будет описаны методы, ООП составляющая и методы для работы с ядром. Для удобства API разбито на несколько файлов и категорий.

## Оглавление:  
- [Структуры]()  
  - [Базовый класс и методы доступные везде]()
  - [Как работать с сущностями Entity && Meta](/docs/API/RU/Entity.md)
    - [Интерфейс модели](/docs/API/RU/Entity/Model.md)
- [Класс игрока и все что от него наследуется](/docs/API/RU/Player.md)
  - [Работа с инвентарём](/docs/API/RU/Player/Inventory.md)
  - [Прогресс бар](/docs/API/RU/Player/Progress.md)
  - [Класс игрока](/docs/API/RU/Player/Class.md)
- [Класс хранилище gamemode]()
- [Таймеры]()
- [Singleton]()
- [Менеджер]()
- [World Text]()
- [Logger]()
- [События ядра](/docs/%5BRU%5D%20API.md#%D0%BE%D0%BF%D0%B8%D1%81%D0%B0%D0%BD%D0%B8%D1%8F-%D1%84%D1%83%D0%BD%D0%BA%D1%86%D0%B8%D0%B9-%D0%B8-%D1%81%D0%BE%D0%B1%D1%8B%D1%82%D0%B8%D0%B9-%D1%8F%D0%B4%D1%80%D0%B0)

------------------

Основные события ядра
===

SCP_RegisterMetaData - Регистрация метаданных объекта
---
```js
public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "ent_class", "Function name"); // arg1 Player, arg2 Entity
    gamemode.meta.RegEntEvent(ON_TOUCH, "ent_class", "Function name"); // arg1 Entity, arg2 Entity
    gamemode.meta.RegEntEvent(ON_USE, "ent_class", "Function name"); // arg1 Player, arg2 Entity, @arg3 char[] sound
    gamemode.meta.RegEntEvent(ON_DROP, "ent_class", "Function name"); // arg1 Player, arg2 Entity, @arg3 char[] sound
}
```

SCP_OnLoad - После загрузки мода
---
```js
public void SCP_OnLoad() {
    LoadTranslations("scpcore.phrases");
    PrecacheModel("models/.../*.mdl");
    PrecacheSound(".../*.mp3");
}
```

SCP_OnUnload - При выгрузке мода
---
```js
public void SCP_OnUnload() {
    GlobalObject.Dispose();
}
```

SCP_OnRoundStart - Начало раунда
---
```sp
public void SCP_OnRoundStart() {
    
}
```

SCP_OnRoundEnd - Конец раунда
---
```sp
public void SCP_OnRoundEnd() {
    
}
```

События игрока
===

SCP_OnPlayerJoin - Когда игрок зашёл на сервер
---
```js
public void SCP_OnPlayerJoin(Player &ply) {
    char clientname[32], steamid[32];
    ply.GetName(clientname, sizeof(clientname));
    ply.GetAuth(steamid, sizeof(steamid));
    PrintToChatAll("Поприветствуем же игрока %s! <%s>", playername, steamid);
}
```

SCP_OnPlayerLeave - При выходе игрока с сервера
---
```js
public void SCP_OnPlayerLeave(Player &ply) {
    
}
```

SCP_OnPlayerClear - В Случае смерти/очистки/выхода игрока
---
```sp
public void SCP_OnPlayerClear(Player &ply) {
    
}
```

SCP_PrePlayerSpawn - Перед появлением игрока на карте
---
```sp
public void SCP_PrePlayerSpawn(Player &ply) {
    
}
```

SCP_OnPlayerSpawn - При появлении игрока на карте
---
```sp
public void SCP_OnPlayerSpawn(Player &ply) {
    
}
```

SCP_OnPlayerSetupOverlay - Когда игроку устанавливается оверлей
---
```sp
public void SCP_OnPlayerSetupOverlay(Player &ply) {
    
}
```

OnPlayerTakeWeapon - При попытке подобрать оружие
---
```sp
public void OnPlayerTakeWeapon(Player &ply, Entity &ent) {
    
}
```

SCP_OnPlayerSwitchWeapon - При смене оружия
---
```sp
public void SCP_OnPlayerSwitchWeapon(Player &ply, Entity &ent) {
    
}
```

SCP_OnTakeDamage - Обработчик события получения урона игроком
---
```sp
public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor) {
    
}
```

SCP_OnPlayerDeath - Обработчик события смерти игрока
---
```sp
public void SCP_OnPlayerDeath(Player &vic, Player &atk) {

}
```

SCP_OnPlayerEscape - Срабатывает когда игрок наступает на тригер зоны выхода
---
```sp
public void SCP_OnPlayerEscape(Player &vic, Player &atk) {

}
```

SCP_OnButtonPressed - При взаимодействии с func_button
---
```sp
public void SCP_OnButtonPressed(Player &ply, int doorId) {
    
}
```

SCP_OnInput - Аналог OnPlayerRunCmd с объектом игрока
---
```sp
public void SCP_OnInput(Player &ply, int buttons) {
    
}
```

SCP_OnCallAction - Вызывается при нажатии игроком клавиши TAB
---
```sp
public void SCP_OnCallAction(Player &ply) {
    
}
```

SCP_Log_PlayerDeath - Вызывается при срабатывании какого либо события логгера. Для блокировки события необходимо вернуть false
---
```sp
public bool SCP_Log_PlayerDeath(Player &vic, Player &atk, float damage, int damagetype, int inflictor) {
    
}
```