#include "mylib.h"
#include <bits/stdc++.h>
#include <fstream>
#include <conio.h>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27

const string menuMain[] = {
    "1. Quan ly Mon Hoc",
    "2. Quan ly Lop - Sinh Vien",
    "3. Quan ly Lop Tin Chi",
    "4. Dang ky Tin Chi (Diem)",
    "5. Thoat chuong trinh"
};
const int soItem = 5;
const int MAXLOP = 10000;
const int MAXLTC = 10000;

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

void insertSinhVien(PTRSV &First, Sinhvien sv) {
    // Cấp phát vùng nhớ cho node sinh viên mới
    PTRSV p = new nodeSV;
    p->sv = sv;
    
    // Nối node mới vào đầu danh sách
    p->next = First;
    First = p;
}

// Trả về chỉ số vị trí index nếu tìm thấy, ngược lại trả về -1
int timKiemLop(DS_LOP &dslop, string maLop) {
    for (int i = 0; i < dslop.n; i++) {
        if (dslop.nodes[i].MALOP == maLop) {
            return i;
        }
    }
    return -1;
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

void nhapMonHoc(treeMH &t) {
    clrscr(); // Dùng hàm xóa màn hình của mylib
    Monhoc mh;
    gotoxy(10, 5); cout << "--- NHAP THONG TIN MON HOC ---";
    
    gotoxy(10, 7); cout << "Nhap Ma Mon Hoc: ";
    cin >> mh.MAMH;
    
    cin.ignore(); // Xóa bộ đệm trước khi nhập chuỗi có khoảng trắng
    gotoxy(10, 8); cout << "Nhap Ten Mon Hoc: ";
    getline(cin, mh.TENMH);
    
    gotoxy(10, 9); cout << "So Tin Chi Ly Thuyet: ";
    cin >> mh.STC_LT;
    
    gotoxy(10, 10); cout << "So Tin Chi Thuc Hanh: ";
    cin >> mh.STC_TH;
    
    // Gọi hàm insertMH đã viết ở Giai đoạn 1 để đưa vào cây BST
    insertMH(t, mh);
    
    gotoxy(10, 12); 
    SetColor(10); // Đổi chữ màu xanh lá báo thành công
    cout << "Them mon hoc thanh cong!";
    SetColor(7); // Trả lại màu xám mặc định
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

void inDanhSachMonHoc(treeMH t) {
    clrscr();
    int x = 5, y = 3; // Dịch màn hình sang trái một chút để bảng 75 ký tự đứng giữa
    
    gotoxy(x + 25, y - 2); SetColor(11); cout << "DANH SACH MON HOC"; SetColor(7);
    SetColor(9);
    
    // 1. Vẽ viền trên
    gotoxy(x, y);
    cout << char(218);
    for(int i=0; i<15; i++) cout << char(196); cout << char(194);
    for(int i=0; i<35; i++) cout << char(196); cout << char(194);
    for(int i=0; i<10; i++) cout << char(196); cout << char(194);
    for(int i=0; i<10; i++) cout << char(196); cout << char(191);
    
    // 2. Vẽ Header
    gotoxy(x, y + 1);
    cout << char(179) << " MA MON HOC    " 
         << char(179) << " TEN MON HOC                       " 
         << char(179) << " STC LT   " 
         << char(179) << " STC TH   " << char(179);
         
    // 3. Vẽ đường ngăn cách Header
    gotoxy(x, y + 2);
    cout << char(195);
    for(int i=0; i<15; i++) cout << char(196); cout << char(197);
    for(int i=0; i<35; i++) cout << char(196); cout << char(197);
    for(int i=0; i<10; i++) cout << char(196); cout << char(197);
    for(int i=0; i<10; i++) cout << char(196); cout << char(180);
    
    SetColor(7);
    
    // 4. In dữ liệu môn học
    int toaDoY = y + 3;
    duyetInOrder(t, toaDoY, x); // Gọi hàm đệ quy đã cập nhật tham số X
    
    SetColor(9);
    // 5. Vẽ viền dưới
    gotoxy(x, toaDoY);
    cout << char(192);
    for(int i=0; i<15; i++) cout << char(196); cout << char(193);
    for(int i=0; i<35; i++) cout << char(196); cout << char(193);
    for(int i=0; i<10; i++) cout << char(196); cout << char(193);
    for(int i=0; i<10; i++) cout << char(196); cout << char(217);
    SetColor(7);
    
    gotoxy(x, toaDoY + 2);
    cout << "Nhan phim bat ky de quay lai...";
    _getch();
}

void nhapLop(DS_LOP &dslop) {
    clrscr();
    Lop lopMoi;
    gotoxy(10, 5); cout << "--- NHAP THONG TIN LOP ---";
    
    gotoxy(10, 7); cout << "Nhap Ma Lop (VD: D25CQCN): ";
    cin >> lopMoi.MALOP;
    
    // Kiểm tra trùng mã lớp (sử dụng hàm timKiemLop đã viết ở Giai đoạn 1)
    if (timKiemLop(dslop, lopMoi.MALOP) != -1) {
        gotoxy(10, 9); SetColor(12); // Màu đỏ báo lỗi
        cout << "Loi: Ma lop da ton tai!";
        SetColor(7);
        return;
    }
    
    cin.ignore();
    gotoxy(10, 8); cout << "Nhap Ten Lop: ";
    getline(cin, lopMoi.TENLOP);
    
    lopMoi.dssv = NULL; // Khởi tạo DSLK sinh viên của lớp này là rỗng
    
    if (insertLop(dslop, lopMoi)) {
        gotoxy(10, 10); SetColor(10);
        cout << "Them lop thanh cong!";
        SetColor(7);
    }
}

void nhapSinhVien(DS_LOP &dslop) {
    clrscr();
    gotoxy(10, 5); cout << "--- THEM SINH VIEN VAO LOP ---";
    
    string maLop;
    gotoxy(10, 7); cout << "Nhap Ma Lop can them SV: ";
    cin >> maLop;
    
    int indexLop = timKiemLop(dslop, maLop);
    if (indexLop == -1) {
        gotoxy(10, 9); SetColor(12);
        cout << "Loi: Khong tim thay ma lop nay!";
        SetColor(7);
        return;
    }
    
    // Nếu tìm thấy lớp, tiến hành nhập thông tin SV
    Sinhvien sv;
    gotoxy(10, 9); cout << "Nhap Ma SV (VD: N25DCCN001): ";
    cin >> sv.MASV;
    
    // Lưu ý: Cần viết thêm hàm kiểm tra trùng Mã SV trên TOÀN BỘ các lớp (để sau nhé)
    
    cin.ignore();
    gotoxy(10, 10); cout << "Nhap Ho SV: ";
    getline(cin, sv.HO);
    
    gotoxy(10, 11); cout << "Nhap Ten SV: ";
    getline(cin, sv.TEN);
    
    gotoxy(10, 12); cout << "Phai (Nam/Nu): ";
    getline(cin, sv.PHAI);
    
    gotoxy(10, 13); cout << "So dien thoai: ";
    getline(cin, sv.SODT);
    
    // Truy cập vào con trỏ dssv của lớp vừa tìm được và thêm SV vào
    insertSinhVien(dslop.nodes[indexLop].dssv, sv);
    
    gotoxy(10, 15); SetColor(10);
    cout << "Them Sinh vien thanh cong vao lop " << maLop << "!";
    SetColor(7);
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

void nhapLopTinChi(DS_LTC &dsltc, treeMH dsmh) {
    clrscr();
    if (dsltc.n >= MAXLTC) {
        gotoxy(10, 5); SetColor(12); 
        cout << "Loi: Danh sach lop tin chi da day!"; 
        SetColor(7);
        return;
    }

    Loptinchi ltc;
    gotoxy(10, 5); cout << "--- MO LOP TIN CHI MOI ---";
    
    gotoxy(10, 7); cout << "Nhap Ma Mon Hoc: ";
    cin >> ltc.MAMH;
    
    // Gọi hàm tìm kiếm trên cây BST để kiểm tra
    if (!kiemTraMAMH(dsmh, ltc.MAMH)) {
        gotoxy(10, 9); SetColor(12);
        cout << "Loi: Ma mon hoc khong ton tai trong he thong!";
        SetColor(7);
        return;
    }
    
    cin.ignore();
    gotoxy(10, 8); cout << "Nhap Nien khoa (VD: 2026-2027): ";
    getline(cin, ltc.NIENKHOA);
    
    gotoxy(10, 9); cout << "Hoc ky (1/2/3): ";
    cin >> ltc.HOCKY;
    
    gotoxy(10, 10); cout << "Nhom: ";
    cin >> ltc.NHOM;
    
    gotoxy(10, 11); cout << "So SV Toi Thieu: ";
    cin >> ltc.SOSVMIN;
    
    gotoxy(10, 12); cout << "So SV Toi Da: ";
    cin >> ltc.SOSVMAX;
    
    // Tự động cấp phát Mã Lớp Tín Chỉ (MALOPTC) dựa trên số lượng lớp hiện có
    ltc.MALOPTC = dsltc.n + 1; 
    
    // Khởi tạo các giá trị mặc định cho Lớp tín chỉ
    ltc.HUYLOP = false;
    ltc.dssvdk = NULL; // Chưa có sinh viên nào đăng ký
    
    // Đưa vào mảng con trỏ (dùng hàm insertLTC đã có)
    if (insertLTC(dsltc, ltc)) {
        gotoxy(10, 14); SetColor(10);
        cout << "Mo lop tin chi thanh cong! MA LOP LTC cua ban la: " << ltc.MALOPTC;
        SetColor(7);
    }
}

void inDanhSachLop(DS_LOP &dslop) {
    clrscr();
    int x = 10, y = 3; // Tọa độ bắt đầu vẽ bảng
    
    // In tiêu đề
    gotoxy(x + 20, y - 2); 
    SetColor(11); 
    cout << "DANH SACH CAC LOP"; 
    SetColor(7);
    
    SetColor(9); // Màu xanh dương sáng cho viền bảng
    
    // 1. Vẽ viền trên cùng (Top Border)
    gotoxy(x, y);
    cout << char(218); // Góc trên trái
    for(int i=0; i<5; i++) cout << char(196); cout << char(194); // Ngã ba cột 1
    for(int i=0; i<15; i++) cout << char(196); cout << char(194); // Ngã ba cột 2
    for(int i=0; i<35; i++) cout << char(196); cout << char(191); // Góc trên phải
    
    // 2. Vẽ dòng Header (Tiêu đề cột)
    gotoxy(x, y + 1);
    cout << char(179) << " STT " 
         << char(179) << " MA LOP        " 
         << char(179) << " TEN LOP                           " 
         << char(179);
         
    // 3. Vẽ đường ngăn cách Header và Dữ liệu (Middle Border)
    gotoxy(x, y + 2);
    cout << char(195); // Ngã ba trái
    for(int i=0; i<5; i++) cout << char(196); cout << char(197); // Ngã tư cột 1
    for(int i=0; i<15; i++) cout << char(196); cout << char(197); // Ngã tư cột 2
    for(int i=0; i<35; i++) cout << char(196); cout << char(180); // Ngã ba phải
    
    SetColor(7); // Trả lại màu xám trắng cho dữ liệu
    
    // 4. In dữ liệu từng dòng
    int toaDoY = y + 3;
    for (int i = 0; i < dslop.n; i++) {
        // Vẽ vách ngăn dọc
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
    // 5. Vẽ viền dưới cùng (Bottom Border)
    gotoxy(x, toaDoY);
    cout << char(192); // Góc dưới trái
    for(int i=0; i<5; i++) cout << char(196); cout << char(193); // Ngã ba lên cột 1
    for(int i=0; i<15; i++) cout << char(196); cout << char(193); // Ngã ba lên cột 2
    for(int i=0; i<35; i++) cout << char(196); cout << char(217); // Góc dưới phải
    SetColor(7);
    
    // Tạm dừng để xem danh sách
    gotoxy(x, toaDoY + 2);
    cout << "Nhan phim bat ky de quay lai...";
    _getch();
}

void inSinhVienTheoLop(DS_LOP &dslop) {
    clrscr();
    string maLop;
    gotoxy(10, 3); cout << "Nhap Ma Lop can xem: ";
    cin >> maLop;
    
    int index = timKiemLop(dslop, maLop);
    if (index == -1) {
        gotoxy(10, 5); SetColor(12);
        cout << "Khong tim thay lop " << maLop;
        SetColor(7);
        return;
    }
    
    clrscr();
    int x = 5, y = 3;
    
    gotoxy(x + 20, y - 2); SetColor(11); 
    cout << "DANH SACH SINH VIEN LOP " << maLop; 
    SetColor(7);
    
    SetColor(9); // Màu xanh dương cho khung
    // 1. Viền trên
    gotoxy(x, y);
    cout << char(218);
    for(int i=0; i<15; i++) cout << char(196); cout << char(194);
    for(int i=0; i<35; i++) cout << char(196); cout << char(194);
    for(int i=0; i<10; i++) cout << char(196); cout << char(194);
    for(int i=0; i<15; i++) cout << char(196); cout << char(191);
    
    // 2. Header
    gotoxy(x, y + 1);
    cout << char(179) << " MASV          " 
         << char(179) << " HO VA TEN                         " 
         << char(179) << " PHAI     " 
         << char(179) << " SDT           " << char(179);
         
    // 3. Viền giữa ngăn cách Header
    gotoxy(x, y + 2);
    cout << char(195);
    for(int i=0; i<15; i++) cout << char(196); cout << char(197);
    for(int i=0; i<35; i++) cout << char(196); cout << char(197);
    for(int i=0; i<10; i++) cout << char(196); cout << char(197);
    for(int i=0; i<15; i++) cout << char(196); cout << char(180);
    
    SetColor(7);
    
    // 4. In dữ liệu sinh viên
    int toaDoY = y + 3;
    PTRSV p = dslop.nodes[index].dssv;
    
    while (p != NULL) {
        gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 2, toaDoY); cout << p->sv.MASV;
        
        gotoxy(x + 16, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 18, toaDoY); cout << p->sv.HO << " " << p->sv.TEN;
        
        gotoxy(x + 52, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 54, toaDoY); cout << p->sv.PHAI;
        
        gotoxy(x + 63, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        gotoxy(x + 65, toaDoY); cout << p->sv.SODT;
        
        gotoxy(x + 79, toaDoY); SetColor(9); cout << char(179); SetColor(7);
        
        toaDoY++;
        p = p->next;
    }
    
    SetColor(9);
    // 5. Viền dưới
    gotoxy(x, toaDoY);
    cout << char(192);
    for(int i=0; i<15; i++) cout << char(196); cout << char(193);
    for(int i=0; i<35; i++) cout << char(196); cout << char(193);
    for(int i=0; i<10; i++) cout << char(196); cout << char(193);
    for(int i=0; i<15; i++) cout << char(196); cout << char(217);
    SetColor(7);
    
    gotoxy(x, toaDoY + 2);
    cout << "Nhan phim bat ky de quay lai...";
    _getch();
}

void inDanhSachLTC(DS_LTC &dsltc) {
    clrscr();
    int x = 5, y = 3;
    
    gotoxy(x + 20, y - 2); SetColor(11); 
    cout << "DANH SACH LOP TIN CHI DA MO"; 
    SetColor(7);
    
    SetColor(9); // Màu xanh dương cho khung
    // 1. Viền trên
    gotoxy(x, y);
    cout << char(218);
    for(int i=0; i<10; i++) cout << char(196); cout << char(194);
    for(int i=0; i<15; i++) cout << char(196); cout << char(194);
    for(int i=0; i<12; i++) cout << char(196); cout << char(194);
    for(int i=0; i<5; i++) cout << char(196); cout << char(194);
    for(int i=0; i<5; i++) cout << char(196); cout << char(194);
    for(int i=0; i<8; i++) cout << char(196); cout << char(194);
    for(int i=0; i<8; i++) cout << char(196); cout << char(191);
    
    // 2. Header
    gotoxy(x, y + 1);
    cout << char(179) << " MA LTC   " 
         << char(179) << " MA MON HOC    " 
         << char(179) << " NIEN KHOA  " 
         << char(179) << " HK  " 
         << char(179) << " NHOM" 
         << char(179) << " SV MIN " 
         << char(179) << " SV MAX " << char(179);
         
    // 3. Viền giữa
    gotoxy(x, y + 2);
    cout << char(195);
    for(int i=0; i<10; i++) cout << char(196); cout << char(197);
    for(int i=0; i<15; i++) cout << char(196); cout << char(197);
    for(int i=0; i<12; i++) cout << char(196); cout << char(197);
    for(int i=0; i<5; i++) cout << char(196); cout << char(197);
    for(int i=0; i<5; i++) cout << char(196); cout << char(197);
    for(int i=0; i<8; i++) cout << char(196); cout << char(197);
    for(int i=0; i<8; i++) cout << char(196); cout << char(180);
    
    SetColor(7);
    
    // 4. In dữ liệu Lớp tín chỉ
    int toaDoY = y + 3;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->HUYLOP == false) {
            gotoxy(x, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 2, toaDoY); cout << dsltc.nodes[i]->MALOPTC;
            
            gotoxy(x + 11, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 13, toaDoY); cout << dsltc.nodes[i]->MAMH;
            
            gotoxy(x + 27, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 29, toaDoY); cout << dsltc.nodes[i]->NIENKHOA;
            
            gotoxy(x + 40, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 42, toaDoY); cout << dsltc.nodes[i]->HOCKY;
            
            gotoxy(x + 46, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 48, toaDoY); cout << dsltc.nodes[i]->NHOM;
            
            gotoxy(x + 52, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 54, toaDoY); cout << dsltc.nodes[i]->SOSVMIN;
            
            gotoxy(x + 61, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            gotoxy(x + 63, toaDoY); cout << dsltc.nodes[i]->SOSVMAX;
            
            gotoxy(x + 70, toaDoY); SetColor(9); cout << char(179); SetColor(7);
            toaDoY++;
        }
    }
    
    SetColor(9);
    // 5. Viền dưới
    gotoxy(x, toaDoY);
    cout << char(192);
    for(int i=0; i<10; i++) cout << char(196); cout << char(193);
    for(int i=0; i<15; i++) cout << char(196); cout << char(193);
    for(int i=0; i<12; i++) cout << char(196); cout << char(193);
    for(int i=0; i<5; i++) cout << char(196); cout << char(193);
    for(int i=0; i<5; i++) cout << char(196); cout << char(193);
    for(int i=0; i<8; i++) cout << char(196); cout << char(193);
    for(int i=0; i<8; i++) cout << char(196); cout << char(217);
    SetColor(7);
    
    gotoxy(x, toaDoY + 2);
    cout << "Nhan phim bat ky de quay lai...";
    _getch();
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

// Hàm chèn một lượt đăng ký vào đầu danh sách dssvdk
void insertDangKy(PTRDK &First, Dangky dk) {
    PTRDK p = new nodeDK;
    p->dk = dk;
    p->next = First;
    First = p;
}

void dangKyTinChi(DS_LTC &dsltc, DS_LOP &dslop) {
    clrscr();
    gotoxy(15, 5); cout << "--- DANG KY LOP TIN CHI ---";
    
    string maSV;
    gotoxy(10, 7); cout << "Nhap Ma Sinh Vien: ";
    cin >> maSV;
    
    if (!kiemTraSinhVienTonTai(dslop, maSV)) {
        gotoxy(10, 9); SetColor(12); 
        cout << "Loi: Ma Sinh Vien khong ton tai trong he thong!"; 
        SetColor(7);
        return;
    }
    
    int maLTC;
    gotoxy(10, 9); cout << "Nhap Ma Lop Tin Chi can dang ky: ";
    cin >> maLTC;
    
    // Tìm kiếm vị trí (index) của Lớp tín chỉ trong mảng con trỏ
    int indexLTC = -1;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC) {
            indexLTC = i;
            break;
        }
    }
    
    // Kiểm tra lớp tín chỉ có hợp lệ không
    if (indexLTC == -1 || dsltc.nodes[indexLTC]->HUYLOP == true) {
        gotoxy(10, 11); SetColor(12); 
        cout << "Loi: Lop Tin Chi khong ton tai hoac da bi huy!"; 
        SetColor(7);
        return;
    }
    
    // (Tuỳ chọn nâng cao: Bạn có thể viết thêm vòng lặp duyệt dsltc.nodes[indexLTC]->dssvdk 
    // để kiểm tra xem sinh viên này đã đăng ký lớp này trước đó chưa, hoặc lớp đã đầy chưa)
    
    // Tiến hành đăng ký
    Dangky dkMoi;
    dkMoi.MASV = maSV;
    dkMoi.DIEM = 0; // Điểm mặc định khi mới đăng ký là 0
    dkMoi.HUYDK = false;
    
    insertDangKy(dsltc.nodes[indexLTC]->dssvdk, dkMoi);
    
    gotoxy(10, 13); SetColor(10); 
    cout << "Sinh vien " << maSV << " dang ky vao lop " << maLTC << " thanh cong!"; 
    SetColor(7);
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

void nhapDiem(DS_LTC &dsltc, DS_LOP &dslop) {
    clrscr();
    gotoxy(15, 5); cout << "--- NHAP DIEM CHO LOP TIN CHI ---";
    
    int maLTC;
    gotoxy(10, 7); cout << "Nhap Ma Lop Tin Chi: ";
    cin >> maLTC;
    
    // Tìm vị trí của lớp tín chỉ
    int indexLTC = -1;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC) {
            indexLTC = i;
            break;
        }
    }
    
    if (indexLTC == -1 || dsltc.nodes[indexLTC]->HUYLOP == true) {
        gotoxy(10, 9); SetColor(12); 
        cout << "Loi: Lop Tin Chi khong ton tai hoac da bi huy!"; 
        SetColor(7);
        return;
    }
    
    PTRDK p = dsltc.nodes[indexLTC]->dssvdk;
    if (p == NULL) {
        gotoxy(10, 9); SetColor(6); // Màu vàng cảnh báo
        cout << "Lop tin chi nay chua co sinh vien nao dang ky!";
        SetColor(7);
        return;
    }
    
    // Bắt đầu duyệt danh sách đăng ký và nhập điểm
    int toaDoY = 10;
    while (p != NULL) {
        gotoxy(10, toaDoY); cout << "MASV: " << p->dk.MASV;
        gotoxy(30, toaDoY); cout << layTenSinhVien(dslop, p->dk.MASV);
        gotoxy(60, toaDoY); cout << "Nhap diem: ";
        cin >> p->dk.DIEM; // Cập nhật điểm trực tiếp vào node
        
        toaDoY++;
        p = p->next;
    }
    
    gotoxy(10, toaDoY + 2); SetColor(10);
    cout << "Da nhap diem xong cho lop LTC " << maLTC << "!";
    SetColor(7);
}

void inBangDiemLTC(DS_LTC &dsltc, DS_LOP &dslop) {
    clrscr();
    int maLTC;
    gotoxy(10, 5); cout << "Nhap Ma Lop Tin Chi can xem diem: ";
    cin >> maLTC;
    
    int indexLTC = -1;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && dsltc.nodes[i]->MALOPTC == maLTC) {
            indexLTC = i;
            break;
        }
    }
    
    if (indexLTC == -1 || dsltc.nodes[indexLTC]->HUYLOP == true) {
        gotoxy(10, 7); SetColor(12); cout << "Lop Tin Chi khong ton tai!"; SetColor(7);
        return;
    }
    
    clrscr();
    gotoxy(30, 2); cout << "BANG DIEM LOP TIN CHI: " << maLTC;
    
    // Header
    gotoxy(10, 4); cout << "MASV";
    gotoxy(25, 4); cout << "HO VA TEN";
    gotoxy(60, 4); cout << "DIEM";
    gotoxy(10, 5); for(int i=0; i<60; i++) cout << "-";
    
    PTRDK p = dsltc.nodes[indexLTC]->dssvdk;
    int toaDoY = 6;
    while (p != NULL) {
        gotoxy(10, toaDoY); cout << p->dk.MASV;
        gotoxy(25, toaDoY); cout << layTenSinhVien(dslop, p->dk.MASV);
        gotoxy(60, toaDoY); cout << p->dk.DIEM;
        toaDoY++;
        p = p->next;
    }
}

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

