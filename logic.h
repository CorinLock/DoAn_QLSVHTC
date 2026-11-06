#pragma once
#ifndef LOGIC_H
#define LOGIC_H

#include "model.h"
#include <string>
#include <fstream>
#include <cctype>
#include <sstream>

using std::string;
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::getline;
using std::to_string;
using std::stoi;
using std::stof;
using std::endl;

// ========================================================
// 1. NHÓM CHUẨN HÓA VÀ XỬ LÝ CHUỖI
// ========================================================

// Hàm chuyển toàn bộ thành chữ In Hoa và xóa mọi khoảng trắng (dành cho Mã)
inline void inHoaToanBo(string &str) {
    string temp = "";
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] != ' ') {
            temp += toupper(static_cast<unsigned char>(str[i]));
        }
    }
    str = temp;
}

// Hàm chuẩn hóa Tên: Xóa khoảng trắng thừa và Viết hoa chữ cái đầu mỗi từ
inline void chuanHoaChuoi(string &str) {
    // 1. Xóa khoảng trắng ở đầu
    while (str.length() > 0 && str[0] == ' ') {
        str.erase(0, 1);
    }
    // 2. Xóa khoảng trắng ở cuối
    while (str.length() > 0 && str[str.length() - 1] == ' ') {
        str.erase(str.length() - 1, 1);
    }
    if (str.length() == 0) return;

    // 3. Chuẩn hóa hoa thường và khoảng trắng giữa
    string result = "";
    bool inSpace = false;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == ' ') {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        } else {
            if (result.length() == 0 || result[result.length() - 1] == ' ') {
                result += toupper(static_cast<unsigned char>(str[i]));
            } else {
                result += tolower(static_cast<unsigned char>(str[i]));
            }
            inSpace = false;
        }
    }
    str = result;
}

// ========================================================
// RÀNG BUỘC DỮ LIỆU NGẶT NGHÈO (STRICT VALIDATION HELPERS)
// ========================================================

