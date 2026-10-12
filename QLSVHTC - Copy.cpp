#include "mylib.h"
#include <bits/stdc++.h>
#include <fstream>
#include <conio.h>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_F1 59
#define KEY_F2 60
#define KEY_F3 61
#define KEY_DEL 83
#define KEY_F4 62
#define KEY_F5 63

const int soItem = 6;
const int MAXLOP = 10000;
const int MAXLTC = 10000;

string menuMain[6] = {
    "1. QUAN LY MON HOC",
    "2. QUAN LY LOP & SINH VIEN",
    "3. QUAN LY LOP TIN CHI",
    "4. DANG KY TIN CHI (SINH VIEN)",
    "5. QUAN LY DIEM & BAO CAO (GIANG VIEN)",
    "6. LUU VA THOAT CHUONG TRINH"
};

struct Monhoc{
    string MAMH, TENMH;
    int STC_LT, STC_TH; 
};

struct nodeMH{
    Monhoc mh;
    nodeMH *left;
    nodeMH *right;
};
typedef nodeMH *treeMH;

//DANG KY
struct Dangky{
    string MASV;
    float DIEM;
    bool HUYDK = false;
};

struct nodeDK {
    Dangky dk;
    nodeDK *next;
};
typedef nodeDK *PTRDK;

// DSLTC
struct Loptinchi{
    int MALOPTC;
    string MAMH, NIENKHOA;
    int HOCKY, NHOM, SOSVMIN, SOSVMAX;
    bool HUYLOP = false;
    PTRDK dssvdk = NULL;
};

struct DS_LTC{
    Loptinchi *nodes[MAXLTC];
    int n = 0;

    DS_LTC() {
        for (int i = 0; i < MAXLTC; i++) {
            nodes[i] = NULL;
        }
    }
};

struct Sinhvien{
    string MASV, HO, TEN, PHAI, SODT; 
};

struct nodeSV{
    Sinhvien sv;
    nodeSV *next;
};
typedef nodeSV *PTRSV;

struct Lop{
	string MALOP, TENLOP;
	PTRSV dssv=NULL;
};
struct DS_LOP{
    Lop nodes[MAXLOP];
    int n = 0;
};
struct KetQuaHocTap {
    string MAMH;
    string TENMH;
    int STC;
    float DIEM;
};

// NHÓM TIỆN ÍCH UI CƠ SỞ
void veKhung(int x, int y, int width, int height) {
    SetColor(9); // Viền màu xanh dương sáng
    
    // Vẽ góc trên trái và viền trên
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < width; i++) cout << char(196);
    cout << char(191); // Góc trên phải
    
    // Vẽ hai cạnh bên
    for (int i = 1; i < height; i++) {
        gotoxy(x, y + i); cout << char(179);
        gotoxy(x + width, y + i); cout << char(179);
    }
    
    // Vẽ góc dưới trái và viền dưới
    gotoxy(x, y + height); cout << char(192);
    for (int i = 1; i < width; i++) cout << char(196);
    cout << char(217); // Góc dưới phải
    
    SetColor(7); // Trả lại màu chữ mặc định
}

// Hàm chuyển toàn bộ thành chữ In Hoa và xóa mọi khoảng trắng (Dành cho các loại MÃ)
void inHoaToanBo(string &str) {
    string temp = "";
    for (int i = 0; i < str.length(); i++) {
        if (str[i] != ' ') { // Bỏ qua khoảng trắng nếu lỡ nhập
            temp += toupper(str[i]);
        }
    }
    str = temp;
}

// Hàm chuẩn hóa Tên: Xóa khoảng trắng thừa và Viết hoa chữ cái đầu mỗi từ
void chuanHoaChuoi(string &str) {
    // 1. Xóa khoảng trắng ở đầu
    while (str.length() > 0 && str[0] == ' ') {
        str.erase(0, 1);
    }
    
    // 2. Xóa khoảng trắng ở cuối
    while (str.length() > 0 && str[str.length() - 1] == ' ') {
        str.erase(str.length() - 1, 1);
    }
    
    // 3. Nếu chuỗi rỗng thì thoát
    if (str.length() == 0) return;
    
    // 4. Xóa khoảng trắng thừa ở giữa và chuẩn hóa chữ Hoa/Thường
    string result = "";
    bool inSpace = false;
    
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ' ') {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        } else {
            // Nếu là ký tự đầu tiên của chuỗi hoặc đứng ngay sau khoảng trắng -> Viết hoa
            if (result.length() == 0 || result[result.length() - 1] == ' ') {
                result += toupper(str[i]);
            } else { // Các ký tự còn lại viết thường
                result += tolower(str[i]);
            }
            inSpace = false;
        }
    }
    str = result;
}

bool nhapChuoiCoCancel(string &str, int maxLength) {
    str = "";
    while (true) {
        char c = _getch();
        if (c == KEY_ESC) return false; 
        
        if (c == KEY_ENTER) {
            if (!str.empty()) return true; 
        } 
        else if (c == 8 && str.length() > 0) { // Xử lý Backspace
            str.pop_back();
            cout << "\b \b"; // Lùi con trỏ, in khoảng trắng đè lên, lùi lại lần nữa
        } 
        else if (c >= 32 && c <= 126 && str.length() < maxLength) {
            str += c;
            cout << c;
        }
    }
}

// 1. Popup nhỏ để nhập Mã Môn (Dùng chung cho Sửa và Xóa)
bool popupNhapMa(string tieuDe, string nhan, string &maNhap) {
    int x = 20, y = 10, w = 40, h = 5;
    SetBGColor(14); SetColor(0); // Nền vàng, chữ đen
    
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        for (int j = 0; j <= w; j++) cout << " ";
    }
    
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(191);
    for (int i = 1; i < h; i++) { gotoxy(x, y + i); cout << char(179); gotoxy(x + w, y + i); cout << char(179); }
    gotoxy(x, y + h); cout << char(192); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(217);

    gotoxy(x + w - 28, y + h); SetColor(4); cout << " Enter: OK    ESC: Exit "; SetColor(0);
    
    gotoxy(x + (w - tieuDe.length()) / 2, y + 1); cout << tieuDe;
    gotoxy(x + 1, y + 2); for(int i = 1; i < w; i++) cout << char(196);
    
    // Sử dụng biến nhan thay cho chuỗi cố định
    gotoxy(x + 5, y + 3); cout << nhan;
    bool kq = nhapChuoiCoCancel(maNhap, 15);
    
    SetBGColor(0); SetColor(7);
    return kq;
}

// 2. Popup cảnh báo màu đỏ (Để xác nhận xóa hoặc báo lỗi)
bool popupCanhBao(string thongBao, bool canXacNhan = true) {
    int x = 18, y = 11, w = thongBao.length() + 10;
    if (w < 40) w = 40;
    int h = 4;
    
    SetBGColor(12); SetColor(15); // Nền đỏ, chữ trắng
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        for (int j = 0; j <= w; j++) cout << " ";
    }
    
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(191);
    for (int i = 1; i < h; i++) { gotoxy(x, y + i); cout << char(179); gotoxy(x + w, y + i); cout << char(179); }
    gotoxy(x, y + h); cout << char(192); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(217);

    gotoxy(x + (w - thongBao.length()) / 2, y + 2); cout << thongBao;
    
    if (canXacNhan) {
        gotoxy(x + w - 30, y + h); SetColor(14); cout << " Enter: Dong y    ESC: Huy "; SetColor(15);
        while(true) {
            char c = _getch();
            if (c == KEY_ENTER) { SetBGColor(0); SetColor(7); return true; }
            if (c == KEY_ESC) { SetBGColor(0); SetColor(7); return false; }
        }
    } else {
        gotoxy(x + w - 24, y + h); SetColor(14); cout << " Nhan phim bat ky... "; SetColor(15);
        _getch();
        SetBGColor(0); SetColor(7);
        return false;
    }
}

// NHÓM THUẬT TOÁN LÕI: MÔN HỌC (CÂY NHỊ PHÂN)
// Hàm tìm kiếm trả về con trỏ tới node Môn học
treeMH timKiemMonHoc(treeMH t, string maMH) {
    if (t == NULL) return NULL;
    if (t->mh.MAMH == maMH) return t;
    if (maMH < t->mh.MAMH) return timKiemMonHoc(t->left, maMH);
    return timKiemMonHoc(t->right, maMH);
}

bool kiemTraMAMH(treeMH t, string maMH) {
    if (t == NULL) {
        return false; // Đi đến lá mà không thấy
    }
    if (t->mh.MAMH == maMH) {
        return true;  // Đã tìm thấy
    }
    if (maMH < t->mh.MAMH) {
        return kiemTraMAMH(t->left, maMH); // Tìm bên nhánh trái
    }
    return kiemTraMAMH(t->right, maMH);    // Tìm bên nhánh phải
}

void insertMH(treeMH &t, Monhoc mh) {
    if (t == NULL) {
        // Nếu cây rỗng, cấp phát node mới
        t = new nodeMH;
        t->mh = mh;
        t->left = NULL;
        t->right = NULL;
    } else {
        // So sánh mã môn học để quyết định rẽ trái hay phải
        if (mh.MAMH < t->mh.MAMH) {
            insertMH(t->left, mh);
        } else if (mh.MAMH > t->mh.MAMH) {
            insertMH(t->right, mh);
        } else {
            // Trường hợp mã môn học đã tồn tại
            cout << "Ma mon hoc da ton tai!" << endl;
        }
    }
}

// Hàm hỗ trợ tìm Node thế mạng (Trái nhất của nhánh phải)
void timNodeTheMang(treeMH &p, treeMH &q) {
    if (q->left != NULL) {
        timNodeTheMang(p, q->left);
    } else {
        // Đã tìm thấy node trái nhất
        p->mh = q->mh; // Copy dữ liệu từ node thế mạng lên node cần xóa
        p = q;         // Trỏ p vào q để lát nữa dùng lệnh delete p
        q = q->right;  // Giữ lại nhánh con bên phải của node thế mạng (nếu có)
    }
}

// Hàm đệ quy xóa môn học
bool xoaNodeMonHoc(treeMH &t, string maMH) {
    if (t == NULL) return false; // Không tìm thấy
    
    if (maMH < t->mh.MAMH) {
        return xoaNodeMonHoc(t->left, maMH);
    } else if (maMH > t->mh.MAMH) {
        return xoaNodeMonHoc(t->right, maMH);
    } else {
        // Đã tìm thấy node cần xóa (t->mh.MAMH == maMH)
        treeMH p = t; // Giữ địa chỉ để giải phóng bộ nhớ
        
        if (t->left == NULL) {
            t = t->right; // Trực tiếp kéo nhánh phải lên (Bao gồm cả TH node lá)
        } else if (t->right == NULL) {
            t = t->left;  // Trực tiếp kéo nhánh trái lên
        } else {
            // Trường hợp node có 2 con
            timNodeTheMang(p, t->right); 
        }
        
        delete p; // Xóa node ra khỏi RAM
        return true;
    }
}

// Hàm đếm số node trên cây
int demSoMonHoc(treeMH t) {
    if (t == NULL) return 0;
    return 1 + demSoMonHoc(t->left) + demSoMonHoc(t->right);
}

// Hàm duyệt LNR để đổ dữ liệu từ cây vào mảng
void doMonHocVaoMang(treeMH t, Monhoc arr[], int &index) {
    if (t != NULL) {
        doMonHocVaoMang(t->left, arr, index);
        arr[index++] = t->mh;
        doMonHocVaoMang(t->right, arr, index);
    }
}

