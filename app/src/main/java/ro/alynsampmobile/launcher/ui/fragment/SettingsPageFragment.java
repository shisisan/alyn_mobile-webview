package ro.alynsampmobile.launcher.ui.fragment;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.textfield.MaterialAutoCompleteTextView;
import com.google.android.material.textfield.TextInputEditText;
import com.joom.paranoid.Obfuscate;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Function;

import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.launcher.SplashActivity;
import ro.alynsampmobile.launcher.utils.Utils;

@Obfuscate
public class SettingsPageFragment extends Fragment {
    private MaterialAutoCompleteTextView samp_version, game_version;
    private Switch voice, timestamp, fullscreen, displayfps, systemkeyboard, cleo, aml, monet, modloader, modify_files;
    private MaterialButton data_full, data_lite, restore_button;
    private MaterialButton chat_minus, chat_strings, chat_plus;
    private TextInputEditText chatposx, chatposy, chatsizex, chatsizey, fontsize;
    private SharedPreferences settings_prefs;

    private LinearLayout mods_layout;
    private RelativeLayout game_version_layout;

    public SettingsPageFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_settings_page, container, false);
        settings_prefs = requireActivity().getSharedPreferences("samp_settings", Context.MODE_PRIVATE);

        initViews(view);
        setupListeners();
        setWidgetsForSettings();

        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        view.findViewById(R.id.sampIcon).setOnClickListener(v ->
                showOptionInfo("SA-MP Version", "Select whichever SA-MP version you prefer from the dropdown."));

        view.findViewById(R.id.gameIcon).setOnClickListener(v ->
                showOptionInfo("Game Version", "Choose whether to use the old version of the Game or thew Newest, old version supports x32 bit devices Only, and the newest supports x32 and x64 bit devices."));

        view.findViewById(R.id.systemkeyboardIcon).setOnClickListener(v ->
                showOptionInfo("System Keyboard", "Enable or disable the system keyboard for input."));

        view.findViewById(R.id.displayfpsIcon).setOnClickListener(v ->
                showOptionInfo("Display FPS", "Display in-game FPS and Memory information."));

        view.findViewById(R.id.fullscreenmodeIcon).setOnClickListener(v ->
                showOptionInfo("Fullscreen Mode", "Force the app to be in full screen."));

        view.findViewById(R.id.voicechatIcon).setOnClickListener(v ->
                showOptionInfo("Voice Chat", "Toggle voice chat support for in-game communication."));

        view.findViewById(R.id.chattimestampIcon).setOnClickListener(v ->
                showOptionInfo("Chat Timestamp", "Enable timestamps for chat messages."));

        view.findViewById(R.id.modifygamefilesIcon).setOnClickListener(v ->
                showOptionInfo("Modify Game Files", "Allow modifications to game files, make sure to backup the official data somewhere if you faced any issues."));

        view.findViewById(R.id.cleoIcon).setOnClickListener(v ->
                showOptionInfo("CLEO Scripts", "Enable CLEO scripts, allows you to inject CLEO scripts in-game."));

        view.findViewById(R.id.amlIcon).setOnClickListener(v ->
                showOptionInfo("AML Scripts", "Enable AML scripts, allows you to inject AML scripts in-game."));

        view.findViewById(R.id.monetloaderIcon).setOnClickListener(v ->
                showOptionInfo("Monetloader Scripts", "Enable Monetloader scripts, allows you to inject LUA scripts in-game."));

        view.findViewById(R.id.modloaderIcon).setOnClickListener(v ->
                showOptionInfo("Mod Loader", "Enable Modloader, allows you to load custom game textures without modifying the official data files."));

        view.findViewById(R.id.gamefilestypeIcon).setOnClickListener(v ->
                showOptionInfo("Game Files Type", "Choose between full or lite game files."));

        view.findViewById(R.id.fontsizeIcon).setOnClickListener(v ->
                showOptionInfo("Font Size", "Adjust the in-game font size for better readability."));

        view.findViewById(R.id.chatstringsIcon).setOnClickListener(v ->
                showOptionInfo("Chat Strings", "Set the number of chat lines displayed."));

        view.findViewById(R.id.chatposIcon).setOnClickListener(v ->
                showOptionInfo("Chat Position", "Modify the position of the chat window."));

        view.findViewById(R.id.chatsizeIcon).setOnClickListener(v ->
                showOptionInfo("Chat Size", "Change the size of the chat window."));

        view.findViewById(R.id.resetIcon).setOnClickListener(v ->
                showOptionInfo("Reset Settings", "Restore all settings to default values."));
    }


    private void initViews(View view) {
        samp_version = view.findViewById(R.id.samp_version);
        game_version = view.findViewById(R.id.game_version);
        game_version_layout = view.findViewById(R.id.game_version_layout);
        voice = view.findViewById(R.id.voice_switch);
        timestamp = view.findViewById(R.id.timestamp_switch);
        fullscreen = view.findViewById(R.id.fullscreen_switch);
        displayfps = view.findViewById(R.id.displayfps_switch);
        systemkeyboard = view.findViewById(R.id.systemkeyboard_switch);
        mods_layout = view.findViewById(R.id.mods_layout);
        cleo = view.findViewById(R.id.cleo_switch);
        aml = view.findViewById(R.id.aml_switch);
        monet = view.findViewById(R.id.monet_switch);
        modloader = view.findViewById(R.id.modloader_switch);
        modify_files = view.findViewById(R.id.modify_switch);
        data_full = view.findViewById(R.id.data_full);
        data_lite = view.findViewById(R.id.data_lite);
        restore_button = view.findViewById(R.id.restore_button);
        chat_minus = view.findViewById(R.id.chat_minus);
        chat_strings = view.findViewById(R.id.chat_strings);
        chat_plus = view.findViewById(R.id.chat_plus);
        chatposx = view.findViewById(R.id.chatposx);
        chatposy = view.findViewById(R.id.chatposy);
        chatsizex = view.findViewById(R.id.chatsizex);
        chatsizey = view.findViewById(R.id.chatsizey);

        fontsize = view.findViewById(R.id.fontsize);
    }

    private void setupListeners() {
        String[] items = {"0.3.7", "0.3.7-R4"};
        ArrayAdapter<String> selectCoreAdapter = new ArrayAdapter<>(requireContext(), R.layout.layout_setting_version, items);
        samp_version.setAdapter(selectCoreAdapter);
        samp_version.setText(items[settings_prefs.getInt("samp_version", 0)], false);
        samp_version.setOnItemClickListener((adapterView, v, i, l) -> savePreference("samp_version", i));

        /*String[] gameItems = {"New", "Old"};
        ArrayAdapter<String> selectGameAdapter = new ArrayAdapter<>(requireContext(), R.layout.layout_setting_version, gameItems);
        game_version.setAdapter(selectGameAdapter);
        game_version.setText(gameItems[settings_prefs.getInt("game_version", 0)], false);
        game_version.setOnItemClickListener((adapterView, v, i, l) -> {
            savePreference("game_version", i);
            setWidgetsForSettings();
        });*/
        savePreference("game_version", 0);
        game_version_layout.setVisibility(View.GONE);

        setupSwitchListeners();
        setupDataButtons();
        setupRestoreButton();
        setupChatButtons();
        setupTextWatchers();
    }

    private void setupSwitchListeners() {
        Map<CompoundButton, String> switchMap = new HashMap<>();
        switchMap.put(systemkeyboard, "system_keyboard");
        switchMap.put(timestamp, "timestamp");
        switchMap.put(fullscreen, "fullscreen");
        switchMap.put(displayfps, "display_fps");
        switchMap.put(voice, "voice_chat");
        switchMap.put(cleo, "cleo_scripts");
        switchMap.put(aml, "aml_scripts");
        switchMap.put(monet, "monet_scripts");
        switchMap.put(modloader, "modloader");

        for (Map.Entry<CompoundButton, String> entry : switchMap.entrySet()) {
            setupSwitch(entry.getKey(), entry.getValue());
        }

        setupSwitchWithWarning(modify_files);
    }

    private void setupSwitch(CompoundButton switchButton, String key) {
        switchButton.setOnCheckedChangeListener(null);
        switchButton.setChecked(settings_prefs.getBoolean(key, false));
        switchButton.setOnCheckedChangeListener((buttonView, isChecked) -> savePreference(key, isChecked));
    }

    private void setupSwitchWithWarning(CompoundButton switchButton) {
        switchButton.setOnCheckedChangeListener(null);
        switchButton.setChecked(settings_prefs.getBoolean("modify_files", false));

        switchButton.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked && buttonView.isPressed()) {
                showAlertDialog("If you enable this feature, the launcher will not ask you to update game files anymore. If you face crashes, disable this option and update game files!",
                        (dialog, which) -> dialog.dismiss(), (dialog, which) -> {
                            switchButton.setChecked(false);
                            dialog.dismiss();
                        });
            }
            savePreference("modify_files", isChecked);
        });
    }

    private void setupDataButtons() {
        data_full.setOnClickListener(v -> handleDataChange("full", data_full));
        data_lite.setOnClickListener(v -> handleDataChange("lite", data_lite));
    }

    private void setupRestoreButton() {
        restore_button.setOnClickListener(v -> showAlertDialog("Are you sure you want to restore default settings?", (dialog, which) -> {
            Utils.restoreSettings(requireContext());
            setWidgetsForSettings();
            dialog.dismiss();
        }, (dialog, which) -> dialog.dismiss()));
    }

    private void handleDataChange(String type, MaterialButton button) {
        if (settings_prefs.getString("files_type", "none").equals(type)) {
            button.setChecked(true);
            return;
        }

        showAlertDialog("Are you sure you want to change the game files to " + type + "?", (dialog, which) -> {
                    savePreference("files_type", type);
                    Intent intent = new Intent(getContext(), SplashActivity.class);
                    startActivity(intent);
                    requireActivity().finish();
                },
                (dialog, which) -> {
                    button.setChecked(false);
                    dialog.dismiss();
                });
    }

    private void setupChatButtons() {
        chat_minus.setOnClickListener(v -> {
            adjustChatStrings(-1);
            chat_minus.setChecked(false);
        });
        chat_plus.setOnClickListener(v -> {
            adjustChatStrings(1);
            chat_plus.setChecked(false);
        });
    }

    private void adjustChatStrings(int delta) {
        int currentValue = settings_prefs.getInt("chat_strings", 5);
        int newValue = currentValue + delta;

        if (newValue < 5 || newValue > 20) return;

        chat_strings.setText(String.valueOf(newValue));
        savePreference("chat_strings", newValue);
    }

    private void setupTextWatchers() {
        setupTextWatcher(chatposx, "chat_posx", text -> text.isEmpty() ? 100 : Integer.parseInt(text));
        setupTextWatcher(chatposy, "chat_posy", text -> text.isEmpty() ? 10 : Integer.parseInt(text));
        setupTextWatcher(chatsizex, "chat_sizex", text -> text.isEmpty() ? 400 : Integer.parseInt(text));
        setupTextWatcher(chatsizey, "chat_sizey", text -> text.isEmpty() ? 150 : Integer.parseInt(text));
        setupTextWatcher(fontsize, "font_size", text -> text.isEmpty() ? 0.0f : Float.parseFloat(text));
    }

    private <T> void setupTextWatcher(TextInputEditText editText, String key, Function<String, T> parser) {
        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                String text = s.toString();
                T value = parser.apply(text);
                if (value instanceof Integer) savePreference(key, value);
                if (value instanceof Float) savePreference(key, value);
            }

            @Override
            public void afterTextChanged(Editable s) {
            }
        });
    }

    private void setWidgetsForSettings() {
        Map<CompoundButton, String> switchMap = new HashMap<>();
        switchMap.put(systemkeyboard, "system_keyboard");
        switchMap.put(timestamp, "timestamp");
        switchMap.put(fullscreen, "fullscreen");
        switchMap.put(displayfps, "display_fps");
        switchMap.put(voice, "voice_chat");
        switchMap.put(cleo, "cleo_scripts");
        switchMap.put(aml, "aml_scripts");
        switchMap.put(monet, "monet_scripts");
        switchMap.put(modloader, "modloader");
        switchMap.put(modify_files, "modify_files");

        if (settings_prefs.getInt("game_version", 0) == 0) {
            mods_layout.setVisibility(View.GONE);
        } else {
            mods_layout.setVisibility(View.VISIBLE);
        }

        for (Map.Entry<CompoundButton, String> entry : switchMap.entrySet()) {
            entry.getKey().setChecked(settings_prefs.getBoolean(entry.getValue(), false));
        }

        String fileType = settings_prefs.getString("files_type", "none");
        data_full.setChecked("full".equals(fileType));
        data_lite.setChecked("lite".equals(fileType));

        chat_strings.setText(String.valueOf(settings_prefs.getInt("chat_strings", 5)));
        chatposx.setText(String.valueOf(settings_prefs.getInt("chat_posx", 100)));
        chatposy.setText(String.valueOf(settings_prefs.getInt("chat_posy", 10)));
        chatsizex.setText(String.valueOf(settings_prefs.getInt("chat_sizex", 400)));
        chatsizey.setText(String.valueOf(settings_prefs.getInt("chat_sizey", 150)));

        fontsize.setText(String.valueOf(settings_prefs.getFloat("font_size", 26.0f)));
    }

    private void savePreference(String key, Object value) {
        SharedPreferences.Editor editor = settings_prefs.edit();

        if (value instanceof Integer) editor.putInt(key, (Integer) value);
        else if (value instanceof Boolean) editor.putBoolean(key, (Boolean) value);
        else if (value instanceof Float) editor.putFloat(key, (Float) value);
        else if (value instanceof String) editor.putString(key, (String) value);

        editor.apply();
        Utils.saveSettings(getContext());
    }

    private void showAlertDialog(String message, DialogInterface.OnClickListener positiveListener, DialogInterface.OnClickListener negativeListener) {
        new AlertDialog.Builder(requireActivity())
                .setTitle("Warning:")
                .setMessage(message)
                .setPositiveButton("OK", positiveListener)
                .setNegativeButton("Cancel", negativeListener)
                .show();
    }

    private void showOptionInfo(String title, String message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(requireContext());
        builder.setTitle(title);
        builder.setMessage(message);
        builder.setPositiveButton("OK", (dialog, which) -> dialog.dismiss());
        builder.show();
    }
}