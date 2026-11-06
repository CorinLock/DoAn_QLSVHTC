#include "model.h"
#include "logic.h"
#include "Ui.h"

int main() {
    // 0. Cấu hình console kích thước chuẩn và ẩn con trỏ chống giật màn hình
    caiDatConsole();

    // Khởi tạo các cấu trúc dữ liệu chính của hệ thống
    treeMH dsmh = NULL;
    static DS_LTC dsltc;
    static DS_LOP dslop;

    // 1. Đọc dữ liệu từ file khi khởi động chương trình
    docFileMonHoc(dsmh);
    docFileLop(dslop);
    docFileLTC(dsltc);

    // 2. Khởi chạy giao diện Menu điều khiển
    xuLyMenu(dsmh, dslop, dsltc);

    // 3. Giải phóng toàn bộ bộ nhớ động khi thoát chương trình
    giaiPhongToanBoHeThong(dsmh, dslop, dsltc);

    return 0;
}