// Hàm đệ quy duyệt LNR
// Thêm tham số int x để canh lề
void duyetInOrder(treeMH t, int &toaDoY, int x) {
    if (t != NULL) {
        duyetInOrder(t->left, toaDoY, x);
        
        // Vách ngăn cột 1 (MAMH - rộng 15)
        gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 2, toaDoY); cout << t->mh.MAMH;
        
        // Vách ngăn cột 2 (TENMH - rộng 35)
        gotoxy(x + 16, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 18, toaDoY); cout << t->mh.TENMH;
        
        // Vách ngăn cột 3 (STC LT - rộng 10)
        gotoxy(x + 52, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 54, toaDoY); cout << t->mh.STC_LT;
        
        // Vách ngăn cột 4 (STC TH - rộng 10)
        gotoxy(x + 63, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 65, toaDoY); cout << t->mh.STC_TH;
        
        // Vách ngăn chốt cuối cùng
        gotoxy(x + 74, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        
        toaDoY++; 
        
        duyetInOrder(t->right, toaDoY, x);
    }
}

// NHÓM THUẬT TOÁN LÕI: LỚP & SINH VIÊN (MẢNG VÀ DSLK)
// Trả về chỉ số vị trí index nếu tìm thấy, ngược lại trả về -1
int timKiemLop(DS_LOP &dslop, string maLop) {
    for (int i = 0; i < dslop.n; i++) {
        if (dslop.nodes[i].MALOP == maLop) {
            return i;
        }
    }
    return -1;
}

// Hàm kiểm tra sinh viên có tồn tại trên toàn trường không
bool kiemTraSinhVienTonTai(DS_LOP &dslop, string maSV) {
    for (int i = 0; i < dslop.n; i++) {
        PTRSV p = dslop.nodes[i].dssv;
        while (p != NULL) {
            if (p->sv.MASV == maSV) {
                return true; // Đã tìm thấy
            }
            p = p->next;
        }
    }
    return false; // Duyệt hết các lớp mà không thấy
}

string layTenSinhVien(DS_LOP &dslop, string maSV) {
    for (int i = 0; i < dslop.n; i++) {
        PTRSV p = dslop.nodes[i].dssv;
        while (p != NULL) {
            if (p->sv.MASV == maSV) {
                return p->sv.HO + " " + p->sv.TEN; // Nối Họ và Tên
            }
            p = p->next;
        }
    }
    return "Khong tim thay";
}

// Hàm so sánh Tên và Họ theo Alphabet để sắp xếp
// Trả về true nếu sv1 lớn hơn sv2 (Cần đổi chỗ để sv2 lên trước)
bool canDoiCho(Sinhvien sv1, Sinhvien sv2) {
    // 1. So sánh Tên trước
    if (sv1.TEN > sv2.TEN) return true;
    
    // 2. Nếu Tên giống hệt nhau, ta tiếp tục so sánh Họ
    if (sv1.TEN == sv2.TEN) {
        if (sv1.HO > sv2.HO) return true;
    }
    
    // Nếu sv1 nhỏ hơn hoặc bằng, không cần đổi chỗ
    return false;
}

// Thuật toán sắp xếp Danh sách liên kết Sinh Viên
void sapXepSinhVien(PTRSV &First) {
    if (First == NULL || First->next == NULL) return; // Rỗng hoặc chỉ có 1 node thì không cần xếp
    
    for (PTRSV p = First; p->next != NULL; p = p->next) {
        for (PTRSV q = p->next; q != NULL; q = q->next) {
            // Nếu node đứng trước (p) có Alphabet lớn hơn node đứng sau (q)
            if (canDoiCho(p->sv, q->sv)) {
                // Đổi chỗ phần dữ liệu (struct Sinhvien) giữa 2 node
                Sinhvien temp = p->sv;
                p->sv = q->sv;
                q->sv = temp;
            }
        }
    }
}

bool insertLop(DS_LOP &dslop, Lop lopMoi) {
    // 1. Kiểm tra danh sách đầy
    if (dslop.n >= MAXLOP) {
        cout << "Danh sach lop da day!" << endl;
        return false;
    }
    // 2. Kiểm tra trùng mã lớp
    if (timKiemLop(dslop, lopMoi.MALOP) != -1) {
        cout << "Ma lop da ton tai!" << endl;
        return false;
    }
    // 3. Thêm lớp vào cuối mảng và tăng số lượng n
    dslop.nodes[dslop.n] = lopMoi;
    dslop.nodes[dslop.n].dssv = NULL; // Khởi tạo con trỏ danh sách sinh viên rỗng
    dslop.n++;
    return true;
}

void insertSinhVien(PTRSV &First, Sinhvien sv) {
    // Cấp phát vùng nhớ cho node sinh viên mới
    PTRSV p = new nodeSV;
    p->sv = sv;
    
    // Nối node mới vào đầu danh sách
    p->next = First;
    First = p;
}

// NHÓM THUẬT TOÁN LÕI: LỚP TÍN CHỈ & ĐĂNG KÝ
bool insertLTC(DS_LTC &dsltc, Loptinchi ltc) {
    // Kiểm tra giới hạn mảng con trỏ
    if (dsltc.n >= MAXLTC) { 
        cout << "Danh sach lop tin chi da day!" << endl;
        return false;
    }
    
    // Cấp phát động một đối tượng Loptinchi mới
    dsltc.nodes[dsltc.n] = new Loptinchi;
    // Gán dữ liệu
    *dsltc.nodes[dsltc.n] = ltc; 
    dsltc.n++;
    
    return true;
}

// Hàm chèn một lượt đăng ký vào đầu danh sách dssvdk
void insertDangKy(PTRDK &First, Dangky dk) {
    PTRDK p = new nodeDK;
    p->dk = dk;
    p->next = First;
    First = p;
}

// NHÓM XỬ LÝ FILE (GHI / ĐỌC)
// Hàm đệ quy duyệt NLR để ghi từng node ra file
void ghiNodeMonHoc(treeMH t, ofstream &fileOut) {
    if (t != NULL) {
        // Ghi dữ liệu node hiện tại, các trường cách nhau bằng dấu phẩy
        fileOut << t->mh.MAMH << "," 
                << t->mh.TENMH << "," 
                << t->mh.STC_LT << "," 
                << t->mh.STC_TH << endl;
        
        // Đệ quy ghi nhánh trái và nhánh phải
        ghiNodeMonHoc(t->left, fileOut);
        ghiNodeMonHoc(t->right, fileOut);
    }
}

// Hàm chính mở file và gọi hàm đệ quy
void ghiFileMonHoc(treeMH t) {
    ofstream fileOut("MonHoc.txt"); // Mở file để ghi (tự tạo nếu chưa có)
    if (fileOut.is_open()) {
        ghiNodeMonHoc(t, fileOut);
        fileOut.close();
    }
}

void docFileMonHoc(treeMH &t) {
    ifstream fileIn("MonHoc.txt"); // Mở file để đọc
    if (fileIn.is_open()) {
        Monhoc mh;
        string stc_lt_str, stc_th_str;
        
        // Đọc liên tục cho đến khi hết file
        while (getline(fileIn, mh.MAMH, ',')) {
            getline(fileIn, mh.TENMH, ',');
            getline(fileIn, stc_lt_str, ',');
            getline(fileIn, stc_th_str); // Đọc đến hết dòng (bỏ qua ký tự \n)
            
            if (!mh.MAMH.empty()) {
                mh.STC_LT = stoi(stc_lt_str); 
                mh.STC_TH = stoi(stc_th_str);
                insertMH(t, mh); // Thêm vào cây BST
            }
        }
        fileIn.close();
    }
}

