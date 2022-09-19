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

enum LogType
{
    ERROR = 0,
    Warning,
    Info,
    Admin,
    Debug,
    Unknown
}

methodmap Logger < Base
{
    public Logger(char[] fwname, int mode, bool debugmode)
    {
        Logger logger = view_as<Logger>(new Base());
        logger.SetHandle("fw", CreateGlobalForward(fwname, ET_Event, Param_Cell, Param_CellByRef, Param_String));
        logger.SetInt("mode", mode);
        logger.SetBool("debug", debugmode);
        return logger;
    }

    property Handle fw {
        public get() { return this.GetHandle("fw"); }
    }

    property int mode {
        public get() { return this.GetInt("mode"); }
    }

    property bool debug {
        public get() { return this.GetBool("debug"); }
    }

    public void SendMessageToConsole(const char[] type, const char[] formatMessage)
    {
        char timestring[10];
        FormatTime(timestring, 10, "%H:%M:%S");
        PrintToServer("[%s][%s] %s", timestring, type, formatMessage);
    }
    
    public void SendMessageToFile(const char[] type, const char[] formatMessage)
    {
        LogToFileEx("addons/sourcemod/logs/SCP.log", "[%s] %s", type, formatMessage);
    }

    public void SendMessageToForward(LogType type, Base admin, const char[] formatMessage) // Base user, Base admin
    {
        Call_StartForward(this.fw);
        Call_PushCell(type);
        Call_PushCellRef(admin);
        Call_PushString(formatMessage);
        Call_Finish();
    }

    public void SendMessage(LogType type, const char[] text, Base admin = null) //const char[] logid, Base user, Base admin
    {
        char strtype[32];
    
        switch(type)
        {
            case ERROR:     { strtype = "ERROR";   }
            case Warning:   { strtype = "Warning"; }
            case Info:      { strtype = "Info";    }
            case Admin:     { strtype = "Admin";   }
            case Debug:     { strtype = "Debug";   }
            default:        { strtype = "Unknown"; }
        }

        switch(this.mode)
        {
            case 1:
            {
                this.SendMessageToConsole(strtype, text);
            }
            case 2:
            {
                this.SendMessageToFile(strtype, text);
            }
            case 3:
            {
                this.SendMessageToForward(type, admin, text);
            }
            default:
            {
                this.SendMessageToConsole(strtype, text);
                //this.SendMessageToFile(type, text);
                this.SendMessageToForward(type, admin, text);
            }
        }
    }

    public void Error(const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 3);
        this.SendMessage(ERROR, text);
    }

    public void Warning(const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 3);
        this.SendMessage(Warning, text);
    }

    public void Info(const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 3);
        this.SendMessage(Info, text);
    }

    public void Admin(Base admin, const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 4);
        this.SendMessage(Admin, text, admin);
    }
    
    public void Debug(const char[] format, any ...)
    {
        if (this.debug)
        {
            int len = strlen(format) + 4096;
            char[] text = new char[len];
            VFormat(text, len, format, 3);
            char pname[64];
            GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
            Format(text, len, "%s | %s", pname, text);
            this.SendMessage(Debug, text);
        }
    }

    public void Dispose()
    {
        delete this.fw;
        delete this;
    }
}