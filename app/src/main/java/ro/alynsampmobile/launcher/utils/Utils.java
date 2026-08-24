package ro.alynsampmobile.launcher.utils;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.util.Base64;

import androidx.appcompat.app.AlertDialog;

import com.joom.paranoid.Obfuscate;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.CharacterIterator;
import java.text.StringCharacterIterator;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import ro.alynsampmobile.launcher.ui.adapter.PreviewAdapter;
import ro.alynsampmobile.launcher.ui.fragment.SupportPageFragment;

@Obfuscate
public class Utils {
    public static String copyright = "Revitalize Roleplay Test";
    public static String web = "https://alynsampmobile.pro/";
    public static String github = "https://github.com/alyn-dev";
    public static String update = web + "update.json";
    public static String discord = web + "discord";
    public static String changelog = web + "changelog.txt";
    public static String hostedServersFileStr = web + "servers.json";
    public static String bannedServersFileStr = web + "banned.json";
    public static String faqURL = web + "faq.json";
    public static String previewsUrl = web + "images/previews.json";

    private static List<PreviewAdapter.PreviewItem> cachedPreviews = null;
    private static List<SupportPageFragment.FAQItem> cachedFaqs = null;

    public enum GPUType {
        NONE,
        DXT,
        PVR,
        ETC
    }

    public static GPUType GPU_TYPE = GPUType.NONE;

    public static boolean appStatus() {
        ExecutorService executor = Executors.newSingleThreadExecutor();
        Future<Boolean> future = executor.submit(() -> {
            try {
                String out = getStringOutputByURL(update);
                JSONObject json = new JSONObject(out);
                return json.getBoolean("app_status");
            } catch (IOException | JSONException e) {
                System.out.println("Failed to get app status: " + e.getMessage());
                return false;
            }
        });

        boolean status = false;
        try {
            // Wait for the completion of the thread and get the result.
            // You might want to specify a timeout to avoid waiting indefinitely.
            status = future.get(); // You can add a timeout here if necessary
        } catch (InterruptedException | ExecutionException e) {
            System.out.println("2Failed to get app status: " + e.getMessage());
            Thread.currentThread().interrupt(); // Restore interrupted status
        } finally {
            executor.shutdown();
        }

        System.out.println("App server status: " + status);
        return status;
    }

