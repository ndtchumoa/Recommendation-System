"""
Amazon Sales Dataset — Item-Based Collaborative Filtering (IBCF)
=================================================================
Input  : output/interaction_matrix.pkl  (product_id × user_id, pd.DataFrame)
         output/item_mapping.pkl        (dict {product_id: product_name})
Output : Top-N product recommendations given a query product_id

Thuật toán:
    1. Chuẩn hoá ma trận (Mean-centering theo từng item)  ← giảm bias đánh giá
    2. Tính Cosine Similarity giữa tất cả các cặp item     ← similarity matrix
    3. getRecommendations(product_id, top_n)
         └─ Tra cứu hàng similarity → sắp xếp giảm dần → trả về top_n item
"""

import logging
import pickle
import sys
from pathlib import Path

import numpy as np
import pandas as pd
from sklearn.metrics.pairwise import cosine_similarity

# ─────────────────────────────────────────────────────────────────────────────
# Logging
# ─────────────────────────────────────────────────────────────────────────────
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%H:%M:%S",
    handlers=[logging.StreamHandler(sys.stdout)],
)
log = logging.getLogger(__name__)


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 5: Load dữ liệu đã xử lý
# ─────────────────────────────────────────────────────────────────────────────
def loadProcessedData(output_dir: str = "output") -> tuple[pd.DataFrame, dict]:
    out = Path(output_dir)
    matrix_path  = out / "interaction_matrix.pkl"
    mapping_path = out / "item_mapping.pkl"

    for p in [matrix_path, mapping_path]:
        if not p.exists():
            raise FileNotFoundError(
                f"[loadProcessedData] Không tìm thấy: '{p}'. "
                "Hãy chạy data_pipeline.py trước."
            )

    with open(matrix_path, "rb") as f:
        matrix: pd.DataFrame = pickle.load(f)
    with open(mapping_path, "rb") as f:
        mapping: dict = pickle.load(f)

    log.info("[loadProcessedData] Đã load: %d sản phẩm × %d users.", *matrix.shape)
    return matrix, mapping


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 6: Chuẩn hoá ma trận — Mean-centering theo từng Item
# ─────────────────────────────────────────────────────────────────────────────
def normalizeMatrix(matrix: pd.DataFrame) -> pd.DataFrame:
    """
    Mean-center mỗi item (hàng) chỉ dựa trên các ô đã được đánh giá (≠ 0).
    *Cập nhật: Chỉ trừ mean nếu điểm đánh giá của item có sự biến thiên (std > 0). 
    Nếu item toàn 5 sao, giữ nguyên để không bị biến thành vector 0.
    """
    matrix_norm = matrix.copy().astype(float)
    for idx in matrix_norm.index:
        row = matrix_norm.loc[idx]
        rated_mask = row != 0
        if rated_mask.sum() > 0:
            std_val = row[rated_mask].std()
            # Tránh lỗi zero-out: Chỉ trừ mean nếu mảng có độ lệch chuẩn > 0
            if pd.notna(std_val) and std_val > 0:
                mean_val = row[rated_mask].mean()
                matrix_norm.loc[idx, rated_mask] -= mean_val
                
    log.info("[normalizeMatrix] Mean-centering hoàn tất.")
    return matrix_norm


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 7: Xây dựng ma trận tương đồng — Cosine Similarity
# ─────────────────────────────────────────────────────────────────────────────
def buildSimilarityMatrix(matrix_norm: pd.DataFrame) -> pd.DataFrame:
    values = matrix_norm.values  # (n_items × n_users)
    sim_matrix = cosine_similarity(values)

    similarity_df = pd.DataFrame(
        sim_matrix,
        index=matrix_norm.index,
        columns=matrix_norm.index,
    )

    log.info(
        "[buildSimilarityMatrix] Ma trận tương đồng: %d × %d. "
        "Similarity trung bình (off-diag): %.4f",
        *similarity_df.shape,
        (sim_matrix.sum() - np.trace(sim_matrix))
        / (sim_matrix.size - len(sim_matrix)) if sim_matrix.size > len(sim_matrix) else 0.0,
    )
    return similarity_df


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 8: Gợi ý sản phẩm — getRecommendations()
# ─────────────────────────────────────────────────────────────────────────────
def getRecommendations(
    product_id: str,
    similarity_df: pd.DataFrame,
    item_mapping: dict,
    top_n: int = 5,
) -> pd.DataFrame:
    if product_id not in similarity_df.index:
        raise KeyError(f"[getRecommendations] product_id '{product_id}' không tồn tại.")

    # Lấy hàng similarity, loại bỏ chính nó
    sim_scores = similarity_df.loc[product_id].drop(index=product_id)
    
    # CHỈ LẤY CÁC ITEM CÓ ĐỘ TƯƠNG ĐỒNG > 0
    sim_scores = sim_scores[sim_scores > 0].sort_values(ascending=False).head(top_n)

    if sim_scores.empty:
        return pd.DataFrame() # Trả về DF rỗng nếu không có tương đồng

    results = pd.DataFrame({
        "rank":         range(1, len(sim_scores) + 1),
        "product_id":   sim_scores.index,
        "product_name": sim_scores.index.map(lambda pid: item_mapping.get(pid, "Unknown")),
        "similarity":   sim_scores.values.round(4),
    }).reset_index(drop=True)

    query_name = item_mapping.get(product_id, product_id)
    log.info("[getRecommendations] Query: '%s' → %d gợi ý.", query_name[:60], len(results))
    return results


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 9: Dự đoán rating — predictRating()
# ─────────────────────────────────────────────────────────────────────────────
def predictRating(
    user_id: str,
    product_id: str,
    interaction_matrix: pd.DataFrame,
    similarity_df: pd.DataFrame,
    k: int = 10,
) -> float:
    if user_id not in interaction_matrix.columns:
        return float(interaction_matrix.values[interaction_matrix.values != 0].mean())

    if product_id not in interaction_matrix.index:
        raise KeyError(f"[predictRating] product_id '{product_id}' không tồn tại.")

    user_ratings = interaction_matrix[user_id]
    rated_items  = user_ratings[user_ratings != 0].drop(index=product_id, errors="ignore")

    def item_mean(iid):
        row = interaction_matrix.loc[iid]
        nz  = row[row != 0]
        return float(nz.mean()) if not nz.empty else 0.0

    query_mean = item_mean(product_id)

    if rated_items.empty:
        return query_mean

    sim_scores = similarity_df.loc[product_id, rated_items.index]
    top_k = sim_scores.nlargest(k)
    top_k = top_k[top_k > 0]

    if top_k.empty:
        return query_mean

    numerator   = sum(top_k[j] * (rated_items[j] - item_mean(j)) for j in top_k.index)
    denominator = top_k.abs().sum()

    if denominator == 0:
        return query_mean

    predicted = query_mean + numerator / denominator
    return float(np.clip(predicted, 1.0, 5.0))


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 10: Đánh giá mô hình — Leave-One-Out Cross Validation
# ─────────────────────────────────────────────────────────────────────────────
def evaluateModel(
    interaction_matrix: pd.DataFrame,
    similarity_df: pd.DataFrame,
    item_mapping: dict,
    top_n: int = 5,
    min_rated: int = 2,
) -> dict:
    log.info("[evaluateModel] Bắt đầu LOO-CV với top_n=%d ...", top_n)

    hits        = 0
    total_users = 0
    rmse_errors = []
    rng = np.random.default_rng(seed=42)

    for user_id in interaction_matrix.columns:
        user_row     = interaction_matrix[user_id]
        rated_items  = user_row[user_row != 0].index.tolist()

        if len(rated_items) < min_rated:
            continue

        test_item = rng.choice(rated_items)
        actual_rating = user_row[test_item]

        pred_rating = predictRating(user_id, test_item, interaction_matrix, similarity_df, k=10)
        rmse_errors.append((pred_rating - actual_rating) ** 2)

        remaining_items = [i for i in rated_items if i != test_item]
        if not remaining_items:
            continue

        query_item = user_row[remaining_items].idxmax()

        try:
            recs = getRecommendations(query_item, similarity_df, item_mapping, top_n=top_n)
            rec_ids = set(recs["product_id"].tolist()) if not recs.empty else set()
        except KeyError:
            continue

        if test_item in rec_ids:
            hits += 1

        total_users += 1

    if total_users == 0:
        log.warning("[evaluateModel] Không đủ users để đánh giá.")
        return {}

    precision_at_k = hits / (total_users * top_n)
    recall_at_k    = hits / total_users
    hit_rate       = hits / total_users
    rmse           = float(np.sqrt(np.mean(rmse_errors))) if rmse_errors else None

    metrics = {
        "precision_at_k":    round(precision_at_k, 4),
        "recall_at_k":       round(recall_at_k, 4),
        "hit_rate":          round(hit_rate, 4),
        "rmse":              round(rmse, 4) if rmse is not None else None,
        "n_users_evaluated": total_users,
        "top_n":             top_n,
    }

    log.info("[evaluateModel] Kết quả:")
    log.info("  %-20s %d  (trên %d users)", "Top-N (K):", top_n, total_users)
    log.info("  %-20s %.4f", "Precision@K:", precision_at_k)
    log.info("  %-20s %.4f", "Recall@K / Hit Rate:", hit_rate)
    log.info("  %-20s %s", "RMSE:", f"{rmse:.4f}" if rmse is not None else "N/A")
    return metrics


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 11: Lưu mô hình đã huấn luyện
# ─────────────────────────────────────────────────────────────────────────────
def saveModel(similarity_df: pd.DataFrame, output_dir: str = "output") -> None:
    out  = Path(output_dir)
    out.mkdir(parents=True, exist_ok=True)
    dest = out / "similarity_matrix.pkl"

    with open(dest, "wb") as f:
        pickle.dump(similarity_df, f, protocol=pickle.HIGHEST_PROTOCOL)

    size_kb = dest.stat().st_size / 1024
    log.info("[saveModel] Đã lưu similarity_matrix.pkl (%.1f KB) → %s", size_kb, dest.resolve())


