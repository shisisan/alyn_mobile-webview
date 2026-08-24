package ro.alynsampmobile.game.ui.widgets;

import android.app.Activity;
import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.joom.paranoid.Obfuscate;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

import ro.alynsampmobile.game.Utils;
import ro.alynsampmobile.game.ui.widgets.adapter.DialogAdapter;
import ro.alynsampmobile.launcher.R;

@Obfuscate
public class Dialog {

    private Activity activity;
    private Listener listener;

    public final int DIALOG_STYLE_INPUT;
    public final int DIALOG_STYLE_LIST;
    public final int DIALOG_STYLE_MSGBOX;
    public final int DIALOG_STYLE_PASSWORD;
    public final int DIALOG_STYLE_TABLIST;
    public final int DIALOG_STYLE_TABLIST_HEADERS;

    private String mButtonNegative;
    private String mButtonPositive;
    private String mCaption;
    private int mCurrentDialogId;

    public String mCurrentInputtext;
    public int mCurrentListitem;

    public ConstraintLayout mMainLayout;
    public View mDialogBody;
    private View mDialogButtonNegative;
    private TextView mDialogButtonNegativeText;
    private View mDialogButtonPositive;
    private TextView mDialogButtonPositiveText;
    private TextView mDialogCaption;
    private EditText mDialogInput;
    private View mDialogInputLayout;

    public View mDialogList;

    public View mDialogListLayout;

    public RecyclerView mDialogListRecycler;

    public int mDialogStyle;

    public final TextView[] mDialogTabField;
    private View mDialogTabListRow;
    private TextView mDialogText;
    private View mDialogTextLayout;

    public DialogAdapter mListAdapter;
    private final ArrayList<String> mRowsList;

    public int[] mTabSizes;
    private String mText;
    public boolean isShow;

    public interface Listener {
        void _sendDialogResponse(int dialog_id, int button_id, int list_item, byte[] input);
    }

    public Dialog(Activity activity, Listener listener) {
        this.activity = activity;
        this.listener = listener;

        ConstraintLayout layout = (ConstraintLayout) activity.getLayoutInflater().inflate(R.layout.activity_game_dialog, null);
        activity.addContentView(layout, new ConstraintLayout.LayoutParams(-1, -1));

        DIALOG_STYLE_MSGBOX = 0;
        DIALOG_STYLE_INPUT = 1;
        DIALOG_STYLE_LIST = 2;
        DIALOG_STYLE_PASSWORD = 3;
        DIALOG_STYLE_TABLIST = 4;
        DIALOG_STYLE_TABLIST_HEADERS = 5;
        mDialogTabField = new TextView[4];
        mRowsList = new ArrayList<>();
        mCurrentDialogId = 0;
        mDialogStyle = 0;
        mCaption = "";
        mText = "";
        mButtonPositive = "";
        mButtonNegative = "";
        mCurrentListitem = -1;
        mCurrentInputtext = "";
        mTabSizes = new int[]{0, 0, 0, 0};

        mMainLayout = layout;
        mDialogBody = layout.findViewById(R.id.dialog_body);
        mDialogTextLayout = layout.findViewById(R.id.dialog_text_layout);
        mDialogInputLayout = layout.findViewById(R.id.dialog_input_layout);
        mDialogListLayout = layout.findViewById(R.id.dialog_list_layout);
        mDialogTabListRow = layout.findViewById(R.id.dialog_tablist_row);
        mDialogList = layout.findViewById(R.id.dialog_list);
        mDialogCaption = layout.findViewById(R.id.dialog_caption);
        mDialogText = layout.findViewById(R.id.dialog_text);
        mDialogInput = layout.findViewById(R.id.dialog_input);
        mDialogTabField[0] = layout.findViewById(R.id.dialog_field1);
        mDialogTabField[1] = layout.findViewById(R.id.dialog_field2);
        mDialogTabField[2] = layout.findViewById(R.id.dialog_field3);
        mDialogTabField[3] = layout.findViewById(R.id.dialog_field4);
        mDialogListRecycler = layout.findViewById(R.id.dialog_list_recycler);
        mDialogButtonPositive = layout.findViewById(R.id.button_positive);
        mDialogButtonNegative = layout.findViewById(R.id.button_negative);
        mDialogButtonPositiveText = layout.findViewById(R.id.button_positive_text);
        mDialogButtonNegativeText = layout.findViewById(R.id.button_negative_text);

        mDialogInput.setOnClickListener(view -> {
            mDialogInput.requestFocus();
            ((InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE)).showSoftInput(mDialogInput, InputMethodManager.SHOW_IMPLICIT);
        });

        Utils.scaleViewAndChildren(activity, mMainLayout);
        Hide();
    }

