package ro.alynsampmobile.game.ui.widgets;

import android.app.Activity;
import android.view.View;
import android.widget.ImageButton;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.joom.paranoid.Obfuscate;

import ro.alynsampmobile.launcher.R;

@Obfuscate
public class Voice {
    private final ConstraintLayout voice_layout;

    private final ImageButton voice_button;

    public interface Listener {
        void _voicePressed();
    }

    private final Listener listener;

    private boolean voice_pressed = false;

    public Voice(Activity activity, Listener listener) {
        this.listener = listener;

        voice_layout = (ConstraintLayout) activity.getLayoutInflater().inflate(R.layout.activity_game_voice, null);
        activity.addContentView(voice_layout, new ConstraintLayout.LayoutParams(-1, -1));

        voice_button = voice_layout.findViewById(R.id.voice_button);
        voice_button.setOnClickListener(view -> {
            this.listener._voicePressed();
            voice_pressed = !voice_pressed;
            voice_button.setColorFilter(voice_pressed ? 0xFFFF0000 : 0xFF000000);
        });

        show(false);
    }

    public boolean isVisible() {
        return voice_layout.getVisibility() == View.VISIBLE;
    }

    public void show(boolean z) {
        voice_layout.setVisibility(z ? View.VISIBLE : View.GONE);

        if (!z) {
            voice_pressed = false;
            voice_button.setColorFilter(0xFF000000);
        }
    }
}
