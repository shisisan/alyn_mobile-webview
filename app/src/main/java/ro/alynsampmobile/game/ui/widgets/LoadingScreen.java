package ro.alynsampmobile.game.ui.widgets;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.joom.paranoid.Obfuscate;

import ro.alynsampmobile.launcher.R;

@Obfuscate
public class LoadingScreen {
    ConstraintLayout layout;

    public LoadingScreen(Activity activity) {
        layout = (ConstraintLayout) activity.getLayoutInflater().inflate(R.layout.activity_game_loadingscreen, null);
        activity.addContentView(layout, new ConstraintLayout.LayoutParams(-1, -1));
        show(false);
    }

    public void show(boolean z) {
        layout.setVisibility(z ? View.VISIBLE : View.GONE);
    }
}