void ghiFileLop(DS_LOP &dslop) {
    ofstream fileLop("Lop.txt");
    ofstream fileSV("SinhVien.txt");
    
    if (fileLop.is_open() && fileSV.is_open()) {
        for (int i = 0; i < dslop.n; i++) {
            // 1. Ghi thông tin Lớp
            fileLop << dslop.nodes[i].MALOP << "," << dslop.nodes[i].TENLOP << endl;
            
            // 2. Ghi danh sách Sinh Viên thuộc lớp đó
            PTRSV p = dslop.nodes[i].dssv;
            while (p != NULL) {
                // Định dạng: MALOP,MASV,HO,TEN,PHAI,SODT
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

void docFileLop(DS_LOP &dslop) {
    // 1. Đọc dữ liệu Lớp
    ifstream fileLop("Lop.txt");
    if (fileLop.is_open()) {
        Lop lopMoi;
        while (getline(fileLop, lopMoi.MALOP, ',')) {
            getline(fileLop, lopMoi.TENLOP);
            
            if (!lopMoi.MALOP.empty()) {
                lopMoi.dssv = NULL;
                insertLop(dslop, lopMoi); // Nạp vào mảng tĩnh
            }
        }
        fileLop.close();
    }

    // 2. Đọc dữ liệu Sinh Viên
    ifstream fileSV("SinhVien.txt");
    if (fileSV.is_open()) {
        string maLop;
        Sinhvien sv;
        while (getline(fileSV, maLop, ',')) {
            getline(fileSV, sv.MASV, ',');
            getline(fileSV, sv.HO, ',');
            getline(fileSV, sv.TEN, ',');
            getline(fileSV, sv.PHAI, ',');
            getline(fileSV, sv.SODT);
            
            if (!maLop.empty()) {
                // Tìm lớp tương ứng và chèn sinh viên vào
                int index = timKiemLop(dslop, maLop);
                if (index != -1) {
                    insertSinhVien(dslop.nodes[index].dssv, sv);
                }
            }
        }
        fileSV.close();
    }
}

void ghiFileLTC(DS_LTC &dsltc) {
    ofstream fileLTC("LopTinChi.txt");
    ofstream fileDK("DangKy.txt");
    
    if (fileLTC.is_open() && fileDK.is_open()) {
        for (int i = 0; i < dsltc.n; i++) {
            if (dsltc.nodes[i] != NULL) {
                // 1. Ghi thông tin Lớp Tín Chỉ
                // Định dạng: MALOPTC,MAMH,NIENKHOA,HOCKY,NHOM,SVMIN,SVMAX,HUYLOP
                fileLTC << dsltc.nodes[i]->MALOPTC << ","
                        << dsltc.nodes[i]->MAMH << ","
                        << dsltc.nodes[i]->NIENKHOA << ","
                        << dsltc.nodes[i]->HOCKY << ","
                        << dsltc.nodes[i]->NHOM << ","
                        << dsltc.nodes[i]->SOSVMIN << ","
                        << dsltc.nodes[i]->SOSVMAX << ","
                        << dsltc.nodes[i]->HUYLOP << endl;
                
                // 2. Ghi danh sách Sinh Viên Đăng Ký vào lớp đó
                PTRDK p = dsltc.nodes[i]->dssvdk;
                while (p != NULL) {
                    // Định dạng: MALOPTC,MASV,DIEM,HUYDK
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

void docFileLTC(DS_LTC &dsltc) {
    // 1. Đọc dữ liệu Lớp Tín Chỉ
    ifstream fileLTC("LopTinChi.txt");
    if (fileLTC.is_open()) {
        string maLTC_str, hk_str, nhom_str, min_str, max_str, huy_str;
        Loptinchi ltc;
        
        while (getline(fileLTC, maLTC_str, ',')) {
            getline(fileLTC, ltc.MAMH, ',');
            getline(fileLTC, ltc.NIENKHOA, ',');
            getline(fileLTC, hk_str, ',');
            getline(fileLTC, nhom_str, ',');
            getline(fileLTC, min_str, ',');
            getline(fileLTC, max_str, ',');
            getline(fileLTC, huy_str); // Đọc đến cuối dòng
            
            if (!maLTC_str.empty()) {
                // Chuyển đổi chuỗi thành số
                ltc.MALOPTC = stoi(maLTC_str);
                ltc.HOCKY = stoi(hk_str);
                ltc.NHOM = stoi(nhom_str);
                ltc.SOSVMIN = stoi(min_str);
                ltc.SOSVMAX = stoi(max_str);
                ltc.HUYLOP = (huy_str == "1"); // 1 là true, 0 là false
                ltc.dssvdk = NULL;
                
                // Đưa vào mảng con trỏ
                dsltc.nodes[dsltc.n] = new Loptinchi;
                *dsltc.nodes[dsltc.n] = ltc;
                dsltc.n++;
            }
        }
        fileLTC.close();
    }

    // 2. Đọc dữ liệu Đăng Ký
    ifstream fileDK("DangKy.txt");
    if (fileDK.is_open()) {
        string maLTC_str, diem_str, huyDK_str;
        Dangky dk;
        
        while (getline(fileDK, maLTC_str, ',')) {
            getline(fileDK, dk.MASV, ',');
            getline(fileDK, diem_str, ',');
            getline(fileDK, huyDK_str);
            
            if (!maLTC_str.empty()) {
                int maLTC = stoi(maLTC_str);
                dk.DIEM = stof(diem_str); // String to Float cho Điểm
                dk.HUYDK = (huyDK_str == "1");
                
                // Tìm Lớp tín chỉ có mã tương ứng để đưa vào
                for (int i = 0; i < dsltc.n; i++) {
                    if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC) {
                        insertDangKy(dsltc.nodes[i]->dssvdk, dk);
                        break;
                    }
                }
            }
        }
        fileDK.close();
    }
}

// NHÓM GIAO DIỆN: MÔN HỌC (HƯỚNG SỰ KIỆN)
bool hienThiFormMonHoc(Monhoc &mh, bool laThemMoi) {
    int x = 12, y = 7, w = 60, h = 8;
    
    // 1. TÔ NỀN ĐẶC (Xóa sạch chữ cũ bên dưới)
    SetBGColor(14); // Nền màu vàng
    SetColor(0);    // Chữ màu đen
    
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        for (int j = 0; j <= w; j++) {
            cout << " "; 
        }
    }
    
    // 2. Vẽ viền
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196);
    cout << char(191);
    
    for (int i = 1; i < h; i++) {
        gotoxy(x, y + i); cout << char(179);
        gotoxy(x + w, y + i); cout << char(179);
    }
    
    gotoxy(x, y + h); cout << char(192);
    for (int i = 1; i < w; i++) cout << char(196);
    cout << char(217);

    // -------------------------------------------------------------
    // CHÈN THÊM CHỮ HƯỚNG DẪN Ở GÓC DƯỚI BÊN PHẢI (ĐÈ LÊN VIỀN)
    // -------------------------------------------------------------
    gotoxy(x + w - 28, y + h); 
    SetColor(4); // Đổi sang chữ màu đỏ để nổi bật sự chú ý
    cout << " Enter: OK    ESC: Exit ";
    SetColor(0); // Trả lại chữ màu đen cho các text bên trong form
    // -------------------------------------------------------------

    // 3. In Text hướng dẫn và Nội dung Form
    gotoxy(x + (w - 30) / 2, y + 1); 
    cout << (laThemMoi ? "--- THEM MON HOC MOI ---" : "--- SUA THONG TIN MON HOC ---");
    
    gotoxy(x + 1, y + 2);
    for(int i = 1; i < w; i++) cout << char(196);
    
    string mhlt_str, mhth_str;
    
    if (laThemMoi) {
        gotoxy(x + 5, y + 3); cout << "Ma Mon Hoc: ";
        if (!nhapChuoiCoCancel(mh.MAMH, 15)) { SetBGColor(0); SetColor(7); return false; }
    } else {
        gotoxy(x + 5, y + 3); cout << "Ma Mon Hoc: " << mh.MAMH << " (Khong the sua)";
    }

    gotoxy(x + 5, y + 4); cout << "Ten Mon Hoc: ";
    if (!nhapChuoiCoCancel(mh.TENMH, 35)) { SetBGColor(0); SetColor(7); return false; }

    gotoxy(x + 5, y + 5); cout << "STC Ly thuyet: ";
    if (!nhapChuoiCoCancel(mhlt_str, 2)) { SetBGColor(0); SetColor(7); return false; }
    if (!mhlt_str.empty()) mh.STC_LT = stoi(mhlt_str); 

    gotoxy(x + 5, y + 6); cout << "STC Thuc hanh: ";
    if (!nhapChuoiCoCancel(mhth_str, 2)) { SetBGColor(0); SetColor(7); return false; }
    if (!mhth_str.empty()) mh.STC_TH = stoi(mhth_str);

    // Gọi hàm chuẩn hóa TRƯỚC KHI kết thúc form
    inHoaToanBo(mh.MAMH);
    chuanHoaChuoi(mh.TENMH);

    // 4. Trả lại nền đen và chữ trắng cho hệ thống Console sau khi làm xong
    SetBGColor(0);
    SetColor(7);
    return true; 
}

void quanLyMonHocToanDien(treeMH &t) {
    int limit = 15;
    int currentPage = 1;
    bool sapXepTheoTen = false; // Biến trạng thái bật/tắt sắp xếp

    while (true) {
        clrscr();
        
        int total = demSoMonHoc(t);
        Monhoc* arrMH = new Monhoc[total];
        int index = 0;
        doMonHocVaoMang(t, arrMH, index);
        
        // --- LOGIC SẮP XẾP TẠI CHỖ NẾU BẬT F4 ---
        if (sapXepTheoTen) {
            for (int i = 0; i < total - 1; i++) {
                for (int j = i + 1; j < total; j++) {
                    if (arrMH[i].TENMH > arrMH[j].TENMH) {
                        Monhoc temp = arrMH[i];
                        arrMH[i] = arrMH[j];
                        arrMH[j] = temp;
                    }
                }
            }
        }
        
        int totalPage = (total == 0) ? 1 : (total + limit - 1) / limit;
        if (currentPage > totalPage) currentPage = totalPage;

        int x = 2, y = 3; 
        gotoxy(x + 20, y - 2); SetColor(11); 
        if (sapXepTheoTen) cout << "QUAN LY MON HOC (DANG XEP THEO: TEN - ALPHABET)";
        else cout << "QUAN LY MON HOC (DANG XEP THEO: MA MON HOC)";
        SetColor(7);
        
        // --- VẼ VIỀN VÀ HEADER (5 - 15 - 35 - 10 - 10) ---
        SetColor(9);
        gotoxy(x, y); cout << char(218);
        for(int i=0; i<5; i++) cout << char(196); cout << char(194);
        for(int i=0; i<15; i++) cout << char(196); cout << char(194);
        for(int i=0; i<35; i++) cout << char(196); cout << char(194);
        for(int i=0; i<10; i++) cout << char(196); cout << char(194);
        for(int i=0; i<10; i++) cout << char(196); cout << char(191);
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA MON HOC    " 
             << char(179) << " TEN MON HOC                       " 
             << char(179) << " STC LT   " << char(179) << " STC TH   " << char(179);
             
        gotoxy(x, y + 2); cout << char(195);
        for(int i=0; i<5; i++) cout << char(196); cout << char(197);
        for(int i=0; i<15; i++) cout << char(196); cout << char(197);
        for(int i=0; i<35; i++) cout << char(196); cout << char(197);
        for(int i=0; i<10; i++) cout << char(196); cout << char(197);
        for(int i=0; i<10; i++) cout << char(196); cout << char(180);
        SetColor(7);
        
        // --- IN DỮ LIỆU ---
        int toaDoY = y + 3;
        int start = (currentPage - 1) * limit;
        int end = (start + limit > total) ? total : start + limit;

        for (int i = start; i < end; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 1, toaDoY); cout << " " << i + 1;
            gotoxy(x + 6, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 8, toaDoY); cout << arrMH[i].MAMH;
            gotoxy(x + 22, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 24, toaDoY); cout << arrMH[i].TENMH;
            gotoxy(x + 58, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 60, toaDoY); cout << arrMH[i].STC_LT;
            gotoxy(x + 69, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 71, toaDoY); cout << arrMH[i].STC_TH;
            gotoxy(x + 80, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
        
        // --- VẼ VIỀN DƯỚI ---
        SetColor(9);
        gotoxy(x, toaDoY); cout << char(192);
        for(int i=0; i<5; i++) cout << char(196); cout << char(193);
        for(int i=0; i<15; i++) cout << char(196); cout << char(193);
        for(int i=0; i<35; i++) cout << char(196); cout << char(193);
        for(int i=0; i<10; i++) cout << char(196); cout << char(193);
        for(int i=0; i<10; i++) cout << char(196); cout << char(217);
        
        // --- THANH CÔNG CỤ CẬP NHẬT THÊM F4 ---
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " F1: Insert "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " F2: Save "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " F3: Edit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " DEL: Delete "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(4); cout << " F4: Sort "; SetBGColor(0); cout << " ";
        SetColor(14); cout << " [<- / ->] Trang " << currentPage << "/" << totalPage; SetColor(7);

        // --- BẮT SỰ KIỆN PHÍM ---
        char c = _getch();
        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_LEFT && currentPage > 1) currentPage--;
            else if (c == KEY_RIGHT && currentPage < totalPage) currentPage++;
            else if (c == KEY_F4) sapXepTheoTen = !sapXepTheoTen; // <--- CÔNG TẮC ĐẢO CHIỀU SẮP XẾP
            else if (c == KEY_F1) {
                Monhoc mhMoi;
                if (hienThiFormMonHoc(mhMoi, true)) insertMH(t, mhMoi);
            }
            else if (c == KEY_F2) {
                ghiFileMonHoc(t);
                gotoxy(x + 60, toaDoY + 3); SetColor(10); cout << "Da luu Data!"; SetColor(7); _getch();
            }
            else if (c == KEY_F3) {
                string maSua;
                if (popupNhapMa("--- SUA MON HOC ---", "Nhap Ma MH: ", maSua)) {
                    treeMH nodeSua = timKiemMonHoc(t, maSua);
                    if (nodeSua != NULL) hienThiFormMonHoc(nodeSua->mh, false);
                    else popupCanhBao("Loi: Khong tim thay Mon hoc " + maSua, false);
                }
            }
            else if (c == KEY_DEL) {
                string maXoa;
                if (popupNhapMa("--- XOA MON HOC ---", "Nhap Ma MH: ", maXoa)) {
                    treeMH nodeXoa = timKiemMonHoc(t, maXoa);
                    if (nodeXoa != NULL) {
                        if (popupCanhBao("Chac chan xoa: " + nodeXoa->mh.TENMH + " ?", true)) xoaNodeMonHoc(t, maXoa);
                    } else popupCanhBao("Loi: Khong tim thay Mon hoc " + maXoa, false);
                }
            }
        } else if (c == KEY_ESC) {
            delete[] arrMH;
            break; 
        }
        delete[] arrMH;
    }
}

// NHÓM GIAO DIỆN: QUẢN LÝ LỚP & SINH VIÊN
bool hienThiFormLop(Lop &lop, bool laThemMoi) {
    int x = 20, y = 8, w = 50, h = 6;
    SetBGColor(14); SetColor(0); // Nền vàng, chữ đen
    
    // Tô nền đặc
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        for (int j = 0; j <= w; j++) cout << " ";
    }
    
    // Vẽ viền
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(191);
    for (int i = 1; i < h; i++) { gotoxy(x, y + i); cout << char(179); gotoxy(x + w, y + i); cout << char(179); }
    gotoxy(x, y + h); cout << char(192); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(217);

    // Chữ hướng dẫn Enter/ESC
    gotoxy(x + w - 28, y + h); SetColor(4); cout << " Enter: OK    ESC: Exit "; SetColor(0);
    
    gotoxy(x + (w - 24) / 2, y + 1); 
    cout << (laThemMoi ? "--- THEM LOP MOI ---" : "--- SUA THONG TIN LOP ---");
    gotoxy(x + 1, y + 2); for(int i = 1; i < w; i++) cout << char(196);
    
    if (laThemMoi) {
        gotoxy(x + 5, y + 3); cout << "Ma Lop: ";
        if (!nhapChuoiCoCancel(lop.MALOP, 15)) { SetBGColor(0); SetColor(7); return false; }
    } else {
        gotoxy(x + 5, y + 3); cout << "Ma Lop: " << lop.MALOP << " (Khong the sua)";
    }

    gotoxy(x + 5, y + 4); cout << "Ten Lop: ";
    if (!nhapChuoiCoCancel(lop.TENLOP, 35)) { SetBGColor(0); SetColor(7); return false; }

    // Gọi hàm chuẩn hóa
    inHoaToanBo(lop.MALOP);
    chuanHoaChuoi(lop.TENLOP);

    SetBGColor(0); SetColor(7);
    return true; 
}

void quanLyLopToanDien(DS_LOP &dslop) {
    int limit = 15;
    int currentPage = 1;

    while (true) {
        clrscr();
        int totalPage = (dslop.n == 0) ? 1 : (dslop.n + limit - 1) / limit;
        if (currentPage > totalPage) currentPage = totalPage;

        int x = 10, y = 3; 
        gotoxy(x + 20, y - 2); SetColor(11); cout << "QUAN LY DANH SACH LOP"; SetColor(7);
        
        SetColor(9); 
        // Viền trên
        gotoxy(x, y); cout << char(218); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<35; i++) cout << char(196); cout << char(191); 
        
        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA LOP        " << char(179) << " TEN LOP                           " << char(179);
             
        // Viền giữa
        gotoxy(x, y + 2); cout << char(195); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<35; i++) cout << char(196); cout << char(180); 
        
        SetColor(7); 
        
        int toaDoY = y + 3;
        int start = (currentPage - 1) * limit;
        int end = (start + limit > dslop.n) ? dslop.n : start + limit;

        for (int i = start; i < end; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 1, toaDoY); cout << " " << i + 1;
            
            gotoxy(x + 6, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 8, toaDoY); cout << dslop.nodes[i].MALOP;
            
            gotoxy(x + 22, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 24, toaDoY); cout << dslop.nodes[i].TENLOP;
            
            gotoxy(x + 58, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
        
        SetColor(9);
        // Viền dưới
        gotoxy(x, toaDoY); cout << char(192); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<35; i++) cout << char(196); cout << char(217); 
        
        // Thanh công cụ Footer
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F1: Insert "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F2: Save "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F3: Edit "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " DEL: Delete "; SetBGColor(0); cout << "  ";
        SetColor(14); cout << "   [<- / ->] Trang " << currentPage << "/" << totalPage; SetColor(7);

        // Bắt sự kiện bàn phím
        char c = _getch();
        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_LEFT && currentPage > 1) currentPage--;
            else if (c == KEY_RIGHT && currentPage < totalPage) currentPage++;
            else if (c == KEY_F1) {
                if (dslop.n >= 100) { // MAXLOP (kiểm tra giới hạn mảng tĩnh)
                    popupCanhBao("Loi: Danh sach lop da day!", false);
                } else {
                    Lop lopMoi;
                    lopMoi.dssv = NULL; // Khởi tạo con trỏ sinh viên = NULL
                    if (hienThiFormLop(lopMoi, true)) {
                        if (timKiemLop(dslop, lopMoi.MALOP) != -1) {
                            popupCanhBao("Loi: Ma Lop da ton tai!", false);
                        } else {
                            dslop.nodes[dslop.n] = lopMoi; // Thêm vào mảng
                            dslop.n++;
                        }
                    }
                }
            }
            else if (c == KEY_F2) {
                ghiFileLop(dslop);
                gotoxy(x + 40, toaDoY + 3); SetColor(10); cout << "Da luu Data!"; SetColor(7);
                _getch();
            }
            else if (c == KEY_F3) {
                string maSua;
                if (popupNhapMa("--- SUA THONG TIN LOP ---", "Nhap Ma Lop: ", maSua)) {
                    int index = timKiemLop(dslop, maSua);
                    if (index != -1) hienThiFormLop(dslop.nodes[index], false);
                    else popupCanhBao("Loi: Khong tim thay Lop " + maSua, false);
                }
            }
            else if (c == KEY_DEL) {
                string maXoa;
                if (popupNhapMa("--- XOA LOP ---", "Nhap Ma Lop: ", maXoa)) {
                    int index = timKiemLop(dslop, maXoa);
                    if (index != -1) {
                        if (dslop.nodes[index].dssv != NULL) {
                            popupCanhBao("Loi: Lop dang co sinh vien, khong the xoa!", false);
                        } else {
                            if (popupCanhBao("Chac chan xoa Lop: " + maXoa + " ?", true)) {
                                for (int i = index; i < dslop.n - 1; i++) { // Dồn mảng
                                    dslop.nodes[i] = dslop.nodes[i + 1];
                                }
                                dslop.n--;
                            }
                        }
                    } else {
                        popupCanhBao("Loi: Khong tim thay Lop " + maXoa, false);
                    }
                }
            }
        } else if (c == KEY_ESC) {
            break; // Thoát ra Menu chính
        }
    }
}

