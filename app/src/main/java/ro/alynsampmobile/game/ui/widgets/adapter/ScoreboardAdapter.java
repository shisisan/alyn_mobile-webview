package ro.alynsampmobile.game.ui.widgets.adapter;

import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Filter;
import android.widget.Filterable;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.imageview.ShapeableImageView;
import com.google.android.material.textview.MaterialTextView;
import com.joom.paranoid.Obfuscate;

import java.util.ArrayList;
import java.util.List;

import ro.alynsampmobile.launcher.R;
import ro.alynsampmobile.game.ui.widgets.Scoreboard;

@Obfuscate
public class ScoreboardAdapter extends RecyclerView.Adapter<ScoreboardAdapter.ViewHolder> implements Filterable {
    public List<PlayerData> playerData;
    public List<PlayerData> playerDataCopy;
    private int selectedPosition = 0;
    private View selectedView;
    private Scoreboard.DoubleClickListener onDoubleClickListener;

    public ScoreboardAdapter(List<PlayerData> playerData) {
        this.playerData = playerData;
        playerDataCopy = playerData;
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        public MaterialTextView id;
        public MaterialTextView name;
        public MaterialTextView score;
        public MaterialTextView ping;

        private final View view;
        public ShapeableImageView background;

        public ViewHolder(View itemView) {
            super(itemView);
            view = itemView;
            background = itemView.findViewById(R.id.row_bg);

            id = itemView.findViewById(R.id.player_id);
            name = itemView.findViewById(R.id.player_name);
            score = itemView.findViewById(R.id.player_score);
            ping = itemView.findViewById(R.id.player_ping);
        }

        public View getView() {
            return view;
        }
    }
    
    @NonNull
    @Override
    public ScoreboardAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int i) {
        return new ViewHolder(LayoutInflater.from(parent.getContext()).inflate(R.layout.layout_scoreboard_item, parent, false));
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        PlayerData data = playerData.get(holder.getAdapterPosition());

        holder.id.setTextColor(Color.parseColor(data.getColor()));
        holder.name.setTextColor(Color.parseColor(data.getColor()));
        holder.score.setTextColor(Color.parseColor(data.getColor()));
        holder.ping.setTextColor(Color.parseColor(data.getColor()));

        holder.id.setText(String.valueOf(data.getId()));
        holder.name.setText(data.getName());
        holder.score.setText(String.valueOf(data.getScore()));
        holder.ping.setText(String.valueOf(data.getPing()));

        if (selectedPosition == holder.getAdapterPosition()) {
            selectedView = holder.background;
            holder.background.setVisibility(View.VISIBLE);
        } else {
            holder.background.setVisibility(View.GONE);
        }

        holder.getView().setOnClickListener(view -> {
            if (holder.getAdapterPosition() != selectedPosition) {
                if (selectedView != null) {
                    selectedView.setVisibility(View.GONE);
                }
                selectedPosition = holder.getAdapterPosition();
                selectedView = holder.background;
                holder.background.setVisibility(View.VISIBLE);
            } else if (onDoubleClickListener != null) {
                onDoubleClickListener.onDoubleClick(data.getId());
            }
        });
    }
    public void setOnDoubleClickListener(Scoreboard.DoubleClickListener doubleClickListener) {
        onDoubleClickListener = doubleClickListener;
    }

    @Override
    public int getItemCount() {
        return playerData.size();
    }

    @Override
    public Filter getFilter() {
        return new Filter() {
            public Filter.FilterResults performFiltering(CharSequence charSequence) {
                String input = charSequence.toString();
                List<PlayerData> newPlayersDataList = new ArrayList<>();
                if (input.isEmpty()) {
                    newPlayersDataList = playerDataCopy;
                } else {
                    for (PlayerData playersData : playerDataCopy) {
                        if (playersData.getName().toLowerCase().contains(input.toLowerCase())) {
                            newPlayersDataList.add(playersData);
                        }
                    }
                }
                Filter.FilterResults filterResults = new Filter.FilterResults();
                filterResults.values = newPlayersDataList;
                return filterResults;
            }

            public void publishResults(CharSequence charSequence, Filter.FilterResults filterResults) {
                playerData = (List) filterResults.values;
                notifyDataSetChanged();
            }
        };
    }

    public static class PlayerData {
        private int id;
        private String name;
        private int score;
        private int ping;
        String color;

        public PlayerData(int id, String name, int level, int ping, String color) {
            this.id = id;
            this.name = name;
            this.score = level;
            this.ping = ping;
            this.color = color;
        }

        public int getId() {
            return id;
        }

        public String getName() {
            return name;
        }

        public int getScore() {
            return score;
        }

        public int getPing() {
            return ping;
        }

        public String getColor() {
            return color;
        }
    }
}
