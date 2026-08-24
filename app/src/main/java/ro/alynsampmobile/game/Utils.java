package ro.alynsampmobile.game;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.graphics.Point;
import android.text.Html;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.view.Display;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.joom.paranoid.Obfuscate;

@Obfuscate
public class Utils {
    public static CharSequence getColoredString(String str) {
        return Html.fromHtml(getStringWithColors(str));
    }

    private static String getStringWithColors(String str) {
        int i;
        int count = 0;
        StringBuilder sb = new StringBuilder();
        int i2 = 0;
        boolean z = false;
        while (i2 < str.length()) {
            if (str.charAt(i2) == '{' && (i = i2 + 7) < str.length() && str.charAt(i) == '}') {
                if (z) {
                    sb.append("</font>");
                }
                sb.append("<font color=#");
                while (true) {
                    i2++;
                    if (i2 >= i) {
                        break;
                    }
                    sb.append(str.charAt(i2));
                }
                sb.append('>');
                i2 = i;
                z = true;
            } else {
                sb.append(str.charAt(i2));
            }
            i2++;
            count++;
        }
        if (z) {
            sb.append("</font>");
        }
        if (count > 0) {
            return sb.toString().replace("\n", "<br/>");
        } else return str;
    }

    private static final float MULT_X = 5.2083336E-4f;
    private static final float MULT_Y = 9.259259E-4f;

    public static void scaleViewAndChildren(Activity activity, View view) {
        Display defaultDisplay = activity.getWindowManager().getDefaultDisplay();
        Point point = new Point();
        defaultDisplay.getSize(point);
        float min = Math.min(((float) point.x) * MULT_X, ((float) point.y) * MULT_Y);
        ViewGroup.LayoutParams layoutParams = view.getLayoutParams();
        if (!(layoutParams.width == -1 || layoutParams.width == -2 || ((int) (((float) layoutParams.width) * min)) == 0)) {
            layoutParams.width = (int) (((float) layoutParams.width) * min);
        }
        if (!(layoutParams.height == -1 || layoutParams.height == -2 || ((int) (((float) layoutParams.height) * min)) == 0)) {
            layoutParams.height = (int) (((float) layoutParams.height) * min);
        }
        if (layoutParams instanceof ViewGroup.MarginLayoutParams) {
            ViewGroup.MarginLayoutParams marginLayoutParams = (ViewGroup.MarginLayoutParams) layoutParams;
            marginLayoutParams.leftMargin = (int) (((float) marginLayoutParams.leftMargin) * min);
            marginLayoutParams.rightMargin = (int) (((float) marginLayoutParams.rightMargin) * min);
            marginLayoutParams.topMargin = (int) (((float) marginLayoutParams.topMargin) * min);
            marginLayoutParams.bottomMargin = (int) (((float) marginLayoutParams.bottomMargin) * min);
        }
        if (layoutParams instanceof ConstraintLayout.LayoutParams) {
            ConstraintLayout.LayoutParams layoutParams2 = (ConstraintLayout.LayoutParams) layoutParams;
            layoutParams2.matchConstraintMinHeight = (int) (((float) layoutParams2.matchConstraintMinHeight) * min);
            layoutParams2.matchConstraintMinWidth = (int) (((float) layoutParams2.matchConstraintMinWidth) * min);
            layoutParams2.matchConstraintMaxHeight = (int) (((float) layoutParams2.matchConstraintMaxHeight) * min);
            layoutParams2.matchConstraintMaxWidth = (int) (((float) layoutParams2.matchConstraintMaxWidth) * min);
        }
        view.setLayoutParams(layoutParams);
        view.setPadding((int) (((float) view.getPaddingLeft()) * min), (int) (((float) view.getPaddingTop()) * min), (int) (((float) view.getPaddingRight()) * min), (int) (((float) view.getPaddingBottom()) * min));
        view.setMinimumHeight((int) (((float) view.getMinimumHeight()) * min));
        view.setMinimumWidth((int) (((float) view.getMinimumWidth()) * min));
        if (view instanceof TextView) {
            TextView textView = (TextView) view;
            textView.setTextSize(0, textView.getTextSize() * min);
        }

        if (view instanceof RecyclerView) {
            RecyclerView customRecyclerView = (RecyclerView) view;
            customRecyclerView.setScrollBarSize((int) (((float) customRecyclerView.getScrollBarSize()) * min));
        }

        if (view instanceof ViewGroup) {
            ViewGroup viewGroup = (ViewGroup) view;
            for (int i = 0; i < viewGroup.getChildCount(); i++) {
                scaleViewAndChildren(activity, viewGroup.getChildAt(i));
            }
        }
    }

    public static float scale(Activity activity, float f) {
        Display defaultDisplay = activity.getWindowManager().getDefaultDisplay();
        Point point = new Point();
        defaultDisplay.getSize(point);
        return f * Math.min(((float) point.x) * MULT_X, ((float) point.y) * MULT_Y);
    }

    public static int getTextWidth(String str, TextPaint textPaint) {
        return (int) new StaticLayout(str, textPaint, Integer.MAX_VALUE, Layout.Alignment.ALIGN_NORMAL, 1.0f, 0.0f, false).getLineWidth(0);
    }

    public static String getStringWithoutColors(String str) {
        int i;
        StringBuilder sb = new StringBuilder();
        int i2 = 0;
        while (i2 < str.length()) {
            if (str.charAt(i2) == '{' && (i = i2 + 7) < str.length() && str.charAt(i) == '}') {
                i2 = i;
            } else {
                sb.append(str.charAt(i2));
            }
            i2++;
        }
        return sb.toString();
    }
}
