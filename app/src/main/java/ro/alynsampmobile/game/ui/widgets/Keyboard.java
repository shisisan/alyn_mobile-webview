package ro.alynsampmobile.game.ui.widgets;

import android.app.Activity;
import android.content.Context;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import com.google.android.material.textfield.TextInputEditText;

import java.util.ArrayList;

import ro.alynsampmobile.launcher.R;


public class Keyboard {
    private final Activity activity;
    private Listener listener;
    private boolean visible;

    private final ArrayList<String> history;
    private int current_history;

    private final RelativeLayout keyboard_layout;
    private final TextInputEditText keyboard_input;
    private final LinearLayout keyboard_input_layout;

    public interface Listener {
        void _keyboardSend(String str);
    }

    public Keyboard(Activity activity, Listener listener) {
        this.activity = activity;
        this.listener = listener;

        history = new ArrayList<>();
        current_history = 0;

        keyboard_layout = (RelativeLayout) activity.getLayoutInflater().inflate(R.layout.activity_game_keyboard, null);
        activity.addContentView(keyboard_layout, new RelativeLayout.LayoutParams(-1, -1));

        keyboard_input = keyboard_layout.findViewById(R.id.keyboard_input);
        keyboard_input_layout = keyboard_layout.findViewById(R.id.keyboard_input_layout);
        ImageButton keyboard_button_ok = keyboard_layout.findViewById(R.id.keyboard_button_ok);
        ImageButton keyboard_button_prev = keyboard_layout.findViewById(R.id.keyboard_button_prev);
        ImageButton keyboard_button_next = keyboard_layout.findViewById(R.id.keyboard_button_next);
        ImageButton keyboard_button_slash = keyboard_layout.findViewById(R.id.keyboard_button_slash);

        show(false);

        keyboard_input.setOnEditorActionListener((textView, i, keyEvent) -> {
            if (i == EditorInfo.IME_ACTION_DONE || i == EditorInfo.IME_ACTION_NEXT) {
                if (keyboard_input.getText() != null) {
                    send(keyboard_input.getText().toString());
                    keyboard_input.setText("");
                    show(false);
                } else {
                    show(false);
                }
            }
            return false;
        });

        keyboard_button_slash.setOnClickListener(view -> keyboard_input.getText().insert(keyboard_input.getSelectionStart(), "/"));

        keyboard_button_ok.setOnClickListener(view -> {
            if (keyboard_input.getText() != null) {
                send(keyboard_input.getText().toString());
                keyboard_input.setText("");
                show(false);
            } else {
                show(false);
            }
        });

        keyboard_button_prev.setOnClickListener(view -> {
            current_history--;

            if (current_history < 0) {
                current_history = 0;
            }

            if (current_history <= 0) {
                keyboard_input.setText("");
                return;
            }

            keyboard_input.setText(history.get(current_history - 1));
            keyboard_input.setSelection(keyboard_input.getText().length());
        });

        keyboard_button_next.setOnClickListener(view -> {
            current_history++;

            if ((current_history - 1) >= history.size()) {
                current_history--;
            }

            if (current_history <= 0) {
                return;
            }

            keyboard_input.setText(history.get(current_history - 1));
            keyboard_input.setSelection(keyboard_input.getText().length());
        });
    }

    public boolean isVisible() {
        return visible;
    }

    public void show(boolean z) {
        visible = z;
        current_history = 0;

        keyboard_layout.setVisibility(z ? View.VISIBLE : View.GONE);

        if (z) {
            keyboard_input.requestFocus();
            ((InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE)).showSoftInput(keyboard_input, InputMethodManager.SHOW_IMPLICIT);
        } else {
            /*if (activity.getCurrentFocus() != null) {
                ((InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(activity.getCurrentFocus().getWindowToken(), 0);
            }*/

            ((InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(keyboard_input.getWindowToken(), 0);
        }
    }

    private void send(String str) {
        if (history.size() >= 20) {
            history.remove(history.size() - 1);
        }

        history.add(0, str);
        listener._keyboardSend(str);
    }

    public void onHeightChanged(int height) {
        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) keyboard_layout.getLayoutParams();
        params.setMargins(0, 0, 0, height);
        keyboard_layout.setLayoutParams(params);
    }

    public void onTouchEvent(MotionEvent event) {
        // if its visible and touches outside keyboard layout, hide it
        if (keyboard_input_layout.getVisibility() == View.VISIBLE && event.getAction() == MotionEvent.ACTION_DOWN) {
            int[] location = new int[2];
            keyboard_input_layout.getLocationOnScreen(location);

            if (event.getX() < location[0] || event.getX() > location[0] + keyboard_input_layout.getWidth() || event.getY() < location[1] || event.getY() > location[1] + keyboard_input_layout.getHeight()) {
                show(false);
            }
        }
    }
}