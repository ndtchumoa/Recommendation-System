"""
Amazon Sales Dataset — Data Engineering Pipeline
=================================================
Dataset  : Amazon Sales Dataset (amazon.csv)
Tác giả  : Data Engineering Team
amazon.csv
    │
    ▼ loadData()
    ├─ usecols → chỉ đọc 4 cột [Bước 1]
    ├─ to_numeric + dropna → làm sạch cơ bản [Bước 2a, 2b]
    └─ str.split + explode → chuẩn hóa 1 hàng/user [Bước 1]
    │
    ▼ buildMatrix()
    ├─ filter ngưỡng ≥ 10 → loại sản phẩm thưa [Bước 2c]
    ├─ groupby + mean → dedup [Bước 2d]
    ├─ pivot → Interaction Matrix 91×698 [Bước 3]
    ├─ fillna(0) → điền ô trống [Bước 3]
    └─ to_dict → item_mapping [Bước 3]
    │
    ▼ saveData()
    ├─ interaction_matrix.pkl [Bước 4]
    ├─ item_mapping.pkl [Bước 4]
    └─ matrix_metadata.pkl [Bước 4]
"""

import pickle
import pandas as pd
from pathlib import Path
from scipy.sparse import csr_matrix


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 1 & 2 & 3 (Đọc → Làm sạch → Reshape)
# ─────────────────────────────────────────────────────────────────────────────

def loadData(filepath: str) -> pd.DataFrame:
    """
    Đọc và xử lý bước đầu file CSV Amazon Sales Dataset.

    Quy trình bên trong:
        1. Chỉ đọc 4 cột cần thiết để tiết kiệm RAM (usecols).
        2. Ép cột `rating` sang float; lỗi → NaN.
        3. Xóa hàng có NaN ở 3 cột cốt lõi.
        4. Tách chuỗi user_id gộp → mỗi hàng chỉ chứa 1 user.

    Parameters
    ----------
    filepath : str
        Đường dẫn tới file CSV.

    Returns
    -------
    pd.DataFrame
        DataFrame sạch với các cột: user_id, product_id, rating, product_name.
        Mỗi hàng đảm bảo: 1 user_id + 1 product_id + 1 rating.
    """
    print(f"[loadData] Bắt đầu đọc file: {filepath}")

    try:
        # ── Bước 1a: Đọc CSV, chỉ lấy 4 cột cần thiết ───────────────────────
        df = pd.read_csv(
            filepath,
            usecols=["user_id", "product_id", "rating", "product_name"],
            dtype=str,          # đọc tất cả là string trước, sẽ ép kiểu sau
        )
        print(f"[loadData] Đọc thô xong — {len(df):,} dòng, {df.shape[1]} cột.")

        # ── Bước 2a: Ép kiểu cột rating sang float ────────────────────────────
        # strip() loại bỏ khoảng trắng thừa; errors='coerce' → NaN nếu không hợp lệ
        df["rating"] = pd.to_numeric(df["rating"].str.strip(), errors="coerce")
        print(f"[loadData] Ép kiểu 'rating' sang float hoàn tất.")

        # ── Bước 2b: Xóa hàng có NaN ở 3 cột cốt lõi ────────────────────────
        before = len(df)
        df.dropna(subset=["user_id", "product_id", "rating"], inplace=True)
        after = len(df)
        print(f"[loadData] Drop NaN: {before - after:,} hàng bị xóa → còn {after:,} hàng.")

        # ── Bước 4: Tách user_id gộp (1 chuỗi nhiều ID → nhiều hàng đơn) ────
        # Ví dụ: "userA,userB,userC" → 3 hàng riêng biệt, mỗi hàng 1 user_id
        df["user_id"] = df["user_id"].str.split(",")   # tách thành list
        df = df.explode("user_id")                     # mỗi phần tử list → 1 hàng
        df["user_id"] = df["user_id"].str.strip()      # xóa khoảng trắng thừa sau khi tách

        # Xóa user_id rỗng có thể phát sinh sau khi tách (ví dụ: "userA,,userB")
        df = df[df["user_id"] != ""]
        df.reset_index(drop=True, inplace=True)

        print(f"[loadData] Sau khi explode user_id: {len(df):,} hàng "
              f"(mỗi hàng = 1 user + 1 product + 1 rating).")

    except FileNotFoundError:
        print(f"[loadData] LỖI: Không tìm thấy file tại '{filepath}'. "
              "Kiểm tra lại đường dẫn.")
        raise
    except Exception as exc:
        print(f"[loadData] LỖI không xác định: {exc}")
        raise

    print("[loadData] Hoàn tất. Trả về DataFrame.\n")
    return df


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 2c & 3: Làm sạch nâng cao + Biến đổi cấu trúc
# ─────────────────────────────────────────────────────────────────────────────

