# Configuration

All configuration files are located in `/addons/sourcemod/configs/scp/*map_name*`. If you use a map from the workshop, the path will change to `/addons/sourcemod/configs/scp/workshop/*id*/*map_name*`.  
With the exception of the user content download file `downloads.txt`. He is located in `/addons/sourcemod/configs/scp/`  
For each map needs its own folder with configuration files.  

For any manipulations, it is recommended to use a text editor with support for syntax highlighting and errors.  

## Table of contents
1. [Setting up Player Classes](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#classesjson)
2. [General settings of the mod and its modules](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#configjson)
3. [Doors and access to them](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#doorsjson)
4. [Own items and their customization, access levels](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#entitiesjson)
5. [Pointers on the map](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#regionsjson)
6. [Spawn of items on the map](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#spawnlistjson)
7. [Recipes of SCP-914](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#914json3)
8. [List of commands](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BEN%5D%20Configuration.md#list-of-commands)

------------------

## [Classes.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/classes.json)

Responsible for setting up player classes.  
By default, it is divided into 4 groups (**Personnel**, **MOG**, **Chaos**, **SCP**). You can add as many teams as you want. To do this, just add a section (for example, GOK).  

| Parameter     | Value         | Details  |
| ------------- |:-------------:| ---------|
| percent       | Number from 0 to 100 | Percentage of players in teams [^1]|
| priority      | true or false    | Sets the team to priority [^2]|
| randompick    | true or false    | Random class selection|
| reinforce     | true or false    | Is it possible for reinforcements to arrive for this team|

Block starts with the command name, it must be unique.

**Example of a block**  
```json
"SCP": {
        "percent":10,
        "priority":true,
        "randompick":true,
	"reinforce":false,
        "classes": {
		Structure is described in detail below
	}
}
```

### Classes – this section describes the classes of the command  
Not all parameters are required to be configured. If the parameter isn't specified, it will be assigned a standard value (for example, health will be equal to 100) or it will be ignored (in the case of doors that are needed, mainly by the SCP class).

| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| percent   | Number from 0 to 100            | Percentage ratio of this class in the team when appearing [^1]|
| priority  | true or false               | Sets the class to priority [^2]|
| overlay   | "overlay name"                | Overlay that will be shown to the player when it appears |
| health    | Number from 1 to 100            | Quantity hp |
| armor     | Number from 0 to 100            | Quantity armor |
| helmet    | true or false               | Giving a helmet to a player |
| speed     | Integer                  | Player Speed |
| multipler | Floating spot number     | Base Speed Multiplier |
| items     | \["Item 1", "Item 2"\] | The items that the player will have (access cards, etc.). Described in the file entities.json |
| weapons   | \["Weapon 1", "Weapon 2"\]   | Weapon that the player will have and its quantity |
| doors     | \["Door 1", "Door 2"\]     | Door ID that will be opened when it appears. |
| comment   | "Text"                      | Section for notes |

### Player's position when appearing  
Setting up a player's position can consist of either a single line or a large array. In the second case, the players will be randomly assigned to the spots.  

| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| pos |               | The beginning of the block |
| vec | \[1, 1, 1\]   | Point of appearance of players on the map |
| ang | \[0, 180, 0\] | Angle of rotation of the player when appearing |

### Player Model
Models are installed in two ways (simple and advanced).  

**Simple method:**  
1. Specify the path to the model.
2. Don't forget to add it to the downloads file [downloads.txt](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/downloads.txt).

Example:
```json
"model":"models/player/custom_player/eternity/scp_049.mdl"
```

**Advanced method:**  
This method supports randomization of body groups and installation of skins.  

The model is divided into several groups:  
| Parameter      | Value     |
| ------------- |------------- |
| base    | Body                  |
| head    | Head                |
| eyes    | Eyes                 |
| helmet  | Helmet                  |
| mask    | Face Accessories       |
| chevron | Chevron                |
| armor   | Armor, waistcoat, etc.  |
| belt    | Belt                |
| legs    | Legs                  |
| pl      | Pocket on the left leg  |
| pr      | Pocket on the right leg |

It's not necessary to strictly follow the name. For example, when creating a model, you can write a "backpack" to the "chevron" parameter. But we highly recommend that you still stick to the concept!  

The main parameters of the model:  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| id | Integer | Matches the ID from the Meta block in [config.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/config.json) |
| bodygroups | Integer Object | A data object with the ids of the body groups. Supports randomization. |
| skin | Integer | Skin number |

### Escape or evacuation the player  
If this parameter is specified, the player will be able to leave the complex and the specified team will be credited with a victory point.

| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| trigger | Integer | ID of the escape/escape trigger |
| team | "Team" | The team that the player will get after evacuation/escape |
| class | "Sub-class" | The class that the player will get after evacuation/escape|
| savepos | true or false | Save the player's position after evacuation/escape or teleport to the point of spawning of the class |
| wp | "Team" | The team that will get a victory point during evacuation/escape |

### Iteam and weapon 
These blocks are extremely variable. It is possible to set the randomization of the item, its quantity, etc.  

Randomization of several subjects is indicated in the format `[{"Percent":"Item", "Percent":"Item"}]`. After the player spawning, one of the listed items will drop out to it[^1].  
**Example:**  
```json
"items":[{"70":"card_scientist","30":"card_major_scientist"}]
```

The number of items is indicated in the format `[["Item Name", quantity]]`  
**Example:**  
```json
"items":[["weapon_healthshot", 5]]
```
------------------

### Example of a class block  
```json
"Scientist": {
  "pos":[
	{"vec":[-2751,-2967,0]},
	{"vec":[-7776,-2569,0]},
	{"vec":[-2279,-2745,0],"ang":[0,225,0]},
	{"vec":[-5859,-5337,0],"ang":[0,180,0]}
  ],
  "percent":20,
  "priority": true,
  "overlay":"scientist",
  "model":{
    	"id":"scientist",
    	"bodygroups":{
		"base":0,
		"head":[0,1,2,3,4,5],
		"eyes":[0,1,2,3]
        },
	"skin":0
  },
  "escape":{
	"trigger":330025,
	"team":"MOG",
	"class":"Lieutenant",
	"savepos":true,
	"wp":"MOG"
  }
  "health": 100,
  "armor": 100,
  "helmet":true,
  "speed": 260,
  "multipler": 1.0,
  "items":[{"70":"card_scientist","30":"card_major_scientist"},"268_cap"],
  "weapons": ["weapon_m4a1_silencer", "weapon_usp_silencer", ["weapon_healthshot", 2], "weapon_tagrenade"],
  "doors": [447],
  "comment": "test"
}
```

## [config.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/config.json)

In this file, the main parameters of the mod are configured.  

### Main parameters  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| ff | true or false | Allows or prohibits fire on their own |
| debug | true or false | Configuring the output of information for developers [^3] |
| logmode | 0, 1, 2, 3 | Type of log output [^10] |
| DefaultGlobalClass | "Team Name" | The player's default team |
| DefaultClass | "Class name" | Default player class [^4] |
| invsize | Integer | The maximum size of the player's inventory |
| usablecards | true or false | Method of operation of access cards [^9] |
| tsto | Integer | The time when the overlay with info is displayed to the player at the start of the round |
| psars | Integer | The time from the start of the round after which new players will stop spawning |
| AdminRoom | \[0, 0, 0\] | Coordinates of the administrator's room [^5] |

### Reinforcements  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| time | Integer | The time, once in which the percentage of the dead to the living is checked |
| ratiodeadplayers | Integer | The percentage of the dead to the living, which is necessary for the arrival of reinforcements |

### Meta, models and body groups  
This section contains information about the model that cannot be obtained from the game[^11].  

| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| id | "Name" | Short model ID (must be unique) |
| path | "Path" | The path to the model |
| bginf | \[0,0,0,0,0,0,0,0,0,0,0\] | Setting up body groups (more details below) |

Bginf – information about the number of body groups in the model. At the moment, there are 11 body groups in the modification. You can find out more in [player class settings](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/docs/%5BRU%5D%20Configuration.md#classesjson). You need to specify the number of details for each body group.
You can find out the quantity in Model Viewer from Valve (included in the CS:GO SDK) or by decompiling the model (a file with the extension QC).

### Alpha Warhead  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| time | Integer | The time from the moment of activation to the explosion of the warhead |
| killpos | Integer | The coordinate of the height below which all players will be killed in an explosion |
| sound | "The path to the audio file" | The audio file that will be played when the warhead is activated |
| cancelsound | "The path to the audio file" | The audio file that will be played when canceled |

**Button Block:**  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| ready | Integer | ID of the button on the card that unlocks the activation button |
| active | Integer | ID of the button on the map that activates the warhead |
| cancel | Integer | ID of the button on the map that deactivates the warhead |

**Timers**  
Array of coordinates where the countdown timer will appear before the explosion of the Alpha warhead.  
Format: `{"pos":[Position],"ang":[Angle],"color":[Text color (RGB)],"size":Text size}`  

Example:  
```json
"spawnlist":[{"pos":[0,0,0],"ang":[0,0,0],"color":[255,255,255],"size":15}]
```

### Teleportation  
You can place teleportation points on the map for faster movement by administrators of players or yourself.  
The teleportation point is written in the format: `"Name":{"vec":[coordinates], "ang":[angle of rotation of the player's camera]}`  
**Example:**  
```json
"D spawn":{"vec":[-2413,-5632,0],"ang":[0,0,0]},
```

### \[Optional\] SCP-914[^6]  
Any changes are highly not recommended![^3]  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| usemathcounter | true or false | Mode selection (countername or switchbutton) |
| countername | "Name" | Counter's name |
| switchbutton | Integer | ID of the quality switch button |
| runbutton | Integer | ID of the machine start button |
| runtime | Integer | the time before teleporting to another section after pressing the runbutton |
| searchzone | \[\[Coordinates\], \[Coordinates\]\] | Two vectors from which the item search cube is built [^7] |
| distance | \[Coordinates\] | Displacement vector relative to the first camera [^8] |  

### \[Optional\] SCP-049[^6]  
The parameters listed below are configured in the "revive" block"
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| time | Integer | The time it will take to "heal" the player |
| multi | true or false | "Heal" several players at once |
| inpvs | true or false | Raise players in or around the 049 visibility zone |

### \[Optional\] SCP-173[^6]  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| radius | Integer | The distance from player to SCP, which is necessary to trigger the lock when viewing and activate the blinking effect |
| blinktime | Integer | After how many seconds the player will blink |

**Block of sounds:**  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| neckbroke | "The path to the sound file" | The sound that will be played when the player is killed |
| death | "The path to the sound file" | The sound that will be played at the death of SCP-173 |

Both parameters support randomization.  

### \[Optional\] SCP-457[^6]  
| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| ignitetime | Integer | The player's burn time on impact |

------------------

### Example of a configuration file:  
```json
    "ff":true,
    "debug":true,
    "logmode":0,
    "DefaultGlobalClass":"Personnel",
    "DefaultClass":"D Class",
    "invsize": 5,
    "usablecards":true,
    "tsto":5,
    "psars": 30,
    "AdminRoom": [-4890,-6340,0],
    "reinforce":{
        "time":180,
        "ratiodeadplayers":60
    },
    "meta": {
        "models": [
            {"id":"dclass","path":"models/player/custom_player/eternity/d_class.mdl","bginf":[0,7,4,1,0,0,1,0,1,0,0]},
            {"id":"scientist","path":"models/player/custom_player/eternity/scientists.mdl","bginf":[1,6,4,0,0,0,0,0,0,0,0]},
            {"id":"mog","path":"models/player/custom_player/eternity/mog.mdl","bginf":[0,4,3,5,2,15,3,3,0,1,1]},
            {"id":"chaos","path":"models/player/custom_player/eternity/chaos.mdl","bginf":[3,4,4,4,2,0,0,0,3,0,0]}
        ]
    },
    "nuke": {
        "time":110,
        "killpos":1690,
        "sound":"*/scp/warhead_announcement.mp3",
        "cancelsound":"*/scp/warhead_canceled.mp3",
        "buttons":{
            "ready":356001,
            "active":356023,
            "cancel":356001
        },
        "spawnlist":[]
    },
    "teleport": {
        "D spawn":{"vec":[-2413,-5632,0],"ang":[0,0,0]},
        "MOG spawn":{"vec":[-10739,-5920,1712],"ang":[0,0,0]},
        "Chaos spawn":{"vec":[-7128,-5693,1712],"ang":[0,-135,0]},
        "Security spawn":{"vec":[-11089,-2298,-978],"ang":[1,-87,0]},
        "Medic spawn":{"vec":[-6107,-7136,14],"ang":[-2,-180,0]},
        "Light armory":{"vec":[-5753,-1513,0],"ang":[0,-135,0]},
        "Heavy armory":{"vec":[-9423,2250,0],"ang":[0,90,0]},
        "Nuke room":{"vec":[-7821,-5978,200],"ang":[0,0,0]},
        "Nuke site":{"vec":[-7939,-5752,0],"ang":[0,135,0]},
        "SCP-035":{"vec":[-7683,-147,18],"ang":[-3,89,0]},
        "SCP-049":{"vec":[-8531,1470,28],"ang":[0,-89,0]},
        "SCP-079":{"vec":[-776,-4708,22],"ang":[3,-91,0]},
        "SCP-096":{"vec":[-12544,-2558,30],"ang":[1,180,0]},
        "SCP-106":{"vec":[-11141,-287,30],"ang":[-2,89,0]},
        "SCP-173":{"vec":[-1811,-1343,28],"ang":[1,91,0]},
        "SCP-457":{"vec":[-5440,-3092,21],"ang":[1,-1,0]},
        "SCP-914":{"vec":[3100,-2231,0],"ang":[0,0,0]},
        "SCP-996":{"vec":[-5406,2626,4],"ang":[-2,-1,0]}
    },
    "[SCP] 914": {
        "usemathcounter":true,
        "countername": "scp_914_logic_counter",
        "switchbutton": 443873,
        "runbutton": 516151,
        "runtime": 8,
        "searchzone":[[3630, -2072, 20],[3762, -1947, 90]],
        "distance":[0, 425, 0]
    },
    "[SCP] 049": {
        "revive":{
            "time":3,
            "multi":true,
            "inpvs":true
        }
    },
    "[SCP] 173": {
        "radius":2000,
        "blinktime":5,
        "sound": {
            "neckbroke":"*/scp/173/neckbroke.mp3",
            "death": [
                "*/scp/173/death1.mp3",
                "*/scp/173/death2.mp3",
                "*/scp/173/death3.mp3"
            ]
        }
    },
    "[SCP] 457": {
        "ignitetime":10
    }
```


## [doors.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/doors.json)

In this file, doors and access to them are configured. The block starts with the ID Door or button, which can be found when the parameter is enabled `debug` в [config.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/config.json).  

| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| location | "Text" | Title, so that you don't get confused in this pile of ID (not used in fashion)|
| access | Number from 0 to 10 | Access level |
| scp | true or false | Can SCP open the door |

**Example:**  
```json
"354154": {
	"location": "Gate A Up",
	"access": 0,
	"scp": false
}
```

## [entities.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/entities.json)

A file for customizing your items (entity). We highly don't recommend any modifications to this file (especially if you do not understand how it works).  

| Parameter      | Value      | Details  |
| ------------- |:-------------:| ---------|
| model | "The path to the model" | Item Model |
| mass | Floating spot number | Item weight |
| skin | Integer | Item Texture number |
| access | Array of access levels | Access that the subject gives |
| cooldown | Integer | Delay time after use |

**Example:**  
```json
"card_mog_commander": {
	"model":"models/eternity/props/keycard.mdl",
	"mass":0.2,
	"skin":3,
	"access":[1,2,4,5,6,7,8,9]
}
```

### Access levels 
Делятся на 10 категорий:
1. Holding cells, level 1
2. Holding cells, level 2
3. Holding cells, level 3
4. Armory, level 1
5. Armory, level 2
6. Armory, level 3
7. Checkpoints
8. Internal communication
9. Escapes
10. Alpha Warhead

Each level is set separately. 
**An example of a card with access level - Content Cameras 1 and Internal communication:**  
```json
"card_facility_manager": {
	"model":"models/eternity/props/keycard.mdl",
	"mass":0.2,
	"skin":1,
	"access":[1,9]
}
```

## [regions.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/regions.json)

Setting up your own zones (signs on the map).  
String format: `{"radius":Radius,"pos":[Position],"ltag":"Localization string name"}`  
The name of the location itself is stored in the translation file `translations/scpcore.regions.txt`. Here you specify only the name of the translation line!  

**Example:**  
```json
{"radius":800,"pos":[3250,-2222,100],"ltag":"SCP-914"}
```

## [spawnlist.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/spawnlist.json)

This file is used to configure the appearance of objects on the map. Randomization is supported.    

The parameters of the appearance at a certain point are prescribed in the item block.  
The setup is done in the format: `{"chance":Chance,"vec":[Coordinates],"ang":[Angle]}`

Example:
```json
"weapon_deagle":[
	{"chance":100,"vec":[-9434,2441,36],"ang":[2,-1,0]},
        {"chance":100,"vec":[-9240,2388,0],"ang":[16,0,0]},
        {"chance":100,"vec":[-9240,2415,0],"ang":[16,0,0]}
]
```

Randomization is configured in a separate block according to the format: `"Percent":{"vec":[Coordinates],"ang":Angle]}`.  

Unlike other parameters, the total amount of interest should not be equal to 100. If you want to make a 50 percent chance of items appearing, then divide that 50 percent between all the items. Thus, the chance of an item appearing will work first, and then which item from the list will spawn.  

Example of randomization:  
```json
"weapon_deagle":[
        {
            "49":{"vec":[-2113,-6599,259],"ang":[3,-34,0]},
            "51":{"vec":[-2009,-6137,223],"ang":[8,-89,0]}
        }
]
```

## [914.json](https://github.com/GeTtOo/SCP-Breach-CSGO/blob/main/addons/sourcemod/configs/scp/workshop/2424265786/scp_site101/914.json)[^3]

If the module 914 is disabled, this configuration file is not used.  
Setting up recipes to be divided into quality blocks. A total of 5 qualities are supported:  
1. Rough
2. Coarse
3. One by one
4. Fine
5. Very Fine

In each recipe, you first specify the name of the item, and then what can come out of it in the format:  
```json
"item name": [
  "Percentage ratio":["Item being received", "Chance of destruction is from 0 to 100", "The chance of successful improvement is from 0 to 100 (If the improvement is a failure, the item will remain the same)"]
]
```

In the absence of the "Percentage ratio" parameter, the ratio is divided into equal shares between all the items received.  
For example, automatic 50-50 split:
```json
"weapon_usp_silencer": [
  ["weapon_deagle", 30, 70],
  ["weapon_awp", 40, 90]
]
```

If it is necessary to carry out manipulations with the player:
```json
"player": [
  ["Эффект", "Процент успешного «улучшения» от 0 до 100"]
]
```

4 effects are supported:
1. Injure - Bleeding
2. Regeneration - Health Regeneration
3. Speed - Speed increase
4. Butchering - Death by sawing into pieces

------------------

### Example
```json
"recipes": {
        "rough": {
            "player": [
                ["Butchering", 100]
            ],
	    "card_janitor": [
                ["card_facility_manager", 100, 0]
            ],
	    "card_senior_guard": [
                ["card_major_scientist", 50, 100],
                ["card_zone_manager", 50, 100]
            ],
	    "weapon_nova": [
                ["iron_ingot", 50, 100]
            ],
	    "weapon_m4a1": [
                ["iron_ingot", 50, 100],
                ["weapon_deagle", 50, 100],
                ["weapon_revolver", 50, 100],
                ["weapon_cz75a", 50, 100]
            ],
	    "iron_ingot": [
                ["iron_ingot", 100, 0]
            ]
	},
        "coarse": {
            "player": [
                ["Injure", 100]
            ],
	    "weapon_usp_silencer": {
                "10":["iron_ingot", 0, 100],
                "30":["weapon_axe", 0, 100],
                "29":["weapon_hammer", 0, 100],
                "31":["weapon_spanner", 0, 100]
            },
	},
        "one_by_one": {
            "card_o5": [
                ["plastic", 0, 100],
                ["card_o5", 0, 100]
            ]
	},
        "fine": {
            "player": [
                ["Regeneration", 50],
                ["Speed", 50]
            ]
	},
        "very_fine": {
            "card_o5": [
                ["plastic", 0, 100],
                ["weapon_flashbang", 0, 100]
            ],
	    "weapon_cz75a": [
                ["weapon_ak47", 0, 100],
                ["weapon_m4a1_silencer", 0, 100],
                ["weapon_m4a1", 0, 100]
            ]
	}
}
```

## List of commands  

### Commands for the Administrator 
| Command       | Description |
| ------------- | ---------|
| scp_admin     | Show the admin menu |
| gm status     | Output a list of players in teams to the console |
| gm timers     | Output a list of timers to the console |
| ents getall   | Output a list of all entities in the repository to the console |

### Development commands (work only when the debug parameter is enabled)  
| Command            | Description |
| ------------------ | ---------|
| getmypos           | Output the current position to the console |
| getentsinbox       | Display a list of objects around the player in the chat |
| debug set body     | Set the body group |
| debug set skin     | Set Skin |
| debug flashlight   | Turn on/off the flashlight |
| debug nvgs         | Enable/disable NVD |
| debug round end    | Forcibly end the round |
| debug round lock   | Block a round |
| debug round unlock | Unlock the round |
| debug voice mute   | Disable the option to player {1} hear the player {2} (player id) |
| debug voice unmute | Installation to player {1} ability to hear the player {2} (player id) |

[^1]: ATTENTION! The total share should be 100!
[^2]: Required to appear at the beginning of the round. For example: with a small number of players, you need at least one SCP. 
If the value of this parameter is "true", SCP will be required to spawn.
[^3]: ATTENTION if you don't understand what it is, DO NOT TOUCH it!
[^4]: Must be located in the team section.
[^5]: The player will be teleported to "conduct a conversation" with it.
[^6]: This setting is optional. It may not be used if the plugin of the same name is disabled.
[^7]: First the bottom point, then the top.
[^8]: To put it simply - where the player teleports after the "call".
[^9]: In the first case, the player needs to open the inventory and use the card on the door control unit. In the second case, the player only needs to press the button, the system itself will search for the card in the inventory. 
[^10]: 0 - All the listed methods. 1 - Console only. 2 - File only. 3 - Only third-party plugins (if you use Discord Logger, which is included in the set, it will be used).
[^11]: This setting appeared due to the lack of necessary functions in SourceMod. We were too lazy to write a full-fledged MDL file parser, so everyone will suffer. (c) Get 
