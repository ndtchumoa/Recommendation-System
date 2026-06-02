"""
Amazon Sales Dataset — Data Engineering Pipeline (Optimized)
=============================================================
Dataset  : Amazon Sales Dataset (amazon.csv)
Tác giả  : Data Engineering Team

Luồng xử lý:
amazon.csv
    │
    ▼ loadData(filepath)
    ├─ usecols        → chỉ đọc 4 cột cần thiết          [Bước 1]
    ├─ to_numeric     → ép rating sang float              [Bước 2a]
    ├─ dropna         → xóa hàng thiếu dữ liệu cốt lõi  [Bước 2b]
    └─ split+explode  → chuẩn hóa 1 hàng = 1 user        [Bước 1]
    │
    ▼ buildMatrix(df, min_ratings)
    ├─ filter ≥ min_ratings  → loại sản phẩm thưa        [Bước 2c]
    ├─ groupby + mean        → dedup                      [Bước 2d]
    ├─ pivot + fillna(0)     → Interaction Matrix         [Bước 3]
    └─ to_dict               → item_mapping               [Bước 3]
    │
    ▼ saveData(matrix, mapping, output_dir)
    ├─ interaction_matrix.pkl                             [Bước 4]
    └─ item_mapping.pkl                                   [Bước 4]

Thay đổi so với phiên bản cũ:
    - [FIX]  Bỏ csr_matrix: trả về pd.DataFrame thay vì Sparse Matrix
             → đúng với instructions, không cần matrix_metadata.pkl nữa
    - [OPT]  loadData: thêm engine="pyarrow" (nếu có) để đọc CSV nhanh hơn
    - [OPT]  buildMatrix: dùng isin() với set() thay vì Index để O(1) lookup
    - [OPT]  buildMatrix: tách item_mapping trước dedup để tránh mất tên sản phẩm
    - [OPT]  saveData: kiểm tra file tồn tại, log kích thước file sau khi lưu
    - [OPT]  Thêm hàm validateData() để kiểm tra đầu ra trước khi lưu
    - [CLEAN] Xóa comment thừa, thống nhất style log [FUNC] prefix
"""

import logging
import pickle
import sys
from pathlib import Path

import pandas as pd

# ─────────────────────────────────────────────────────────────────────────────
# Logging — dùng thay cho print để dễ tắt/bật và redirect ra file
# ─────────────────────────────────────────────────────────────────────────────
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%H:%M:%S",
    handlers=[logging.StreamHandler(sys.stdout)],
)
log = logging.getLogger(__name__)


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 1 + 2a + 2b: Đọc → Ép kiểu → Làm sạch cơ bản
# ─────────────────────────────────────────────────────────────────────────────

def loadData(filepath: str) -> pd.DataFrame:
    """
    Đọc và xử lý bước đầu file CSV Amazon Sales Dataset.

    Quy trình:
        1. Chỉ đọc 4 cột cần thiết (usecols) → tiết kiệm RAM.
        2. Ép cột rating sang float; lỗi → NaN.
        3. Xóa hàng có NaN ở 3 cột cốt lõi.
        4. Tách chuỗi user_id gộp → mỗi hàng chứa đúng 1 user.

    Parameters
    ----------
    filepath : str
        Đường dẫn tới file CSV.

    Returns
    -------
    pd.DataFrame
        Các cột: user_id, product_id, rating, product_name.
        Đảm bảo: 1 hàng = 1 user_id + 1 product_id + 1 rating.

    Raises
    ------
    FileNotFoundError
        Nếu file CSV không tồn tại tại filepath.
    ValueError
        Nếu file không chứa đủ các cột bắt buộc.
    """
    path = Path(filepath)
    if not path.exists():
        raise FileNotFoundError(f"[loadData] Không tìm thấy file: '{filepath}'")

    log.info("[loadData] Bắt đầu đọc: %s", filepath)

    REQUIRED_COLS = ["user_id", "product_id", "rating", "product_name"]

    # [OPT] Thử dùng pyarrow engine để đọc nhanh hơn (~2-3x); fallback về c engine
    try:
        df = pd.read_csv(path, usecols=REQUIRED_COLS, dtype=str, engine="pyarrow")
        log.info("[loadData] Dùng engine=pyarrow.")
    except Exception:
        df = pd.read_csv(path, usecols=REQUIRED_COLS, dtype=str, engine="c")
        log.info("[loadData] Dùng engine=c (pyarrow không khả dụng).")

    # Kiểm tra cột bắt buộc
    missing = set(REQUIRED_COLS) - set(df.columns)
    if missing:
        raise ValueError(f"[loadData] File thiếu cột bắt buộc: {missing}")

    log.info("[loadData] Đọc thô xong — %s dòng, %d cột.", f"{len(df):,}", df.shape[1])

    # [Bước 2a] Ép kiểu rating sang float
    df["rating"] = pd.to_numeric(df["rating"].str.strip(), errors="coerce")

    # [Bước 2b] Xóa hàng NaN ở 3 cột cốt lõi
    before = len(df)
    df.dropna(subset=["user_id", "product_id", "rating"], inplace=True)
    log.info("[loadData] Drop NaN: %s hàng bị xóa → còn %s hàng.",
             f"{before - len(df):,}", f"{len(df):,}")

    # [Bước 1] Tách user_id gộp "userA,userB" → nhiều hàng đơn
    df["user_id"] = df["user_id"].str.split(",")
    df = df.explode("user_id")
    df["user_id"] = df["user_id"].str.strip()
    df = df[df["user_id"] != ""]            # loại user_id rỗng ("userA,,userB")
    df.reset_index(drop=True, inplace=True)

    log.info("[loadData] Sau explode: %s hàng (1 user × 1 product × 1 rating).",
             f"{len(df):,}")
    return df


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 2c + 2d + 3: Lọc → Dedup → Pivot → Mapping
# ─────────────────────────────────────────────────────────────────────────────