bool hienThiFormSinhVien(Sinhvien &sv, bool laThemMoi) {
    int x = 15, y = 6, w = 55, h = 9;
    SetBGColor(14); SetColor(0); // Nền vàng, chữ đen
    
    // Tô nền đặc
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        for (int j = 0; j <= w; j++) cout << " ";
    }
    
    // Vẽ viền
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(191);
    for (int i = 1; i < h; i++) { gotoxy(x, y + i); cout << char(179); gotoxy(x + w, y + i); cout << char(179); }
    gotoxy(x, y + h); cout << char(192); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(217);

    gotoxy(x + w - 28, y + h); SetColor(4); cout << " Enter: OK    ESC: Exit "; SetColor(0);
    
    gotoxy(x + (w - 28) / 2, y + 1); 
    cout << (laThemMoi ? "--- THEM SINH VIEN MOI ---" : "--- SUA THONG TIN SINH VIEN ---");
    gotoxy(x + 1, y + 2); for(int i = 1; i < w; i++) cout << char(196);
    
    if (laThemMoi) {
        gotoxy(x + 3, y + 3); cout << "Ma SV: ";
        if (!nhapChuoiCoCancel(sv.MASV, 15)) { SetBGColor(0); SetColor(7); return false; }
    } else {
        gotoxy(x + 3, y + 3); cout << "Ma SV: " << sv.MASV << " (Khong the sua)";
    }

    gotoxy(x + 3, y + 4); cout << "Ho: ";
    if (!nhapChuoiCoCancel(sv.HO, 30)) { SetBGColor(0); SetColor(7); return false; }

    gotoxy(x + 3, y + 5); cout << "Ten: ";
    if (!nhapChuoiCoCancel(sv.TEN, 10)) { SetBGColor(0); SetColor(7); return false; }

    gotoxy(x + 3, y + 6); cout << "Phai (Nam/Nu): ";
    if (!nhapChuoiCoCancel(sv.PHAI, 3)) { SetBGColor(0); SetColor(7); return false; }

    gotoxy(x + 3, y + 7); cout << "So DT: ";
    if (!nhapChuoiCoCancel(sv.SODT, 15)) { SetBGColor(0); SetColor(7); return false; }

    // Gọi hàm chuẩn hóa
    inHoaToanBo(sv.MASV);
    chuanHoaChuoi(sv.HO);
    chuanHoaChuoi(sv.TEN);
    // Chuẩn hóa giới tính (phái) để lưu đồng nhất Nam/Nu
    chuanHoaChuoi(sv.PHAI);

    SetBGColor(0); SetColor(7);
    return true; 
}

void quanLySinhVienToanDien(DS_LOP &dslop) {
    string maLop;
    if (!popupNhapMa("--- QUAN LY SINH VIEN ---", "Nhap Ma Lop: ", maLop)) return; // Bấm ESC sẽ hủy
    
    int indexLop = timKiemLop(dslop, maLop);
    if (indexLop == -1) {
        popupCanhBao("Loi: Khong tim thay Lop " + maLop, false);
        return;
    }

    int limit = 15;
    int currentPage = 1;

    while (true) {
        clrscr();

        // CHÈN LỆNH SẮP XẾP VÀO ĐÂY (Trước khi đưa vào mảng kết xuất màn hình)
        sapXepSinhVien(dslop.nodes[indexLop].dssv);
        
        // 1. Chuyển DSLK sang mảng tạm để phân trang
        int total = 0;
        PTRSV p = dslop.nodes[indexLop].dssv;
        while (p != NULL) { total++; p = p->next; }

        Sinhvien* arrSV = NULL;
        if (total > 0) {
            arrSV = new Sinhvien[total];
            p = dslop.nodes[indexLop].dssv;
            for (int i = 0; i < total; i++) {
                arrSV[i] = p->sv;
                p = p->next;
            }
        }

        int totalPage = (total == 0) ? 1 : (total + limit - 1) / limit;
        if (currentPage > totalPage) currentPage = totalPage;

        // 2. Vẽ lưới (5 - 15 - 35 - 10 - 15)
        int x = 2, y = 3; 
        gotoxy(x + 20, y - 2); SetColor(11); cout << "DANH SACH SINH VIEN LOP: " << maLop; SetColor(7);
        
        SetColor(9); 
        gotoxy(x, y); cout << char(218); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<35; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<10; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(191); 
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA SV         " 
             << char(179) << " HO VA TEN                         " 
             << char(179) << " PHAI     " << char(179) << " SO DT         " << char(179);
             
        gotoxy(x, y + 2); cout << char(195); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<35; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<10; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(180); 
        SetColor(7); 
        
        // 3. In dữ liệu
        int toaDoY = y + 3;
        int start = (currentPage - 1) * limit;
        int end = (start + limit > total) ? total : start + limit;

        for (int i = start; i < end; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 1, toaDoY); cout << " " << i + 1;
            
            gotoxy(x + 6, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 8, toaDoY); cout << arrSV[i].MASV;
            
            gotoxy(x + 22, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 24, toaDoY); cout << arrSV[i].HO << " " << arrSV[i].TEN;
            
            gotoxy(x + 58, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 60, toaDoY); cout << arrSV[i].PHAI;
            
            gotoxy(x + 69, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 71, toaDoY); cout << arrSV[i].SODT;
            
            gotoxy(x + 85, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
        
        SetColor(9);
        gotoxy(x, toaDoY); cout << char(192); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<35; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<10; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(217); 
        
        // 4. Thanh công cụ Footer
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F1: Insert "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F2: Save "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F3: Edit "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " DEL: Delete "; SetBGColor(0); cout << "  ";
        SetColor(14); cout << "   [<- / ->] Trang " << currentPage << "/" << totalPage; SetColor(7);

        // 5. Bắt sự kiện bàn phím
        char c = _getch();
        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_LEFT && currentPage > 1) currentPage--;
            else if (c == KEY_RIGHT && currentPage < totalPage) currentPage++;
            else if (c == KEY_F1) {
                Sinhvien svMoi;
                if (hienThiFormSinhVien(svMoi, true)) {
                    if (kiemTraSinhVienTonTai(dslop, svMoi.MASV)) { // Quét toàn trường
                        popupCanhBao("Loi: Ma SV da ton tai o mot lop khac!", false);
                    } else {
                        insertSinhVien(dslop.nodes[indexLop].dssv, svMoi);
                    }
                }
            }
            else if (c == KEY_F2) {
                ghiFileLop(dslop);
                gotoxy(x + 40, toaDoY + 3); SetColor(10); cout << "Da luu Data!"; SetColor(7);
                _getch();
            }
            else if (c == KEY_F3) {
                string maSua;
                if (popupNhapMa("--- SUA SINH VIEN ---", "Nhap Ma SV: ", maSua)) {
                    // Tìm SV trong DSLK của Lớp hiện tại
                    PTRSV pSua = dslop.nodes[indexLop].dssv;
                    while (pSua != NULL && pSua->sv.MASV != maSua) pSua = pSua->next;
                    
                    if (pSua != NULL) {
                        hienThiFormSinhVien(pSua->sv, false);
                    } else {
                        popupCanhBao("Loi: SV khong ton tai trong Lop nay!", false);
                    }
                }
            }
            else if (c == KEY_DEL) {
                string maXoa;
                if (popupNhapMa("--- XOA SINH VIEN ---", "Nhap Ma SV: ", maXoa)) {
                    PTRSV pXoa = dslop.nodes[indexLop].dssv;
                    PTRSV truoc = NULL;
                    
                    while (pXoa != NULL && pXoa->sv.MASV != maXoa) {
                        truoc = pXoa;
                        pXoa = pXoa->next;
                    }
                    
                    if (pXoa != NULL) {
                        if (popupCanhBao("Chac chan xoa SV " + pXoa->sv.TEN + " ?", true)) {
                            if (truoc == NULL) dslop.nodes[indexLop].dssv = pXoa->next;
                            else truoc->next = pXoa->next;
                            delete pXoa;
                        }
                    } else {
                        popupCanhBao("Loi: SV khong ton tai trong Lop nay!", false);
                    }
                }
            }
        } else if (c == KEY_ESC) {
            if (arrSV != NULL) delete[] arrSV;
            break; // Thoát ra Menu
        }

        if (arrSV != NULL) delete[] arrSV; // Phải giải phóng mảng tạm sau mỗi lần lặp để tránh tràn RAM
    }
}

