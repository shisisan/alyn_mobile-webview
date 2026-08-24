package ro.alynsampmobile.launcher.ui.fragment;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.joom.paranoid.Obfuscate;

import java.util.ArrayList;
import java.util.List;

import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.launcher.databinding.FragmentServersPageBinding;
import ro.alynsampmobile.launcher.ui.adapter.ServerPickerAdapter;

@Obfuscate
public class ServersPageFragment extends Fragment {

    private FragmentServersPageBinding binding;
    private ServerPickerAdapter menuAdapter;
    private List<ServerPickerAdapter.DataClass> menuList;
    private long lastClickTime = 0;
    private static final long CLICK_DELAY = 500;

    private final FragmentManager.OnBackStackChangedListener backStackListener = () -> {
        if (isAdded() && getParentFragmentManager().getBackStackEntryCount() == 0) {
            showMenuOptions();
        }
    };

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        binding = FragmentServersPageBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        binding.serversView.setLayoutManager(new LinearLayoutManager(getContext()));
        binding.serversView.setHasFixedSize(true);

        menuList = getMenuList();
        menuAdapter = getMenuAdapter();
        binding.serversView.setAdapter(menuAdapter);

        requireActivity().getSharedPreferences("samp_settings", Context.MODE_PRIVATE).edit().putBoolean("offline_mode", false).apply();
    }

    @Override
    public void onStart() {
        super.onStart();
        if (isAdded()) {
            getParentFragmentManager().addOnBackStackChangedListener(backStackListener);
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        if (isAdded()) {
            getParentFragmentManager().removeOnBackStackChangedListener(backStackListener);
        }
    }

    private List<ServerPickerAdapter.DataClass> getMenuList() {
        if (menuList == null) {
            menuList = new ArrayList<>();
            menuList.add(new ServerPickerAdapter.DataClass(getString(R.string.partners_servers), R.drawable.partners));
            menuList.add(new ServerPickerAdapter.DataClass(getString(R.string.favorite_servers), R.drawable.favorite));
            menuList.add(new ServerPickerAdapter.DataClass(getString(R.string.offline_mode), R.drawable.offline));
        }
        return menuList;
    }

    private ServerPickerAdapter getMenuAdapter() {
        if (menuAdapter == null) {
            menuAdapter = new ServerPickerAdapter(getMenuList(), this::handleMenuClick);
        }
        return menuAdapter;
    }

    private void handleMenuClick(String title) {
        if (System.currentTimeMillis() - lastClickTime < CLICK_DELAY) {
            return;
        }
        lastClickTime = System.currentTimeMillis();

        Fragment selectedFragment = null;

        if (title.equals(getString(R.string.partners_servers))) {
            selectedFragment = new HostedServersFragment();
        } else if (title.equals(getString(R.string.favorite_servers))) {
            selectedFragment = new FavoriteServersFragment();
        } else if (title.equals(getString(R.string.offline_mode))) {
            AlertDialog alertDialog = new AlertDialog.Builder(getContext())
                    .setTitle(R.string.offline_mode)
                    .setMessage("Are you sure you want to start the game in offline mode?")
                    .setPositiveButton("Yes", (dialog, which) -> {
                        requireActivity().getSharedPreferences("samp_settings", Context.MODE_PRIVATE).edit().putBoolean("offline_mode", true).apply();
                        // start game
                        try {
                            Intent intent = new Intent(requireActivity(), ro.alynsampmobile.game.SAMP.class);
                            intent.putExtra("extra_check", "alynsampmobile1337");
                            requireActivity().startActivity(intent);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    })
                    .setNegativeButton("No", null)
                    .create();
            alertDialog.show();
        }

        if (selectedFragment != null && !selectedFragment.getClass().equals(getChildFragmentManager().findFragmentById(R.id.fragment_container))) {
            binding.serversView.setVisibility(View.GONE);

            getParentFragmentManager()
                    .beginTransaction()
                    .replace(R.id.fragment_container, selectedFragment)
                    .setReorderingAllowed(true)
                    .addToBackStack(null)
                    .commit();
        }
    }

    private void showMenuOptions() {
        binding.serversView.setVisibility(View.VISIBLE);
        binding.serversView.setAdapter(getMenuAdapter());
    }
}