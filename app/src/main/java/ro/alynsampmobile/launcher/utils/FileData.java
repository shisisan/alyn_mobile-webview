package ro.alynsampmobile.launcher.utils;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

import com.joom.paranoid.Obfuscate;

@Obfuscate
public class FileData {
    private final String name;
    private final String path;
    private final long size;
    private final String url;
    private final String gpu;

    public FileData(String name, String path, long size, String url, String gpu) {
        this.name = name;
        this.path = path;
        this.size = size;
        this.url = url;
        this.gpu = gpu;
    }

    public String getName() {
        return name;
    }

    public String getPath() {
        return path;
    }

    public long getSize() {
        return size;
    }

    public String getUrl() {
        return url;
    }

    public String getGpu() {
        return gpu;
    }

    public static ArrayList<FileData> getListByJson(JSONObject json) throws JSONException {
        ArrayList<FileData> list = new ArrayList<>();
        JSONArray arr = json.getJSONArray("files");
        for (int i = 0; i < arr.length(); i++) {
            list.add(new FileData(arr.getJSONObject(i).getString("name"),
                    arr.getJSONObject(i).getString("path"),
                    arr.getJSONObject(i).getLong("size"),
                    arr.getJSONObject(i).getString("url"),
                    arr.getJSONObject(i).getString("gpu")));
        }
        return list;
    }
}
