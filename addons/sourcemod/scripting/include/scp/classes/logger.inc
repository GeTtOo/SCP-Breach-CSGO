/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Logger------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Logger < Base
{
    public Logger()
    {
        Logger self = view_as<Logger>(new Base());
        return self;
    }

    public void SendMessageConsole(const char[] timestring, const char[] type, const char[] formatMessage)
    {
        PrintToServer("[%s][%s] %s", timestring, type, formatMessage);

        /*if(StrEqual(formatMessage, "ERROR"))
            LogError(formatMessage);
        else
            LogMessage("%s: %s", type, formatMessage);*/
    }
    
    public void SendMessageDiscord(const char[] timestring, const char[] type, const char[] formatMessage)
    {
        //Discord_StartMessage();
        //Discord_SetUsername("SCP");
        //Discord_SetContent("```md\n[%s][%s] %s```", timestring, type, formatMessage);
        //Discord_EndMessage("scp_logger", true);
        //PrintToChatAll("[%s][%s]: %s"timestring, type, formatMessage);
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

        switch(0) //gamemode.log.type
        {
            case 2:     
            { 
                this.SendMessageConsole(timestring, type, text);
                this.SendMessageDiscord(timestring, type, text);
            }
            case 1:     
            { 
                this.SendMessageDiscord(timestring, type, text); 
            }
            default:
            {
                this.SendMessageConsole(timestring, type, text);
            }
        }
    }

    public void Info(const char[] msg, any arg1 = false, any arg2 = false, any arg3 = false, any arg4 = false, any arg5 = false)
    {
        int len = strlen(msg) + 256;
        char[] formtext = new char[len];

        FormatEx(formtext, len, msg, arg1, arg2, arg3, arg4, arg5);

        this.SendMessage(0, formtext);
    }
}