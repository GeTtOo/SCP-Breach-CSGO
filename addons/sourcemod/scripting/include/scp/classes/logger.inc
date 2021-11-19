/**
 * =============================================================================
 * Copyright (C) 2021 Eternity team (Andrey::Dono, GeTtOo).
 * =============================================================================
 *
 * This file is part of the SCP Breach CS:GO.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 **/

/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Logger------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Logger < Base
{
    public Logger(char[] fwname, int mode)
    {
        Logger logger = view_as<Logger>(new Base());
        logger.SetHandle("fw", CreateGlobalForward(fwname, ET_Event, Param_String, Param_String, Param_String));
        logger.SetInt("mode", mode);
        return logger;
    }

    property Handle fw {
        public get() { return this.GetHandle("fw"); }
    }

    property int mode {
        public get() { return this.GetInt("mode"); }
    }

    public void SendMessageToConsole(const char[] timestring, const char[] type, const char[] formatMessage)
    {
        PrintToServer("[%s][%s] %s", timestring, type, formatMessage);
    }
    
    public void SendMessageToFile(const char[] type, const char[] formatMessage)
    {
        LogToFileEx("addons/sourcemod/logs/SCP.log", "[%s] %s", type, formatMessage);
    }

    public void SendMessageToForward(const char[] timestring, const char[] type, const char[] formatMessage)
    {
        Call_StartForward(this.fw);
        Call_PushString(timestring);
        Call_PushString(type);
        Call_PushString(formatMessage);
        Call_Finish();
    }

    public void SendMessage(int logType, const char[] text)
    {
        char type[32];
        
        switch(logType)
        {
            case 0:     { type = "Info";    }
            case 1:     { type = "Warning"; }
            case 2:    { type = "ERROR";   }
            default:    { type = "Unknown";   }
        }

        char timestring[10];
        FormatTime(timestring, 10, "%H:%M:%S");

        switch(this.mode)
        {
            case 1:
            {
                this.SendMessageToConsole(timestring, type, text);
            }
            case 2:
            {
                this.SendMessageToFile(type, text);
            }
            case 3:
            {
                this.SendMessageToForward(timestring, type, text);
            }
            default:
            {
                //this.SendMessageToConsole(timestring, type, text);
                this.SendMessageToFile(type, text);
                this.SendMessageToForward(timestring, type, text);
            }
        }
    }

    public void Info(const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 3);
        this.SendMessage(0, text);
    }

    public void Warning(const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 3);
        this.SendMessage(1, text);
    }
}