// NHÓM GIAO DIỆN: LỚP TÍN CHỈ & ĐĂNG KÝ
bool hienThiFormLTC(Loptinchi &ltc, bool laThemMoi) {
    int x = 15, y = 5, w = 55, h = 11;
    SetBGColor(14); SetColor(0); // Nền vàng, chữ đen
    
    // Tô nền đặc
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        for (int j = 0; j <= w; j++) cout << " ";
    }
    
    // Vẽ viền
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(191);
    for (int i = 1; i < h; i++) { gotoxy(x, y + i); cout << char(179); gotoxy(x + w, y + i); cout << char(179); }
    gotoxy(x, y + h); cout << char(192); 
    for (int i = 1; i < w; i++) cout << char(196); cout << char(217);

    gotoxy(x + w - 28, y + h); SetColor(4); cout << " Enter: OK    ESC: Exit "; SetColor(0);
    
    gotoxy(x + (w - 28) / 2, y + 1); 
    cout << (laThemMoi ? "--- MO LOP TIN CHI MOI ---" : "--- SUA LOP TIN CHI ---");
    gotoxy(x + 1, y + 2); for(int i = 1; i < w; i++) cout << char(196);
    
    string strMa, strHK, strNhom, strMin, strMax;

    if (laThemMoi) {
        gotoxy(x + 5, y + 3); cout << "Ma LTC (So nguyen): ";
        if (!nhapChuoiCoCancel(strMa, 5)) { SetBGColor(0); SetColor(7); return false; }
        if (!strMa.empty()) ltc.MALOPTC = stoi(strMa);
    } else {
        gotoxy(x + 5, y + 3); cout << "Ma LTC: " << ltc.MALOPTC << " (Khong the sua)";
    }

    gotoxy(x + 5, y + 4); cout << "Ma Mon Hoc: ";
    if (!nhapChuoiCoCancel(ltc.MAMH, 15)) { SetBGColor(0); SetColor(7); return false; }

    gotoxy(x + 5, y + 5); cout << "Nien Khoa (VD: 2026-2027): ";
    if (!nhapChuoiCoCancel(ltc.NIENKHOA, 10)) { SetBGColor(0); SetColor(7); return false; }

    gotoxy(x + 5, y + 6); cout << "Hoc Ky (1,2,3): ";
    if (!nhapChuoiCoCancel(strHK, 1)) { SetBGColor(0); SetColor(7); return false; }
    if (!strHK.empty()) ltc.HOCKY = stoi(strHK);

    gotoxy(x + 5, y + 7); cout << "Nhom: ";
    if (!nhapChuoiCoCancel(strNhom, 2)) { SetBGColor(0); SetColor(7); return false; }
    if (!strNhom.empty()) ltc.NHOM = stoi(strNhom);

    gotoxy(x + 5, y + 8); cout << "SV Toi thieu: ";
    if (!nhapChuoiCoCancel(strMin, 3)) { SetBGColor(0); SetColor(7); return false; }
    if (!strMin.empty()) ltc.SOSVMIN = stoi(strMin);

    gotoxy(x + 5, y + 9); cout << "SV Toi da: ";
    if (!nhapChuoiCoCancel(strMax, 3)) { SetBGColor(0); SetColor(7); return false; }
    if (!strMax.empty()) ltc.SOSVMAX = stoi(strMax);

    SetBGColor(0); SetColor(7);
    return true; 
}

void quanLyLopTinChiToanDien(DS_LTC &dsltc, treeMH dsmh) {
    int limit = 15;
    int currentPage = 1;

    while (true) {
        clrscr();
        
        // 1. Lọc các lớp chưa bị hủy vào mảng tạm
        int totalValid = 0;
        for (int i = 0; i < dsltc.n; i++) {
            if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->HUYLOP == false) totalValid++;
        }

        Loptinchi** arrLTC = NULL;
        if (totalValid > 0) {
            arrLTC = new Loptinchi*[totalValid];
            int idx = 0;
            for (int i = 0; i < dsltc.n; i++) {
                if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->HUYLOP == false) {
                    arrLTC[idx++] = dsltc.nodes[i];
                }
            }
        }

        int totalPage = (totalValid == 0) ? 1 : (totalValid + limit - 1) / limit;
        if (currentPage > totalPage) currentPage = totalPage;

        // 2. Vẽ lưới (5 - 8 - 15 - 12 - 5 - 6 - 5 - 5)
        int x = 2, y = 3; 
        gotoxy(x + 25, y - 2); SetColor(11); cout << "QUAN LY LOP TIN CHI"; SetColor(7);
        
        SetColor(9); 
        gotoxy(x, y); cout << char(218); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<8; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<12; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<6; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(194);
        for(int i=0; i<5; i++) cout << char(196); cout << char(191); 
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA LTC " << char(179) << " MA MON HOC    " 
             << char(179) << " NIEN KHOA  " << char(179) << " HK  " << char(179) << " NHOM " 
             << char(179) << " MIN " << char(179) << " MAX " << char(179);
             
        gotoxy(x, y + 2); cout << char(195); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<8; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<12; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<6; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(197);
        for(int i=0; i<5; i++) cout << char(196); cout << char(180); 
        SetColor(7); 
        
        // 3. In dữ liệu
        int toaDoY = y + 3;
        int start = (currentPage - 1) * limit;
        int end = (start + limit > totalValid) ? totalValid : start + limit;

        for (int i = start; i < end; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 1, toaDoY); cout << " " << i + 1;
            gotoxy(x + 6, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 8, toaDoY); cout << arrLTC[i]->MALOPTC;
            gotoxy(x + 15, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 17, toaDoY); cout << arrLTC[i]->MAMH;
            gotoxy(x + 31, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 33, toaDoY); cout << arrLTC[i]->NIENKHOA;
            gotoxy(x + 44, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 46, toaDoY); cout << arrLTC[i]->HOCKY;
            gotoxy(x + 50, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 52, toaDoY); cout << arrLTC[i]->NHOM;
            gotoxy(x + 57, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 59, toaDoY); cout << arrLTC[i]->SOSVMIN;
            gotoxy(x + 63, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 65, toaDoY); cout << arrLTC[i]->SOSVMAX;
            gotoxy(x + 69, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
        
        SetColor(9);
        gotoxy(x, toaDoY); cout << char(192); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<8; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<12; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<6; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(193);
        for(int i=0; i<5; i++) cout << char(196); cout << char(217); 
        
        // 4. Thanh công cụ Footer
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " F1: Insert "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " F2: Save "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " F3: Edit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " F5: Tu dong Huy Lop (< MIN) "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " DEL: Huy Lop "; SetBGColor(0); cout << " ";
        SetColor(14); cout << "  Trang " << currentPage << "/" << totalPage; SetColor(7);

        // 5. Bắt sự kiện
        char c = _getch();
        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_LEFT && currentPage > 1) currentPage--;
            else if (c == KEY_RIGHT && currentPage < totalPage) currentPage++;
            else if (c == KEY_F1) {
                if (dsltc.n >= 1000) { // MAX_LTC
                    popupCanhBao("Loi: Bo nho Lop Tin Chi da day!", false);
                } else {
                    Loptinchi ltcMoi;
                    ltcMoi.dssvdk = NULL;
                    ltcMoi.HUYLOP = false;
                    
                    if (hienThiFormLTC(ltcMoi, true)) {
                        // Kiem tra Ma Mon Hoc ton tai
                        if (timKiemMonHoc(dsmh, ltcMoi.MAMH) == NULL) {
                            popupCanhBao("Loi: Ma Mon Hoc khong ton tai!", false);
                        } else {
                            // Kiem tra Ma LTC ton tai
                            bool trung = false;
                            for (int i = 0; i < dsltc.n; i++) {
                                if (dsltc.nodes[i]->MALOPTC == ltcMoi.MALOPTC) trung = true;
                            }
                            
                            if (trung) popupCanhBao("Loi: Ma LTC da bi trung!", false);
                            else {
                                dsltc.nodes[dsltc.n] = new Loptinchi;
                                *dsltc.nodes[dsltc.n] = ltcMoi;
                                dsltc.n++;
                            }
                        }
                    }
                }
            }
            else if (c == KEY_F2) {
                ghiFileLTC(dsltc);
                gotoxy(x + 40, toaDoY + 3); SetColor(10); cout << "Da luu Data!"; SetColor(7);
                _getch();
            }
            else if (c == KEY_F3) {
                string strMaSua;
                if (popupNhapMa("--- SUA LOP TIN CHI ---", "Nhap Ma LTC: ", strMaSua)) {
                    int maSua = stoi(strMaSua);
                    int idx = -1;
                    for (int i = 0; i < dsltc.n; i++) {
                        if (dsltc.nodes[i]->MALOPTC == maSua && dsltc.nodes[i]->HUYLOP == false) idx = i;
                    }
                    
                    if (idx != -1) {
                        Loptinchi banSao = *dsltc.nodes[idx]; // Copy de sua
                        if (hienThiFormLTC(banSao, false)) {
                            if (timKiemMonHoc(dsmh, banSao.MAMH) == NULL) {
                                popupCanhBao("Loi: Ma Mon Hoc khong ton tai!", false);
                            } else {
                                *dsltc.nodes[idx] = banSao; // Ghi de neu hop le
                            }
                        }
                    } else popupCanhBao("Loi: Khong tim thay LTC " + strMaSua, false);
                }
            } 
            else if (c == KEY_F5) {
                if (popupCanhBao("Chay kiem tra va Huy cac Lop khong du " "SV Toi thieu?", true)) {
                    int soLopBiHuy = 0;
                    
                    // Duyệt toàn bộ mảng con trỏ LTC
                    for (int i = 0; i < dsltc.n; i++) {
                        // Chỉ xét các lớp đang hoạt động
                        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
                            
                            // Đếm số sinh viên đã đăng ký vào lớp này
                            int soSV = 0;
                            for (PTRDK p = dsltc.nodes[i]->dssvdk; p != NULL; p = p->next) {
                                soSV++;
                            }
                            
                            // So sánh với SOSVMIN
                            if (soSV < dsltc.nodes[i]->SOSVMIN) {
                                dsltc.nodes[i]->HUYLOP = true; // Đánh dấu Hủy
                                soLopBiHuy++;
                            }
                        }
                    }
                    
                    // Thông báo kết quả
                    string thongBao = "Hoan tat! Da huy " + to_string(soLopBiHuy) + " Lop Tin Chi.";
                    popupCanhBao(thongBao, false);
                }
            }
            else if (c == KEY_DEL) {
                string strMaXoa;
                if (popupNhapMa("--- HUY LOP TIN CHI ---", "Nhap Ma LTC: ", strMaXoa)) {
                    int maXoa = stoi(strMaXoa);
                    int idx = -1;
                    for (int i = 0; i < dsltc.n; i++) {
                        if (dsltc.nodes[i]->MALOPTC == maXoa && dsltc.nodes[i]->HUYLOP == false) idx = i;
                    }
                    
                    if (idx != -1) {
                        if (dsltc.nodes[idx]->dssvdk != NULL) {
                            popupCanhBao("Loi: Lop dang co SV dang ky, khong the huy!", false);
                        } else {
                            if (popupCanhBao("Chac chan Huy LTC " + strMaXoa + " ?", true)) {
                                dsltc.nodes[idx]->HUYLOP = true; // Huy mem
                            }
                        }
                    } else popupCanhBao("Loi: Khong tim thay LTC " + strMaXoa, false);
                }
            }
        } else if (c == KEY_ESC) {
            if (arrLTC != NULL) delete[] arrLTC;
            break; 
        }

        if (arrLTC != NULL) delete[] arrLTC; 
    }
}

