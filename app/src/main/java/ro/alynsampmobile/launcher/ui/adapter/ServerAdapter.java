package ro.alynsampmobile.launcher.ui.adapter;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.StrictMode;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.imageview.ShapeableImageView;
import com.google.android.material.textfield.TextInputEditText;
import com.google.android.material.textfield.TextInputLayout;
import com.google.android.material.textview.MaterialTextView;
import com.joom.paranoid.Obfuscate;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.File;
import java.util.List;

import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.launcher.utils.SampQuery;
import ro.alynsampmobile.launcher.utils.ServerModel;
import ro.alynsampmobile.launcher.utils.Utils;

@Obfuscate
public class ServerAdapter extends RecyclerView.Adapter<ServerAdapter.ViewHolder> {
    Activity activity;
    public List<ServerModel> serverModel;
    boolean is_hosted;

    static class ViewHolder extends RecyclerView.ViewHolder {
        ConstraintLayout server_item;
        MaterialTextView server_name;
        MaterialTextView server_address;
        ShapeableImageView server_password;
        MaterialTextView server_gamemode;
        MaterialTextView server_players;

        public ViewHolder(View itemView) {
            super(itemView);

            server_item = itemView.findViewById(R.id.server_item);
            server_name = itemView.findViewById(R.id.server_name);
            server_address = itemView.findViewById(R.id.server_address);
            server_password = itemView.findViewById(R.id.server_password);
            server_gamemode = itemView.findViewById(R.id.server_gamemode);
            server_players = itemView.findViewById(R.id.server_players);
        }
    }

    public ServerAdapter(Activity activity, List<ServerModel> serverModel, boolean is_hosted) {
        StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder().permitAll().build());