// Kiểm tra Tên hợp lệ (chỉ 1 từ duy nhất, không dấu cách, chỉ gồm chữ cái A-Z, a-z)
inline bool kiemTraTenHopLe(const string &ten) {
    if (ten.empty()) return false;
    for (char c : ten) {
        if (!isalpha(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

// Kiểm tra Họ hợp lệ (chỉ chữ cái và khoảng trắng, không số, không ký tự đặc biệt)
inline bool kiemTraHoHopLe(const string &ho) {
    if (ho.empty()) return false;
    for (char c : ho) {
        if (!isalpha(static_cast<unsigned char>(c)) && c != ' ') return false;
    }
    return true;
}

// Kiểm tra Phái hợp lệ (chỉ chấp nhận Nam hoặc Nu, không phân biệt hoa thường) và tự động chuẩn hóa
inline bool kiemTraPhaiHopLe(string &phai) {
    string p = "";
    for (char c : phai) {
        if (c != ' ') p += tolower(static_cast<unsigned char>(c));
    }
    if (p == "nam") { phai = "Nam"; return true; }
    if (p == "nu") { phai = "Nu"; return true; }
    return false;
}

// Kiểm tra Số điện thoại hợp lệ (chuẩn 10 chữ số, bắt đầu bằng số 0)
inline bool kiemTraSoDienThoaiHopLe(const string &sdt) {
    if (sdt.length() != 10) return false;
    if (sdt[0] != '0') return false;
    for (char c : sdt) {
        if (!isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

// Kiểm tra Niên khóa hợp lệ (định dạng YYYY-YYYY, năm sau = năm trước + 1, năm >= 2000)
inline bool kiemTraNienKhoaHopLe(const string &nk) {
    if (nk.length() != 9) return false;
    if (nk[4] != '-') return false;
    for (int i = 0; i < 9; i++) {
        if (i != 4 && !isdigit(static_cast<unsigned char>(nk[i]))) return false;
    }
    try {
        int nam1 = stoi(nk.substr(0, 4));
        int nam2 = stoi(nk.substr(5, 4));
        if (nam1 < 2000 || nam2 != nam1 + 1) return false;
    } catch (...) {
        return false;
    }
    return true;
}

// Kiểm tra Điểm số hợp lệ (0.0 đến 10.0)
inline bool kiemTraDiemHopLe(float diem) {
    return (diem >= 0.0f && diem <= 10.0f);
}

// ========================================================
// 2. NHÓM THUẬT TOÁN: MÔN HỌC (CÂY NHỊ PHÂN TÌM KIẾM - BST)
// ========================================================

// Tìm kiếm node môn học theo mã môn học
inline treeMH timKiemMonHoc(treeMH t, const string &maMH) {
    if (t == NULL) return NULL;
    if (t->mh.MAMH == maMH) return t;
    if (maMH < t->mh.MAMH) return timKiemMonHoc(t->left, maMH);
    return timKiemMonHoc(t->right, maMH);
}

// Kiểm tra mã môn học có tồn tại trên cây hay không
inline bool kiemTraMAMH(treeMH t, const string &maMH) {
    return (timKiemMonHoc(t, maMH) != NULL);
}

// Chèn môn học mới vào cây BST. Trả về true nếu thành công, false nếu trùng mã
inline bool insertMH(treeMH &t, const Monhoc &mh) {
    if (t == NULL) {
        t = new nodeMH;
        t->mh = mh;
        t->left = NULL;
        t->right = NULL;
        return true;
    }
    if (mh.MAMH < t->mh.MAMH) {
        return insertMH(t->left, mh);
    } else if (mh.MAMH > t->mh.MAMH) {
        return insertMH(t->right, mh);
    }
    return false; // Trùng mã môn học
}

// Tìm node thế mạng (node trái nhất của cây con bên phải)
inline void timNodeTheMang(treeMH &p, treeMH &q) {
    if (q->left != NULL) {
        timNodeTheMang(p, q->left);
    } else {
        p->mh = q->mh;
        p = q;
        q = q->right;
    }
}

// Xóa môn học khỏi cây BST. Trả về true nếu xóa thành công, false nếu không tìm thấy
inline bool xoaNodeMonHoc(treeMH &t, const string &maMH) {
    if (t == NULL) return false;

    if (maMH < t->mh.MAMH) {
        return xoaNodeMonHoc(t->left, maMH);
    } else if (maMH > t->mh.MAMH) {
        return xoaNodeMonHoc(t->right, maMH);
    } else {
        treeMH p = t;
        if (t->left == NULL) {
            t = t->right;
        } else if (t->right == NULL) {
            t = t->left;
        } else {
            timNodeTheMang(p, t->right);
        }
        delete p;
        return true;
    }
}

// Đếm tổng số môn học hiện có trên cây
inline int demSoMonHoc(treeMH t) {
    if (t == NULL) return 0;
    return 1 + demSoMonHoc(t->left) + demSoMonHoc(t->right);
}

// Duyệt LNR để đổ toàn bộ môn học ra mảng tuyến tính
inline void doMonHocVaoMang(treeMH t, Monhoc arr[], int &index) {
    if (t != NULL) {
        doMonHocVaoMang(t->left, arr, index);
        arr[index++] = t->mh;
        doMonHocVaoMang(t->right, arr, index);
    }
}

// ========================================================
// THUẬT TOÁN QUICKSORT PHÂN HOẠCH HOARE (O(N log N))
// ========================================================

// Hàm QuickSort tổng quát sử dụng comparator tuỳ biến
template <typename T, typename Compare>
inline void quickSort(T arr[], int left, int right, Compare comp) {
    if (left >= right) return;
    int i = left;
    int j = right;
    T pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (comp(arr[i], pivot)) i++;
        while (comp(pivot, arr[j])) j--;
        if (i <= j) {
            T temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }
    if (left < j) quickSort(arr, left, j, comp);
    if (i < right) quickSort(arr, i, right, comp);
}

// Thuật toán QuickSort tường minh sắp xếp mảng môn học theo Tên (Alphabet) - O(N log N)
inline void quickSortMonHocTheoTen(Monhoc arr[], int left, int right) {
    if (left >= right) return;
    int i = left;
    int j = right;
    string pivot = arr[(left + right) / 2].TENMH;

    while (i <= j) {
        while (arr[i].TENMH < pivot) i++;
        while (arr[j].TENMH > pivot) j--;
        if (i <= j) {
            Monhoc temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }
    if (left < j) quickSortMonHocTheoTen(arr, left, j);
    if (i < right) quickSortMonHocTheoTen(arr, i, right);
}

// Sắp xếp mảng môn học theo Tên Môn Học (Alphabet) bằng QuickSort - Phục vụ chức năng f
inline void sapXepMonHocTheoTen(Monhoc arr[], int total) {
    if (total > 1) {
        quickSortMonHocTheoTen(arr, 0, total - 1);
    }
}

// ========================================================
// 3. NHÓM THUẬT TOÁN: LỚP & SINH VIÊN (MẢNG TUYẾN TÍNH & DSLK)
// ========================================================

// Tìm kiếm lớp theo mã lớp, trả về vị trí index trong mảng hoặc -1 nếu không thấy
inline int timKiemLop(DS_LOP &dslop, const string &maLop) {
    for (int i = 0; i < dslop.n; i++) {
        if (dslop.nodes[i].MALOP == maLop) {
            return i;
        }
    }
    return -1;
}

// Thêm một lớp mới vào danh sách.
// Trả về: 1 nếu thành công, 0 nếu trùng mã, -1 nếu danh sách đã đầy
inline int themLop(DS_LOP &dslop, const Lop &lopMoi) {
    if (dslop.n >= MAXLOP) return -1;
    if (timKiemLop(dslop, lopMoi.MALOP) != -1) return 0;
    dslop.nodes[dslop.n] = lopMoi;
    dslop.nodes[dslop.n].dssv = NULL;
    dslop.n++;
    return 1;
}

// Giữ lại hàm insertLop nguyên bản trả về bool
inline bool insertLop(DS_LOP &dslop, const Lop &lopMoi) {
    return (themLop(dslop, lopMoi) == 1);
}

// Kiểm tra lớp tại vị trí index có sinh viên nào hay không
inline bool lopCoSinhVien(DS_LOP &dslop, int index) {
    if (index < 0 || index >= dslop.n) return false;
    return (dslop.nodes[index].dssv != NULL);
}

// Xóa lớp tại vị trí index và dồn mảng
inline bool xoaLop(DS_LOP &dslop, int index) {
    if (index < 0 || index >= dslop.n) return false;
    for (int i = index; i < dslop.n - 1; i++) {
        dslop.nodes[i] = dslop.nodes[i + 1];
    }
    dslop.n--;
    return true;
}

// Kiểm tra mã sinh viên đã tồn tại ở bất kỳ lớp nào trên toàn trường hay chưa
inline bool kiemTraSinhVienTonTai(DS_LOP &dslop, const string &maSV) {
    for (int i = 0; i < dslop.n; i++) {
        PTRSV p = dslop.nodes[i].dssv;
        while (p != NULL) {
            if (p->sv.MASV == maSV) return true;
            p = p->next;
        }
    }
    return false;
}

// Lấy Họ và Tên đầy đủ của sinh viên dựa vào mã sinh viên
inline string layTenSinhVien(DS_LOP &dslop, const string &maSV) {
    for (int i = 0; i < dslop.n; i++) {
        PTRSV p = dslop.nodes[i].dssv;
        while (p != NULL) {
            if (p->sv.MASV == maSV) {
                return p->sv.HO + " " + p->sv.TEN;
            }
            p = p->next;
        }
    }
    return "Khong tim thay";
}

// So sánh 2 sinh viên theo Tên rồi đến Họ để sắp xếp Alphabet
inline bool canDoiCho(const Sinhvien &sv1, const Sinhvien &sv2) {
    if (sv1.TEN > sv2.TEN) return true;
    if (sv1.TEN == sv2.TEN) {
        if (sv1.HO > sv2.HO) return true;
    }
    return false;
}

// Sắp xếp danh sách liên kết đơn sinh viên
inline void sapXepSinhVien(PTRSV &First) {
    if (First == NULL || First->next == NULL) return;
    for (PTRSV p = First; p->next != NULL; p = p->next) {
        for (PTRSV q = p->next; q != NULL; q = q->next) {
            if (canDoiCho(p->sv, q->sv)) {
                Sinhvien temp = p->sv;
                p->sv = q->sv;
                q->sv = temp;
            }
        }
    }
}

// Chèn sinh viên vào danh sách liên kết theo thứ tự tăng dần theo Tên (nếu trùng Tên thì theo Họ) - Chuẩn câu c
inline void insertSinhVien(PTRSV &First, const Sinhvien &sv) {
    PTRSV p = new nodeSV;
    p->sv = sv;
    p->next = NULL;

    // Nếu danh sách rỗng hoặc sv mới đứng trước First
    if (First == NULL || canDoiCho(First->sv, sv)) {
        p->next = First;
        First = p;
        return;
    }

    // Tìm vị trí chèn thích hợp
    PTRSV current = First;
    while (current->next != NULL && !canDoiCho(current->next->sv, sv)) {
        current = current->next;
    }
    p->next = current->next;
    current->next = p;
}

// Đếm số lượng sinh viên trong danh sách liên kết
inline int demSinhVienTheoLop(PTRSV First) {
    int total = 0;
    PTRSV p = First;
    while (p != NULL) {
        total++;
        p = p->next;
    }
    return total;
}

// Đổ danh sách sinh viên từ DSLK ra mảng tạm
inline void doSinhVienVaoMang(PTRSV First, Sinhvien arr[]) {
    int idx = 0;
    PTRSV p = First;
    while (p != NULL) {
        arr[idx++] = p->sv;
        p = p->next;
    }
}

// Tìm kiếm sinh viên trong DSLK của một lớp
inline PTRSV timKiemSinhVienTrongLop(PTRSV First, const string &maSV) {
    PTRSV p = First;
    while (p != NULL && p->sv.MASV != maSV) {
        p = p->next;
    }
    return p;
}

// Xóa sinh viên khỏi DSLK của một lớp theo mã sinh viên
inline bool xoaSinhVienKhoiLop(PTRSV &First, const string &maSV) {
    PTRSV pXoa = First;
    PTRSV truoc = NULL;
    while (pXoa != NULL && pXoa->sv.MASV != maSV) {
        truoc = pXoa;
        pXoa = pXoa->next;
    }
    if (pXoa == NULL) return false;
    if (truoc == NULL) {
        First = pXoa->next;
    } else {
        truoc->next = pXoa->next;
    }
    delete pXoa;
    return true;
}

// ========================================================
// 4. NHÓM THUẬT TOÁN: LỚP TÍN CHỈ & ĐĂNG KÝ
// ========================================================

// Chèn một Lớp Tín Chỉ mới vào mảng con trỏ
inline bool insertLTC(DS_LTC &dsltc, const Loptinchi &ltc) {
    if (dsltc.n >= MAXLTC) return false;
    dsltc.nodes[dsltc.n] = new Loptinchi;
    *dsltc.nodes[dsltc.n] = ltc;
    dsltc.n++;
    return true;
}

// Tìm vị trí LTC đang hoạt động (chưa hủy) theo MALOPTC. Trả về index hoặc -1
inline int timKiemLTC(DS_LTC &dsltc, int maLTC) {
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC && !dsltc.nodes[i]->HUYLOP) {
            return i;
        }
    }
    return -1;
}

// Kiểm tra mã LTC đã từng tồn tại trong hệ thống hay chưa (bao gồm cả lớp đã hủy)
inline bool kiemTraTrungMaLTC(DS_LTC &dsltc, int maLTC) {
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC) {
            return true;
        }
    }
    return false;
}

// Đếm số lớp tín chỉ đang hoạt động (chưa hủy)
inline int demLTCHoatDong(DS_LTC &dsltc) {
    int count = 0;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            count++;
        }
    }
    return count;
}

// Trích xuất các lớp tín chỉ đang hoạt động ra mảng con trỏ phục vụ hiển thị
inline void layDanhSachLTCHoatDong(DS_LTC &dsltc, Loptinchi* arrLTC[]) {
    int idx = 0;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            arrLTC[idx++] = dsltc.nodes[i];
        }
    }
}