void quanLyDangKyToanDien(DS_LTC &dsltc, DS_LOP &dslop) {
    string maSV;
    if (!popupNhapMa("--- DANG KY TIN CHI ---", "Nhap Ma SV: ", maSV)) return;
    
    if (!kiemTraSinhVienTonTai(dslop, maSV)) {
        popupCanhBao("Loi: Sinh vien khong ton tai trong he thong!", false);
        return;
    }

    int limit = 15;
    int currentPage = 1;

    while (true) {
        clrscr();
        
        // Lọc các lớp đang mở
        int totalValid = 0;
        for (int i = 0; i < dsltc.n; i++) {
            if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->HUYLOP == false) totalValid++;
        }

        Loptinchi** arrLTC = NULL;
        if (totalValid > 0) {
            arrLTC = new Loptinchi*[totalValid];
            int idx = 0;
            for (int i = 0; i < dsltc.n; i++) {
                if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->HUYLOP == false) arrLTC[idx++] = dsltc.nodes[i];
            }
        }

        int totalPage = (totalValid == 0) ? 1 : (totalValid + limit - 1) / limit;
        if (currentPage > totalPage) currentPage = totalPage;

        int x = 2, y = 3; 
        gotoxy(x + 10, y - 2); SetColor(11); 
        cout << "CONG DANG KY TIN CHI - SINH VIEN: " << maSV << " - " << layTenSinhVien(dslop, maSV); 
        SetColor(7);
        
        // Vẽ lưới (5 - 8 - 15 - 12 - 5)
        SetColor(9); 
        gotoxy(x, y); cout << char(218); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<8; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<12; i++) cout << char(196); cout << char(194); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(191); 
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA LTC " << char(179) << " MA MON HOC    " 
             << char(179) << " NIEN KHOA  " << char(179) << " HK  " << char(179);
             
        gotoxy(x, y + 2); cout << char(195); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<8; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<12; i++) cout << char(196); cout << char(197); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(180); 
        SetColor(7); 
        
        int toaDoY = y + 3;
        int start = (currentPage - 1) * limit;
        int end = (start + limit > totalValid) ? totalValid : start + limit;

        for (int i = start; i < end; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 1, toaDoY); cout << " " << i + 1;
            gotoxy(x + 6, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 8, toaDoY); cout << arrLTC[i]->MALOPTC;
            gotoxy(x + 15, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 17, toaDoY); cout << arrLTC[i]->MAMH;
            gotoxy(x + 31, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 33, toaDoY); cout << arrLTC[i]->NIENKHOA;
            gotoxy(x + 44, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 46, toaDoY); cout << arrLTC[i]->HOCKY;
            gotoxy(x + 50, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            
            // Highlight màu xanh lá nếu SV đã đăng ký lớp này
            PTRDK p = arrLTC[i]->dssvdk;
            bool daDangKy = false;
            while(p != NULL) { if (p->dk.MASV == maSV) daDangKy = true; p = p->next; }
            if (daDangKy) { gotoxy(x + 55, toaDoY); SetColor(10); cout << "[DA DANG KY]"; SetColor(7); }

            toaDoY++;
        }
        
        SetColor(9);
        gotoxy(x, toaDoY); cout << char(192); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<8; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<15; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<12; i++) cout << char(196); cout << char(193); 
        for(int i=0; i<5; i++) cout << char(196); cout << char(217); 
        
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F1: Dang Ky "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F2: Save "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " DEL: Huy Dang Ky "; SetBGColor(0); cout << "  ";
        SetColor(14); cout << "   [<- / ->] Trang " << currentPage << "/" << totalPage; SetColor(7);

        char c = _getch();
        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_LEFT && currentPage > 1) currentPage--;
            else if (c == KEY_RIGHT && currentPage < totalPage) currentPage++;
            else if (c == KEY_F1) {
                string strMaLTC;
                if (popupNhapMa("--- DANG KY ---", "Nhap Ma LTC: ", strMaLTC)) {
                    try {
                        int maLTC = stoi(strMaLTC);
                        int idx = -1;
                        for (int i = 0; i < dsltc.n; i++) {
                            if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC && !dsltc.nodes[i]->HUYLOP) idx = i;
                        }
                        
                        if (idx == -1) popupCanhBao("Loi: LTC khong ton tai!", false);
                        else {
                            // Check max sinh viên & trùng
                            int count = 0; bool trung = false;
                            PTRDK p = dsltc.nodes[idx]->dssvdk;
                            while(p != NULL) {
                                count++;
                                if (p->dk.MASV == maSV) trung = true;
                                p = p->next;
                            }
                            if (trung) popupCanhBao("Loi: Ban da dang ky LTC nay roi!", false);
                            else if (count >= dsltc.nodes[idx]->SOSVMAX) popupCanhBao("Loi: LTC da du so luong!", false);
                            else {
                                Dangky dkMoi; dkMoi.MASV = maSV; dkMoi.DIEM = 0;
                                insertDangKy(dsltc.nodes[idx]->dssvdk, dkMoi);
                            }
                        }
                    } catch(...) { popupCanhBao("Loi: Ma LTC phai la so!", false); }
                }
            }
            else if (c == KEY_F2) {
                // Đăng ký tín chỉ được lưu chung vào file dsltc
                ghiFileLTC(dsltc); 
                gotoxy(x + 40, toaDoY + 3); SetColor(10); cout << "Da luu Data!"; SetColor(7);
                _getch();
            }
            else if (c == KEY_DEL) {
                string strMaLTC;
                if (popupNhapMa("--- HUY DANG KY ---", "Nhap Ma LTC: ", strMaLTC)) {
                    try {
                        int maLTC = stoi(strMaLTC);
                        int idx = -1;
                        for (int i = 0; i < dsltc.n; i++) {
                            if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC && !dsltc.nodes[i]->HUYLOP) idx = i;
                        }
                        if (idx == -1) popupCanhBao("Loi: LTC khong ton tai!", false);
                        else {
                            PTRDK pXoa = dsltc.nodes[idx]->dssvdk;
                            PTRDK truoc = NULL;
                            while (pXoa != NULL && pXoa->dk.MASV != maSV) { truoc = pXoa; pXoa = pXoa->next; }
                            
                            if (pXoa == NULL) popupCanhBao("Loi: Ban chua dang ky LTC nay!", false);
                            else if (popupCanhBao("Chac chan Huy Dang ky?", true)) {
                                if (truoc == NULL) dsltc.nodes[idx]->dssvdk = pXoa->next;
                                else truoc->next = pXoa->next;
                                delete pXoa;
                            }
                        }
                    } catch(...) { popupCanhBao("Loi: Ma LTC phai la so!", false); }
                }
            }
        } else if (c == KEY_ESC) {
            if (arrLTC != NULL) delete[] arrLTC; break; 
        }
        if (arrLTC != NULL) delete[] arrLTC; 
    }
}

void quanLyDiemToanDien(DS_LTC &dsltc, DS_LOP &dslop) {
    string strMaLTC;
    if (!popupNhapMa("--- QUAN LY DIEM ---", "Nhap Ma LTC: ", strMaLTC)) return;
    
    int maLTC = 0, indexLTC = -1;
    try { maLTC = stoi(strMaLTC); } 
    catch(...) { popupCanhBao("Loi: Ma LTC phai la so!", false); return; }

    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC && !dsltc.nodes[i]->HUYLOP) {
            indexLTC = i; break;
        }
    }
    if (indexLTC == -1) { popupCanhBao("Loi: Lop Tin Chi khong ton tai hoac da huy!", false); return; }

    int limit = 15;
    int currentPage = 1;

    while (true) {
        clrscr();
        int totalDK = 0;
        PTRDK p = dsltc.nodes[indexLTC]->dssvdk;
        while (p != NULL) { totalDK++; p = p->next; }

        Dangky* arrDK = NULL;
        if (totalDK > 0) {
            arrDK = new Dangky[totalDK];
            p = dsltc.nodes[indexLTC]->dssvdk;
            for (int i = 0; i < totalDK; i++) { arrDK[i] = p->dk; p = p->next; }
        }

        int totalPage = (totalDK == 0) ? 1 : (totalDK + limit - 1) / limit;
        if (currentPage > totalPage) currentPage = totalPage;

        int x = 10, y = 3;
        gotoxy(x + 15, y - 2); SetColor(11); 
        cout << "BANG DIEM LOP TIN CHI: " << maLTC; SetColor(7);
        
        // Vẽ lưới (5 - 15 - 35 - 10)
        SetColor(9); 
        gotoxy(x, y); cout << char(218);
        for(int i=0; i<5; i++) cout << char(196); cout << char(194);
        for(int i=0; i<15; i++) cout << char(196); cout << char(194);
        for(int i=0; i<35; i++) cout << char(196); cout << char(194);
        for(int i=0; i<10; i++) cout << char(196); cout << char(191);
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MASV          " 
             << char(179) << " HO VA TEN                         " << char(179) << " DIEM     " << char(179);
             
        gotoxy(x, y + 2); cout << char(195);
        for(int i=0; i<5; i++) cout << char(196); cout << char(197);
        for(int i=0; i<15; i++) cout << char(196); cout << char(197);
        for(int i=0; i<35; i++) cout << char(196); cout << char(197);
        for(int i=0; i<10; i++) cout << char(196); cout << char(180);
        SetColor(7);
        
        int toaDoY = y + 3;
        int start = (currentPage - 1) * limit;
        int end = (start + limit > totalDK) ? totalDK : start + limit;

        for (int i = start; i < end; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 1, toaDoY); cout << " " << i + 1;
            gotoxy(x + 6, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 8, toaDoY); cout << arrDK[i].MASV;
            gotoxy(x + 22, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 24, toaDoY); cout << layTenSinhVien(dslop, arrDK[i].MASV);
            gotoxy(x + 58, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x + 60, toaDoY); cout << arrDK[i].DIEM;
            gotoxy(x + 69, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
        
        SetColor(9);
        gotoxy(x, toaDoY); cout << char(192);
        for(int i=0; i<5; i++) cout << char(196); cout << char(193);
        for(int i=0; i<15; i++) cout << char(196); cout << char(193);
        for(int i=0; i<35; i++) cout << char(196); cout << char(193);
        for(int i=0; i<10; i++) cout << char(196); cout << char(217);
        
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F2: Save "; SetBGColor(0); cout << "  ";
        SetBGColor(15); SetColor(0); cout << " F3: Nhap / Sua Diem "; SetBGColor(0); cout << "  ";
        SetColor(14); cout << "   [<- / ->] Trang " << currentPage << "/" << totalPage; SetColor(7);

        char c = _getch();
        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_LEFT && currentPage > 1) currentPage--;
            else if (c == KEY_RIGHT && currentPage < totalPage) currentPage++;
            else if (c == KEY_F2) {
                ghiFileLTC(dsltc);
                gotoxy(x + 40, toaDoY + 3); SetColor(10); cout << "Da luu Diem!"; SetColor(7);
                _getch();
            }
            else if (c == KEY_F3) {
                string maSua;
                if (popupNhapMa("--- TIM SINH VIEN ---", "Nhap Ma SV: ", maSua)) {
                    PTRDK pSua = dsltc.nodes[indexLTC]->dssvdk;
                    while (pSua != NULL && pSua->dk.MASV != maSua) pSua = pSua->next;
                    
                    if (pSua != NULL) {
                        string strDiem;
                        if (popupNhapMa("--- VAO DIEM ---", "Nhap Diem (0-10): ", strDiem)) {
                            try {
                                float d = stof(strDiem);
                                if (d >= 0 && d <= 10) pSua->dk.DIEM = d;
                                else popupCanhBao("Loi: Diem phai tu 0 den 10!", false);
                            } catch(...) { popupCanhBao("Loi: Vui long nhap dung dinh dang so!", false); }
                        }
                    } else popupCanhBao("Loi: Sinh vien chua dang ky lop nay!", false);
                }
            }
        } else if (c == KEY_ESC) {
            if (arrDK != NULL) delete[] arrDK; break; 
        }
        if (arrDK != NULL) delete[] arrDK; 
    }
}

