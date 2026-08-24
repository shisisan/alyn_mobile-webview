package ro.alynsampmobile.game.ui.widgets;

import android.app.Activity;
import android.graphics.Color;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import androidx.appcompat.widget.AppCompatImageView;
import androidx.constraintlayout.widget.ConstraintLayout;

import com.joom.paranoid.Obfuscate;

import ro.alynsampmobile.launcher.R;

@Obfuscate
public class WantedLevel {
    private int wanted_level;

    private final FrameLayout wanted_layout;
    private final RelativeLayout wanted_stars_layout;

    private final AppCompatImageView star_1;
    private final AppCompatImageView star_2;
    private final AppCompatImageView star_3;
    private final AppCompatImageView star_4;
    private final AppCompatImageView star_5;
    private final AppCompatImageView star_6;

    public WantedLevel(Activity activity) {
        wanted_level = 0;

        wanted_layout = (FrameLayout) activity.getLayoutInflater().inflate(R.layout.activity_game_wanted, null);
        activity.addContentView(wanted_layout, new ConstraintLayout.LayoutParams(-1, -1));

        wanted_stars_layout = wanted_layout.findViewById(R.id.wanted_stars_layout);

        star_1 = wanted_layout.findViewById(R.id.star_1);
        star_2 = wanted_layout.findViewById(R.id.star_2);
        star_3 = wanted_layout.findViewById(R.id.star_3);
        star_4 = wanted_layout.findViewById(R.id.star_4);
        star_5 = wanted_layout.findViewById(R.id.star_5);
        star_6 = wanted_layout.findViewById(R.id.star_6);

        show(false);
    }

    public void setLevel(int level) {
        wanted_level = level;
    }

    public void show(boolean z) {
        wanted_layout.setVisibility(z ? View.VISIBLE : View.GONE);

        if (wanted_level <= 0) {
            wanted_stars_layout.setVisibility(View.GONE);
        } else {
            wanted_stars_layout.setVisibility(View.VISIBLE);

            star_1.setColorFilter(wanted_level >= 6 ? Color.parseColor("#F6BE00") : Color.parseColor("#353535"));
            star_2.setColorFilter(wanted_level >= 5 ? Color.parseColor("#F6BE00") : Color.parseColor("#353535"));
            star_3.setColorFilter(wanted_level >= 4 ? Color.parseColor("#F6BE00") : Color.parseColor("#353535"));
            star_4.setColorFilter(wanted_level >= 3 ? Color.parseColor("#F6BE00") : Color.parseColor("#353535"));
            star_5.setColorFilter(wanted_level >= 2 ? Color.parseColor("#F6BE00") : Color.parseColor("#353535"));
            star_6.setColorFilter(wanted_level >= 1 ? Color.parseColor("#F6BE00") : Color.parseColor("#353535"));
        }
    }
}