// Chèn một lượt đăng ký vào đầu danh sách dssvdk
inline void insertDangKy(PTRDK &First, const Dangky &dk) {
    PTRDK p = new nodeDK;
    p->dk = dk;
    p->next = First;
    First = p;
}

// Đếm số sinh viên đã đăng ký vào một lớp tín chỉ (chỉ tính sinh viên chưa hủy)
inline int demSinhVienDangKy(PTRDK First) {
    int count = 0;
    for (PTRDK p = First; p != NULL; p = p->next) {
        if (!p->dk.HUYDK) count++;
    }
    return count;
}

// Kiểm tra sinh viên đã đăng ký LTC hay chưa (chỉ xét lượt đăng ký đang có hiệu lực)
inline bool kiemTraSinhVienDaDangKyLTC(PTRDK First, const string &maSV) {
    for (PTRDK p = First; p != NULL; p = p->next) {
        if (p->dk.MASV == maSV && !p->dk.HUYDK) return true;
    }
    return false;
}

// Tự động hủy các LTC có số SV đăng ký < SOSVMIN trong 1 niên khóa, học kỳ (Mục h của Đề bài)
// Trả về số lớp đã bị hủy
inline int tuDongHuyLopTinChi(DS_LTC &dsltc, const string &nienKhoa = "", int hocKy = 0) {
    int soLopBiHuy = 0;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            bool hopLeNK = nienKhoa.empty() || (dsltc.nodes[i]->NIENKHOA == nienKhoa);
            bool hopLeHK = (hocKy <= 0) || (dsltc.nodes[i]->HOCKY == hocKy);
            if (hopLeNK && hopLeHK) {
                int soSV = demSinhVienDangKy(dsltc.nodes[i]->dssvdk);
                if (soSV < dsltc.nodes[i]->SOSVMIN) {
                    dsltc.nodes[i]->HUYLOP = true;
                    soLopBiHuy++;
                }
            }
        }
    }
    return soLopBiHuy;
}

