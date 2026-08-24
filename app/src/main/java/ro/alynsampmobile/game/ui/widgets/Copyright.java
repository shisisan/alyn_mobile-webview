package ro.alynsampmobile.game.ui.widgets;

import android.app.Activity;
import android.widget.RelativeLayout;

import com.google.android.material.textview.MaterialTextView;
import com.joom.paranoid.Obfuscate;

import ro.alynsampmobile.launcher.BuildConfig;
import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.launcher.utils.Utils;

@Obfuscate
public class Copyright {
    public Copyright(Activity activity) {
        activity.runOnUiThread(() -> {
            RelativeLayout copyright_layout = (RelativeLayout) activity.getLayoutInflater().inflate(R.layout.activity_game_copyright, null);
            activity.addContentView(copyright_layout, new RelativeLayout.LayoutParams(-1, -1));

            MaterialTextView ahahaha = activity.findViewById(R.id.ahahaha);
            ahahaha.setText(BuildConfig.VERSION_NAME);

            MaterialTextView ahahaha2 = activity.findViewById(R.id.ahahaha2);
/*            ahahaha2.setText(Utils.copyright);*/
        });
    }
}
