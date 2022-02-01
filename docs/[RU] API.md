# API

Пожалуй, начнем нашу самую интересную часть документации. Здесь Вы узнаете, что такое боль.  
Далее будет описаны методы, ООП составляющая и методы для работы с ядром. Для удобства API разбито на несколько файлов и категорий.

## Оглавление:  
- [Структуры]()  
- [Базовый класс и методы доступные везде]()  
- [Как работать с сущностями Entity && Meta](/API/%5BRU%5D%20Entity.md)
  - [Интерфейс модели](/API/%5BRU%5D%20Model.md)
- [Класс игрока и все что от него наследуется](/API/%5BRU%5D%20Player.md)  
  - [Работа с инвентарём]()
  - [Прогресс бар]()
  - [Ragdoll и почему Гейб не очень хороший человек]()
  - [Подклассы игрока]()
- [Класс хранилище gamemode]()
- [Таймеры]()
- [Singleton]()
- [Менеджер]()
- [World Text]()
- [Logger]()
- [События ядра](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BRU%5D%20API.md#%D0%BE%D0%BF%D0%B8%D1%81%D0%B0%D0%BD%D0%B8%D1%8F-%D1%84%D1%83%D0%BD%D0%BA%D1%86%D0%B8%D0%B9-%D0%B8-%D1%81%D0%BE%D0%B1%D1%8B%D1%82%D0%B8%D0%B9-%D1%8F%D0%B4%D1%80%D0%B0)  

------------------

## Событий ядра  

| Событие       | Описание |
| ------------- | ---------|
| `void SCP_RegisterMetaData()` | Событие, в котором регистрируется мета события для сущностей.  |
| `void SCP_OnLoad()` | Вызывается, когда мод загружен. Аналог `OnPluginStart` в котором инициализирован весь функционал мода.  |
| `void SCP_OnUnload()` | Вызывается, когда мод выгружается. Аналог `OnPluginStop` в котором выгружаются основные глобальные объекты. |
| `void SCP_OnPlayerJoin(Player &ply)` | Вызывается, когда игрок зашёл на сервер и полностью загружен.  |
| `void SCP_OnPlayerLeave(Player &ply)` | Вызывается, когда игрок покинул сервер.  |
| `void SCP_OnPlayerSpawn(Player &ply)` | Вызывается при появление игрока.  |
| `void SCP_OnPlayerReset(Player &ply)` | Вызывается при перезапуске раунда.  |
| `void SCP_OnPlayerClear(Player &ply)` | Вызывается, когда игрок перестает существовать (отключается, умирает, при рестарте раунда).  |
| `Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype)` | Вызывается, когда игрок получил урон.  |
| `void SCP_OnPlayerDeath(Player &vic, Player &atk)` | Вызывается при смерти игрока.  |
| `void SCP_OnButtonPressed(Player &ply, int doorId)` | Вызывается при нажатие игроком кнопки.  |
| `void SCP_OnRoundStart()` | Вызывается при старте раунда, после распределения игроков.  |
| `void SCP_OnRoundEnd()` | Вызывается в конце раунда.  |
| `void SCP_OnInput(Player &ply, int buttons)` | Аналог OnPlayerRunCmd.  |
| `void SCP_OnCallActionMenu(Player &ply)` | Вызывается при открытии меню (клавиша TAB).  |