void veMenu(int chon) {
    int x = 35, y = 8; // Tọa độ đặt khung Menu
    int width = 40, height = soItem + 3;
    
    // 1. Vẽ khung viền bao quanh
    veKhung(x, y, width, height);
    
    // 2. Vẽ Tiêu đề Menu
    gotoxy(x + 10, y + 1); 
    SetColor(11); // Màu xanh ngọc sáng
    cout << "HE THONG QUAN LY TIN CHI";
    SetColor(7);
    
    // 3. Vẽ đường gạch ngang ngăn cách tiêu đề
    gotoxy(x + 1, y + 2);
    SetColor(9);
    for(int i = 1; i < width; i++) cout << char(196);
    SetColor(7);
    
    // 4. In các Item với hiệu ứng Highlight
    for (int i = 0; i < soItem; i++) {
        gotoxy(x + 5, y + 3 + i); // Lùi vào 5 khoảng trắng so với khung
        
        if (i == chon) {
            SetBGColor(11); // Nền xanh ngọc
            SetColor(0);    // Chữ màu đen cho nổi bật
        } else {
            SetBGColor(0);  // Nền đen
            SetColor(15);   // Chữ trắng tinh
        }
        
        // In text và thêm khoảng trắng để lấp đầy phần nền màu (padding)
        cout << menuMain[i];
        // Tính toán khoảng trắng dư để highlight dài đều nhau
        int spaceCount = 32 - (int)menuMain[i].length();
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
    while (true) {
        veMenu(chon);
        
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
            clrscr(); 
            
            switch (chon) {
                case 0: { // 1. Quan ly Mon Hoc
                    const string menuMH[] = {
                        "1. Them Mon Hoc moi", 
                        "2. Xem danh sach Mon Hoc", 
                        "3. Quay lai"
                    };
                    int luaChonMH = hienThiMenuCon("QUAN LY MON HOC", menuMH, 3);
                    
                    if (luaChonMH == 0) nhapMonHoc(dsmh);
                    else if (luaChonMH == 1) inDanhSachMonHoc(dsmh);
                    else break; // Chọn Quay lại hoặc bấm ESC
                    break;
                }
                case 1: { // 2. Quan ly Lop - Sinh Vien
                    const string menuLop[] = {
                        "1. Them Lop moi", 
                        "2. Them Sinh Vien vao Lop", 
                        "3. In danh sach Cac Lop", 
                        "4. In danh sach SV cua mot Lop",
                        "5. Quay lai"
                    };
                    int luaChonLop = hienThiMenuCon("QUAN LY LOP - SINH VIEN", menuLop, 5);
                    
                    if (luaChonLop == 0) nhapLop(dslop);
                    else if (luaChonLop == 1) nhapSinhVien(dslop);
                    else if (luaChonLop == 2) inDanhSachLop(dslop);
                    else if (luaChonLop == 3) inSinhVienTheoLop(dslop);
                    else break;
                    break;
                }
                case 2: { // 3. Quan ly Lop Tin Chi
                    const string menuLTC[] = {
                        "1. Mo Lop Tin Chi moi",
                        "2. In danh sach Lop Tin Chi",
                        "3. Quay lai"
                    };
                    int luaChonLTC = hienThiMenuCon("QUAN LY LOP TIN CHI", menuLTC, 3);
                    
                    if (luaChonLTC == 0) nhapLopTinChi(dsltc, dsmh);
                    else if (luaChonLTC == 1) inDanhSachLTC(dsltc);
                    else break;
                    break;
                }
                case 3: { // 4. Dang ky Tin Chi (Diem)
                    const string menuDK[] = {
                        "1. Dang ky Lop Tin Chi",
                        "2. Nhap Diem",
                        "3. In Bang Diem Lop Tin Chi",
                        "4. Quay lai"
                    };
                    int luaChonDK = hienThiMenuCon("DANG KY & NHAP DIEM", menuDK, 4);
                    
                    if (luaChonDK == 0) dangKyTinChi(dsltc, dslop);
                    else if (luaChonDK == 1) nhapDiem(dsltc, dslop);
                    else if (luaChonDK == 2) inBangDiemLTC(dsltc, dslop);
                    else break;
                    break;
                }
                case 4: { // 5. Thoat
                    ghiFileMonHoc(dsmh);
                    ghiFileLop(dslop);
                    ghiFileLTC(dsltc);

                    clrscr();
                    gotoxy(10, 5); SetColor(10);
                    cout << "Da luu du lieu. Tam biet!";
                    SetColor(7);
                    return; 
                }
            }
            
            gotoxy(10, 20); cout << "\nNhan phim bat ky de quay lai Menu...";
            _getch();
            clrscr();
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