// 1. Hàm hỗ trợ quét hệ thống để lấy điểm cao nhất của 1 Sinh Viên trong 1 Môn Học
float layDiemMonHoc(string maSV, string maMH, DS_LTC &dsltc) {
    float maxDiem = -1; // -1 nghĩa là chưa đăng ký hoặc chưa có điểm
    for (int i = 0; i < dsltc.n; i++) {
        // Tìm các lớp tín chỉ thuộc Môn Học này và chưa bị hủy
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP && dsltc.nodes[i]->MAMH == maMH) {
            // Duyệt danh sách sinh viên đăng ký của lớp đó
            for (PTRDK p = dsltc.nodes[i]->dssvdk; p != NULL; p = p->next) {
                if (p->dk.MASV == maSV) {
                    if (p->dk.DIEM > maxDiem) maxDiem = p->dk.DIEM;
                }
            }
        }
    }
    return maxDiem;
}

// 2. Màn hình Bảng Điểm Tổng Kết (Matrix 2D)
void inBangDiemTongKetToanDien(DS_LOP &dslop, DS_LTC &dsltc, treeMH dsmh) {
    string maLop;
    if (!popupNhapMa("--- BANG DIEM TONG KET ---", "Nhap Ma Lop: ", maLop)) return;
    
    int indexLop = timKiemLop(dslop, maLop);
    if (indexLop == -1) { popupCanhBao("Loi: Khong tim thay Lop " + maLop, false); return; }
    
    // 1. Chuẩn bị mảng Sinh viên (đã được sắp xếp nhờ hàm ở bước trước)
    sapXepSinhVien(dslop.nodes[indexLop].dssv);
    int totalSV = 0;
    for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL; p = p->next) totalSV++;
    if (totalSV == 0) { popupCanhBao("Loi: Lop chua co sinh vien!", false); return; }
    
    Sinhvien* arrSV = new Sinhvien[totalSV];
    int idx = 0;
    for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL; p = p->next) arrSV[idx++] = p->sv;
    
    // 2. Chuẩn bị mảng Môn học
    int totalMH = demSoMonHoc(dsmh);
    if (totalMH == 0) { popupCanhBao("Loi: Chua co mon hoc nao!", false); delete[] arrSV; return; }
    Monhoc* arrMH = new Monhoc[totalMH];
    int idxMH = 0;
    doMonHocVaoMang(dsmh, arrMH, idxMH); 
    
    // 3. Phân trang 2 chiều
    int limitSV = 15; // 15 SV / trang dọc
    int limitMH = 4;  // 4 Môn / trang ngang (để vừa chiều ngang Console)
    int curPageSV = 1, curPageMH = 1;
    int totalPageSV = (totalSV + limitSV - 1) / limitSV;
    int totalPageMH = (totalMH + limitMH - 1) / limitMH;
    
    while (true) {
        clrscr();
        int x = 2, y = 3;
        gotoxy(x + 10, y - 2); SetColor(11);
        cout << "BANG DIEM TONG KET LOP " << maLop << " (Danh sach da sap xep Alphabet)"; SetColor(7);
        
        int startSV = (curPageSV - 1) * limitSV;
        int endSV = (startSV + limitSV > totalSV) ? totalSV : startSV + limitSV;
        
        int startMH = (curPageMH - 1) * limitMH;
        int endMH = (startMH + limitMH > totalMH) ? totalMH : startMH + limitMH;
        int soMonHienThi = endMH - startMH; // Tùy biến lưới dựa trên số môn hiển thị
        
        // --- VẼ HEADER ---
        SetColor(9);
        gotoxy(x, y); cout << char(218);
        for(int i=0; i<5; i++) cout << char(196); cout << char(194);
        for(int i=0; i<15; i++) cout << char(196); cout << char(194);
        for(int i=0; i<35; i++) cout << char(196); cout << char(194);
        for(int k=0; k<soMonHienThi; k++) {
            for(int i=0; i<12; i++) cout << char(196); cout << (k == soMonHienThi - 1 ? char(191) : char(194));
        }
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MASV          " << char(179) << " HO VA TEN                         " << char(179);
        for(int k=startMH; k<endMH; k++) {
            string maStr = arrMH[k].MAMH;
            while(maStr.length() < 10) maStr += " "; // Đệm cho đủ 10 ký tự
            cout << " " << maStr << " " << char(179);
        }
        
        gotoxy(x, y + 2); cout << char(195);
        for(int i=0; i<5; i++) cout << char(196); cout << char(197);
        for(int i=0; i<15; i++) cout << char(196); cout << char(197);
        for(int i=0; i<35; i++) cout << char(196); cout << char(197);
        for(int k=0; k<soMonHienThi; k++) {
            for(int i=0; i<12; i++) cout << char(196); cout << (k == soMonHienThi - 1 ? char(180) : char(197));
        }
        SetColor(7);
        
        // --- IN DỮ LIỆU ĐIỂM SỐ ---
        int toaDoY = y + 3;
        for (int i = startSV; i < endSV; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+1, toaDoY); cout << " " << i + 1;
            gotoxy(x+6, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+8, toaDoY); cout << arrSV[i].MASV;
            gotoxy(x+22, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+24, toaDoY); cout << arrSV[i].HO << " " << arrSV[i].TEN;
            
            int cotX = x + 58;
            for (int k = startMH; k < endMH; k++) {
                gotoxy(cotX, toaDoY); SetColor(9); cout << char(179); SetColor(7);
                float diem = layDiemMonHoc(arrSV[i].MASV, arrMH[k].MAMH, dsltc);
                gotoxy(cotX + 4, toaDoY); 
                if (diem == -1) cout << "-"; // Chưa có điểm
                else cout << diem;
                cotX += 13;
            }
            gotoxy(cotX, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
        
        // --- VẼ VIỀN DƯỚI ---
        SetColor(9);
        gotoxy(x, toaDoY); cout << char(192);
        for(int i=0; i<5; i++) cout << char(196); cout << char(193);
        for(int i=0; i<15; i++) cout << char(196); cout << char(193);
        for(int i=0; i<35; i++) cout << char(196); cout << char(193);
        for(int k=0; k<soMonHienThi; k++) {
            for(int i=0; i<12; i++) cout << char(196); cout << (k == soMonHienThi - 1 ? char(217) : char(193));
        }
        
        // --- THANH ĐIỀU HƯỚNG ---
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << "  ";
        SetColor(14); cout << "[W / S]: Cuon SV (" << curPageSV << "/" << totalPageSV << ")   ";
        cout << "[A / D]: Cuon Mon Hoc (" << curPageMH << "/" << totalPageMH << ")"; SetColor(7);

        // --- BẮT SỰ KIỆN PHÍM ĐIỀU HƯỚNG 4 CHIỀU ---
        char c = _getch();
        if (c == KEY_ESC) { break; }
        // Nhận cả phím W/A/S/D hoặc 4 Phím mũi tên
        else if (c == 'w' || c == 'W' || c == KEY_UP) { if (curPageSV > 1) curPageSV--; }
        else if (c == 's' || c == 'S' || c == KEY_DOWN) { if (curPageSV < totalPageSV) curPageSV++; }
        else if (c == 'a' || c == 'A' || c == KEY_LEFT) { if (curPageMH > 1) curPageMH--; }
        else if (c == 'd' || c == 'D' || c == KEY_RIGHT) { if (curPageMH < totalPageMH) curPageMH++; }
        else if (c == -32) {
            c = _getch();
            if (c == KEY_UP) { if (curPageSV > 1) curPageSV--; }
            else if (c == KEY_DOWN) { if (curPageSV < totalPageSV) curPageSV++; }
            else if (c == KEY_LEFT) { if (curPageMH > 1) curPageMH--; }
            else if (c == KEY_RIGHT) { if (curPageMH < totalPageMH) curPageMH++; }
        }
    }
    
    // Đừng quên giải phóng mảng
    delete[] arrSV;
    delete[] arrMH;
}

