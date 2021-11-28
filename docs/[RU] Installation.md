# Установка
Переместить все файлы в корень сервера.  
В случае необходимости отключения некоторых SCP объектов, необходимо удалить (или переместить в папку disabled) файл с именем `SCP_НомерОбъекта.smx`. Находящимся по адресу `/csgo/addons/sourcemod/plugins/`.  
Также не забудьте удалить секцию персонажа в конфигурационном файле [classes.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/scp_site101/classes.json)  

## Обновление 
Заменить файлы находящиеся `/csgo/addons/sourcemod/plugins/`  
Не забывайте следить за версией контент пака. Вполне возможно, что когда-нибудь мы его обновим и могут возникнуть ошибку.  
Обновление контента происходит извлечением всех файлов в корень сервера.  

## Настройка

Переменные необходимые для работы модификации:  

```c
mp_autokick 0
mp_freezetime 0
mp_warmuptime 0
mp_autoteambalance 0
mp_limitteams 0
mp_teammates_are_enemies 1
spec_freeze_deathanim_time 7
mp_maxmoney	 0
mp_startmoney 0
mp_playercashawards 0
mp_teamcashawards 0
```

## Дополнительные плагины
[Discord API Core](https://google.com) - для отправления логов в Discord (необязательно)