    public static void showAppStatusWarning(Activity activity) {
        System.out.println("showAppStatusWarning");

        try {
            new AlertDialog.Builder(activity).setTitle("Status:")
                    .setMessage("App server is down, make sure you have the latest version of the client. If the problem persists, contact the developer.")
                    .setPositiveButton("Ok", null).setCancelable(false).show();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static String xor_key = "alyn_samp_mobile";

    public static String xorEncrypt(String input) {
        StringBuilder output = new StringBuilder();
        for (int i = 0; i < input.length(); i++) {
            output.append((char) (input.charAt(i) ^ xor_key.charAt(i % xor_key.length())));
        }
        return Base64.encodeToString(output.toString().getBytes(), Base64.NO_WRAP);
    }

    public static String xorDecrypt(String input) {
        byte[] data = Base64.decode(input, Base64.NO_WRAP);
        String decodedString = new String(data);
        StringBuilder output = new StringBuilder();
        for (int i = 0; i < decodedString.length(); i++) {
            output.append((char) (decodedString.charAt(i) ^ xor_key.charAt(i % xor_key.length())));
        }
        return output.toString();
    }

    public static boolean isTester(Context context) {
        ExecutorService executor = Executors.newSingleThreadExecutor();
        Future<Boolean> result = executor.submit(() -> {
            try {
                String out = getStringOutputByURL(update);
                JSONObject json = new JSONObject(out);
                JSONArray testers = json.getJSONArray("app_testers");

                String hwid = getHWID(context);
                for (int i = 0; i < testers.length(); i++) {
                    if (hwid.equals(xorDecrypt(testers.getString(i)))) {
                        return true;
                    }
                }
            } catch (IOException | JSONException e) {
                e.printStackTrace();
                return false;
            }
            return false;
        });

        try {
            boolean isTester = result.get();  // This will block, which might not be ideal.
            if (isTester) {
                System.out.println("You are a tester!");
            }
            return isTester;
        } catch (ExecutionException | InterruptedException e) {
            e.printStackTrace();
            return false;
        } finally {
            executor.shutdown();
        }
    }

    public static String getHWID(Context context) {
        String androidId = Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
        String hwid = "alyn_samp_mobile_" + androidId;

        try {
            JSONObject json = new JSONObject();
            json.put("hwid", hwid);
            writeJSONToFile(new File(context.getExternalFilesDir(null), "hwid.json"), json);
        } catch (JSONException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return hwid;
    }

    public static String getChangelog() {
        System.out.println("getChangelog");

        try {
            return getStringOutputByURL(changelog);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }

    public static void fetchPreviews(final PreviewCallback callback) {
        if (cachedPreviews != null && !cachedPreviews.isEmpty()) {
            if (callback != null) {
                new Handler(Looper.getMainLooper()).post(() -> callback.onSuccess(cachedPreviews));
            }
            return;
        }

        OkHttpClient client = new OkHttpClient();
        Request request = new Request.Builder()
                .url(previewsUrl)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                new Handler(Looper.getMainLooper()).post(() -> {
                    e.printStackTrace();
                    if (callback != null) {
                        callback.onError(e);
                    }
                });
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (!response.isSuccessful()) {
                    new Handler(Looper.getMainLooper()).post(() -> {
                        if (callback != null) {
                            callback.onError(new IOException("Unexpected response code: " + response.code()));
                        }
                    });
                    return;
                }

                String jsonResponse = response.body().string();
                try {
                    JSONArray jsonArray = new JSONArray(jsonResponse);
                    List<PreviewAdapter.PreviewItem> previews = new ArrayList<>();

                    for (int i = 0; i < jsonArray.length(); i++) {
                        JSONObject jsonObject = jsonArray.getJSONObject(i);
                        String imageUrl = jsonObject.getString("imageUrl");
                        String title = jsonObject.getString("title");
                        String subtitle = jsonObject.getString("subtitle");

                        PreviewAdapter.PreviewItem item = new PreviewAdapter.PreviewItem(imageUrl, title, subtitle);
                        previews.add(item);
                    }

                    cachedPreviews = previews;

                    new Handler(Looper.getMainLooper()).post(() -> {
                        if (callback != null) {
                            callback.onSuccess(previews);
                        }
                    });

                } catch (JSONException e) {
                    new Handler(Looper.getMainLooper()).post(() -> {
                        e.printStackTrace();
                        if (callback != null) {
                            callback.onError(e);
                        }
                    });
                }
            }
        });
    }

    public interface PreviewCallback {
        void onSuccess(List<PreviewAdapter.PreviewItem> previews);

        void onError(Exception e);
    }

    public static void fetchFaqs(FaqCallback callback) {
        if (cachedFaqs != null && !cachedFaqs.isEmpty()) {
            if (callback != null) {
                new Handler(Looper.getMainLooper()).post(() -> callback.onSuccess(cachedFaqs));
            }
            return;
        }

        OkHttpClient client = new OkHttpClient();
        Request request = new Request.Builder()
                .url(faqURL)
                .get()
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                callback.onError("Failed to load FAQs");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (!response.isSuccessful()) {
                    callback.onError("Failed to load FAQs. Error code: " + response.code());
                    return;
                }

                String responseBody = response.body().string();
                try {
                    JSONArray jsonArray = new JSONArray(responseBody);
                    List<SupportPageFragment.FAQItem> faqList = new ArrayList<>();

                    for (int i = 0; i < jsonArray.length(); i++) {
                        JSONObject jsonObject = jsonArray.getJSONObject(i);
                        String question = jsonObject.getString("question");
                        String answer = jsonObject.getString("answer");

                        // Create a new FAQItem using the constructor
                        SupportPageFragment.FAQItem faqItem = new SupportPageFragment.FAQItem(question, answer);
                        faqList.add(faqItem);
                    }

                    // Cache the FAQs
                    cachedFaqs = faqList;

                    new Handler(Looper.getMainLooper()).post(() -> callback.onSuccess(faqList));
                } catch (JSONException e) {
                    callback.onError("Failed to parse FAQs");
                }
            }
        });
    }

    public interface FaqCallback {
        void onSuccess(List<SupportPageFragment.FAQItem> faqList);

        void onError(String errorMessage);
    }

    public static boolean isOnline(Activity activity) {
        System.out.println("isOnline");
        ConnectivityManager connectivityManager = (ConnectivityManager) activity.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetworkInfo = connectivityManager != null ? connectivityManager.getActiveNetworkInfo() : null;
        return activeNetworkInfo != null && activeNetworkInfo.isConnectedOrConnecting();
    }

    public static void saveSettings(Context ctx) {
        System.out.println("saveSettings");

        try {
            File file = new File(ctx.getExternalFilesDir(null), "SAMP/settings.json");

            if (file.exists()) {
                file.delete();
            }

            SharedPreferences settings_prefs = ctx.getSharedPreferences("samp_settings", Context.MODE_PRIVATE);
            SharedPreferences server_prefs = ctx.getSharedPreferences("samp_server", Context.MODE_PRIVATE);

            JSONObject jSONObject = new JSONObject();
            jSONObject.put("client", new JSONObject());

            jSONObject.getJSONObject("client").put("server", new JSONObject());
            jSONObject.getJSONObject("client").getJSONObject("server").put("host", server_prefs.getString("host", "127.0.0.1"));
            jSONObject.getJSONObject("client").getJSONObject("server").put("port", Integer.valueOf(server_prefs.getString("port", "7777")));
            jSONObject.getJSONObject("client").getJSONObject("server").put("password", server_prefs.getString("password", ""));

            jSONObject.getJSONObject("client").put("settings", new JSONObject());
            jSONObject.getJSONObject("client").getJSONObject("settings").put("nick_name", settings_prefs.getString("nick_name", "samp_player"));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("samp_version", settings_prefs.getInt("samp_version", 0));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("game_version", settings_prefs.getInt("game_version", 0));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("new_interface", settings_prefs.getBoolean("new_interface", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("system_keyboard", settings_prefs.getBoolean("system_keyboard", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("timestamp", settings_prefs.getBoolean("timestamp", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("fullscreen", settings_prefs.getBoolean("fullscreen", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("fast_connect", settings_prefs.getBoolean("fast_connect", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("voice_chat", settings_prefs.getBoolean("voice_chat", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("display_fps", settings_prefs.getBoolean("display_fps", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("cleo_scripts", settings_prefs.getBoolean("cleo_scripts", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("aml_scripts", settings_prefs.getBoolean("aml_scripts", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("monet_scripts", settings_prefs.getBoolean("monet_scripts", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("modloader", settings_prefs.getBoolean("modloader", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("modify_files", settings_prefs.getBoolean("modify_files", false));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("fps_limit", settings_prefs.getInt("fps_limit", 60));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("chat_strings", settings_prefs.getInt("chat_strings", 5));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("font_size", settings_prefs.getFloat("font_size", 28.0f));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("chat_posx", settings_prefs.getInt("chat_posx", 100));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("chat_posy", settings_prefs.getInt("chat_posy", 10));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("chat_sizex", settings_prefs.getInt("chat_sizex", 400));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("chat_sizey", settings_prefs.getInt("chat_sizey", 150));
            jSONObject.getJSONObject("client").getJSONObject("settings").put("font_size", settings_prefs.getFloat("font_size", 26.0f));

            file.createNewFile();

            writeJSONToFile(file, jSONObject);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void restoreSettings(Context ctx) {
        System.out.println("restoreSettings");

        SharedPreferences.Editor edit = ctx.getSharedPreferences("samp_settings", Context.MODE_PRIVATE).edit();
        edit.putString("nick_name", "samp_player");
        edit.putInt("samp_version", 0);
        edit.putInt("game_version", 0);
        edit.putBoolean("new_interface", false);
        edit.putBoolean("system_keyboard", false);
        edit.putBoolean("timestamp", false);
        edit.putBoolean("fullscreen", false);
        edit.putBoolean("fast_connect", false);
        edit.putBoolean("voice_chat", false);
        edit.putBoolean("display_fps", false);
        edit.putBoolean("cleo_scripts", false);
        edit.putBoolean("aml_scripts", false);
        edit.putBoolean("monet_scripts", false);
        edit.putBoolean("modloader", false);
        edit.putBoolean("modify_files", false);
        edit.putInt("fps_limit", 60);
        edit.putInt("chat_strings", 5);
        edit.putFloat("font_size", 28.0f);
        edit.putInt("chat_posx", 100);
        edit.putInt("chat_posy", 10);
        edit.putInt("chat_sizex", 400);
        edit.putInt("chat_sizey", 150);
        edit.putFloat("font_size", 26.0f);
        edit.apply();

        saveSettings(ctx);
    }

    public static boolean addServerToFavorites(Context ctx, String address) {
        try {
            File serversFile = new File(ctx.getExternalFilesDir(null), "servers.json");

            if (!serversFile.exists()) {
                // initialize servers.json with an empty array
                JSONObject initialJson = new JSONObject();
                initialJson.put("servers", new JSONArray());
                writeJSONToFile(serversFile, initialJson);
            } else {
                if (serversFile.length() == 0) {
                    serversFile.delete();
                }
            }

            String jsonString = readJSONFromFile(serversFile);
            JSONObject json = new JSONObject(jsonString);
            JSONArray serversArray = json.getJSONArray("servers");

            // check if the server already exists in favorites
            for (int i = 0; i < serversArray.length(); i++) {
                JSONObject serverJson = serversArray.getJSONObject(i);
                String host = serverJson.getString("host");
                String port = serverJson.getString("port");

                if (address.equals(host + ":" + port)) {
                    return false; // Server already exists in favorites
                }
            }

            // add the new server to the JSON array
            String host = address.split(":")[0];
            String port = address.split(":")[1];

            JSONObject newServerJson = new JSONObject();
            newServerJson.put("host", host);
            newServerJson.put("port", port);
            serversArray.put(newServerJson);

            // update the JSON object and write it back to the file
            writeJSONToFile(serversFile, json);

            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    public static void deleteServerFromFavorites(Context ctx, String address) throws Exception {
        File serversFile = new File(ctx.getExternalFilesDir(null), "servers.json");

        String jsonString = readJSONFromFile(serversFile);
        JSONObject json = new JSONObject(jsonString);
        JSONArray serversArray = json.getJSONArray("servers");

        JSONArray newServersArray = new JSONArray();
        String[] parts = address.split(":");
        String host = parts[0];
        String port = parts[1];

        for (int i = 0; i < serversArray.length(); i++) {
            JSONObject serverJson = serversArray.getJSONObject(i);
            String serverHost = serverJson.getString("host");
            String serverPort = serverJson.getString("port");

            if (!host.equals(serverHost) || !port.equals(serverPort)) {
                newServersArray.put(serverJson);
            }
        }

        json.put("servers", newServersArray);
        writeJSONToFile(serversFile, json);
    }

    public static boolean isHostIP(String host) {
        try {
            InetAddress ip = InetAddress.getByName(host);
            System.out.println("Host is IP: " + ip.getHostAddress());
            return true;
        } catch (UnknownHostException e) {
            return false;
        }
    }

    public static String getHostIP(String host) {
        try {
            InetAddress ip = InetAddress.getByName(host);
            return ip.getHostAddress();
        } catch (UnknownHostException e) {
            return null;
        }
    }

    public static boolean isServerBanned(String host, String port) {
        String address;
        if (isHostIP(host)) {
            address = getHostIP(host) + ":" + port;
        } else {
            address = host + ":" + port;
        }

        try {
            String jsonString = getStringOutputByURL(bannedServersFileStr);
            JSONObject json = new JSONObject(jsonString);
            JSONArray serversArray = json.getJSONArray("banned_servers");

            for (int i = 0; i < serversArray.length(); i++) {
                JSONObject serverJson = serversArray.getJSONObject(i);
                String bannedAddress = serverJson.getString("host") + ":" + serverJson.getString("port");
                if (address.equals(bannedAddress)) {
                    System.out.println("Server is banned: " + address);
                    return true;
                }
                if (address.split(":")[0].equals(bannedAddress.split(":")[0])) { // check for host only too
                    System.out.println("Server is banned: " + address);
                    return true;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    public static String readJSONFromFile(File file) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(file));
        StringBuilder sb = new StringBuilder();
        String line;

        while ((line = br.readLine()) != null) {
            sb.append(line);
        }

        br.close();
        return sb.toString();
    }

    public static void writeJSONToFile(File file, JSONObject json) throws IOException, JSONException {
        BufferedWriter bw = new BufferedWriter(new FileWriter(file));
        bw.write(json.toString(4));
        bw.write(System.lineSeparator());
        bw.close();
    }

    public static String getStringOutputByURL(final String url) throws IOException {
        OkHttpClient client = new OkHttpClient();
        Request request = new Request.Builder()
                .url(url)
                .build();

        try (Response response = client.newCall(request).execute()) {
            if (!response.isSuccessful()) {
                throw new IOException("Unexpected response code: " + response.code());
            }

            assert response.body() != null;
            return response.body().string();
        }
    }

    public static String convertBytesToString(long bytes) {
        if (-1000 < bytes && bytes < 1000) {
            return bytes + " B";
        }
        CharacterIterator ci = new StringCharacterIterator("kMGTPE");
        while (bytes <= -999_950 || bytes >= 999_950) {
            bytes /= 1000;
            ci.next();
        }
        return String.format("%.1f %cB", bytes / 1000.0, ci.current());
    }
}
