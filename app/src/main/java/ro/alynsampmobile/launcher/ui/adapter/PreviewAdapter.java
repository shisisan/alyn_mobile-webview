package ro.alynsampmobile.launcher.ui.adapter;

import android.app.Dialog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.joom.paranoid.Obfuscate;

import java.util.List;

import ro.alynsampmobile.launcher.R;

@Obfuscate
public class PreviewAdapter extends RecyclerView.Adapter<PreviewAdapter.ViewHolder> {
    private final List<PreviewItem> itemList;

    public PreviewAdapter(List<PreviewItem> itemList) {
        this.itemList = itemList;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.layout_preview_item, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        PreviewItem item = itemList.get(position);

        // Load image from URL using Glide
        Glide.with(holder.imageView.getContext())
                .load(item.getImageUrl())
                //.placeholder(R.drawable.placeholder)
                //.error(R.drawable.error_image)
                .into(holder.imageView);

        holder.title.setText(item.getTitle());
        holder.subtitle.setText(item.getSubtitle());

        holder.itemView.setOnClickListener(v -> {
            // todo show image
        });
    }

    @Override
    public int getItemCount() {
        return itemList.size();
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        ImageView imageView;
        TextView title, subtitle;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            imageView = itemView.findViewById(R.id.imageView);
            title = itemView.findViewById(R.id.title);
            subtitle = itemView.findViewById(R.id.subtitle);
        }
    }

    public static class PreviewItem {
        private String imageUrl;
        private String title;
        private String subtitle;

        public PreviewItem(String imageUrl, String title, String subtitle) {
            this.imageUrl = imageUrl;
            this.title = title;
            this.subtitle = subtitle;
        }

        public String getImageUrl() {
            return imageUrl;
        }

        public String getTitle() {
            return title;
        }

        public String getSubtitle() {
            return subtitle;
        }
    }
}