// Hủy một LTC:
// Trả về: 1 nếu hủy thành công, 0 nếu không tìm thấy, -1 nếu lớp đang có SV đăng ký không thể hủy
inline int huyLopTinChi(DS_LTC &dsltc, int maLTC) {
    int idx = timKiemLTC(dsltc, maLTC);
    if (idx == -1) return 0;
    if (demSinhVienDangKy(dsltc.nodes[idx]->dssvdk) > 0) return -1;
    dsltc.nodes[idx]->HUYLOP = true;
    return 1;
}


// Tìm lượt đăng ký của một SV trong LTC
inline PTRDK timKiemDangKy(PTRDK First, const string &maSV) {
    PTRDK p = First;
    while (p != NULL && p->dk.MASV != maSV) {
        p = p->next;
    }
    return p;
}

// Đăng ký một sinh viên vào LTC:
// Trả về: 1 (thành công), 0 (LTC không tồn tại / đã hủy), -1 (đã đăng ký), -2 (lớp đã đủ số lượng)
inline int dangKyLTC(DS_LTC &dsltc, int maLTC, const string &maSV) {
    int idx = timKiemLTC(dsltc, maLTC);
    if (idx == -1) return 0;

    PTRDK p = timKiemDangKy(dsltc.nodes[idx]->dssvdk, maSV);
    if (p != NULL && !p->dk.HUYDK) return -1; // Đang đăng ký rồi

    int count = demSinhVienDangKy(dsltc.nodes[idx]->dssvdk);
    if (count >= dsltc.nodes[idx]->SOSVMAX) return -2;

    if (p != NULL) {
        // Sinh viên đã từng đăng ký và đã hủy, nay đăng ký lại
        p->dk.HUYDK = false;
        return 1;
    }

    Dangky dkMoi;
    dkMoi.MASV = maSV;
    dkMoi.DIEM = 0;
    dkMoi.HUYDK = false;
    insertDangKy(dsltc.nodes[idx]->dssvdk, dkMoi);
    return 1;
}

// Hủy đăng ký của một sinh viên khỏi LTC (đánh dấu HUYDK = true theo chuẩn đề bài):
// Trả về: 1 (thành công), 0 (LTC không tồn tại), -1 (sinh viên chưa đăng ký hoặc đã hủy)
inline int huyDangKyLTC(DS_LTC &dsltc, int maLTC, const string &maSV) {
    int idx = timKiemLTC(dsltc, maLTC);
    if (idx == -1) return 0;

    PTRDK p = timKiemDangKy(dsltc.nodes[idx]->dssvdk, maSV);
    if (p == NULL || p->dk.HUYDK) return -1;

    p->dk.HUYDK = true;
    return 1;
}