def buildMatrix(df: pd.DataFrame, min_ratings: int = 10):
    """
    Từ DataFrame thô đã load, thực hiện:
        - Lọc ngưỡng: chỉ giữ product có >= min_ratings lượt đánh giá.
        - Xử lý trùng lặp: lấy trung bình rating nếu 1 user rate 1 sản phẩm nhiều lần.
        - Tạo ma trận tương tác (Interaction Matrix): product_id × user_id.
        - Tạo bảng ánh xạ product_id → product_name.
        - Tạo bảng metadata cho ma trận.

    Parameters
    ----------
    df          : pd.DataFrame — output của loadData().
    min_ratings : int          — ngưỡng tối thiểu số lượt đánh giá của một sản phẩm.

    Returns
    -------
    interaction_matrix : pd.DataFrame — ma trận (product_id × user_id), NaN → 0.
    item_mapping       : dict          — {product_id: product_name}.
    matrix_metadata    : dict          — { "product_ids": [...], "user_ids": [...] }.
    """
    print(f"[buildMatrix] Bắt đầu xây dựng ma trận. min_ratings={min_ratings}")

    # ── Bước 2c: Lọc ngưỡng — chỉ giữ product có đủ lượt đánh giá ───────────
    rating_counts = df.groupby("product_id")["rating"].count()
    valid_products = rating_counts[rating_counts >= min_ratings].index
    before = df["product_id"].nunique()
    df = df[df["product_id"].isin(valid_products)]
    after = df["product_id"].nunique()
    print(f"[buildMatrix] Lọc ngưỡng (>= {min_ratings} ratings): "
          f"{before} → {after} sản phẩm còn lại.")

    # ── Bước 2d: Xử lý trùng lặp — 1 user rate 1 product nhiều lần → mean ───
    df = (
        df.groupby(["product_id", "user_id"], as_index=False)
          .agg(rating=("rating", "mean"),
               product_name=("product_name", "first"))   # giữ tên sản phẩm
    )
    print(f"[buildMatrix] Sau deduplication: {len(df):,} cặp (product, user) duy nhất.")

    # ── Bước 3a: Tạo bảng ánh xạ product_id → product_name ──────────────────
    item_mapping: dict = (
        df[["product_id", "product_name"]]
        .drop_duplicates("product_id")
        .set_index("product_id")["product_name"]
        .to_dict()
    )
    print(f"[buildMatrix] Bảng ánh xạ: {len(item_mapping):,} sản phẩm.")

    # ── Bước 3b: Pivot — tạo ma trận tương tác (Item-Based CF) ───────────────
    # Hàng = product_id | Cột = user_id | Giá trị = rating
    interaction_df = df.pivot(
        index="product_id",
        columns="user_id",
        values="rating",
    ).fillna(0)
    
    # Ép kiểu ma trận tương tác thành định dạng nén thưa (Sparse)
    interaction_matrix_sparse = csr_matrix(interaction_df.values)
    
    # Vì Sparse Matrix bị mất index/columns của Pandas, 
    # ta phải lưu metadata lại để team Algorithm biết hàng/cột nào là của ai
    matrix_metadata = {
        "product_ids": interaction_df.index.tolist(),
        "user_ids": interaction_df.columns.tolist()
    }

    print(f"[buildMatrix] Ma trận tương tác (Sparse): "
          f"{interaction_matrix_sparse.shape[0]} sản phẩm × {interaction_matrix_sparse.shape[1]} users.")
    print("[buildMatrix] Hoàn tất.\n")

    return interaction_matrix_sparse, item_mapping, matrix_metadata


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 4: Xuất và Ghi file
# ─────────────────────────────────────────────────────────────────────────────

def saveData(matrix, mapping: dict, metadata: dict, output_dir: str) -> None:
    out = Path(output_dir)
    out.mkdir(parents=True, exist_ok=True)

    matrix_path  = out / "interaction_matrix.pkl"
    mapping_path = out / "item_mapping.pkl"
    metadata_path = out / "matrix_metadata.pkl" # File mới

    with open(matrix_path, "wb") as f:
        pickle.dump(matrix, f, protocol=pickle.HIGHEST_PROTOCOL)

    with open(mapping_path, "wb") as f:
        pickle.dump(mapping, f, protocol=pickle.HIGHEST_PROTOCOL)
        
    with open(metadata_path, "wb") as f:
        pickle.dump(metadata, f, protocol=pickle.HIGHEST_PROTOCOL)

    print(f"[saveData] Đã lưu thành công 3 file (Matrix, Mapping, Metadata) vào '{out.resolve()}'.\n")

# ─────────────────────────────────────────────────────────────────────────────
# ENTRY POINT — chạy toàn bộ pipeline
# ─────────────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    CSV_PATH   = "amazon.csv"   # ← chỉ khai báo 1 lần duy nhất ở đây
    OUTPUT_DIR = "output"
    MIN_RATINGS = 10

    # Bước 1 + 2a + 2b + 4 (explode): đọc và làm sạch cơ bản
    df_raw = loadData(CSV_PATH)

    # Bước 2c + 2d + 3: lọc, dedup, pivot, mapping
    interaction_matrix, item_mapping, matrix_metadata = buildMatrix(df_raw, min_ratings=MIN_RATINGS)

    # Bước 4: xuất file
    saveData(interaction_matrix, item_mapping, matrix_metadata, OUTPUT_DIR)

    # ── Tóm tắt kết quả ──────────────────────────────────────────────────────
    print("=" * 60)
    print("Pipeline hoàn tất!")
    print(f"  Ma trận : {interaction_matrix.shape[0]} sản phẩm × "
          f"{interaction_matrix.shape[1]} users")
    print(f"  Mapping : {len(item_mapping)} sản phẩm")
    print(f"  Metadata: {len(matrix_metadata['product_ids'])} sản phẩm, {len(matrix_metadata['user_ids'])} người dùng")
    print(f"  Output  : {Path(OUTPUT_DIR).resolve()}/")
    print("=" * 60)