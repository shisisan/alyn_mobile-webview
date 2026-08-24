package ro.alynsampmobile.launcher.utils;

import com.joom.paranoid.Obfuscate;

@Obfuscate
public class ServerModel {
    public String server_name = "Loading...";
    public String server_host = "127.0.0.1";
    public String server_port = "7777";
    public String server_password = "0";
    public String server_gamemode = "Mobile / PC";
    public String server_players = "0";
    public String server_maxplayers = "0";
    public String server_language = "English";

    public ServerModel(String server_name, String server_host, String server_port, String server_password, String server_gamemode, String server_players, String server_maxplayers, String server_language) {
        this.server_name = server_name;
        this.server_host = server_host;
        this.server_port = server_port;
        this.server_password = server_password;
        this.server_gamemode = server_gamemode;
        this.server_players = server_players;
        this.server_maxplayers = server_maxplayers;
        this.server_language = server_language;
    }
}
