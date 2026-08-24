package ro.alynsampmobile.launcher.ui.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;

import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.textfield.TextInputEditText;
import com.joom.paranoid.Obfuscate;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.launcher.ui.adapter.ServerAdapter;
import ro.alynsampmobile.launcher.utils.ServerModel;

@Obfuscate
public class HostedServersFragment extends Fragment {
    private ServerAdapter serverAdapter;
    private List<ServerModel> serverModel;
    private SwipeRefreshLayout swipeRefreshLayout;

    public HostedServersFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.layout_servers, container, false);

        serverModel = new ArrayList<>();

        RecyclerView recyclerView = view.findViewById(R.id.servers_list);
        recyclerView.setLayoutManager(new LinearLayoutManager(view.getContext()));
        recyclerView.addItemDecoration(new DividerItemDecoration(recyclerView.getContext(), new LinearLayoutManager(view.getContext()).getOrientation()));

        serverAdapter = new ServerAdapter(getActivity(), serverModel, true);
        recyclerView.setAdapter(serverAdapter);

        TextInputEditText searchEditText = view.findViewById(R.id.search_edit_text);

        searchEditText.setOnEditorActionListener((v, actionId, event) -> {
            if (actionId == EditorInfo.IME_ACTION_SEARCH) {
                performSearch(Objects.requireNonNull(searchEditText.getText()).toString());
                return true;
            }
            return false;
        });

        View btnBack = view.findViewById(R.id.btnBack);
        btnBack.setOnClickListener(v -> {
            getParentFragmentManager().popBackStack();
        });

        MaterialButton add_server = view.findViewById(R.id.add_server);
        add_server.setVisibility(View.GONE);

        swipeRefreshLayout = view.findViewById(R.id.refresh);
        swipeRefreshLayout.setOnRefreshListener(this::refreshServers);

        refreshServers();

        return view;
    }

    private void performSearch(String query) {
        List<ServerModel> model = new ArrayList<>();

        if (query.isEmpty()) {
            model.addAll(serverModel);
        } else {
            query = query.toLowerCase();

            // search by host or hostname
            for (ServerModel server : serverModel) {
                if (server.server_host.toLowerCase().contains(query) || server.server_name.toLowerCase().contains(query)) {
                    model.add(server);
                }
            }
        }

        serverAdapter.serverModel = model;
        serverAdapter.notifyDataSetChanged();
    }

    public void refreshServers() {
        swipeRefreshLayout.setRefreshing(true);
        serverAdapter.refreshServers();
        swipeRefreshLayout.setRefreshing(false);
    }
}
