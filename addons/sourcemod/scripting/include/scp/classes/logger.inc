/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Logger------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

methodmap Logger < Base
{
    public Logger()
    {
        Logger self = view_as<Logger>(new Base());
    }

    gamemmode.log.error(message);

    public void Info()
    {
        this.SendMessage();
    }

    public void SendMessage(const char[] text, int logType)
    {
        char type[32];
        int len = strlen(text) + 255;
        char[] formatMessage = new char[len];
        VFormat(formatMessage, len, text, 3);
        
        switch(logType)
        {
            case 0:     { type = "Info";    }
            case 1:     { type = "Warning"; }
            default:    { type = "ERROR";   }
        }

        char timestring[10];
        FormatTime(timestring, 10, "%H:%M:%S");

        switch(gamemode.log.type)
        {
            case 2:     
            { 
                this.SendMessageConsole(timestring, type, formatMessage);
                this.SendMessageDiscord(timestring, type, formatMessage);
            }
            case 1:     
            { 
                this.SendMessageDiscord(timestring, type, formatMessage); 
            }
            default:
            {
                this.SendMessageConsole(timestring, type, formatMessage);
            }
        }
    }

    public void SendMessageConsole(const char[] timestring, const char[] type, const char[] formatMessage)
    {
        PrintToServer("[%s] %s: %s", timestring, type, formatMessage);

        if(StrEqual(formatMessage, "ERROR"))
            LogError(formatMessage);
        else
            LogMessage("%s: %s", type, formatMessage);
    }
    
    public void SendMessageDiscord(const char[] timestring, const char[] type, const char[] formatMessage)
    {
        Discord_StartMessage();
        Discord_SetUsername("SCP");
        Discord_SetContent("```md\n[%s][%s] %s```", timestring, type, formatMessage);
        Discord_EndMessage("scp_logger", true);
    }
}