// Đổ danh sách đăng ký của một LTC ra mảng tạm
inline void doDangKyVaoMang(PTRDK First, Dangky arr[]) {
    int idx = 0;
    PTRDK p = First;
    while (p != NULL) {
        arr[idx++] = p->dk;
        p = p->next;
    }
}


// Cập nhật điểm cho một sinh viên trong LTC
inline bool capNhatDiemSinhVien(DS_LTC &dsltc, int maLTC, const string &maSV, float diem) {
    int idx = timKiemLTC(dsltc, maLTC);
    if (idx == -1) return false;
    PTRDK p = timKiemDangKy(dsltc.nodes[idx]->dssvdk, maSV);
    if (p == NULL) return false;
    p->dk.DIEM = diem;
    return true;
}

// Kiểm tra Môn học đã mở Lớp Tín Chỉ hay chưa (dùng chặn xóa Môn học nếu đã mở LTC)
inline bool kiemTraMonHocDaMoLTC(DS_LTC &dsltc, const string &maMH) {
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MAMH == maMH) {
            return true;
        }
    }
    return false;
}

// Kiểm tra Sinh viên đã từng đăng ký bất kỳ LTC nào hay chưa (dùng chặn xóa Sinh viên)
inline bool kiemTraSinhVienDaDangKyLTCNaoChua(DS_LTC &dsltc, const string &maSV) {
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL) {
            for (PTRDK p = dsltc.nodes[i]->dssvdk; p != NULL; p = p->next) {
                if (p->dk.MASV == maSV) return true;
            }
        }
    }
    return false;
}

// Kiểm tra xem Môn học trong cùng Niên khóa, Học kỳ đã mở cho Nhóm này chưa (chặn trùng LTC)
inline bool kiemTraLtcTrungNhom(DS_LTC &dsltc, const string &nienKhoa, int hocKy, const string &maMH, int nhom, int maLTCHienTai = -1) {
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            if (dsltc.nodes[i]->MALOPTC != maLTCHienTai &&
                dsltc.nodes[i]->NIENKHOA == nienKhoa &&
                dsltc.nodes[i]->HOCKY == hocKy &&
                dsltc.nodes[i]->MAMH == maMH &&
                dsltc.nodes[i]->NHOM == nhom) {
                return true;
            }
        }
    }
    return false;
}

// ========================================================
// 5. NHÓM ĐIỂM & BÁO CÁO THỐNG KÊ
// ========================================================

// Lấy điểm cao nhất của một Sinh Viên trong một Môn Học (quét toàn bộ LTC không bị hủy)
inline float layDiemMonHoc(const string &maSV, const string &maMH, DS_LTC &dsltc) {
    float maxDiem = -1; // -1 nghĩa là chưa đăng ký hoặc chưa có điểm
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP && dsltc.nodes[i]->MAMH == maMH) {
            for (PTRDK p = dsltc.nodes[i]->dssvdk; p != NULL; p = p->next) {
                if (p->dk.MASV == maSV && !p->dk.HUYDK) {
                    if (p->dk.DIEM > maxDiem) maxDiem = p->dk.DIEM;
                }
            }
        }
    }
    return maxDiem;
}

// Lấy danh sách kết quả học tập và tính GPA của một sinh viên
inline void tinhBangDiemCaNhan(const string &maSV, DS_LTC &dsltc, treeMH dsmh,
                              KetQuaHocTap arrKQ[], int &soMon, float &gpa, int &tongTC) {
    soMon = 0;
    tongTC = 0;
    float tongDiem = 0;

    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            for (PTRDK p = dsltc.nodes[i]->dssvdk; p != NULL; p = p->next) {
                if (p->dk.MASV == maSV && !p->dk.HUYDK) {
                    treeMH nodeMH = timKiemMonHoc(dsmh, dsltc.nodes[i]->MAMH);
                    if (nodeMH != NULL) {
                        // Kiểm tra môn này đã có trong mảng chưa (xử lý học lại)
                        bool daCo = false;
                        for (int k = 0; k < soMon; k++) {
                            if (arrKQ[k].MAMH == dsltc.nodes[i]->MAMH) {
                                daCo = true;
                                if (p->dk.DIEM > arrKQ[k].DIEM) {
                                    arrKQ[k].DIEM = p->dk.DIEM;
                                }
                                break;
                            }
                        }
                        if (!daCo) {
                            arrKQ[soMon].MAMH = dsltc.nodes[i]->MAMH;
                            arrKQ[soMon].TENMH = nodeMH->mh.TENMH;
                            arrKQ[soMon].STC = nodeMH->mh.STC_LT + nodeMH->mh.STC_TH;
                            arrKQ[soMon].DIEM = p->dk.DIEM;
                            soMon++;
                        }
                    }
                }
            }
        }
    }

    // Tính GPA
    for (int i = 0; i < soMon; i++) {
        tongDiem += arrKQ[i].DIEM * arrKQ[i].STC;
        tongTC += arrKQ[i].STC;
    }
    gpa = (tongTC == 0) ? 0 : (tongDiem / tongTC);
}

