package ro.alynsampmobile.launcher.ui.adapter;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.cardview.widget.CardView;
import androidx.recyclerview.widget.RecyclerView;

import com.joom.paranoid.Obfuscate;

import java.util.List;

import ro.alynsampmobile.launcher.R;

@Obfuscate
public class ServerPickerAdapter extends RecyclerView.Adapter<ServerPickerAdapter.MyViewHolder> {

    public interface OnItemClickListener {
        void onItemClicked(String title);
    }

    private final List<DataClass> dataList;
    private final OnItemClickListener itemClickListener;

    public ServerPickerAdapter(List<DataClass> dataList, OnItemClickListener itemClickListener) {
        this.dataList = dataList;
        this.itemClickListener = itemClickListener;
    }

    @NonNull
    @Override
    public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.layout_serverpicker_item, parent, false);
        return new MyViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull MyViewHolder holder, int position) {
        DataClass data = dataList.get(position);

        holder.recImage.setImageResource(data.getDataImage());
        holder.recTitle.setText(data.getDataTitle());

        holder.recCard.setOnClickListener(v -> {
            if (itemClickListener != null) {
                itemClickListener.onItemClicked(data.getDataTitle());
            }
        });
    }

    @Override
    public int getItemCount() {
        return dataList.size();
    }

    public static class MyViewHolder extends RecyclerView.ViewHolder {
        ImageView recImage;
        TextView recTitle;
        CardView recCard;

        public MyViewHolder(@NonNull View view) {
            super(view);
            recImage = view.findViewById(R.id.recImage);
            recTitle = view.findViewById(R.id.recTitle);
            recCard = view.findViewById(R.id.recCard);
        }
    }

    // Inner DataClass
    public static class DataClass {
        private final String dataTitle;
        private final int dataImage;

        public DataClass(String dataTitle, int dataImage) {
            this.dataTitle = dataTitle;
            this.dataImage = dataImage;
        }

        public String getDataTitle() {
            return dataTitle;
        }

        public int getDataImage() {
            return dataImage;
        }
    }
}