        this.activity = activity;
        this.serverModel = serverModel;
        this.is_hosted = is_hosted;
    }

    @NonNull
    public ViewHolder onCreateViewHolder(ViewGroup parent, int i) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.layout_server_item, parent, false);
        return new ViewHolder(view);
    }

    public void onBindViewHolder(ViewHolder viewHolder, int position) {
        // crash fix for index out of bounds
        if (position >= serverModel.size()) {
            return;
        }

        viewHolder.server_name.setText(serverModel.get(position).server_name);
        viewHolder.server_address.setText("Address: " + serverModel.get(position).server_host + ":" + serverModel.get(position).server_port);

        if (serverModel.get(position).server_password.equals("0")) {
            viewHolder.server_password.setImageDrawable(activity.getResources().getDrawable(R.drawable.ic_unlocked));
        }
        if (serverModel.get(position).server_password.equals("1")) {
            viewHolder.server_password.setImageDrawable(activity.getResources().getDrawable(R.drawable.ic_locked));
        }

        viewHolder.server_gamemode.setText("Gamemode: " + serverModel.get(position).server_gamemode);
        viewHolder.server_players.setText("Players: " + serverModel.get(position).server_players + " / " + serverModel.get(position).server_maxplayers);

        viewHolder.server_item.setOnClickListener(v -> {
            View view = activity.getLayoutInflater().inflate(R.layout.layout_server_info, null);

            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(activity).setView(view);
            AlertDialog dialog = dialogBuilder.create();

            MaterialTextView server_name = view.findViewById(R.id.server_name);
            MaterialTextView server_address = view.findViewById(R.id.server_address);
            MaterialTextView server_players = view.findViewById(R.id.server_players);
            MaterialTextView server_gamemode = view.findViewById(R.id.server_gamemode);
            MaterialTextView server_language = view.findViewById(R.id.server_language);

            TextInputEditText nickname_input_text = view.findViewById(R.id.nickname_input_text);
            TextInputEditText password_input_text = view.findViewById(R.id.password_input_text);
            TextInputLayout password_input = view.findViewById(R.id.password_input);

            MaterialButton delete_button = view.findViewById(R.id.delete_button);
            MaterialButton connect_button = view.findViewById(R.id.connect_button);

            nickname_input_text.setOnEditorActionListener((textView, i, keyEvent) -> i == EditorInfo.IME_ACTION_DONE || i == EditorInfo.IME_ACTION_NEXT || i == EditorInfo.IME_ACTION_UNSPECIFIED);
            password_input_text.setOnEditorActionListener((textView, i, keyEvent) -> i == EditorInfo.IME_ACTION_DONE || i == EditorInfo.IME_ACTION_NEXT || i == EditorInfo.IME_ACTION_UNSPECIFIED);

            String savedNick = activity.getSharedPreferences("samp_settings", Context.MODE_PRIVATE).getString("nick_name", "");
            String savedPass = activity.getSharedPreferences("samp_server", Context.MODE_PRIVATE).getString("password", "");

            nickname_input_text.setText(savedNick);
            password_input_text.setText(savedPass);

            server_name.setText(serverModel.get(position).server_name);
            server_address.setText("Address: " + serverModel.get(position).server_host + ":" + serverModel.get(position).server_port);
            server_players.setText("Players: " + serverModel.get(position).server_players + " / " + serverModel.get(position).server_maxplayers);
            server_gamemode.setText("Gamemode: " + serverModel.get(position).server_gamemode);
            server_language.setText("Language: " + serverModel.get(position).server_language);

            boolean has_password;
            if (serverModel.get(position).server_password.equals("0")) {
                password_input.setVisibility(View.GONE);
                has_password = false;
            } else {
                password_input.setVisibility(View.VISIBLE);
                has_password = true;
            }

            if (is_hosted) {
                delete_button.setVisibility(View.GONE);
            } else {
                delete_button.setVisibility(View.VISIBLE);
            }

            delete_button.setText("Delete");
            delete_button.setIcon(activity.getResources().getDrawable(R.drawable.ic_delete));

            delete_button.setOnClickListener(view2 -> {
                try {
                    Utils.deleteServerFromFavorites(activity, serverModel.get(position).server_host + ":" + serverModel.get(position).server_port);
                    Toast.makeText(activity, "Successfully removed server from favorites.", Toast.LENGTH_LONG).show();
                    dialog.dismiss();
                    serverModel.remove(position);
                    notifyItemRemoved(position);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });

            connect_button.setOnClickListener(view3 -> {
                if (!Utils.isOnline(activity)) {
                    Toast.makeText(activity, "No internet connection!", Toast.LENGTH_LONG).show();
                    dialog.dismiss();
                    return;
                }

/*
                if (!Utils.appStatus()) {
                    dialog.dismiss();
                    Utils.showAppStatusWarning(activity);
                    return;
                }
*/

                if (nickname_input_text.getText().toString().isEmpty()) {
                    Toast.makeText(activity, "Please set nickname.", Toast.LENGTH_LONG).show();
                    return;
                }

                if (nickname_input_text.getText().toString().length() > 24) {
                    Toast.makeText(activity, "Nickname can't be longer than 24 characters.", Toast.LENGTH_LONG).show();
                    return;
                }

                if (Utils.isServerBanned(serverModel.get(position).server_host, serverModel.get(position).server_port)) {
                    Toast.makeText(activity, "This server is banned on this launcher.", Toast.LENGTH_LONG).show();
                    dialog.dismiss();
                    return;
                }

                dialog.dismiss();

                // save settings prefs
                activity.getSharedPreferences("samp_settings", Context.MODE_PRIVATE).edit().putString("nick_name", nickname_input_text.getText().toString()).apply();

                // save server settings prefs
                SharedPreferences.Editor edit = activity.getSharedPreferences("samp_server", Context.MODE_PRIVATE).edit();
                edit.putString("host", serverModel.get(position).server_host);
                edit.putString("port", serverModel.get(position).server_port);
                edit.putString("password", has_password ? password_input_text.getText().toString() : "");
                edit.apply();

                Utils.saveSettings(activity);

                // start game
                try {
                    Intent intent = new Intent(activity, ro.alynsampmobile.game.SAMP.class);
                    intent.putExtra("extra_check", "alynsampmobile1337");
                    activity.startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });

            dialog.show();
        });
    }

    public void refreshServers() {
        serverModel.clear();

        try {
            File serversFile = new File(activity.getExternalFilesDir(null), "servers.json");
            String jsonString;
            if (is_hosted) {
                jsonString = Utils.getStringOutputByURL(Utils.hostedServersFileStr);
            } else {
                jsonString = Utils.readJSONFromFile(serversFile);
            }
            JSONObject json = new JSONObject(jsonString);
            JSONArray serversArray = json.getJSONArray("servers");

            for (int i = 0; i < serversArray.length(); i++) {
                JSONObject serverJson = serversArray.getJSONObject(i);
                String host = serverJson.getString("host");
                String port = serverJson.getString("port");

                serverModel.add(new ServerModel("Loading...", host, port, "0", "Mobile / PC", "0", "0", "English"));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        notifyDataSetChanged();

        for (int i = 0; i < serverModel.size(); i++) {
            new LoadServers(i, serverModel.get(i).server_host + ":" + serverModel.get(i).server_port).start();
        }
    }

    class LoadServers extends Thread {
        int position;
        String address;
        String host;
        String port;
        int retryCount = 0;
        boolean isDone = false;

        public LoadServers(int position, String address) {
            this.position = position;
            this.address = address;
            host = address.split(":")[0];
            port = address.split(":")[1];
        }

        @Override
        public void run() {
            // retry 10 times and fail
            while (retryCount < 10 && !isDone && !isInterrupted()) {
                try {
                    isDone = getServerInfo(position, address);
                } catch (Exception e) {
                    Log.e("LoadServers", "Exception occurred: " + e.getMessage());
                }
                retryCount++;
                if (!isDone) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        Log.e("LoadServers", "Thread interrupted while sleeping: " + e.getMessage());
                        // Restore interrupted status
                        Thread.currentThread().interrupt();
                    }
                }
            }

            if (!isDone) {
                try {
                    serverModel.set(position, new ServerModel("Loading...", host, port, "0", "Mobile / PC", "0", "0", "English"));
                    activity.runOnUiThread(() -> notifyItemChanged(position));
                } catch (Exception e) {
                    Log.e("LoadServers", "Error updating UI: " + e.getMessage());
                }
            }
        }
    }

    public boolean getServerInfoFromApi(int position, String address) {
        /*String host = address.split(":")[0];
        String port = address.split(":")[1];

        try {
            String json = Utils.getStringOutputByURL(Utils.web + "serverinfo.php?host=" + host + "&port=" + port);
            JSONObject data = new JSONObject(json).getJSONObject("server");
            String password = String.valueOf(data.getInt("password"));
            String players = String.valueOf(data.getInt("players"));
            String maxplayers = String.valueOf(data.getInt("maxplayers"));
            String hostname = data.getString("hostname");
            String gamemode = data.getString("gamemode");
            String language = data.getString("language");

            if (hostname.replace(" ", "").equals("")) {
                return false;
            } else {
                serverModel.set(position, new ServerModel(hostname, host, port, password, gamemode, players, maxplayers, language));
                activity.runOnUiThread(() -> notifyItemChanged(position));
            }
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }*/

        return false;
    }

    public boolean getServerInfo(int position, String address) {
        try {
            if (!getServerInfoFromApi(position, address)) {
                String host = address.split(":")[0];
                String port = address.split(":")[1];
                SampQuery sampQuery = new SampQuery(host, Integer.parseInt(port));

                if (sampQuery.isOnline()) {
                    String[] infos = sampQuery.getInfo();
                    String password = infos[0];
                    String players = infos[1];
                    String maxplayers = infos[2];
                    String hostname = infos[3];
                    String gamemode = infos[4];
                    String language = infos[5];

                    if (hostname.replace(" ", "").equals("")) {
                        return false;
                    } else {
                        serverModel.set(position, new ServerModel(hostname, host, port, password, gamemode, players, maxplayers, language));
                        activity.runOnUiThread(() -> notifyItemChanged(position));
                    }
                } else {
                    return false;
                }
                sampQuery.close();
            }
        } catch (Exception e) {
            Log.e("SAMPQuery", e.toString());
        }
        return true;
    }

    public int getItemCount() {
        return serverModel.size();
    }
}