// Tìm kiếm Lớp Tín Chỉ theo đúng 4 tham số: Niên khóa, Học kỳ, Mã môn học, Nhóm (Mục i, j)
inline int timKiemLTCTheoThongSo(DS_LTC &dsltc, const string &nienKhoa, int hocKy, const string &maMH, int nhom) {
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            if (dsltc.nodes[i]->NIENKHOA == nienKhoa &&
                dsltc.nodes[i]->HOCKY == hocKy &&
                dsltc.nodes[i]->MAMH == maMH &&
                dsltc.nodes[i]->NHOM == nhom) {
                return i;
            }
        }
    }
    return -1;
}

// Tìm kiếm con trỏ Sinh Viên trên toàn trường (quét qua tất cả các Lớp)
inline Sinhvien* timSinhVienToanTruong(DS_LOP &dslop, const string &maSV) {
    for (int i = 0; i < dslop.n; i++) {
        for (PTRSV p = dslop.nodes[i].dssv; p != NULL; p = p->next) {
            if (p->sv.MASV == maSV) {
                return &(p->sv);
            }
        }
    }
    return NULL;
}

// Lấy danh sách SV đã đăng ký 1 LTC (Mục b, i, j), sắp xếp Alphabet theo Tên, Họ
inline int layDanhSachSVDangKyLTC(Loptinchi* ltc, DS_LOP &dslop, SinhVienDiemLTC arr[], int maxSV, bool layCaDaHuy = false) {
    if (ltc == NULL) return 0;
    int count = 0;
    for (PTRDK p = ltc->dssvdk; p != NULL && count < maxSV; p = p->next) {
        if (layCaDaHuy || !p->dk.HUYDK) {
            arr[count].MASV = p->dk.MASV;
            arr[count].DIEM = p->dk.DIEM;
            arr[count].HUYDK = p->dk.HUYDK;
            Sinhvien* sv = timSinhVienToanTruong(dslop, p->dk.MASV);
            if (sv != NULL) {
                arr[count].HO = sv->HO;
                arr[count].TEN = sv->TEN;
            } else {
                arr[count].HO = "";
                arr[count].TEN = "";
            }
            count++;
        }
    }
    // Sắp xếp tăng dần theo Tên, nếu trùng tên sắp theo Họ
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i].TEN > arr[j].TEN || (arr[i].TEN == arr[j].TEN && arr[i].HO > arr[j].HO)) {
                SinhVienDiemLTC temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
    return count;
}

// Thống kê Điểm TB kết thúc khóa học của 1 Lớp theo số tín chỉ (Mục k)
inline int layDiemTBKhoaHocCuaLop(DS_LOP &dslop, int indexLop, DS_LTC &dsltc, treeMH dsmh, SinhVienDiemTBKhoaHoc arr[], int maxSV) {
    if (indexLop < 0 || indexLop >= dslop.n) return 0;
    int count = 0;
    for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL && count < maxSV; p = p->next) {
        arr[count].MASV = p->sv.MASV;
        arr[count].HO = p->sv.HO;
        arr[count].TEN = p->sv.TEN;
        
        KetQuaHocTap arrKQ[500];
        int soMon = 0, tongTC = 0;
        float gpa = 0.0f;
        tinhBangDiemCaNhan(p->sv.MASV, dsltc, dsmh, arrKQ, soMon, gpa, tongTC);
        arr[count].DIEMTB = gpa;
        arr[count].TONGTC = tongTC;
        count++;
    }
    // Sắp xếp tăng dần theo Tên, nếu trùng tên sắp theo Họ
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i].TEN > arr[j].TEN || (arr[i].TEN == arr[j].TEN && arr[i].HO > arr[j].HO)) {
                SinhVienDiemTBKhoaHoc temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
    return count;
}

// ========================================================
// 6. NHÓM XỬ LÝ FILE (GHI / ĐỌC)
// ========================================================

// Đệ quy ghi node môn học ra file theo thứ tự NLR
inline void ghiNodeMonHoc(treeMH t, ofstream &fileOut) {
    if (t != NULL) {
        fileOut << t->mh.MAMH << ","
                << t->mh.TENMH << ","
                << t->mh.STC_LT << ","
                << t->mh.STC_TH << endl;
        ghiNodeMonHoc(t->left, fileOut);
        ghiNodeMonHoc(t->right, fileOut);
    }
}

// Ghi toàn bộ cây môn học ra file MonHoc.txt
inline void ghiFileMonHoc(treeMH t) {
    ofstream fileOut("MonHoc.txt");
    if (fileOut.is_open()) {
        ghiNodeMonHoc(t, fileOut);
        fileOut.close();
    }
}

