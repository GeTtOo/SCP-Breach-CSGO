# Installation
Move all files to the root of the server.  
If it is necessary to disable some SCP objects, so you need to delete (or move to the disabled folder) a file named `SCP_ObjectNumber.smx`. Located at `/csgo/addons/sourcemod/plugins/`.  
Also don't forget to delete the character section in the config file [classes.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/classes.json)  

### Map list
1. [Site 101](https://steamcommunity.com/sharedfiles/filedetails/?id=2424265786)
2. WIP

If you have made a map, please contact me and I will add it to this list. It must be published in the Steam Workshop.  

## Update 
Replace files located in `/csgo/addons/sourcemod/`  
Don't forget to keep track of the content pack version. It is quite possible that someday we will update it and errors may occur.  
The content is updated by extracting all files to the root of the server.  

## Сonfiguration

Variables required to modification's work:  

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

## Translation

The localization file setup is taken from Sourcemod. If the required language is not available, English will be used. This applies both to translation files and to models and inscriptions on them.  

## Additional plugins
[RIPEXT](https://github.com/ErikMinekus/sm-ripext/releases) - required for the SCP_Discord_Log  
[Fix-Pickup-Shield](https://github.com/theelsaud/Fix-Pickup-Shield) - for picking up shields (recommended)  
[Always Weapon Skins](https://forums.alliedmods.net/showthread.php?t=237114) - for saving player skins (optional)  


## For content creators 
Some aspects of the modification aren't being configured at the moment. They should be taken into account when developing third-party content. The following are the features that you will have to put up with:

- **Countdown timer for the destruction of the complex**  
Automatically installed on all models `models/eternity/map/monitor.mdl`.  

- **Intercom timer**  
Automatically installed on all models `models/props/coop_autumn/surveillance_monitor/surveillance_monitor_32.mdl`.  

- **Entrance to the complex**  
After the destruction of the complex, the doors whose name has the substring "DoorGate" will be closed and locked.  

- **Access Panel**  
If the model `models/eternity/map/keypad.mdl` is linked to the button (via the section "parent"), the access verification system will automatically start working on it.  

- **Zombie skins**  
After the SCP-049 player is cured, his skin will be shifted by one.  