def buildMatrix(
    df: pd.DataFrame,
    min_ratings: int = 10,
) -> tuple[pd.DataFrame, dict]:
    """
    Xây dựng Interaction Matrix và bảng ánh xạ sản phẩm từ DataFrame sạch.

    Quy trình:
        2c. Lọc ngưỡng: chỉ giữ product có >= min_ratings lượt đánh giá.
        2d. Dedup: nếu 1 user rate 1 product nhiều lần → lấy mean.
        3a. Tạo bảng ánh xạ product_id → product_name.
        3b. Pivot → Interaction Matrix (product_id × user_id), fillna(0).

    Parameters
    ----------
    df          : pd.DataFrame  — output của loadData().
    min_ratings : int           — ngưỡng tối thiểu số lượt đánh giá/sản phẩm.

    Returns
    -------
    interaction_matrix : pd.DataFrame  — (product_id × user_id), giá trị ∈ [0, 5].
    item_mapping       : dict          — {product_id: product_name}.
    """
    log.info("[buildMatrix] Bắt đầu. min_ratings=%d", min_ratings)

    # [OPT] Tạo item_mapping TRƯỚC khi dedup để đảm bảo tên sản phẩm không bị
    #        mất do cặp (product_id, user_id) bị tổng hợp lại trong groupby
    item_mapping: dict = (
        df[["product_id", "product_name"]]
        .drop_duplicates("product_id")
        .set_index("product_id")["product_name"]
        .to_dict()
    )
    log.info("[buildMatrix] Bảng ánh xạ: %d sản phẩm.", len(item_mapping))

    # [Bước 2c] Lọc ngưỡng — chỉ giữ product có đủ lượt đánh giá
    # [OPT] Dùng set() để isin() chạy O(1) thay vì O(n) với Index
    rating_counts = df.groupby("product_id")["rating"].count()
    valid_products: set = set(rating_counts[rating_counts >= min_ratings].index)

    n_before = df["product_id"].nunique()
    df = df[df["product_id"].isin(valid_products)].copy()
    n_after = df["product_id"].nunique()
    log.info("[buildMatrix] Lọc ngưỡng: %d → %d sản phẩm còn lại.", n_before, n_after)

    # Cập nhật item_mapping chỉ giữ product vượt ngưỡng
    item_mapping = {k: v for k, v in item_mapping.items() if k in valid_products}

    # [Bước 2d] Dedup — 1 user rate 1 product nhiều lần → mean
    df = (
        df.groupby(["product_id", "user_id"], as_index=False)
          .agg(rating=("rating", "mean"))
    )
    log.info("[buildMatrix] Sau dedup: %s cặp (product, user) duy nhất.", f"{len(df):,}")

    # [Bước 3b] Pivot → Interaction Matrix
    # Hàng = product_id | Cột = user_id | Giá trị = rating | ô trống = 0
    interaction_matrix: pd.DataFrame = (
        df.pivot(index="product_id", columns="user_id", values="rating")
          .fillna(0)
    )
    interaction_matrix.columns.name = None  # xóa label thừa "user_id" trên axis cột

    log.info(
        "[buildMatrix] Interaction Matrix: %d sản phẩm × %d users. "
        "Sparsity: %.1f%%",
        *interaction_matrix.shape,
        100 * (interaction_matrix == 0).values.mean(),
    )
    return interaction_matrix, item_mapping


