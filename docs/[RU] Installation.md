# Установка
Переместить все файлы в корень сервера.  

В случае необходимости отключения некоторых SCP объектов, необходимо удалить (или переместить в папку disabled) файл с именем `SCP_НомерОбъекта.smx`. Находящимся по адресу `/csgo/addons/sourcemod/plugins/`.  
Также не забудьте удалить секцию персонажа в конфигурационном файле [classes.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/classes.json)  

### Список карт
1. [Site 101](https://steamcommunity.com/sharedfiles/filedetails/?id=2424265786)
2. WIP

Если Вы сделали карту, пожалуйста, свяжитесь со мной и я добавлю её в этот список. Она обязательно должна быть опубликована в Мастерской Steam.  

## Обновление 
Заменить файлы находящиеся `/csgo/addons/sourcemod/`  
Не забывайте следить за версией контент пака. Вполне возможно, что когда-нибудь мы его обновим и могут возникнуть ошибки.  
Обновление контента происходит извлечением всех файлов в корень сервера.  

## Настройка

Переменные необходимые для работы модификации:  

```c
mp_ct_default_melee	  ""
mp_ct_default_secondary	  ""
mp_ct_default_primary	  ""
mp_t_default_melee        ""
mp_t_default_secondary    ""
mp_t_default_primary      ""


mp_autokick 0
mp_freezetime 0
mp_warmuptime 30
mp_autoteambalance 0
mp_limitteams 2
mp_teammates_are_enemies 1
spec_freeze_deathanim_time 7
mp_maxmoney	0
mp_startmoney 0
mp_playercashawards 0
mp_teamcashawards 0
mp_force_pick_time 0
mp_force_assigm_teams 1
mp_radar_showall 1
mp_drop_knife_enable 1
sv_shield_hitpoints 12500
ammo_item_limit_healthshot 1
```

## Перевод

Настройка файла локализации берется из Sourcemod. В случае отсутствия необходимого языка, будет использоваться английский. Это относится как к файлам перевода, так и к моделям и надписям на них.  

## Дополнительные плагины
[RIPEXT](https://github.com/ErikMinekus/sm-ripext/releases) - необходимо для работы плагина SCP_Discord_Log  
[Fix-Pickup-Shield](https://github.com/theelsaud/Fix-Pickup-Shield) - для подбирания щитов (рекомендуется)  
[Always Weapon Skins](https://forums.alliedmods.net/showthread.php?t=237114) - для сохранения скинов игроков (необязательно)  
[-N- Arms Fix](https://github.com/NomisCZ/Arms-Fix/releases) - для исправления проблем с нестандартными руками (рекомендуется)  


## Для создателей контента 
Некоторые аспекты модификации на данный момент не настраиваются. Их стоит учесть при разработке стороннего контента. Ниже перечисляются особенности, с которыми придется смириться:

- **Таймер отсчета уничтожения комплекса**  
Автоматически устанавливается на всех моделях `models/eternity/map/monitor.mdl`.  

- **Таймер интеркома**  
Автоматически устанавливается на всех моделях `models/props/coop_autumn/surveillance_monitor/surveillance_monitor_32.mdl`. 

- **Вход в комплекс**  
После уничтожения комплекса, двери, в чьем имени есть подстрока «DoorGate», будут закрыты и заблокированы.  

- **Панель доступа**  
Если модель `models/eternity/map/keypad.mdl` привязана к кнопке (через поле parent), на ней автоматически начнет работать система проверки доступа.  

- **Скины зомби**  
После излечения SCP-049 игрока, ему будет смещен скин на один.  