// Hàm xóa khoảng trắng thừa và ký tự \r, \n ở 2 đầu chuỗi
inline string trimString(const string &str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Đọc toàn bộ danh sách môn học từ file MonHoc.txt vào cây BST
inline void docFileMonHoc(treeMH &t) {
    ifstream fileIn("MonHoc.txt");
    if (fileIn.is_open()) {
        string line;
        while (getline(fileIn, line)) {
            line = trimString(line);
            if (line.empty()) continue;

            stringstream ss(line);
            string mamh, tenmh, stc_lt_str, stc_th_str;
            if (getline(ss, mamh, ',') && getline(ss, tenmh, ',') &&
                getline(ss, stc_lt_str, ',') && getline(ss, stc_th_str)) {
                mamh = trimString(mamh);
                tenmh = trimString(tenmh);
                stc_lt_str = trimString(stc_lt_str);
                stc_th_str = trimString(stc_th_str);
                if (!mamh.empty()) {
                    try {
                        Monhoc mh;
                        mh.MAMH = mamh;
                        mh.TENMH = tenmh;
                        mh.STC_LT = stoi(stc_lt_str);
                        mh.STC_TH = stoi(stc_th_str);
                        insertMH(t, mh);
                    } catch(...) {}
                }
            }
        }
        fileIn.close();
    }
}

// Ghi danh sách Lớp và Sinh Viên ra Lop.txt và SinhVien.txt
inline void ghiFileLop(DS_LOP &dslop) {
    ofstream fileLop("Lop.txt");
    ofstream fileSV("SinhVien.txt");

    if (fileLop.is_open() && fileSV.is_open()) {
        for (int i = 0; i < dslop.n; i++) {
            fileLop << dslop.nodes[i].MALOP << "," << dslop.nodes[i].TENLOP << endl;
            PTRSV p = dslop.nodes[i].dssv;
            while (p != NULL) {
                fileSV << dslop.nodes[i].MALOP << ","
                       << p->sv.MASV << ","
                       << p->sv.HO << ","
                       << p->sv.TEN << ","
                       << p->sv.PHAI << ","
                       << p->sv.SODT << endl;
                p = p->next;
            }
        }
        fileLop.close();
        fileSV.close();
    }
}

// Đọc danh sách Lớp và Sinh Viên từ Lop.txt và SinhVien.txt
inline void docFileLop(DS_LOP &dslop) {
    ifstream fileLop("Lop.txt");
    if (fileLop.is_open()) {
        string line;
        while (getline(fileLop, line)) {
            line = trimString(line);
            if (line.empty()) continue;

            size_t comma = line.find(',');
            if (comma == string::npos) continue;

            string malop = trimString(line.substr(0, comma));
            string tenlop = trimString(line.substr(comma + 1));
            if (!malop.empty()) {
                Lop lopMoi;
                lopMoi.MALOP = malop;
                lopMoi.TENLOP = tenlop;
                lopMoi.dssv = NULL;
                insertLop(dslop, lopMoi);
            }
        }
        fileLop.close();
    }

    ifstream fileSV("SinhVien.txt");
    if (fileSV.is_open()) {
        string line;
        while (getline(fileSV, line)) {
            line = trimString(line);
            if (line.empty()) continue;

            stringstream ss(line);
            string maLop, masv, ho, ten, phai, sodt;
            if (getline(ss, maLop, ',') && getline(ss, masv, ',') && getline(ss, ho, ',') &&
                getline(ss, ten, ',') && getline(ss, phai, ',') && getline(ss, sodt)) {
                maLop = trimString(maLop);
                masv = trimString(masv);
                ho = trimString(ho);
                ten = trimString(ten);
                phai = trimString(phai);
                sodt = trimString(sodt);

                if (!maLop.empty() && !masv.empty()) {
                    int index = timKiemLop(dslop, maLop);
                    if (index != -1) {
                        Sinhvien sv;
                        sv.MASV = masv;
                        sv.HO = ho;
                        sv.TEN = ten;
                        sv.PHAI = phai;
                        sv.SODT = sodt;
                        insertSinhVien(dslop.nodes[index].dssv, sv);
                    }
                }
            }
        }
        fileSV.close();
    }
}

// Ghi danh sách LTC và Đăng Ký ra LopTinChi.txt và DangKy.txt
inline void ghiFileLTC(DS_LTC &dsltc) {
    ofstream fileLTC("LopTinChi.txt");
    ofstream fileDK("DangKy.txt");

    if (fileLTC.is_open() && fileDK.is_open()) {
        for (int i = 0; i < dsltc.n; i++) {
            if (dsltc.nodes[i] != NULL) {
                fileLTC << dsltc.nodes[i]->MALOPTC << ","
                        << dsltc.nodes[i]->MAMH << ","
                        << dsltc.nodes[i]->NIENKHOA << ","
                        << dsltc.nodes[i]->HOCKY << ","
                        << dsltc.nodes[i]->NHOM << ","
                        << dsltc.nodes[i]->SOSVMIN << ","
                        << dsltc.nodes[i]->SOSVMAX << ","
                        << dsltc.nodes[i]->HUYLOP << endl;

                PTRDK p = dsltc.nodes[i]->dssvdk;
                while (p != NULL) {
                    fileDK << dsltc.nodes[i]->MALOPTC << ","
                           << p->dk.MASV << ","
                           << p->dk.DIEM << ","
                           << p->dk.HUYDK << endl;
                    p = p->next;
                }
            }
        }
        fileLTC.close();
        fileDK.close();
    }
}

// Đọc danh sách LTC và Đăng Ký từ LopTinChi.txt và DangKy.txt
inline void docFileLTC(DS_LTC &dsltc) {
    ifstream fileLTC("LopTinChi.txt");
    if (fileLTC.is_open()) {
        string line;
        while (getline(fileLTC, line)) {
            line = trimString(line);
            if (line.empty()) continue;

            stringstream ss(line);
            string maLTC_str, mamh, nienkhoa, hk_str, nhom_str, min_str, max_str, huy_str;
            if (getline(ss, maLTC_str, ',') && getline(ss, mamh, ',') && getline(ss, nienkhoa, ',') &&
                getline(ss, hk_str, ',') && getline(ss, nhom_str, ',') && getline(ss, min_str, ',') &&
                getline(ss, max_str, ',') && getline(ss, huy_str)) {
                maLTC_str = trimString(maLTC_str);
                mamh = trimString(mamh);
                nienkhoa = trimString(nienkhoa);
                hk_str = trimString(hk_str);
                nhom_str = trimString(nhom_str);
                min_str = trimString(min_str);
                max_str = trimString(max_str);
                huy_str = trimString(huy_str);

                if (!maLTC_str.empty()) {
                    try {
                        Loptinchi ltc;
                        ltc.MALOPTC = stoi(maLTC_str);
                        ltc.MAMH = mamh;
                        ltc.NIENKHOA = nienkhoa;
                        ltc.HOCKY = stoi(hk_str);
                        ltc.NHOM = stoi(nhom_str);
                        ltc.SOSVMIN = stoi(min_str);
                        ltc.SOSVMAX = stoi(max_str);
                        ltc.HUYLOP = (huy_str == "1");
                        ltc.dssvdk = NULL;

                        dsltc.nodes[dsltc.n] = new Loptinchi;
                        *dsltc.nodes[dsltc.n] = ltc;
                        dsltc.n++;
                    } catch(...) {}
                }
            }
        }
        fileLTC.close();
    }

    ifstream fileDK("DangKy.txt");
    if (fileDK.is_open()) {
        string line;
        while (getline(fileDK, line)) {
            line = trimString(line);
            if (line.empty()) continue;

            stringstream ss(line);
            string maLTC_str, masv, diem_str, huyDK_str;
            if (getline(ss, maLTC_str, ',') && getline(ss, masv, ',') &&
                getline(ss, diem_str, ',') && getline(ss, huyDK_str)) {
                maLTC_str = trimString(maLTC_str);
                masv = trimString(masv);
                diem_str = trimString(diem_str);
                huyDK_str = trimString(huyDK_str);

                if (!maLTC_str.empty() && !masv.empty()) {
                    try {
                        int maLTC = stoi(maLTC_str);
                        Dangky dk;
                        dk.MASV = masv;
                        dk.DIEM = stof(diem_str);
                        dk.HUYDK = (huyDK_str == "1");

                        for (int i = 0; i < dsltc.n; i++) {
                            if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC) {
                                insertDangKy(dsltc.nodes[i]->dssvdk, dk);
                                break;
                            }
                        }
                    } catch(...) {}
                }
            }
        }
        fileDK.close();
    }
}