    public void show(int dialog_id, int dialog_style, String title, String message, String button1, String button2) {
        isShow = true;
        mCurrentDialogId = dialog_id;
        mDialogStyle = dialog_style;
        mCaption = title;
        mText = message;
        mButtonPositive = button1;
        mButtonNegative = button2;

        ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) mDialogTextLayout.getLayoutParams();
        layoutParams.height = -2;
        mDialogTextLayout.setLayoutParams(layoutParams);
        FrameLayout.LayoutParams layoutParams2 = (FrameLayout.LayoutParams) mMainLayout.getLayoutParams();
        layoutParams2.gravity = 17;
        mMainLayout.setLayoutParams(layoutParams2);

        loadDialog();
        mMainLayout.setVisibility(View.VISIBLE);
    }

    public void showWithoutReset(boolean z) {
        if (isShow) {
            mMainLayout.setVisibility(z ? View.VISIBLE : View.GONE);
        }
    }

    public void Hide() {
        /*if (activity.getCurrentFocus() != null) {
            ((InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(activity.getCurrentFocus().getWindowToken(), 0);
            System.out.println("[Hide] closed input");
        }*/

        isShow = false;
        ((InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(mDialogInput.getWindowToken(), 0);
        mMainLayout.setVisibility(View.GONE);
    }

    public void onHeightChanged(int i) {
        if (mMainLayout.getVisibility() == View.VISIBLE) {
            int i2 = mDialogStyle;
            if (i2 != 1 && i2 != 3) {
                return;
            }
            if (i <= 150) {
                ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) mDialogTextLayout.getLayoutParams();
                layoutParams.height = -2;
                mDialogTextLayout.setLayoutParams(layoutParams);
                FrameLayout.LayoutParams layoutParams2 = (FrameLayout.LayoutParams) mMainLayout.getLayoutParams();
                layoutParams2.gravity = 17;
                mMainLayout.setLayoutParams(layoutParams2);
                return;
            }
            ConstraintLayout.LayoutParams layoutParams3 = (ConstraintLayout.LayoutParams) mDialogTextLayout.getLayoutParams();
            layoutParams3.height = (int) Utils.scale(activity, 104.0f);
            mDialogTextLayout.setLayoutParams(layoutParams3);
            FrameLayout.LayoutParams layoutParams4 = (FrameLayout.LayoutParams) mMainLayout.getLayoutParams();
            layoutParams4.gravity = 49;
            mMainLayout.setLayoutParams(layoutParams4);
        }
    }

    public void SendDialogResponse(int i, int i2, String str) {
        if (i2 == -1) {
            int i3 = mDialogStyle;
            if (i3 == 2 || i3 == 4 || i3 == 5) {
                i2 = 0;
            }
        }

        try {
            byte[] Str2 = str.getBytes("windows-1251");
            listener._sendDialogResponse(mCurrentDialogId, i, i2, Str2);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        Hide();
    }

    public void loadDialog() {
        clearDialogData();
        loadDialogData();
        loadButtons();
        loadRecycler();
        loadSizes();
    }

    public void clearDialogData() {
        mRowsList.clear();
        mDialogInput.setText("");
        mCurrentListitem = -1;
        mCurrentInputtext = "";
        int[] iArr = mTabSizes;
        iArr[0] = 0;
        iArr[1] = 0;
        iArr[2] = 0;
        iArr[3] = 0;
    }

    public void loadDialogData() {
        setDialogStyle(mDialogStyle);
        mDialogCaption.setText(Utils.getColoredString(mCaption));
        int i = mDialogStyle;
        if (i == 0 || i == 1 || i == 3) {
            mDialogText.setText(Utils.getColoredString(mText));
            return;
        }
        if (i != 2) {
            StringBuilder sb = new StringBuilder(mText);
            for (int i2 = 0; i2 < sb.length(); i2++) {
                if (sb.charAt(i2) == 9 && i2 < sb.length() - 1) {
                    while (true) {
                        int i3 = i2 + 1;
                        if (sb.charAt(i3) != 9) {
                            break;
                        }
                        sb.deleteCharAt(i3);
                    }
                }
            }
            mText = sb.toString();
        }
        mCurrentListitem = 0;
        String[] split = mText.split("\n");
        for (int i4 = 0; i4 < split.length; i4++) {
            if (i4 == 0 && mDialogStyle == 5) {
                String[] split2 = split[i4].split("\t");
                int length = split2.length;
                if (length > 4) {
                    length = 4;
                }
                for (int i5 = 0; i5 < 4; i5++) {
                    if (length <= i5) {
                        mDialogTabField[i5].setVisibility(View.GONE);
                    } else {
                        mDialogTabField[i5].setVisibility(View.VISIBLE);
                        mTabSizes[i5] = Utils.getTextWidth(Utils.getColoredString(split2[i5]).toString(), mDialogTabField[i5].getPaint()) + ((int) Utils.scale(activity, 32.0f));
                        mDialogTabField[i5].setText(Utils.getColoredString(split2[i5]));
                    }
                }
            } else {
                if ((i4 == 1 && mDialogStyle == 5) || i4 == 0) {
                    String stringWithoutColors = Utils.getStringWithoutColors(split[i4]);
                    int i6 = mDialogStyle;
                    if (i6 == 5 || i6 == 4) {
                        stringWithoutColors = stringWithoutColors.split("\n")[0];
                    }
                    mCurrentInputtext = stringWithoutColors;
                }
                mRowsList.add(split[i4]);
            }
        }
    }

    public void loadSizes() {
        ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) mDialogListLayout.getLayoutParams();
        layoutParams.width = -2;
        mDialogListLayout.setLayoutParams(layoutParams);
        ConstraintLayout.LayoutParams layoutParams2 = (ConstraintLayout.LayoutParams) mDialogList.getLayoutParams();
        layoutParams2.width = -2;
        mDialogList.setLayoutParams(layoutParams2);
        ConstraintLayout.LayoutParams layoutParams3 = (ConstraintLayout.LayoutParams) mDialogListRecycler.getLayoutParams();
        layoutParams3.width = -2;
        mDialogListRecycler.setLayoutParams(layoutParams3);
        mDialogBody.post(new Runnable() {
            public void run() {
                if (mDialogBody.getWidth() > mDialogList.getWidth()) {
                    if (mDialogBody.getWidth() > mDialogListLayout.getWidth()) {
                        ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) mDialogListLayout.getLayoutParams();
                        layoutParams.width = 0;
                        mDialogListLayout.setLayoutParams(layoutParams);
                    }
                    ConstraintLayout.LayoutParams layoutParams2 = (ConstraintLayout.LayoutParams) mDialogList.getLayoutParams();
                    layoutParams2.width = 0;
                    mDialogList.setLayoutParams(layoutParams2);
                    ConstraintLayout.LayoutParams layoutParams3 = (ConstraintLayout.LayoutParams) mDialogListRecycler.getLayoutParams();
                    layoutParams3.width = 0;
                    mDialogListRecycler.setLayoutParams(layoutParams3);
                    mListAdapter.setMatchParent(true);
                } else {
                    mListAdapter.setMatchParent(false);
                }
                if (mDialogStyle == 5) {
                    mTabSizes = mListAdapter.mergeTabSizes(mTabSizes);
                    for (int i = 0; i < 4; i++) {
                        ViewGroup.LayoutParams layoutParams4 = mDialogTabField[i].getLayoutParams();
                        layoutParams4.width = mTabSizes[i];
                        mDialogTabField[i].setLayoutParams(layoutParams4);
                    }
                }
            }
        });
    }

    public void loadButtons() {
        mDialogButtonPositive.setVisibility(View.VISIBLE);
        mDialogButtonNegative.setVisibility(View.VISIBLE);
        mDialogButtonPositiveText.setText(mButtonPositive);
        mDialogButtonNegativeText.setText(mButtonNegative);
        mDialogButtonPositive.setOnClickListener(view -> {
            SendDialogResponse(1, mCurrentListitem, mCurrentInputtext);
        });
        if (mButtonNegative.isEmpty()) {
            mDialogButtonNegative.setVisibility(View.GONE);
        } else {
            mDialogButtonNegative.setOnClickListener(view -> {
                SendDialogResponse(0, mCurrentListitem, mCurrentInputtext);
            });
        }
        mDialogInput.addTextChangedListener(new TextWatcher() {
            public void afterTextChanged(Editable editable) {
            }

            public void beforeTextChanged(CharSequence charSequence, int i, int i2, int i3) {
            }

            public void onTextChanged(CharSequence charSequence, int i, int i2, int i3) {
                mCurrentInputtext = charSequence.toString();
            }
        });
    }

    public void setDialogStyle(int i) {
        if (i == 0) {
            mDialogTextLayout.setVisibility(View.VISIBLE);
            mDialogInputLayout.setVisibility(View.GONE);
            mDialogListLayout.setVisibility(View.GONE);
        } else if (i == 1) {
            mDialogTextLayout.setVisibility(View.VISIBLE);
            mDialogInputLayout.setVisibility(View.VISIBLE);
            mDialogInput.setInputType(524353);
            mDialogListLayout.setVisibility(View.GONE);
        } else if (i == 2) {
            mDialogTextLayout.setVisibility(View.GONE);
            mDialogInputLayout.setVisibility(View.GONE);
            mDialogListLayout.setVisibility(View.VISIBLE);
            mDialogTabListRow.setVisibility(View.GONE);
        } else if (i == 3) {
            mDialogTextLayout.setVisibility(View.VISIBLE);
            mDialogInputLayout.setVisibility(View.VISIBLE);
            mDialogInput.setInputType(524417);
            mDialogListLayout.setVisibility(View.GONE);
        } else if (i == 4) {
            mDialogTextLayout.setVisibility(View.GONE);
            mDialogInputLayout.setVisibility(View.GONE);
            mDialogListLayout.setVisibility(View.VISIBLE);
            mDialogTabListRow.setVisibility(View.GONE);
        } else if (i == 5) {
            mDialogTextLayout.setVisibility(View.GONE);
            mDialogInputLayout.setVisibility(View.GONE);
            mDialogListLayout.setVisibility(View.VISIBLE);
            mDialogTabListRow.setVisibility(View.VISIBLE);
        }
    }

    public void loadRecycler() {
        mDialogListRecycler.setLayoutManager(new LinearLayoutManager(activity, RecyclerView.VERTICAL, false));
        ArrayList<String> arrayList = mRowsList;
        int i = mDialogStyle;
        mListAdapter = new DialogAdapter(arrayList, activity, i == 4 || i == 5);
        mDialogListRecycler.setAdapter(mListAdapter);

        mListAdapter.setOnClickListener((i1, str) -> {
            if (i1 != -1) {
                mCurrentListitem = i1;
                mCurrentInputtext = str;
            }
        });
        mListAdapter.setOnDoubleClickListener(() -> SendDialogResponse(1, mCurrentListitem, mCurrentInputtext));
    }
}