# ─────────────────────────────────────────────────────────────────────────────
# ENTRY POINT
# ─────────────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    OUTPUT_DIR = "output"
    TOP_N      = 5

    interaction_matrix, item_mapping = loadProcessedData(OUTPUT_DIR)
    matrix_norm = normalizeMatrix(interaction_matrix)
    similarity_df = buildSimilarityMatrix(matrix_norm)
    saveModel(similarity_df, OUTPUT_DIR)

    demo_products = list(item_mapping.keys())[:3]

    print("\n" + "=" * 70)
    print("  DEMO — Item-Based Collaborative Filtering Recommendations")
    print("=" * 70)

    for pid in demo_products:
        print(f"\nQuery: [{pid}] {item_mapping[pid][:65]}...")
        print("-" * 70)
        recs = getRecommendations(pid, similarity_df, item_mapping, top_n=TOP_N)
        
        if recs.empty:
            print("  (Không có dữ liệu mua chung để tạo gợi ý)")
        else:
            for _, row in recs.iterrows():
                print(f"  #{int(row['rank'])}  sim={row['similarity']:.4f}  {row['product_name'][:58]}...")

    print("\n" + "=" * 70)
    print("  ĐÁNH GIÁ MÔ HÌNH — Leave-One-Out Cross Validation")
    print("=" * 70)
    
    metrics = evaluateModel(interaction_matrix, similarity_df, item_mapping, top_n=TOP_N)

    if metrics:
        print(f"\n  Số users được đánh giá  : {metrics['n_users_evaluated']}")
        print(f"  Top-N (K)               : {metrics['top_n']}")
        print(f"  Precision@{TOP_N}            : {metrics['precision_at_k']:.4f}  ({metrics['precision_at_k']*100:.2f}%)")
        print(f"  Hit Rate / Recall@{TOP_N}   : {metrics['hit_rate']:.4f}  ({metrics['hit_rate']*100:.2f}%)")
        
        rmse_val = metrics.get('rmse')
        rmse_str = f"{rmse_val:.4f}" if rmse_val is not None else "N/A"
        print(f"  RMSE                    : {rmse_str}")
    print("=" * 70)