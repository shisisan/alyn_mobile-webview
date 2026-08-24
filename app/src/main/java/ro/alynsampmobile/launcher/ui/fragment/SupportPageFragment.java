package ro.alynsampmobile.launcher.ui.fragment;

import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import com.joom.paranoid.Obfuscate;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.launcher.utils.Utils;

@Obfuscate
public class SupportPageFragment extends Fragment {

    private FAQAdapter faqAdapter;
    private final List<FAQItem> faqList = new ArrayList<>();

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_support_page, container, false);

        EditText searchEditText = rootView.findViewById(R.id.faqsearch);
        RecyclerView faqRecyclerView = rootView.findViewById(R.id.faqView);
        faqRecyclerView.setLayoutManager(new LinearLayoutManager(getContext()));

        faqAdapter = new FAQAdapter();
        faqRecyclerView.setAdapter(faqAdapter);

        Utils.fetchFaqs(new Utils.FaqCallback() {
            @Override
            public void onSuccess(List<FAQItem> faqItems) {
                faqList.clear();
                faqList.addAll(faqItems);
                faqAdapter.submitList(new ArrayList<>(faqItems));
            }

            @Override
            public void onError(String errorMessage) {
                // TODO Handle error
            }
        });

        searchEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int start, int before, int count) {
                filterFaqList(charSequence.toString());
            }

            @Override
            public void afterTextChanged(Editable editable) {
            }
        });

        return rootView;
    }

    private void filterFaqList(String query) {
        if (query.isEmpty()) {
            faqAdapter.submitList(new ArrayList<>(faqList));
        } else {
            List<FAQItem> filteredList = faqList.stream()
                    .filter(item -> item.getQuestion().toLowerCase().contains(query.toLowerCase()) ||
                            item.getAnswer().toLowerCase().contains(query.toLowerCase()))
                    .collect(Collectors.toList());
            faqAdapter.submitList(filteredList);
        }
    }

    public static class FAQItem {
        private final String question;
        private final String answer;

        public FAQItem(String question, String answer) {
            this.question = question;
            this.answer = answer;
        }

        public String getQuestion() {
            return question;
        }

        public String getAnswer() {
            return answer;
        }
    }

    public static class FAQAdapter extends ListAdapter<FAQItem, FAQAdapter.FAQViewHolder> {

        protected FAQAdapter() {
            super(DIFF_CALLBACK);
        }

        private static final DiffUtil.ItemCallback<FAQItem> DIFF_CALLBACK = new DiffUtil.ItemCallback<FAQItem>() {
            @Override
            public boolean areItemsTheSame(@NonNull FAQItem oldItem, @NonNull FAQItem newItem) {
                return oldItem.getQuestion().equals(newItem.getQuestion());
            }

            @Override
            public boolean areContentsTheSame(@NonNull FAQItem oldItem, @NonNull FAQItem newItem) {
                return oldItem.getAnswer().equals(newItem.getAnswer());
            }
        };

        @NonNull
        @Override
        public FAQViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.layout_faq_item, parent, false);
            return new FAQViewHolder(itemView);
        }

        @Override
        public void onBindViewHolder(@NonNull FAQViewHolder holder, int position) {
            FAQItem currentItem = getItem(position);
            holder.bind(currentItem);
        }

        static class FAQViewHolder extends RecyclerView.ViewHolder {
            private final TextView questionTextView;
            private final TextView answerTextView;
            private final ImageView iconImageView;

            public FAQViewHolder(@NonNull View itemView) {
                super(itemView);
                questionTextView = itemView.findViewById(R.id.faq_question);
                answerTextView = itemView.findViewById(R.id.faq_answer);
                iconImageView = itemView.findViewById(R.id.faq_icon);
            }

            public void bind(FAQItem faqItem) {
                questionTextView.setText(faqItem.getQuestion());
                answerTextView.setText(faqItem.getAnswer());

                answerTextView.setVisibility(View.GONE);
                iconImageView.setImageResource(R.drawable.ic_add);

                itemView.setOnClickListener(v -> {
                    boolean isExpanded = answerTextView.getVisibility() == View.VISIBLE;
                    answerTextView.setVisibility(isExpanded ? View.GONE : View.VISIBLE);
                    iconImageView.setImageResource(isExpanded ? R.drawable.ic_add : R.drawable.ic_remove);
                });
            }
        }
    }
}