# ─────────────────────────────────────────────────────────────────────────────
# VALIDATION: Kiểm tra đầu ra trước khi lưu
# ─────────────────────────────────────────────────────────────────────────────

def validateData(matrix: pd.DataFrame, mapping: dict) -> None:
    """
    Kiểm tra tính hợp lệ của Interaction Matrix và item_mapping trước khi lưu.

    Raises
    ------
    ValueError
        Nếu phát hiện bất kỳ vấn đề nào trong dữ liệu.
    """
    errors = []

    if matrix.empty:
        errors.append("Interaction Matrix rỗng.")

    if not mapping:
        errors.append("item_mapping rỗng.")

    # Mọi product_id trong ma trận phải có trong mapping
    matrix_ids = set(matrix.index)
    mapping_ids = set(mapping.keys())
    orphan_ids = matrix_ids - mapping_ids
    if orphan_ids:
        errors.append(f"{len(orphan_ids)} product_id trong ma trận không có tên: {list(orphan_ids)[:5]}...")

    # Rating phải nằm trong [0, 5]
    out_of_range = ((matrix < 0) | (matrix > 5)).values.any()
    if out_of_range:
        errors.append("Có giá trị rating ngoài khoảng [0, 5].")

    if errors:
        raise ValueError("[validateData] Phát hiện lỗi:\n  - " + "\n  - ".join(errors))

    log.info("[validateData] Kiểm tra đầu ra: OK.")


# ─────────────────────────────────────────────────────────────────────────────
# BƯỚC 4: Xuất và ghi file
# ─────────────────────────────────────────────────────────────────────────────

def saveData(matrix: pd.DataFrame, mapping: dict, output_dir: str) -> None:
    """
    Lưu Interaction Matrix và item_mapping ra file pickle.

    File xuất ra:
        {output_dir}/interaction_matrix.pkl  — pd.DataFrame (product_id × user_id)
        {output_dir}/item_mapping.pkl        — dict {product_id: product_name}

    Parameters
    ----------
    matrix     : pd.DataFrame  — output của buildMatrix().
    mapping    : dict          — output của buildMatrix().
    output_dir : str           — thư mục đích; được tạo tự động nếu chưa tồn tại.
    """
    out = Path(output_dir)
    out.mkdir(parents=True, exist_ok=True)

    files = {
        "interaction_matrix.pkl": matrix,
        "item_mapping.pkl": mapping,
    }

    for filename, obj in files.items():
        dest = out / filename
        with open(dest, "wb") as f:
            pickle.dump(obj, f, protocol=pickle.HIGHEST_PROTOCOL)
        size_kb = dest.stat().st_size / 1024
        log.info("[saveData] Đã lưu: %-30s (%.1f KB)", filename, size_kb)

    log.info("[saveData] Tất cả file đã lưu vào: %s", out.resolve())


# ─────────────────────────────────────────────────────────────────────────────
# ENTRY POINT
# ─────────────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    CSV_PATH    = "amazon.csv"
    OUTPUT_DIR  = "output"
    MIN_RATINGS = 10

    df_raw = loadData(CSV_PATH)

    interaction_matrix, item_mapping = buildMatrix(df_raw, min_ratings=MIN_RATINGS)

    validateData(interaction_matrix, item_mapping)

    saveData(interaction_matrix, item_mapping, OUTPUT_DIR)

    log.info("=" * 55)
    log.info("Pipeline hoàn tất!")
    log.info("  Ma trận : %d sản phẩm × %d users", *interaction_matrix.shape)
    log.info("  Mapping : %d sản phẩm", len(item_mapping))
    log.info("  Output  : %s/", Path(OUTPUT_DIR).resolve())
    log.info("=" * 55)