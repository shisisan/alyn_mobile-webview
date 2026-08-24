package ro.alynsampmobile.game.ui.widgets;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;

import com.joom.paranoid.Obfuscate;
import ro.alynsampmobile.launcher.R;

@Obfuscate
public class ButtonPanel {
    private boolean extended;
    private final ConstraintLayout buttonpanel_layout;

    private final MaterialButton button_extend;
    private final MaterialButton button_esc;
    private final MaterialButton button_tab;
    private final MaterialButton button_alt;
    private final MaterialButton button_spc;
    private final MaterialButton button_f;
    private final MaterialButton button_h;
    private final MaterialButton button_2;
    private final MaterialButton button_y;
    private final MaterialButton button_n;
    private final MaterialButton button_c;
    private final MaterialButton button_g;

    public interface Listener {
        void _sendButtonPanelKey(int id);
    }

    private Listener listener;

    public ButtonPanel(Activity activity, Listener listener) {
        this.listener = listener;

        extended = true;

        buttonpanel_layout = (ConstraintLayout) activity.getLayoutInflater().inflate(R.layout.activity_game_buttonpanel, null);
        activity.addContentView(buttonpanel_layout, new ConstraintLayout.LayoutParams(-1, -1));

        button_extend = buttonpanel_layout.findViewById(R.id.button_extend);
        button_esc = buttonpanel_layout.findViewById(R.id.button_esc);
        button_tab = buttonpanel_layout.findViewById(R.id.button_tab);
        button_alt = buttonpanel_layout.findViewById(R.id.button_alt);
        button_spc = buttonpanel_layout.findViewById(R.id.button_spc);
        button_f = buttonpanel_layout.findViewById(R.id.button_f);
        button_h = buttonpanel_layout.findViewById(R.id.button_h);
        button_2 = buttonpanel_layout.findViewById(R.id.button_2);
        button_y = buttonpanel_layout.findViewById(R.id.button_y);
        button_n = buttonpanel_layout.findViewById(R.id.button_n);
        button_c = buttonpanel_layout.findViewById(R.id.button_c);
        button_g = buttonpanel_layout.findViewById(R.id.button_g);

        button_extend.setOnClickListener(view -> extend(!extended));

        button_esc.setOnClickListener(view -> listener._sendButtonPanelKey(1));
        button_tab.setOnClickListener(view -> listener._sendButtonPanelKey(2));
        button_alt.setOnClickListener(view -> listener._sendButtonPanelKey(3));
        button_spc.setOnClickListener(view -> listener._sendButtonPanelKey(4));
        button_f.setOnClickListener(view -> listener._sendButtonPanelKey(5));
        button_h.setOnClickListener(view -> listener._sendButtonPanelKey(6));
        button_2.setOnClickListener(view -> listener._sendButtonPanelKey(7));
        button_y.setOnClickListener(view -> listener._sendButtonPanelKey(8));
        button_n.setOnClickListener(view -> listener._sendButtonPanelKey(9));
        button_c.setOnClickListener(view -> listener._sendButtonPanelKey(10));
        button_g.setOnClickListener(view -> listener._sendButtonPanelKey(11));

        show(false);
        extend(false);
    }

    public boolean isVisible() {
        return buttonpanel_layout.getVisibility() == View.VISIBLE;
    }

    public void show(boolean z) {
        buttonpanel_layout.setVisibility(z ? View.VISIBLE : View.GONE);
    }

    private void extend(boolean ext) {
        extended = ext;

        button_extend.setText(ext ? "<<" : ">>");

        button_esc.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_tab.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_alt.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_spc.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_f.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_h.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_2.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_y.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_n.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_c.setVisibility(ext ? View.VISIBLE : View.GONE);
        button_g.setVisibility(ext ? View.VISIBLE : View.GONE);
    }
}
