package ro.alynsampmobile.launcher.ui.fragment;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.joom.paranoid.Obfuscate;

import java.util.ArrayList;
import java.util.List;

import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.launcher.databinding.FragmentInformationPageBinding;
import ro.alynsampmobile.launcher.ui.adapter.PreviewAdapter;
import ro.alynsampmobile.launcher.utils.Utils;

@Obfuscate
public class InformationPageFragment extends Fragment {
    private FragmentInformationPageBinding binding;
    private RecyclerView recyclerView;
    private PreviewAdapter adapter;
    private List<PreviewAdapter.PreviewItem> itemList = new ArrayList<>();

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        binding = FragmentInformationPageBinding.inflate(inflater, container, false);
        View view = binding.getRoot();

        ImageView discord = binding.iconDiscord;
        ImageView website = binding.iconWebsite;
        ImageView github = binding.iconGithub;
        ImageView changelogs = binding.iconChangelogs;

        discord.setOnClickListener(v -> openUrl(Utils.discord));
        website.setOnClickListener(v -> openUrl(Utils.web));
        github.setOnClickListener(v -> openUrl(Utils.github));
        changelogs.setOnClickListener(v -> new AlertDialog.Builder(requireActivity()).setTitle("Changelog:").setMessage(Utils.getChangelog()).setPositiveButton("OK", (dialog, which) -> dialog.dismiss()).show());

        recyclerView = view.findViewById(R.id.recyclerView);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext(), LinearLayoutManager.HORIZONTAL, false));
        recyclerView.setHasFixedSize(true);

        Utils.fetchPreviews(new Utils.PreviewCallback() {
            @Override
            public void onSuccess(List<PreviewAdapter.PreviewItem> previews) {
                itemList = previews;
                adapter = new PreviewAdapter(itemList);
                recyclerView.setAdapter(adapter);
            }

            @Override
            public void onError(Exception e) {
                e.printStackTrace();
            }
        });

        return view;
    }

    private void openUrl(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        startActivity(intent);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }
}