// Hàm In Bảng điểm cá nhân và Tính GPA
void inBangDiemCaNhan(DS_LOP &dslop, DS_LTC &dsltc, treeMH dsmh) {
    string maSV;
    if (!popupNhapMa("--- BANG DIEM CA NHAN ---", "Nhap Ma SV: ", maSV)) return;
    
    if (!kiemTraSinhVienTonTai(dslop, maSV)) {
        popupCanhBao("Loi: Khong tim thay Sinh vien!", false);
        return;
    }
    string tenSV = layTenSinhVien(dslop, maSV);

    // 1. Quét hệ thống và gom dữ liệu điểm
    KetQuaHocTap arrKQ[500]; // Mảng tạm chứa kết quả
    int soMon = 0;
    
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            for (PTRDK p = dsltc.nodes[i]->dssvdk; p != NULL; p = p->next) {
                if (p->dk.MASV == maSV) {
                    treeMH nodeMH = timKiemMonHoc(dsmh, dsltc.nodes[i]->MAMH);
                    if (nodeMH != NULL) {
                        // Kiểm tra xem môn này đã có trong mảng tạm chưa (Xử lý học lại)
                        bool daCo = false;
                        for (int k = 0; k < soMon; k++) {
                            if (arrKQ[k].MAMH == dsltc.nodes[i]->MAMH) {
                                daCo = true;
                                // Nếu học lại, chỉ lấy điểm cao nhất
                                if (p->dk.DIEM > arrKQ[k].DIEM) arrKQ[k].DIEM = p->dk.DIEM;
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

    // 2. Tính GPA
    float tongDiem = 0;
    int tongTC = 0;
    for (int i = 0; i < soMon; i++) {
        tongDiem += arrKQ[i].DIEM * arrKQ[i].STC;
        tongTC += arrKQ[i].STC;
    }
    float gpa = (tongTC == 0) ? 0 : tongDiem / tongTC;

    // 3. Phân trang và Kết xuất lưới
    int limit = 10;
    int currentPage = 1;
    int totalPage = (soMon == 0) ? 1 : (soMon + limit - 1) / limit;

    while (true) {
        clrscr();
        int x = 5, y = 3;
        gotoxy(x + 10, y - 2); SetColor(11);
        cout << "BANG DIEM CA NHAN - SV: " << maSV << " - " << tenSV; SetColor(7);
        
        SetColor(9);
        gotoxy(x, y); cout << char(218);
        for(int i=0; i<5; i++) cout << char(196); cout << char(194);
        for(int i=0; i<15; i++) cout << char(196); cout << char(194);
        for(int i=0; i<35; i++) cout << char(196); cout << char(194);
        for(int i=0; i<8; i++) cout << char(196); cout << char(194);
        for(int i=0; i<8; i++) cout << char(196); cout << char(191);
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA MON HOC    " << char(179) << " TEN MON HOC                       " 
             << char(179) << " SO TC  " << char(179) << " DIEM   " << char(179);
             
        gotoxy(x, y + 2); cout << char(195);
        for(int i=0; i<5; i++) cout << char(196); cout << char(197);
        for(int i=0; i<15; i++) cout << char(196); cout << char(197);
        for(int i=0; i<35; i++) cout << char(196); cout << char(197);
        for(int i=0; i<8; i++) cout << char(196); cout << char(197);
        for(int i=0; i<8; i++) cout << char(196); cout << char(180);
        SetColor(7);
        
        int toaDoY = y + 3;
        int start = (currentPage - 1) * limit;
        int end = (start + limit > soMon) ? soMon : start + limit;

        for (int i = start; i < end; i++) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+1, toaDoY); cout << " " << i + 1;
            gotoxy(x+6, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+8, toaDoY); cout << arrKQ[i].MAMH;
            gotoxy(x+22, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+24, toaDoY); cout << arrKQ[i].TENMH;
            gotoxy(x+58, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+60, toaDoY); cout << arrKQ[i].STC;
            gotoxy(x+67, toaDoY); SetColor(9); cout << char(179); SetColor(7); gotoxy(x+69, toaDoY); cout << arrKQ[i].DIEM;
            gotoxy(x+76, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
        
        SetColor(9);
        gotoxy(x, toaDoY); cout << char(192);
        for(int i=0; i<5; i++) cout << char(196); cout << char(193);
        for(int i=0; i<15; i++) cout << char(196); cout << char(193);
        for(int i=0; i<35; i++) cout << char(196); cout << char(193);
        for(int i=0; i<8; i++) cout << char(196); cout << char(193);
        for(int i=0; i<8; i++) cout << char(196); cout << char(217);
        
        // In Thống kê GPA
        gotoxy(x, toaDoY + 1); SetColor(14);
        cout << "Tong so Tin chi Tich luy: " << tongTC << "   |   GPA (Thang diem 10): ";
        SetColor(10); cout << gpa; SetColor(7);

        gotoxy(x, toaDoY + 3); 
        SetBGColor(15); SetColor(0); cout << " ESC: Exit "; SetBGColor(0); cout << "  ";
        SetColor(14); cout << "   [<- / ->] Trang " << currentPage << "/" << totalPage; SetColor(7);

        char c = _getch();
        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_LEFT && currentPage > 1) currentPage--;
            else if (c == KEY_RIGHT && currentPage < totalPage) currentPage++;
        } else if (c == KEY_ESC) {
            break;
        }
    }
}

// NHÓM MENU VÀ ĐIỂM NEO KHỞI CHẠY
void veMenu(int chon) {
    int x = 25, y = 8; // Tọa độ đặt khung Menu (dời x sang trái một chút để căn giữa)
    int width = 58, height = soItem + 3; // Tăng độ rộng khung lên 58 để chứa đủ chữ dài
    
    // 1. Vẽ khung viền bao quanh
    veKhung(x, y, width, height);
    
    // 2. Vẽ Tiêu đề Menu
    gotoxy(x + 13, y + 1); 
    SetColor(11); // Màu xanh ngọc sáng
    cout << "=== HE THONG QUAN LY TIN CHI ===";
    SetColor(7);
    
    // 3. Vẽ đường gạch ngang ngăn cách tiêu đề
    gotoxy(x + 1, y + 2);
    SetColor(9);
    for(int i = 1; i < width; i++) cout << char(196);
    SetColor(7);
    
    // 4. In các Item với hiệu ứng Highlight
    for (int i = 0; i < soItem; i++) {
        gotoxy(x + 3, y + 3 + i); // Lùi vào 3 khoảng trắng so với khung
        
        if (i == chon) {
            SetBGColor(11); // Nền xanh ngọc
            SetColor(0);    // Chữ màu đen cho nổi bật
        } else {
            SetBGColor(0);  // Nền đen
            SetColor(15);   // Chữ trắng tinh
        }
        
        // In text
        cout << menuMain[i];
        
        // Tính toán khoảng trắng dư để highlight dài đều nhau chạm sát viền phải
        int spaceCount = (width - 4) - (int)menuMain[i].length();
        if (spaceCount > 0) {
            for(int j = 0; j < spaceCount; j++) cout << " ";
        }
        
        SetBGColor(0); // Trả lại nền đen lập tức
    }
}

int hienThiMenuCon(string tieuDe, const string menu[], int soItem) {
    int chon = 0; 
    int x = 35, y = 8; // Vị trí khung Sub-menu
    int width = 45, height = soItem + 3;

    while (true) {
        // 1. Vẽ khung
        veKhung(x, y, width, height);
        
        // 2. Vẽ Tiêu đề
        gotoxy(x + (width - tieuDe.length()) / 2, y + 1); // Căn giữa tiêu đề
        SetColor(11);
        cout << tieuDe;
        SetColor(7);
        
        // 3. Kẻ gạch ngang
        gotoxy(x + 1, y + 2);
        SetColor(9);
        for(int i = 1; i < width; i++) cout << char(196);
        SetColor(7);

        // 4. In các lựa chọn
        for (int i = 0; i < soItem; i++) {
            gotoxy(x + 5, y + 3 + i);
            
            if (i == chon) {
                SetBGColor(11); // Highlight nền xanh ngọc
                SetColor(0);    // Chữ đen
            } else {
                SetBGColor(0);
                SetColor(15);
            }
            
            cout << menu[i];
            
            // Padding khoảng trắng an toàn
            int spaceCount = (width - 10) - (int)menu[i].length();
            if (spaceCount > 0) {
                for(int j = 0; j < spaceCount; j++) cout << " ";
            }
            SetBGColor(0);
        }

        // 5. Bắt phím điều hướng
        char c = _getch();
        if (c == -32) {
            c = _getch();
            if (c == KEY_UP) {
                chon--;
                if (chon < 0) chon = soItem - 1;
            } else if (c == KEY_DOWN) {
                chon++;
                if (chon >= soItem) chon = 0;
            }
        } else if (c == KEY_ENTER) {
            return chon; // Trả về index người dùng chọn (0, 1, 2...)
        } else if (c == KEY_ESC) {
            return -1;   // Bấm ESC để thoát về menu chính
        }
    }
}

// Sửa 1: Thêm tham số truyền vào hàm (sử dụng tham chiếu &)
void xuLyMenu(treeMH &dsmh, DS_LOP &dslop, DS_LTC &dsltc) { 
    int chon = 0; 
    int soItemThucTe = 6; 
    
    // 1. Dọn dẹp sạch sẽ 1 lần duy nhất TRƯỚC KHI vào vòng lặp Menu chính
    SetBGColor(0); SetColor(7); 
    system("cls"); 
    
    while (true) {
        veMenu(chon); // Vẽ đè lên vị trí cũ bằng gotoxy, không xóa toàn màn hình nên không nhấp nháy
        
        char c = _getch();
        if (c == -32) { 
            c = _getch();
            if (c == KEY_UP) {
                chon--;
                if (chon < 0) chon = soItemThucTe - 1; 
            } else if (c == KEY_DOWN) {
                chon++;
                if (chon >= soItemThucTe) chon = 0; 
            }
        } else if (c == KEY_ENTER) {
            // Khi bấm Enter VÀO một chức năng, ta xóa màn hình để nhường chỗ cho Giao diện mới
            SetBGColor(0); SetColor(7); system("cls");
            
            switch (chon) {
                case 0: { 
                    quanLyMonHocToanDien(dsmh);
                    break;
                }
                case 1: { 
                    while (true) {
                        SetBGColor(0); SetColor(7); system("cls");
                        const string menuLop[] = {
                            "1. Quan ly Danh sach Lop (F1-F3-DEL)", 
                            "2. Quan ly Sinh vien theo Lop (F1-F3-DEL)", 
                            "3. Quay lai"
                        };
                        int luaChonLop = hienThiMenuCon("QUAN LY LOP & SINH VIEN", menuLop, 3);
                        
                        if (luaChonLop == 0) quanLyLopToanDien(dslop);
                        else if (luaChonLop == 1) quanLySinhVienToanDien(dslop);
                        else break; 
                    }
                    break;
                }
                case 2: { 
                    quanLyLopTinChiToanDien(dsltc, dsmh);
                    break;
                }
                case 3: { 
                    quanLyDangKyToanDien(dsltc, dslop);
                    break;
                }
                case 4: { 
                    while (true) {
                        SetBGColor(0); SetColor(7); system("cls");
                        const string menuDiem[] = {
                            "1. Nhap / Sua Diem (Theo LTC)",
                            "2. In Bang Diem Tong Ket Lop (Ma tran 2D)",
                            "3. In Bang Diem Ca Nhan & Tinh GPA", 
                            "4. Quay lai"
                        };
                        int luaChonDiem = hienThiMenuCon("QUAN LY DIEM & BAO CAO KET QUA", menuDiem, 4);
                        
                        if (luaChonDiem == 0) quanLyDiemToanDien(dsltc, dslop);
                        else if (luaChonDiem == 1) inBangDiemTongKetToanDien(dslop, dsltc, dsmh); 
                        else if (luaChonDiem == 2) inBangDiemCaNhan(dslop, dsltc, dsmh);
                        else break; 
                    }
                    break;
                }
                case 5: { 
                    if (popupCanhBao("Ban co muon luu thay doi truoc khi thoat?", true)) {
                        ghiFileMonHoc(dsmh); ghiFileLop(dslop); ghiFileLTC(dsltc); 
                        SetBGColor(0); SetColor(7); system("cls");
                        gotoxy(10, 5); SetColor(10);
                        cout << "Da luu toan bo du lieu xuong o cung. Tam biet!";
                    } else {
                        SetBGColor(0); SetColor(7); system("cls");
                        gotoxy(10, 5); SetColor(11);
                        cout << "Da thoat chuong trinh (Khong luu thay doi). Tam biet!";
                    }
                    SetColor(7); gotoxy(10, 7);
                    return; 
                }
            }
            
            // SAU KHI THOÁT từ một chức năng con về Menu chính, dọn dẹp lại màn hình một lần nữa
            SetBGColor(0); SetColor(7); system("cls");
            
        } else if (c == KEY_ESC) {
            break;
        }
    }
}

int main(){
    treeMH dsmh = NULL;
    static DS_LTC dsltc;
    static DS_LOP dslop;

    docFileMonHoc(dsmh);
    docFileLop(dslop);
    docFileLTC(dsltc);

    xuLyMenu(dsmh, dslop, dsltc);
    return 0;
}