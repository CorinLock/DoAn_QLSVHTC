#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <string>

// ==========================================
// HẰNG SỐ TOÀN CỤC CỦA HỆ THỐNG
// ==========================================
const int MAXLOP = 10000;
const int MAXLTC = 10000;

// ==========================================
// 1. MÔN HỌC (CÂY NHỊ PHÂN TÌM KIẾM - BST)
// ==========================================
struct Monhoc {
    std::string MAMH;
    std::string TENMH;
    int STC_LT = 0;
    int STC_TH = 0;
};

struct nodeMH {
    Monhoc mh;
    nodeMH *left = NULL;
    nodeMH *right = NULL;
};
typedef nodeMH *treeMH;

// ==========================================
// 2. ĐĂNG KÝ (DANH SÁCH LIÊN KẾT ĐƠN)
// ==========================================
struct Dangky {
    std::string MASV;
    float DIEM = 0;
    bool HUYDK = false;
};

struct nodeDK {
    Dangky dk;
    nodeDK *next = NULL;
};
typedef nodeDK *PTRDK;

// ==========================================
// 3. LỚP TÍN CHỈ (MẢNG CON TRỎ)
// ==========================================
struct Loptinchi {
    int MALOPTC = 0;
    std::string MAMH;
    std::string NIENKHOA;
    int HOCKY = 0;
    int NHOM = 0;
    int SOSVMIN = 0;
    int SOSVMAX = 0;
    bool HUYLOP = false;
    PTRDK dssvdk = NULL;
};

struct DS_LTC {
    Loptinchi *nodes[MAXLTC];
    int n = 0;

    DS_LTC() {
        for (int i = 0; i < MAXLTC; i++) {
            nodes[i] = NULL;
        }
    }
};

// ==========================================
// 4. SINH VIÊN (DANH SÁCH LIÊN KẾT ĐƠN)
// ==========================================
struct Sinhvien {
    std::string MASV;
    std::string HO;
    std::string TEN;
    std::string PHAI;
    std::string SODT;
};

struct nodeSV {
    Sinhvien sv;
    nodeSV *next = NULL;
};
typedef nodeSV *PTRSV;

// ==========================================
// 5. LỚP (MẢNG TUYẾN TÍNH)
// ==========================================
struct Lop {
    std::string MALOP;
    std::string TENLOP;
    PTRSV dssv = NULL;
};

struct DS_LOP {
    Lop nodes[MAXLOP];
    int n = 0;
};

// ==========================================
// 6. KẾT QUẢ HỌC TẬP (BẢNG ĐIỂM BÁO CÁO)
// ==========================================
struct KetQuaHocTap {
    std::string MAMH;
    std::string TENMH;
    int STC = 0;
    float DIEM = 0;
};

// Dùng cho mục b, i và j: Bảng điểm và Danh sách sinh viên đăng ký LTC
struct SinhVienDiemLTC {
    std::string MASV;
    std::string HO;
    std::string TEN;
    float DIEM = -1;
    bool HUYDK = false;
};

// Dùng cho mục k: Thống kê Điểm TB kết thúc khóa học của 1 lớp (STT, MASV, HO, TEN, Điểm TB)
struct SinhVienDiemTBKhoaHoc {
    std::string MASV;
    std::string HO;
    std::string TEN;
    float DIEMTB = 0;
    int TONGTC = 0;
};

#endif // MODEL_H