// Lưu toàn bộ dữ liệu hệ thống ra các file
inline void luuTatCaDuLieu(treeMH dsmh, DS_LOP &dslop, DS_LTC &dsltc) {
    ghiFileMonHoc(dsmh);
    ghiFileLop(dslop);
    ghiFileLTC(dsltc);
}

// ========================================================
// 7. NHÓM THUẬT TOÁN GIẢI PHÓNG BỘ NHỚ ĐỘNG (HEAP CLEANUP)
// ========================================================

// Giải phóng bộ nhớ Cây nhị phân tìm kiếm Môn học (Duyệt Hậu thứ tự LRN: Trái -> Phải -> Gốc)
inline void giaiPhongCayMH(treeMH &t) {
    if (t != NULL) {
        giaiPhongCayMH(t->left);
        giaiPhongCayMH(t->right);
        delete t;
        t = NULL;
    }
}

// Giải phóng danh sách liên kết đơn Sinh viên trong 1 lớp
inline void giaiPhongDSSV(PTRSV &First) {
    while (First != NULL) {
        PTRSV temp = First;
        First = First->next;
        delete temp;
    }
}

// Giải phóng toàn bộ danh sách sinh viên của tất cả các Lớp
inline void giaiPhongDSLop(DS_LOP &dslop) {
    for (int i = 0; i < dslop.n; i++) {
        giaiPhongDSSV(dslop.nodes[i].dssv);
    }
    dslop.n = 0;
}

// Giải phóng danh sách liên kết đơn Đăng ký trong 1 Lớp tín chỉ
inline void giaiPhongDSDK(PTRDK &First) {
    while (First != NULL) {
        PTRDK temp = First;
        First = First->next;
        delete temp;
    }
}

// Giải phóng toàn bộ mảng con trỏ Lớp tín chỉ và các danh sách đăng ký đi kèm
inline void giaiPhongDSLTC(DS_LTC &dsltc) {
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL) {
            giaiPhongDSDK(dsltc.nodes[i]->dssvdk);
            delete dsltc.nodes[i];
            dsltc.nodes[i] = NULL;
        }
    }
    dsltc.n = 0;
}

// Giải phóng toàn bộ tài nguyên bộ nhớ động của toàn bộ hệ thống
inline void giaiPhongToanBoHeThong(treeMH &dsmh, DS_LOP &dslop, DS_LTC &dsltc) {
    giaiPhongCayMH(dsmh);
    giaiPhongDSLop(dslop);
    giaiPhongDSLTC(dsltc);
}

#endif // LOGIC_H

