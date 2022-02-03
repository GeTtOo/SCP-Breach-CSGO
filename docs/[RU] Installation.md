# Установка
Переместить все файлы в корень сервера.  
В случае необходимости отключения некоторых SCP объектов, необходимо удалить (или переместить в папку disabled) файл с именем `SCP_НомерОбъекта.smx`. Находящимся по адресу `/csgo/addons/sourcemod/plugins/`.  
Также не забудьте удалить секцию персонажа в конфигурационном файле [classes.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/scp_site101/classes.json)  

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
```

## Перевод

Настройка файла локализации берется из Sourcemod. В случае отсутствия необходимого языка, будет использоваться английский. Это относится как к файлам перевода, так и к моделям и надписям на них.  

## Дополнительные плагины
[Discord API Core](https://github.com/CrazyHackGUT/Discord) - для отправления логов в Discord (необязательно)  
[Fix-Pickup-Shield](https://github.com/theelsaud/Fix-Pickup-Shield) - для подбора щитов (рекомендуется)  
[Always Weapon Skins](https://forums.alliedmods.net/showthread.php?t=237114) - для сохранения скинов игроков (необязательно)  
