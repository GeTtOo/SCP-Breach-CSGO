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
    public Logger(char[] fwname, int mode, int loglevel, bool debugmode)
    {
        Logger logger = view_as<Logger>(new Base());
        logger.SetHandle("fw", CreateGlobalForward(fwname, ET_Event, Param_Cell, Param_String, Param_String));
        logger.SetInt("mode", mode);
        logger.SetInt("loglevel", loglevel);
        logger.SetBool("debug", debugmode);
        return logger;
    }

    property Handle fw {
        public get() { return this.GetHandle("fw"); }
    }

    property int mode {
        public get() { return this.GetInt("mode"); }
    }

    property int loglevel {
        public get() { return this.GetInt("loglevel"); }
    }

    property bool debug {
        public get() { return this.GetBool("debug"); }
    }

    public void SendMessageToConsole(const char[] timestring, const char[] type, const char[] formatMessage)
    {
        PrintToServer("[%s][%s] %s", timestring, type, formatMessage);
    }
    
    public void SendMessageToFile(const char[] type, const char[] formatMessage)
    {
        LogToFileEx("addons/sourcemod/logs/SCP.log", "[%s] %s", type, formatMessage);
    }

    public void SendMessageToForward(LogType type, const char[] timestring, const char[] formatMessage) // Base user, Base admin
    {
        Call_StartForward(this.fw);
        Call_PushCell(type);
        Call_PushString(timestring);
        Call_PushString(formatMessage);
        //Call_PushCellRef(user);
        //Call_PushCellRef(admin);
        Call_Finish();
    }

    public void SendMessage(LogType type, const char[] text) //const char[] logid, Base user, Base admin
    {
        if (this.loglevel >= view_as<int>(type))
        {
            
            /*char buffer[1024], username[64], usersid[64], adminname[64], adminsid[64];
        
            if (user && admin)
            {
                FormatEx(buffer, sizeof(buffer), "%t", logid, adminname, adminsid, username, usersid);
            }
            else if (user)
            {
                FormatEx(buffer, sizeof(buffer), "%t", logid, username, usersid);
            }
            else if (admin)
            {
                FormatEx(buffer, sizeof(buffer), "%t", logid, adminname, adminsid);
            }
            else
            {
                FormatEx(buffer, sizeof(buffer), "%t", logid);
            }*/

            /*int len = strlen(format) + 4096;
            char[] text = new char[len];
            VFormat(text, len, format, 3);
            char pname[64];
            GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
            Format(text, len, "%s | %s", pname, text);*/

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

            char timestring[10];
            FormatTime(timestring, 10, "%H:%M:%S");

            switch(this.mode)
            {
                case 1:
                {
                    this.SendMessageToConsole(timestring, strtype, text);
                }
                case 2:
                {
                    this.SendMessageToFile(strtype, text);
                }
                case 3:
                {
                    this.SendMessageToForward(type, timestring, text);
                }
                default:
                {
                    this.SendMessageToConsole(timestring, strtype, text);
                    //this.SendMessageToFile(type, text);
                    this.SendMessageToForward(type, timestring, text);
                }
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

    public void Admin(const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 3);
        this.SendMessage(Admin, text);
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

    /*public void Error(const char[] logid, Base user = null, Base admin = null)
    {
        this.SendMessage(ERROR, logid, user, admin);
    }

    public void Warning(const char[] logid, Base user = null, Base admin = null)
    {
        this.SendMessage(Warning, logid, user, admin);
    }

    public void Info(const char[] logid, Base user = null, Base admin = null)
    {
        this.SendMessage(Info, logid, user, admin);
    }

    public void Admin(const char[] logid, Base user = null, Base admin = null)
    {
        this.SendMessage(Admin, logid, user, admin);
    }

    public void Debug(const char[] logid, Base user = null, Base admin = null)
    {
        if (this.debug) this.SendMessage(Debug, logid, user, admin);
    }

    public void Custom(LogType logid = Info, const char[] format, any ...)
    {
        int len = strlen(format) + 255;
        char[] text = new char[len];
        VFormat(text, len, format, 4);
        this.SendMessage(logid, text, null, null);
    }*/
}