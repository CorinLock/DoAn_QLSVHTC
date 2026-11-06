#pragma once
#ifndef UI_H
#define UI_H

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>

#include "model.h"
#include "logic.h"

using namespace std;

// ========================================================
// 0. TÍCH HỢP CÁC TIỆN ÍCH CONSOLE (TỪ MYLIB & TỐI ƯU HIỂN THỊ)
// ========================================================

// Bộ đệm trạng thái Console để triệt tiêu toàn bộ độ trễ IPC (Windows Console Subsystem)
static HANDLE s_hStdOut = NULL;
static WORD s_curTextColor = 7;
static WORD s_curBGColor = 0;
static bool s_cursorVisible = true;

inline HANDLE layHandleConsole() {
    if (!s_hStdOut) s_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    return s_hStdOut;
}

// Quản lý ẩn/hiện con trỏ Console (Ẩn khi điều hướng để chống nhấp nháy, kiểm tra cache để không gọi API thừa)
inline void anConTro() {
    if (!s_cursorVisible) return;
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 20;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(layHandleConsole(), &cursorInfo);
    s_cursorVisible = false;
}

inline void hienConTro() {
    if (s_cursorVisible) return;
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 20;
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(layHandleConsole(), &cursorInfo);
    s_cursorVisible = true;
}

// Cấu hình cửa sổ console kích thước cố định chống cuộn màn hình và giật hình (chạy 1 lần duy nhất an toàn)
inline void caiDatConsole() {
    static bool daCaiDat = false;
    if (daCaiDat) {
        anConTro();
        return;
    }
    daCaiDat = true;

    HANDLE hOut = layHandleConsole();
    if (hOut == INVALID_HANDLE_VALUE) return;

    SetConsoleTitleA("CHUONG TRINH QUAN LY SINH VIEN THEO HE TIN CHI");
    anConTro();

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        short targetW = 120;
        short targetH = 32;
        COORD buf = { (csbi.dwSize.X < targetW) ? targetW : csbi.dwSize.X, 
                      (csbi.dwSize.Y < targetH) ? targetH : csbi.dwSize.Y };
        SetConsoleScreenBufferSize(hOut, buf);

        SMALL_RECT win = { 0, 0, (short)(targetW - 1), (short)(targetH - 1) };
        SetConsoleWindowInfo(hOut, TRUE, &win);
    }
}

inline void gotoxy(short x, short y) {
    COORD Cursor_an_Pos = { x, y };
    SetConsoleCursorPosition(layHandleConsole(), Cursor_an_Pos);
}

inline int wherex() {
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    GetConsoleScreenBufferInfo(layHandleConsole(), &screen_buffer_info);
    return screen_buffer_info.dwCursorPosition.X;
}

// Đổi màu siêu tốc: Không gọi GetConsoleScreenBufferInfo, triệt tiêu 100% delay
inline void SetColor(WORD color) {
    s_curTextColor = color & 0x0F;
    SetConsoleTextAttribute(layHandleConsole(), (s_curBGColor << 4) | s_curTextColor);
}

inline void SetBGColor(WORD color) {
    s_curBGColor = color & 0x0F;
    SetConsoleTextAttribute(layHandleConsole(), (s_curBGColor << 4) | s_curTextColor);
}

inline void SetColorAndBG(WORD textColor, WORD bgColor) {
    s_curTextColor = textColor & 0x0F;
    s_curBGColor = bgColor & 0x0F;
    SetConsoleTextAttribute(layHandleConsole(), (s_curBGColor << 4) | s_curTextColor);
}

// Xóa màn hình tốc độ cao < 0.1ms (chỉ xóa vùng hiển thị 35 dòng làm việc)
inline void clrscr() {
    HANDLE hStdOut = layHandleConsole();
    COORD coord = {0, 0};
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
        short visibleLines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        if (visibleLines < 35) visibleLines = 35;
        DWORD cellCount = csbi.dwSize.X * visibleLines;
        FillConsoleOutputCharacter(hStdOut, ' ', cellCount, coord, &count);
        FillConsoleOutputAttribute(hStdOut, 7, cellCount, coord, &count);
        SetConsoleCursorPosition(hStdOut, coord);
    }
}

// ==========================================
// ĐỊNH NGHĨA MÃ PHÍM BÀN PHÍM
// ==========================================
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
#define KEY_F6 64
#define KEY_F7 65
#define KEY_F8 66
#define KEY_HOME 71
#define KEY_PGUP 73
#define KEY_END 79
#define KEY_PGDN 81

// ========================================================
// TIỆN ÍCH ĐỆM CHUỖI VÀ ĐỊNH DẠNG CELL CHUẨN XÁC
// ========================================================
inline string padRight(string s, int width) {
    string clean = "";
    for (char c : s) {
        if (static_cast<unsigned char>(c) >= 32) clean += c;
    }
    if (static_cast<int>(clean.length()) >= width) {
        return clean.substr(0, width);
    }
    return clean + string(width - clean.length(), ' ');
}

inline string padCenter(string s, int width) {
    string clean = "";
    for (char c : s) {
        if (static_cast<unsigned char>(c) >= 32) clean += c;
    }
    if (static_cast<int>(clean.length()) >= width) {
        return clean.substr(0, width);
    }
    int left = (width - static_cast<int>(clean.length())) / 2;
    int right = width - static_cast<int>(clean.length()) - left;
    return string(left, ' ') + clean + string(right, ' ');
}

inline string formatDiem(float d) {
    if (d < 0) return "-";
    char buf[16];
    if (d == static_cast<int>(d)) {
        snprintf(buf, sizeof(buf), "%d", static_cast<int>(d));
    } else {
        snprintf(buf, sizeof(buf), "%.1f", d);
    }
    return string(buf);
}

// Lấy mã màu hiển thị điểm số theo chuẩn học vụ tín chỉ
inline WORD layMauDiem(float d) {
    if (d < 0) return 8;        // Chưa có điểm: Xám tối (Dark gray)
    if (d >= 8.5f) return 10;   // Xuất sắc / Giỏi (A): Xanh lá sáng (Light Green)
    if (d >= 7.0f) return 11;   // Khá (B): Xanh lơ (Light Cyan)
    if (d >= 4.0f) return 14;   // Trung bình (C / D): Vàng (Yellow)
    return 12;                  // Rớt / Học lại (F): Đỏ sáng (Light Red)
}

// Tạo thanh tiến trình mini trực quan (Mini Progress Bar)
inline string taoProgressBar(int current, int max, int barLen = 5) {
    if (max <= 0) return "[" + string(barLen, '-') + "]";
    if (current < 0) current = 0;
    if (current > max) current = max;
    int filled = (current * barLen) / max;
    if (current > 0 && filled == 0) filled = 1;
    string res = "[";
    for (int i = 0; i < filled; i++) res += "=";
    for (int i = filled; i < barLen; i++) res += "-";
    res += "]";
    return res;
}

// Định dạng cột SLOTS chuẩn xác: thanh tiến trình và tỉ lệ số luôn thẳng hàng 100%
inline string dinhDangCotSlots(int current, int max, int width = 16) {
    string bar = taoProgressBar(current, max, 5);
    string sCur = to_string(current);
    int numDigitsMax = static_cast<int>(to_string(max).length());
    if (numDigitsMax < 2) numDigitsMax = 2;
    if (static_cast<int>(sCur.length()) < numDigitsMax) {
        sCur = string(numDigitsMax - sCur.length(), ' ') + sCur;
    }
    string numStr = sCur + "/" + to_string(max);
    string content = bar + " " + numStr; // ví dụ: "[=----] 20/80" -> 13 ký tự

    int leftPad = 2;
    if (static_cast<int>(content.length()) + leftPad > width) {
        leftPad = width - static_cast<int>(content.length());
        if (leftPad < 0) leftPad = 0;
    }
    string res = string(leftPad, ' ') + content;
    if (static_cast<int>(res.length()) < width) {
        res += string(width - res.length(), ' ');
    } else if (static_cast<int>(res.length()) > width) {
        res = res.substr(0, width);
    }
    return res;
}

// Vẽ thanh cuộn dọc trực quan (Visual Scrollbar) bên phải bảng
inline void veThanhCuon(int xRight, int yTop, int limit, int total, int firstVisible) {
    if (limit <= 0) return;
    
    // Mũi tên chỉ hướng cuộn lên (^)
    gotoxy(xRight, yTop - 1);
    if (total > limit && firstVisible > 0) {
        SetColorAndBG(11, 0); cout << "^"; // Cyan sáng
    } else {
        SetColorAndBG(8, 0);  cout << "^"; // Dark gray mờ
    }

    if (total <= limit || total <= 0) {
        for (int r = 0; r < limit; r++) {
            gotoxy(xRight, yTop + r);
            SetColorAndBG(8, 0); cout << (char)179;
        }
        gotoxy(xRight, yTop + limit);
        SetColorAndBG(8, 0); cout << "v";
        SetColorAndBG(7, 0);
        return;
    }

    int thumbSize = (limit * limit) / total;
    if (thumbSize < 1) thumbSize = 1;
    if (thumbSize > limit) thumbSize = limit;

    int maxFirst = total - limit;
    int thumbPos = 0;
    if (maxFirst > 0) {
        thumbPos = (firstVisible * (limit - thumbSize)) / maxFirst;
    }
    if (thumbPos < 0) thumbPos = 0;
    if (thumbPos + thumbSize > limit) thumbPos = limit - thumbSize;

    for (int r = 0; r < limit; r++) {
        gotoxy(xRight, yTop + r);
        if (r >= thumbPos && r < thumbPos + thumbSize) {
            SetColorAndBG(0, 14); // Con trượt màu Vàng sáng (Solid Block)
            cout << " ";
        } else {
            SetColorAndBG(8, 0);  // Rãnh cuộn thanh mảnh (│)
            cout << (char)179;
        }
    }

    // Mũi tên chỉ hướng cuộn xuống (v)
    gotoxy(xRight, yTop + limit);
    if (firstVisible + limit < total) {
        SetColorAndBG(11, 0); cout << "v"; // Cyan sáng
    } else {
        SetColorAndBG(8, 0);  cout << "v"; // Dark gray mờ
    }
    SetColorAndBG(7, 0);
}

// ========================================================
// TRỰC QUAN HÓA CẤU TRÚC DỮ LIỆU: CÂY NHỊ PHÂN TÌM KIẾM (BST VISUALIZER)
// ========================================================

struct TreeLineNode {
    string prefix;          // CP437 branches: ├── or └──
    string branchTag;       // "[ROOT]", "[L]", "[R]"
    WORD tagColor;          // 14 cho ROOT, 11 cho [L], 10 cho [R]
    string maMH;
    string tenMH;
    int stcLT;
    int stcTH;
    int height;
    int bf;                 // hl - hr
    int level;              // Độ sâu (1 = Root, 2 = con Root, ...)
    string parentKey;       // Mã node cha
    string leftChildKey;    // Mã con trái
    int leftChildHeight;
    string rightChildKey;   // Mã con phải
    int rightChildHeight;
    string nodeType;        // "Root Node", "Leaf Node", "Left-Only Node", "Right-Only Node", "Full (2-Child) Node"
};

inline int tinhChieuCaoCay(treeMH t) {
    if (t == NULL) return 0;
    int hl = tinhChieuCaoCay(t->left);
    int hr = tinhChieuCaoCay(t->right);
    return 1 + (hl > hr ? hl : hr);
}

inline bool kiemTraCanBangAVL(treeMH t) {
    if (t == NULL) return true;
    int hl = tinhChieuCaoCay(t->left);
    int hr = tinhChieuCaoCay(t->right);
    if (abs(hl - hr) > 1) return false;
    return kiemTraCanBangAVL(t->left) && kiemTraCanBangAVL(t->right);
}

inline void taoDanhSachNodeCay(treeMH node, string prefix, bool isLeft, bool isRoot, bool isLast, 
                               string parentKey, int level, TreeLineNode lines[], int &totalLines) {
    if (node == NULL) return;

    int hl = tinhChieuCaoCay(node->left);
    int hr = tinhChieuCaoCay(node->right);
    int h = 1 + (hl > hr ? hl : hr);
    int bf = hl - hr;

    TreeLineNode item;
    item.maMH = node->mh.MAMH;
    item.tenMH = node->mh.TENMH;
    item.stcLT = node->mh.STC_LT;
    item.stcTH = node->mh.STC_TH;
    item.height = h;
    item.bf = bf;
    item.level = level;
    item.parentKey = parentKey;
    item.leftChildKey = (node->left != NULL) ? node->left->mh.MAMH : "None";
    item.leftChildHeight = hl;
    item.rightChildKey = (node->right != NULL) ? node->right->mh.MAMH : "None";
    item.rightChildHeight = hr;

    if (isRoot) item.nodeType = "Root Node";
    else if (node->left == NULL && node->right == NULL) item.nodeType = "Leaf Node";
    else if (node->left != NULL && node->right == NULL) item.nodeType = "Left-Only Node";
    else if (node->left == NULL && node->right != NULL) item.nodeType = "Right-Only Node";
    else item.nodeType = "Full (2-Child) Node";

    if (isRoot) {
        item.prefix = "";
        item.branchTag = "[ROOT]";
        item.tagColor = 14; // Vàng sáng
    } else {
        item.prefix = prefix + (isLast ? (string(1, (char)192) + string(2, (char)196) + " ") 
                                       : (string(1, (char)195) + string(2, (char)196) + " "));
        if (isLeft) {
            item.branchTag = "[L]";
            item.tagColor = 11; // Cyan sáng
        } else {
            item.branchTag = "[R]";
            item.tagColor = 10; // Xanh lá sáng
        }
    }
    lines[totalLines++] = item;

    string childPrefix = isRoot ? "" : (prefix + (isLast ? "    " : (string(1, (char)179) + "   ")));

    if (node->left != NULL && node->right != NULL) {
        taoDanhSachNodeCay(node->left, childPrefix, true, false, false, node->mh.MAMH, level + 1, lines, totalLines);
        taoDanhSachNodeCay(node->right, childPrefix, false, false, true, node->mh.MAMH, level + 1, lines, totalLines);
    } else if (node->left != NULL) {
        taoDanhSachNodeCay(node->left, childPrefix, true, false, true, node->mh.MAMH, level + 1, lines, totalLines);
    } else if (node->right != NULL) {
        taoDanhSachNodeCay(node->right, childPrefix, false, false, true, node->mh.MAMH, level + 1, lines, totalLines);
    }
}

inline void xemCayMonHoc(treeMH dsmh) {
    anConTro();
    clrscr();

    int totalNodes = demSoMonHoc(dsmh);
    TreeLineNode* lines = (totalNodes > 0) ? new TreeLineNode[totalNodes] : nullptr;
    int total = 0;
    if (dsmh != NULL && lines != nullptr) {
        taoDanhSachNodeCay(dsmh, "", false, true, true, "None", 1, lines, total);
    }
    int treeHeight = tinhChieuCaoCay(dsmh);
    int minIdealHeight = (totalNodes > 0) ? (static_cast<int>(floor(log2(totalNodes))) + 1) : 0;
    string rootKey = (dsmh != NULL) ? dsmh->mh.MAMH : "NONE";

    string balStatus = "BALANCED";
    WORD balColor = 10;
    if (totalNodes <= 2) {
        balStatus = "OPTIMAL";
        balColor = 10;
    } else if (kiemTraCanBangAVL(dsmh)) {
        balStatus = "AVL BALANCED (|bf|<=1)";
        balColor = 10;
    } else if (treeHeight <= minIdealHeight + 1) {
        balStatus = "WELL BALANCED";
        balColor = 11;
    } else if (treeHeight <= minIdealHeight + 3) {
        balStatus = "SLIGHT SKEW";
        balColor = 14;
    } else {
        balStatus = "UNBALANCED SKEW";
        balColor = 12;
    }

    const int W = 90; // Độ rộng lọt lòng khung nhìn
    int limit = 13;   // 13 dòng cây thông thoáng, không chen chúc
    int selectedIdx = 0;
    int firstVisible = 0;
    int x = 4;
    int y = 4;

    while (true) {
        // Dòng 1: Tiêu đề màn hình
        gotoxy(x, 1);
        SetColor(11); cout << "BST DATA STRUCTURE VISUALIZER";
        SetColor(7);  cout << " - Binary Search Tree Hierarchy (Pre-Order NLR)";

        // Dòng 2: Thanh thông số cấu trúc cây
        gotoxy(x, 2);
        SetColor(14); cout << "Total Nodes: "; SetColor(15); cout << totalNodes;
        SetColor(14); cout << " | Height: "; SetColor(15); cout << treeHeight;
        SetColor(8);  cout << " (Min Ideal: " << minIdealHeight << ")";
        SetColor(14); cout << " | Root: "; SetColor(15); cout << "[" << rootKey << "]";
        SetColor(14); cout << " | Status: "; SetColor(balColor); cout << balStatus;
        SetColor(7);  cout << string(15, ' ');

        // Viền trên khung nhìn cây
        gotoxy(x, y - 1);
        SetColorAndBG(9, 0);
        cout << char(218) << string(4, (char)196) << " [ 2D BINARY SEARCH TREE TOPOLOGY VIEW ] " 
             << string(W - 45, (char)196) << char(191);
        SetColorAndBG(7, 0);

        // Nội dung từng dòng cây (Gọn gàng, thanh thoát)
        for (int r = 0; r < limit; r++) {
            int idx = firstVisible + r;
            gotoxy(x, y + r);
            SetColorAndBG(9, 0); cout << char(179); // Cột trái cố định x

            if (idx < total) {
                bool isSel = (idx == selectedIdx);
                const TreeLineNode &node = lines[idx];

                // 1. Con trỏ lựa chọn ASCII chuẩn (3 cột)
                if (isSel) {
                    SetColorAndBG(14, 0); cout << "-> ";
                } else {
                    SetColorAndBG(8, 0);  cout << "   ";
                }

                // 2. Nhánh phân cấp CP437
                SetColorAndBG(8, 0);
                cout << node.prefix;

                // 3. Nhãn nhánh [ROOT] / [L] / [R]
                SetColorAndBG(node.tagColor, 0);
                cout << node.branchTag << " ";

                // 4. Mã môn học và Tên môn học
                int usedCols = 3 + static_cast<int>(node.prefix.length()) + static_cast<int>(node.branchTag.length()) + 1;
                int maxTitleLen = W - usedCols - 2;
                if (maxTitleLen < 10) maxTitleLen = 10;

                string displayTen = node.tenMH;
                string fullTitle = node.maMH + ": " + displayTen;
                if (static_cast<int>(fullTitle.length()) > maxTitleLen) {
                    fullTitle = fullTitle.substr(0, maxTitleLen - 2) + "..";
                }

                if (isSel) {
                    SetColorAndBG(0, 14); // Highlight dạng badge nổi bật
                    cout << " " << fullTitle << " ";
                    SetColorAndBG(7, 0);
                    usedCols += static_cast<int>(fullTitle.length()) + 2;
                } else {
                    SetColorAndBG(15, 0); cout << node.maMH << ": ";
                    SetColorAndBG(7, 0);  cout << displayTen;
                    usedCols += static_cast<int>(node.maMH.length()) + 2 + static_cast<int>(displayTen.length());
                }

                // Điền khoảng trắng đến đúng viền phải
                int remain = W - usedCols;
                if (remain > 0) cout << string(remain, ' ');
            } else {
                SetColorAndBG(7, 0);
                if (total == 0 && r == 2) {
                    cout << padCenter("Tree is currently empty. Press [ESC] to return.", W);
                } else {
                    cout << string(W, ' ');
                }
            }

            gotoxy(x + W + 1, y + r);
            SetColorAndBG(9, 0); cout << char(179); // Cột phải cố định x + W + 1
            SetColorAndBG(7, 0);
        }

        // Vẽ thanh cuộn trực quan bên ngoài khung nhìn
        veThanhCuon(x + W + 3, y, limit, total, firstVisible);

        // Viền dưới khung nhìn cây
        gotoxy(x, y + limit);
        SetColorAndBG(9, 0);
        cout << char(192) << string(W, (char)196) << char(217);
        SetColorAndBG(7, 0);

        // ==========================================
        // KHỐI THANH TRA NODE ĐANG CHỌN (NODE INSPECTOR PANEL)
        // ==========================================
        int iy = y + limit + 1; // y = 18

        gotoxy(x, iy);
        SetColorAndBG(11, 0);
        cout << char(218) << string(3, (char)196) << " [ SELECTED NODE INSPECTOR ] " 
             << string(W - 32, (char)196) << char(191);

        if (total > 0 && selectedIdx < total) {
            const TreeLineNode &sel = lines[selectedIdx];

            // Dòng 1: Môn học & Số tín chỉ
            gotoxy(x, iy + 1); cout << char(179);
            SetColorAndBG(14, 0); cout << "  Subject : ";
            SetColorAndBG(15, 0); cout << "[" << sel.maMH << "] ";
            SetColorAndBG(11, 0); cout << padRight(sel.tenMH, 32);
            SetColorAndBG(8, 0);  cout << "| ";
            SetColorAndBG(14, 0); cout << "Credits: ";
            SetColorAndBG(15, 0); cout << sel.stcLT << " LT + " << sel.stcTH << " TH (Total: " << (sel.stcLT + sel.stcTH) << " TC)";
            int curX1 = wherex();
            if (curX1 < x + W + 1) cout << string(x + W + 1 - curX1, ' ');
            gotoxy(x + W + 1, iy + 1); SetColorAndBG(11, 0); cout << char(179);

            // Dòng 2: Cấu trúc topology, Chiều cao cây con, Hệ số cân bằng
            gotoxy(x, iy + 2); cout << char(179);
            SetColorAndBG(14, 0); cout << "  Topology: ";
            SetColorAndBG(sel.tagColor, 0); cout << padRight(sel.nodeType + " (Level " + to_string(sel.level) + ")", 32);
            SetColorAndBG(8, 0);  cout << "| ";
            SetColorAndBG(14, 0); cout << "Subtree Height: ";
            SetColorAndBG(15, 0); cout << padRight(to_string(sel.height), 4);
            SetColorAndBG(8, 0);  cout << "| ";
            SetColorAndBG(14, 0); cout << "Balance Factor: ";
            SetColorAndBG(abs(sel.bf) <= 1 ? 10 : 12, 0); cout << (sel.bf >= 0 ? "+" : "") << sel.bf;
            int curX2 = wherex();
            if (curX2 < x + W + 1) cout << string(x + W + 1 - curX2, ' ');
            gotoxy(x + W + 1, iy + 2); SetColorAndBG(11, 0); cout << char(179);

            // Dòng 3: Cây con trái, cây con phải, node cha
            gotoxy(x, iy + 3); cout << char(179);
            SetColorAndBG(14, 0); cout << "  Subtrees: ";
            SetColorAndBG(7, 0);  cout << "Left -> ";
            SetColorAndBG(sel.leftChildKey == "None" ? 8 : 11, 0);
            cout << padRight(sel.leftChildKey == "None" ? "NULL" : ("[" + sel.leftChildKey + "] (h=" + to_string(sel.leftChildHeight) + ")"), 17);
            SetColorAndBG(8, 0);  cout << "| ";
            SetColorAndBG(7, 0);  cout << "Right -> ";
            SetColorAndBG(sel.rightChildKey == "None" ? 8 : 10, 0);
            cout << padRight(sel.rightChildKey == "None" ? "NULL" : ("[" + sel.rightChildKey + "] (h=" + to_string(sel.rightChildHeight) + ")"), 17);
            SetColorAndBG(8, 0);  cout << "| ";
            SetColorAndBG(14, 0); cout << "Parent: ";
            SetColorAndBG(sel.parentKey == "None" ? 14 : 15, 0);
            cout << (sel.parentKey == "None" ? "[ROOT]" : ("[" + sel.parentKey + "]"));
            int curX3 = wherex();
            if (curX3 < x + W + 1) cout << string(x + W + 1 - curX3, ' ');
            gotoxy(x + W + 1, iy + 3); SetColorAndBG(11, 0); cout << char(179);
        } else {
            for (int k = 1; k <= 3; k++) {
                gotoxy(x, iy + k); cout << char(179);
                cout << (k == 2 ? padCenter("No subject node selected.", W) : string(W, ' '));
                gotoxy(x + W + 1, iy + k); cout << char(179);
            }
        }

        // Viền dưới khung thanh tra
        gotoxy(x, iy + 4);
        SetColorAndBG(11, 0);
        cout << char(192) << string(W, (char)196) << char(217);
        SetColorAndBG(7, 0);

        // Dòng 1 hướng dẫn: Navigation Badges
        gotoxy(x, iy + 6);
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit Tree View "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [Up/Down] Select Node "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [PgUp/PgDn] Page Scroll "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [Home/End] Top/Bottom "; SetBGColor(0);
        SetColor(7);

        // Dòng 2: Action Details & Row info
        gotoxy(x, iy + 7);
        SetColor(14);
        cout << "[Legend]: Yellow=[ROOT], Cyan=[L]eft child, Green=[R]ight child.  Node: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "       ";
        SetColor(7);

        char c = _getch();
        if (c == KEY_ESC || c == KEY_ENTER) {
            delete[] lines;
            break;
        } else if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_UP) {
                if (selectedIdx > 0) {
                    selectedIdx--;
                    if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                }
            } else if (c == KEY_DOWN) {
                if (selectedIdx < total - 1) {
                    selectedIdx++;
                    if (selectedIdx >= firstVisible + limit) firstVisible = selectedIdx - limit + 1;
                }
            } else if (c == KEY_LEFT || c == KEY_PGUP) {
                selectedIdx -= limit;
                if (selectedIdx < 0) selectedIdx = 0;
                if (selectedIdx < firstVisible) firstVisible = selectedIdx;
            } else if (c == KEY_RIGHT || c == KEY_PGDN) {
                selectedIdx += limit;
                if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                if (firstVisible < 0) firstVisible = 0;
            } else if (c == KEY_HOME) {
                selectedIdx = 0;
                firstVisible = 0;
            } else if (c == KEY_END) {
                if (total > 0) {
                    selectedIdx = total - 1;
                    if (total > limit) firstVisible = total - limit;
                    else firstVisible = 0;
                }
            }
        }
    }
}

// ========================================================
// TRỰC QUAN HÓA THỐNG KÊ: BIỂU ĐỒ PHÂN PHỐI ĐIỂM (GRADE HISTOGRAM)
// ========================================================

inline void popupBieuDoPhanPhoiDiem(const string &tieuDe, const float danhSachDiem[], int totalSV) {
    anConTro();
    int gradedCount = 0;
    int ungradedCount = 0;
    int countA = 0; // 8.5 - 10.0
    int countB = 0; // 7.0 - 8.4
    int countC = 0; // 5.5 - 6.9
    int countD = 0; // 4.0 - 5.4
    int countF = 0; // 0.0 - 3.9

    float sumDiem = 0.0f;
    float maxDiem = -1.0f;
    float minDiem = 999.0f;

    for (int i = 0; i < totalSV; i++) {
        float d = danhSachDiem[i];
        if (d < 0) {
            ungradedCount++;
        } else {
            gradedCount++;
            sumDiem += d;
            if (d > maxDiem) maxDiem = d;
            if (d < minDiem) minDiem = d;

            if (d >= 8.5f) countA++;
            else if (d >= 7.0f) countB++;
            else if (d >= 5.5f) countC++;
            else if (d >= 4.0f) countD++;
            else countF++;
        }
    }

    if (gradedCount == 0) minDiem = -1.0f;
    float avgDiem = (gradedCount > 0) ? (sumDiem / gradedCount) : -1.0f;
    int passCount = countA + countB + countC + countD;
    float passRate = (gradedCount > 0) ? ((float)passCount * 100.0f / gradedCount) : 0.0f;
    float failRate = (gradedCount > 0) ? ((float)countF * 100.0f / gradedCount) : 0.0f;

    int px = 20;
    int py = 4;
    int innerWidth = 72; // Width between borders

    // 1. Viền trên
    gotoxy(px, py);
    SetColorAndBG(11, 0);
    cout << char(218) << string(innerWidth, (char)196) << char(191);

    // 2. Tiêu đề
    gotoxy(px, py + 1);
    cout << char(179);
    SetColorAndBG(14, 0);
    cout << padCenter("GRADE DISTRIBUTION HISTOGRAM & STATISTICAL KPI", innerWidth);
    SetColorAndBG(11, 0); cout << char(179);

    gotoxy(px, py + 2);
    cout << char(179);
    SetColorAndBG(15, 0);
    cout << " " << padRight(tieuDe, innerWidth - 2) << " ";
    SetColorAndBG(11, 0); cout << char(179);

    // 3. Đường phân cách
    gotoxy(px, py + 3);
    cout << char(195) << string(innerWidth, (char)196) << char(180);

    // 4. Vẽ từng thanh biểu đồ phân phối điểm
    auto veThanhBar = [&](int lineY, string gradeName, string rangeStr, int count, WORD color) {
        gotoxy(px, lineY);
        SetColorAndBG(11, 0); cout << char(179) << " ";
        SetColorAndBG(15, 0); cout << padRight(gradeName, 9);
        SetColorAndBG(8, 0);  cout << padRight(rangeStr, 13) << ": ";

        int maxBarLen = 25;
        int barLen = 0;
        int baseTotal = (totalSV > 0) ? totalSV : 1;
        if (count > 0) {
            barLen = (count * maxBarLen) / baseTotal;
            if (barLen == 0) barLen = 1;
        }

        SetColorAndBG(color, 0);
        for (int b = 0; b < barLen; b++) cout << (char)219;
        SetColorAndBG(7, 0);
        for (int b = barLen; b < maxBarLen; b++) cout << " ";

        float pct = (totalSV > 0) ? ((float)count * 100.0f / totalSV) : 0.0f;
        char buf[32];
        snprintf(buf, sizeof(buf), "%3d SV (%5.1f%%)", count, pct);
        SetColorAndBG(14, 0); cout << " " << buf;

        // Điền khoảng trắng đến viền phải
        int currentLen = 1 + 9 + 13 + 2 + maxBarLen + 1 + (int)strlen(buf);
        int rem = innerWidth - currentLen;
        if (rem > 0) cout << string(rem, ' ');

        SetColorAndBG(11, 0); cout << char(179);
    };

    veThanhBar(py + 4, "Grade A", "[8.5 - 10.0]", countA, 10); // Xanh lá sáng
    veThanhBar(py + 5, "Grade B", "[7.0 -  8.4]", countB, 11); // Xanh lơ sáng
    veThanhBar(py + 6, "Grade C", "[5.5 -  6.9]", countC, 14); // Vàng sáng
    veThanhBar(py + 7, "Grade D", "[4.0 -  5.4]", countD, 6);  // Vàng đậm
    veThanhBar(py + 8, "Grade F", "[0.0 -  3.9]", countF, 12); // Đỏ sáng
    veThanhBar(py + 9, "Ungraded", "[   -    ]", ungradedCount, 8); // Xám tối

    // 5. Đường phân cách khối KPI
    gotoxy(px, py + 10);
    cout << char(195) << string(innerWidth, (char)196) << char(180);

    // 6. Khối thống kê KPI học vụ
    gotoxy(px, py + 11);
    cout << char(179);
    SetColorAndBG(14, 0);
    cout << "  ACADEMIC KPI STATISTICS & METRICS SUMMARY";
    SetColorAndBG(7, 0);
    cout << string(innerWidth - 43, ' ');
    SetColorAndBG(11, 0); cout << char(179);

    auto inDongKPI = [&](int lineY, string kpi1, string val1, WORD col1, string kpi2, string val2, WORD col2) {
        gotoxy(px, lineY);
        SetColorAndBG(11, 0); cout << char(179) << "  ";
        SetColorAndBG(7, 0);  cout << padRight(kpi1, 16) << ": ";
        SetColorAndBG(col1, 0); cout << padRight(val1, 13);

        SetColorAndBG(8, 0);  cout << "| ";
        SetColorAndBG(7, 0);  cout << padRight(kpi2, 16) << ": ";
        SetColorAndBG(col2, 0); cout << padRight(val2, 17);

        SetColorAndBG(11, 0); 
        gotoxy(px + innerWidth + 1, lineY);
        cout << char(179);
    };

    char bufAvg[16], bufMax[16], bufMin[16], bufGraded[32], bufUngraded[32], bufPass[32], bufFail[32];
    if (avgDiem >= 0) snprintf(bufAvg, sizeof(bufAvg), "%.2f / 10", avgDiem);
    else snprintf(bufAvg, sizeof(bufAvg), "N/A");

    if (maxDiem >= 0) snprintf(bufMax, sizeof(bufMax), "%.1f", maxDiem);
    else snprintf(bufMax, sizeof(bufMax), "N/A");

    if (minDiem >= 0) snprintf(bufMin, sizeof(bufMin), "%.1f", minDiem);
    else snprintf(bufMin, sizeof(bufMin), "N/A");

    snprintf(bufGraded, sizeof(bufGraded), "%d SV (%.1f%%)", gradedCount, (totalSV > 0 ? (float)gradedCount * 100.0f / totalSV : 0.0f));
    snprintf(bufUngraded, sizeof(bufUngraded), "%d SV (%.1f%%)", ungradedCount, (totalSV > 0 ? (float)ungradedCount * 100.0f / totalSV : 0.0f));
    snprintf(bufPass, sizeof(bufPass), "%.1f%% (%d SV)", passRate, passCount);
    snprintf(bufFail, sizeof(bufFail), "%.1f%% (%d SV)", failRate, countF);

    inDongKPI(py + 12, "Total Students", to_string(totalSV) + " SV", 15, "Graded Count", bufGraded, 10);
    inDongKPI(py + 13, "Average Score", bufAvg, layMauDiem(avgDiem), "Ungraded Count", bufUngraded, 8);
    inDongKPI(py + 14, "Highest Score", bufMax, 10, "Pass Rate (>=4)", bufPass, 11);
    inDongKPI(py + 15, "Lowest Score", bufMin, 12, "Fail Rate (<4)", bufFail, (failRate > 0 ? 12 : 10));

    // 7. Đường phân cách dưới
    gotoxy(px, py + 16);
    cout << char(195) << string(innerWidth, (char)196) << char(180);

    // 8. Chân thông báo
    gotoxy(px, py + 17);
    cout << char(179);
    SetColorAndBG(0, 15);
    cout << padCenter("[ Press any key or ESC to return ]", innerWidth);
    SetColorAndBG(11, 0); cout << char(179);

    // 9. Viền đáy
    gotoxy(px, py + 18);
    cout << char(192) << string(innerWidth, (char)196) << char(217);
    SetColorAndBG(7, 0);

    _getch();
    if (_kbhit()) _getch();

    // Xoa khung bieu do khi thoat de khong bi chop giat man hinh
    SetColorAndBG(7, 0);
    string emptyLine(innerWidth + 2, ' ');
    for (int i = 0; i <= 18; i++) {
        gotoxy(px, py + i);
        cout << emptyLine;
    }
}

// ========================================================
// 1. NHÓM TIỆN ÍCH UI CƠ SỞ (POPUP, NHẬP LIỆU)
// ========================================================

// Các chế độ kiểm soát và lọc ký tự thời gian thực khi gõ phím
enum KieuNhap {
    NHAP_TAT_CA = 0,         // Mọi ký tự (trừ dấu phẩy ',' để không phá hỏng định dạng lưu CSV)
    NHAP_MA = 1,             // Chỉ chữ cái và số [A-Za-z0-9], tự động viết HOA ngay khi gõ, không nhận khoảng trắng
    NHAP_CHU_SO_CACH = 2,    // Chữ cái, số, khoảng trắng (Tên lớp, Tên môn học)
    NHAP_CHU_CAI = 3,        // Chỉ chữ cái và khoảng trắng (Họ, Tên, Phái)
    NHAP_SO_NGUYEN = 4,      // Chỉ số [0-9] (Học kỳ, Nhóm, STC, Số SV, Số ĐT)
    NHAP_SO_THUC = 5,        // Số [0-9] và duy nhất 1 dấu chấm '.' (Điểm số)
    NHAP_NIEN_KHOA = 6       // Số [0-9] và dấu '-' (Niên khóa YYYY-YYYY)
};

inline bool nhapChuoiCoCancel(
    string &str, 
    int maxLength, 
    KieuNhap kieu = NHAP_TAT_CA, 
    bool choPhepRong = false,
    int inputX = -1,
    int inputY = -1,
    int inputW = 34
) {
    hienConTro(); // Bật con trỏ khi người dùng nhập dữ liệu

    // Hàm lambda nội bộ vẽ lại toàn bộ ô đỏ và đặt con trỏ ở cuối chuỗi
    auto veLaiODo = [&]() {
        if (inputX != -1 && inputY != -1) {
            gotoxy(inputX, inputY);
            SetColorAndBG(15, 12); // Nền đỏ (12), chữ trắng sáng (15) rực rỡ
            cout << padRight(str, inputW);
            int conTroX = inputX + (static_cast<int>(str.length()) > inputW ? inputW : static_cast<int>(str.length()));
            gotoxy(conTroX, inputY);
        } else {
            SetColorAndBG(15, 12);
        }
    };

    veLaiODo();

    while (true) {
        char c = _getch();
        if (c == KEY_ESC) {
            SetColorAndBG(7, 0);
            anConTro();
            return false; 
        }
        
        if (c == KEY_ENTER) {
            if (!str.empty() || choPhepRong) {
                SetColorAndBG(7, 0);
                anConTro();
                return true; 
            }
        } 
        else if (c == 8) { // Phím Backspace
            if (!str.empty()) {
                str.pop_back();
                veLaiODo();
            }
        } 
        else if (static_cast<int>(str.length()) < maxLength) {
            // Chặn tuyệt đối dấu phẩy ',' để không làm hỏng cấu trúc các file lưu trữ CSV
            if (c == ',') continue;

            bool hopLe = false;
            char cOut = c;

            switch (kieu) {
                case NHAP_MA:
                    if (isalnum(static_cast<unsigned char>(c))) {
                        cOut = toupper(static_cast<unsigned char>(c));
                        hopLe = true;
                    }
                    break;

                case NHAP_CHU_SO_CACH:
                    if (isalnum(static_cast<unsigned char>(c)) || (c == ' ' && !str.empty() && str.back() != ' ')) {
                        hopLe = true;
                    }
                    break;

                case NHAP_CHU_CAI:
                    if (isalpha(static_cast<unsigned char>(c)) || (c == ' ' && !str.empty() && str.back() != ' ')) {
                        hopLe = true;
                    }
                    break;

                case NHAP_SO_NGUYEN:
                    if (isdigit(static_cast<unsigned char>(c))) {
                        hopLe = true;
                    }
                    break;

                case NHAP_SO_THUC:
                    if (isdigit(static_cast<unsigned char>(c))) {
                        hopLe = true;
                    } else if (c == '.' && !str.empty() && str.find('.') == string::npos) {
                        hopLe = true;
                    }
                    break;

                case NHAP_NIEN_KHOA:
                    if (isdigit(static_cast<unsigned char>(c))) {
                        hopLe = true;
                    } else if (c == '-' && !str.empty() && str.find('-') == string::npos) {
                        hopLe = true;
                    }
                    break;

                case NHAP_TAT_CA:
                default:
                    if (c >= 32 && c <= 126) {
                        hopLe = true;
                    }
                    break;
            }

            if (hopLe) {
                str += cOut;
                veLaiODo();
            }
        }
    }
}

inline bool popupCanhBao(string thongBao, bool canXacNhan = false) {
    int w = 66;
    if (static_cast<int>(thongBao.length()) + 8 > w) w = static_cast<int>(thongBao.length()) + 8;
    int h = 4;
    int x = (120 - w) / 2;
    int y = 11;
    
    // Nền đỏ (color 12: Light Red), chữ trắng sáng (color 15)
    SetBGColor(12); SetColor(15);
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        cout << string(w + 1, ' ');
    }
    
    // Viền khung trên với tiêu đề căn giữa
    string tag = canXacNhan ? " [ XAC NHAN THAO TAC ] " : " [ CANH BAO ] ";
    int tagPos = x + (w - static_cast<int>(tag.length())) / 2;
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196);
    cout << char(191);
    gotoxy(tagPos, y); SetColor(14); cout << tag; SetColor(15);

    // Hai viền hông
    for (int i = 1; i < h; i++) { 
        gotoxy(x, y + i); cout << char(179); 
        gotoxy(x + w, y + i); cout << char(179); 
    }

    // Viền dưới
    gotoxy(x, y + h); cout << char(192); 
    for (int i = 1; i < w; i++) cout << char(196);
    cout << char(217);

    // Nội dung thông báo căn giữa
    gotoxy(x + 1, y + 2); 
    cout << padCenter(thongBao, w - 1);
    
    auto xoaKhung = [&]() {
        SetColorAndBG(7, 0);
        string empty(w + 1, ' ');
        for (int i = 0; i <= h; i++) {
            gotoxy(x, y + i);
            cout << empty;
        }
    };

    // Hướng dẫn phím ở chân viền dưới
    if (canXacNhan) {
        string hotkeys = " [Enter]: Dong y    [ESC]: Huy ";
        int hkPos = x + (w - static_cast<int>(hotkeys.length())) / 2;
        gotoxy(hkPos, y + h); SetColor(14); cout << hotkeys; SetColor(15);
        while(true) {
            char c = _getch();
            if (c == KEY_ENTER) { SetBGColor(0); SetColor(7); xoaKhung(); return true; }
            if (c == KEY_ESC) { SetBGColor(0); SetColor(7); xoaKhung(); return false; }
        }
    } else {
        string hotkeys = " [ Nhan phim bat ky de tiep tuc... ] ";
        int hkPos = x + (w - static_cast<int>(hotkeys.length())) / 2;
        gotoxy(hkPos, y + h); SetColor(14); cout << hotkeys; SetColor(15);
        _getch();
        SetBGColor(0); SetColor(7);
        xoaKhung();
        return false;
    }
}

inline void popupThongBaoThanhCong(string thongBao) {
    int w = 66;
    if (static_cast<int>(thongBao.length()) + 8 > w) w = static_cast<int>(thongBao.length()) + 8;
    int h = 4;
    int x = (120 - w) / 2;
    int y = 11;
    
    // Nền xanh lá đậm (color 2: Green), chữ trắng sáng (color 15)
    SetBGColor(2); SetColor(15);
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        cout << string(w + 1, ' ');
    }
    
    // Viền khung trên với tiêu đề căn giữa
    string tag = " [ THONG BAO THANH CONG ] ";
    int tagPos = x + (w - static_cast<int>(tag.length())) / 2;
    gotoxy(x, y); cout << char(218); 
    for (int i = 1; i < w; i++) cout << char(196);
    cout << char(191);
    gotoxy(tagPos, y); SetColor(14); cout << tag; SetColor(15);

    // Hai viền hông
    for (int i = 1; i < h; i++) { 
        gotoxy(x, y + i); cout << char(179); 
        gotoxy(x + w, y + i); cout << char(179); 
    }

    // Viền dưới
    gotoxy(x, y + h); cout << char(192); 
    for (int i = 1; i < w; i++) cout << char(196);
    cout << char(217);

    // Nội dung thông báo căn giữa
    gotoxy(x + 1, y + 2); 
    cout << padCenter(thongBao, w - 1);
    
    // Hướng dẫn phím ở chân viền dưới
    string hotkeys = " [ Nhan phim bat ky de tiep tuc... ] ";
    int hkPos = x + (w - static_cast<int>(hotkeys.length())) / 2;
    gotoxy(hkPos, y + h); SetColor(14); cout << hotkeys; SetColor(15);
    _getch();
    SetBGColor(0); SetColor(7);
    string empty(w + 1, ' ');
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        cout << empty;
    }
}

inline bool popupNhapMa(string tieuDe, string nhan, string &maNhap, KieuNhap kieu = NHAP_MA, int maxLen = 15, bool choPhepRong = false) {
    int w = 70;
    if (static_cast<int>(tieuDe.length()) + 8 > w) w = static_cast<int>(tieuDe.length()) + 8;
    if (static_cast<int>(nhan.length()) + 38 > w) w = static_cast<int>(nhan.length()) + 38;
    int x = (120 - w) / 2;
    if (x < 2) x = 2;
    int y = 10, h = 6;

    SetColorAndBG(0, 7);
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        cout << string(w + 1, ' ');
    }
    
    gotoxy(x, y); 
    cout << char(218) << string(2, (char)196) << " " << tieuDe << " ";
    int daIn = 2 + 2 + static_cast<int>(tieuDe.length()) + 1;
    int conLai = w - daIn;
    if (conLai > 0) cout << string(conLai, (char)196);
    cout << char(191);
    
    for (int i = 1; i < h; i++) { gotoxy(x, y + i); cout << char(179); gotoxy(x + w, y + i); cout << char(179); }
    gotoxy(x, y + h); cout << char(192) << string(w - 1, (char)196) << char(217);

    gotoxy(x + 4, y + 2);
    SetColorAndBG(0, 7);
    cout << nhan;

    gotoxy(x + 4, y + 4);
    SetColorAndBG(0, 7);
    cout << "[Enter]: Dong y   [ESC]: Huy";

    int inputX = x + 4 + static_cast<int>(nhan.length());
    int inputY = y + 2;
    int inputW = x + w - inputX - 4;
    if (inputW > 34) inputW = 34;
    if (inputW < 10) inputW = 10;

    bool kq = nhapChuoiCoCancel(maNhap, maxLen, kieu, choPhepRong, inputX, inputY, inputW);
    
    SetColorAndBG(7, 0);
    string emptyLine(w + 1, ' ');
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        cout << emptyLine;
    }
    anConTro();
    return kq;
}

// ========================================================
// HỆ THỐNG FORM NHẬP LIỆU TƯƠNG TÁC (INTERACTIVE FORM ENGINE)
// Hỗ trợ điều hướng đa chiều (Up/Down/Tab/Enter) và Chọn lựa (<- ->)
// ========================================================

enum FormFieldType {
    FF_TEXT,
    FF_CHOICE
};

struct FormField {
    string label;
    string* val;
    int maxLen;
    KieuNhap kieu;
    bool readOnly;
    FormFieldType type;
    string choices[4];
    int numChoices;
    int choiceIdx;
    int labelX, labelY;
    int inputX, inputY;
    int inputW;
};

inline string padRightForm(const string &s, int width) {
    if (static_cast<int>(s.length()) > width) {
        return s.substr(s.length() - width);
    }
    return s + string(width - s.length(), ' ');
}

inline void veKhungForm(int x, int y, int w, int h, const string &tieuDe, const string &huongDan) {
    SetColorAndBG(0, 7); // Nền xám nhạt, chữ đen chuẩn form mẫu
    for (int i = 0; i <= h; i++) {
        gotoxy(x, y + i);
        cout << string(w + 1, ' ');
    }
    
    // Viền trên nhúng tiêu đề vào góc trái như ảnh: ┌── TIEU DE ─────────────────┐
    gotoxy(x, y); 
    cout << char(218) << string(2, (char)196) << " " << tieuDe << " ";
    int daIn = 2 + 2 + static_cast<int>(tieuDe.length()) + 1;
    int conLai = w - daIn;
    if (conLai > 0) cout << string(conLai, (char)196);
    cout << char(191);

    for (int i = 1; i < h; i++) { 
        gotoxy(x, y + i); cout << char(179); 
        gotoxy(x + w, y + i); cout << char(179); 
    }
    gotoxy(x, y + h); 
    cout << char(192) << string(w - 1, (char)196) << char(217);

    if (!huongDan.empty()) {
        gotoxy(x + 3, y + h - 1);
        SetColorAndBG(0, 7);
        cout << huongDan;
    }
}

inline void veMotField(const FormField &f, bool isActive) {
    if (f.readOnly) {
        // Trường chỉ đọc (như Mã thẻ đọc giả trong ảnh mẫu)
        gotoxy(f.labelX, f.labelY);
        SetColorAndBG(0, 7);
        cout << padRight(f.label, 20) << *f.val << " (Co dinh)";
        return;
    }

    gotoxy(f.labelX, f.labelY);
    SetColorAndBG(0, 7);
    cout << padRight(f.label, 20);

    gotoxy(f.inputX, f.inputY);
    if (isActive) {
        // Con trỏ đang ở trường này: ĐỔI MÀU NỀN ĐỎ, CHỮ TRẮNG NỔI BẬT NHƯ ẢNH MẪU
        SetColorAndBG(15, 12);
        if (f.type == FF_CHOICE) {
            cout << padCenter("< " + *f.val + " >", f.inputW);
            anConTro();
        } else {
            cout << padRightForm(*f.val, f.inputW);
            int offset = (static_cast<int>(f.val->length()) > f.inputW) ? f.inputW : static_cast<int>(f.val->length());
            gotoxy(f.inputX + offset, f.inputY);
            hienConTro();
        }
    } else {
        // Trường không chọn: MÀU NỀN ĐEN, CHỮ TRẮNG NHƯ ẢNH MẪU
        SetColorAndBG(15, 0);
        if (f.type == FF_CHOICE) {
            cout << padCenter("< " + *f.val + " >", f.inputW);
        } else {
            cout << padRightForm(*f.val, f.inputW);
        }
    }
    SetColorAndBG(7, 0);
}

inline bool chayFormNhapLieu(
    int x, int y, int w, int h,
    const string &tieuDe,
    const string &huongDan,
    FormField fields[],
    int numFields,
    int &curField
) {
    veKhungForm(x, y, w, h, tieuDe, huongDan);

    if (curField < 0 || curField >= numFields || fields[curField].readOnly) {
        curField = 0;
        for (int i = 0; i < numFields; i++) {
            if (!fields[i].readOnly) {
                curField = i;
                break;
            }
        }
    }

    for (int i = 0; i < numFields; i++) {
        veMotField(fields[i], i == curField);
    }

    while (true) {
        veMotField(fields[curField], true);

        char c = _getch();
        if (c == KEY_ESC) {
            SetColorAndBG(7, 0);
            string emptyLine(w + 1, ' ');
            for (int i = 0; i <= h; i++) {
                gotoxy(x, y + i);
                cout << emptyLine;
            }
            anConTro();
            return false;
        }

        if (c == -32 || c == 0) {
            c = _getch();
            if (c == KEY_UP) {
                int prev = curField;
                do {
                    prev = (prev - 1 + numFields) % numFields;
                } while (fields[prev].readOnly && prev != curField);

                if (prev != curField) {
                    veMotField(fields[curField], false);
                    curField = prev;
                    veMotField(fields[curField], true);
                }
            }
            else if (c == KEY_DOWN) {
                int next = curField;
                do {
                    next = (next + 1) % numFields;
                } while (fields[next].readOnly && next != curField);

                if (next != curField) {
                    veMotField(fields[curField], false);
                    curField = next;
                    veMotField(fields[curField], true);
                }
            }
            else if (c == KEY_LEFT || c == KEY_RIGHT) {
                if (fields[curField].type == FF_CHOICE) {
                    int nC = fields[curField].numChoices;
                    if (nC > 0) {
                        if (c == KEY_LEFT) {
                            fields[curField].choiceIdx = (fields[curField].choiceIdx - 1 + nC) % nC;
                        } else {
                            fields[curField].choiceIdx = (fields[curField].choiceIdx + 1) % nC;
                        }
                        *fields[curField].val = fields[curField].choices[fields[curField].choiceIdx];
                        veMotField(fields[curField], true);
                    }
                }
            }
            else if (c == 68) { // F10 - Lưu nhanh
                anConTro();
                return true;
            }
        }
        else if (c == 9) { // Tab
            int next = curField;
            do {
                next = (next + 1) % numFields;
            } while (fields[next].readOnly && next != curField);

            if (next != curField) {
                veMotField(fields[curField], false);
                curField = next;
                veMotField(fields[curField], true);
            }
        }
        else if (c == KEY_ENTER) {
            bool laFieldCuoi = true;
            for (int i = curField + 1; i < numFields; i++) {
                if (!fields[i].readOnly) {
                    laFieldCuoi = false;
                    break;
                }
            }
            if (laFieldCuoi) {
                SetColorAndBG(7, 0);
                string emptyLine(w + 1, ' ');
                for (int i = 0; i <= h; i++) {
                    gotoxy(x, y + i);
                    cout << emptyLine;
                }
                anConTro();
                return true;
            } else {
                int next = curField;
                do {
                    next = (next + 1) % numFields;
                } while (fields[next].readOnly && next != curField);

                veMotField(fields[curField], false);
                curField = next;
                veMotField(fields[curField], true);
            }
        }
        else if (c == 8) { // Backspace
            if (fields[curField].type == FF_TEXT && !fields[curField].readOnly) {
                if (!fields[curField].val->empty()) {
                    fields[curField].val->pop_back();
                    veMotField(fields[curField], true);
                }
            }
        }
        else if (c == ' ' && fields[curField].type == FF_CHOICE) {
            int nC = fields[curField].numChoices;
            if (nC > 0) {
                fields[curField].choiceIdx = (fields[curField].choiceIdx + 1) % nC;
                *fields[curField].val = fields[curField].choices[fields[curField].choiceIdx];
                veMotField(fields[curField], true);
            }
        }
        else if (fields[curField].type == FF_TEXT && !fields[curField].readOnly) {
            if (static_cast<int>(fields[curField].val->length()) < fields[curField].maxLen && c != ',') {
                bool hopLe = false;
                char cOut = c;
                switch (fields[curField].kieu) {
                    case NHAP_MA:
                        if (isalnum(static_cast<unsigned char>(c))) {
                            cOut = toupper(static_cast<unsigned char>(c));
                            hopLe = true;
                        }
                        break;
                    case NHAP_CHU_SO_CACH:
                        if (isalnum(static_cast<unsigned char>(c)) || (c == ' ' && !fields[curField].val->empty() && fields[curField].val->back() != ' ')) {
                            hopLe = true;
                        }
                        break;
                    case NHAP_CHU_CAI:
                        if (isalpha(static_cast<unsigned char>(c)) || (c == ' ' && !fields[curField].val->empty() && fields[curField].val->back() != ' ')) {
                            hopLe = true;
                        }
                        break;
                    case NHAP_SO_NGUYEN:
                        if (isdigit(static_cast<unsigned char>(c))) {
                            hopLe = true;
                        }
                        break;
                    case NHAP_SO_THUC:
                        if (isdigit(static_cast<unsigned char>(c))) {
                            hopLe = true;
                        } else if (c == '.' && !fields[curField].val->empty() && fields[curField].val->find('.') == string::npos) {
                            hopLe = true;
                        }
                        break;
                    case NHAP_NIEN_KHOA:
                        if (isdigit(static_cast<unsigned char>(c))) {
                            hopLe = true;
                        } else if (c == '-' && !fields[curField].val->empty() && fields[curField].val->find('-') == string::npos) {
                            hopLe = true;
                        }
                        break;
                    case NHAP_TAT_CA:
                    default:
                        if (c >= 32 && c <= 126) hopLe = true;
                        break;
                }
                if (hopLe) {
                    *fields[curField].val += cOut;
                    veMotField(fields[curField], true);
                }
            }
        }
    }
}

// Popup nhập 4 thông số của Lớp Tín Chỉ (Mục i & j: Niên khóa, Học kỳ, Mã MH, Nhóm)
inline bool popupNhapThongSoLTC(string tieuDe, string &nienKhoa, int &hocKy, string &maMH, int &nhom) {
    int x = 26, y = 6, w = 68, h = 11;
    string huongDan = "[Up/Down]: Chon   [Enter]: Dong y   [ESC]: Huy";

    string strHK = (hocKy > 0) ? to_string(hocKy) : "";
    string strNhom = (nhom > 0) ? to_string(nhom) : "";

    FormField fields[] = {
        { "Nien khoa :", &nienKhoa, 9, NHAP_NIEN_KHOA, false, FF_TEXT, {}, 0, 0, x + 4, y + 2, x + 24, y + 2, 38 },
        { "Hoc ky    :", &strHK, 1, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 4, x + 24, y + 4, 38 },
        { "Ma mon hoc:", &maMH, 10, NHAP_MA, false, FF_TEXT, {}, 0, 0, x + 4, y + 6, x + 24, y + 6, 38 },
        { "Nhom      :", &strNhom, 2, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 8, x + 24, y + 8, 38 }
    };
    int numFields = sizeof(fields) / sizeof(fields[0]);
    int curField = 0;

    while (true) {
        if (!chayFormNhapLieu(x, y, w, h, tieuDe, huongDan, fields, numFields, curField)) {
            SetColorAndBG(7, 0);
            anConTro();
            return false;
        }

        try { hocKy = strHK.empty() ? 0 : stoi(strHK); } catch(...) { hocKy = 0; }
        try { nhom = strNhom.empty() ? 0 : stoi(strNhom); } catch(...) { nhom = 0; }
        inHoaToanBo(maMH);

        if (!kiemTraNienKhoaHopLe(nienKhoa)) {
            popupCanhBao("Loi: Nien khoa phai co dang YYYY-YYYY (VD: 2026-2027)!", false);
            curField = 0;
            continue;
        }
        if (hocKy < 1 || hocKy > 3) {
            popupCanhBao("Loi: Hoc ky phai la 1, 2 hoac 3!", false);
            curField = 1;
            continue;
        }
        if (maMH.empty()) {
            popupCanhBao("Loi: Ma mon hoc khong duoc de trong!", false);
            curField = 2;
            continue;
        }
        if (nhom < 1) {
            popupCanhBao("Loi: Nhom phai tu 1 tro len!", false);
            curField = 3;
            continue;
        }

        break;
    }

    SetColorAndBG(7, 0);
    anConTro();
    return true;
}

// ========================================================
// 2. NHÓM FORM NHẬP LIỆU (DIALOG INPUT)
// ========================================================

inline bool hienThiFormMonHoc(Monhoc &mh, bool laThemMoi) {
    int x = 26, y = 6, w = 68, h = 11;
    string tieuDe = laThemMoi ? "THEM MON HOC MOI" : "SUA THONG TIN MON HOC";
    string huongDan = "[Up/Down]: Chon   [Enter]: Luu   [ESC]: Huy";

    string strLT = to_string(mh.STC_LT);
    string strTH = to_string(mh.STC_TH);
    if (laThemMoi && mh.STC_LT == 0) strLT = "";
    if (laThemMoi && mh.STC_TH == 0) strTH = "";

    FormField fields[] = {
        { "Ma mon hoc   :", &mh.MAMH, 10, NHAP_MA, !laThemMoi, FF_TEXT, {}, 0, 0, x + 4, y + 2, x + 24, y + 2, 38 },
        { "Ten mon hoc  :", &mh.TENMH, 50, NHAP_CHU_SO_CACH, false, FF_TEXT, {}, 0, 0, x + 4, y + 4, x + 24, y + 4, 38 },
        { "STC ly thuyet:", &strLT, 2, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 6, x + 24, y + 6, 38 },
        { "STC thuc hanh:", &strTH, 2, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 8, x + 24, y + 8, 38 }
    };
    int numFields = sizeof(fields) / sizeof(fields[0]);
    int curField = (!laThemMoi) ? 1 : 0;

    while (true) {
        if (!chayFormNhapLieu(x, y, w, h, tieuDe, huongDan, fields, numFields, curField)) {
            SetColorAndBG(7, 0);
            anConTro();
            return false;
        }

        inHoaToanBo(mh.MAMH);
        chuanHoaChuoi(mh.TENMH);
        try { mh.STC_LT = strLT.empty() ? 0 : stoi(strLT); } catch(...) { mh.STC_LT = 0; }
        try { mh.STC_TH = strTH.empty() ? 0 : stoi(strTH); } catch(...) { mh.STC_TH = 0; }

        if (mh.MAMH.length() < 2) {
            popupCanhBao("Loi: Ma mon hoc phai co it nhat 2 ky tu!", false);
            curField = 0;
            continue;
        }
        if (mh.TENMH.empty()) {
            popupCanhBao("Loi: Ten mon hoc khong duoc de trong!", false);
            curField = 1;
            continue;
        }
        if (mh.STC_LT < 0 || mh.STC_TH < 0) {
            popupCanhBao("Loi: So tin chi khong duoc la so am!", false);
            curField = 2;
            continue;
        }
        if (mh.STC_LT + mh.STC_TH <= 0) {
            popupCanhBao("Loi: Tong so tin chi (LT + TH) phai lon hon 0!", false);
            curField = 2;
            continue;
        }

        break;
    }

    SetColorAndBG(7, 0);
    anConTro();
    return true; 
}

inline bool hienThiFormLop(Lop &lop, bool laThemMoi) {
    int x = 28, y = 8, w = 64, h = 7;
    string tieuDe = laThemMoi ? "THEM LOP MOI" : "SUA THONG TIN LOP";
    string huongDan = "[Up/Down]: Chon   [Enter]: Luu   [ESC]: Huy";

    FormField fields[] = {
        { "Ma lop :", &lop.MALOP, 15, NHAP_MA, !laThemMoi, FF_TEXT, {}, 0, 0, x + 4, y + 2, x + 20, y + 2, 38 },
        { "Ten lop:", &lop.TENLOP, 40, NHAP_CHU_SO_CACH, false, FF_TEXT, {}, 0, 0, x + 4, y + 4, x + 20, y + 4, 38 }
    };
    int numFields = sizeof(fields) / sizeof(fields[0]);
    int curField = (!laThemMoi) ? 1 : 0;

    while (true) {
        if (!chayFormNhapLieu(x, y, w, h, tieuDe, huongDan, fields, numFields, curField)) {
            SetColorAndBG(7, 0);
            anConTro();
            return false;
        }

        inHoaToanBo(lop.MALOP);
        chuanHoaChuoi(lop.TENLOP);

        if (lop.MALOP.length() < 3) {
            popupCanhBao("Loi: Ma lop phai co it nhat 3 ky tu (VD: D25CQCN01)!", false);
            curField = 0;
            continue;
        }
        if (lop.TENLOP.empty()) {
            popupCanhBao("Loi: Ten lop khong duoc de trong!", false);
            curField = 1;
            continue;
        }

        break;
    }

    SetColorAndBG(7, 0);
    anConTro();
    return true; 
}

inline bool hienThiFormSinhVien(Sinhvien &sv, bool laThemMoi) {
    int x = 26, y = 5, w = 68, h = 13;
    string tieuDe = laThemMoi ? "THEM SINH VIEN MOI" : "SUA THONG TIN SINH VIEN";
    string huongDan = "[Up/Down]: Chon   [<-/->]: Doi G.Tri   [Enter]: Luu   [ESC]: Huy";

    // Phái mặc định là "Nam" nếu chưa có
    if (sv.PHAI != "Nu" && sv.PHAI != "NU" && sv.PHAI != "nu") {
        sv.PHAI = "Nam";
    } else {
        sv.PHAI = "Nu";
    }

    FormField fields[] = {
        { "Ma sinh vien :", &sv.MASV, 15, NHAP_MA, !laThemMoi, FF_TEXT, {}, 0, 0, x + 4, y + 2, x + 24, y + 2, 38 },
        { "Ho va ten dem:", &sv.HO, 30, NHAP_CHU_CAI, false, FF_TEXT, {}, 0, 0, x + 4, y + 4, x + 24, y + 4, 38 },
        { "Ten sinh vien:", &sv.TEN, 10, NHAP_CHU_CAI, false, FF_TEXT, {}, 0, 0, x + 4, y + 6, x + 24, y + 6, 38 },
        { "Phai (Nam/Nu):", &sv.PHAI, 4, NHAP_CHU_CAI, false, FF_CHOICE, {"Nam", "Nu"}, 2, (sv.PHAI == "Nu" ? 1 : 0), x + 4, y + 8, x + 24, y + 8, 38 },
        { "So dien thoai:", &sv.SODT, 10, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 10, x + 24, y + 10, 38 }
    };
    int numFields = sizeof(fields) / sizeof(fields[0]);
    int curField = (!laThemMoi) ? 1 : 0;

    while (true) {
        if (!chayFormNhapLieu(x, y, w, h, tieuDe, huongDan, fields, numFields, curField)) {
            SetColorAndBG(7, 0);
            anConTro();
            return false;
        }

        inHoaToanBo(sv.MASV);
        chuanHoaChuoi(sv.HO);
        chuanHoaChuoi(sv.TEN);

        if (laThemMoi && sv.MASV.empty()) {
            // Cho phép chuỗi rỗng để dừng quá trình nhập SV mới theo quy định đề tài (mục c)
            SetColorAndBG(7, 0);
            anConTro();
            return true; 
        }

        if (sv.MASV.length() < 4) {
            popupCanhBao("Loi: Ma SV phai co it nhat 4 ky tu (VD: N25DCCN001)!", false);
            curField = 0;
            continue;
        }
        if (!kiemTraHoHopLe(sv.HO)) {
            popupCanhBao("Loi: Ho dem khong hop le (chi chua chu cai va khong rong)!", false);
            curField = 1;
            continue;
        }
        if (!kiemTraTenHopLe(sv.TEN)) {
            popupCanhBao("Loi: Ten chi duoc gom 1 tu duy nhat (khong chua dau cach hay chu so)!", false);
            curField = 2;
            continue;
        }
        if (!kiemTraPhaiHopLe(sv.PHAI)) {
            popupCanhBao("Loi: Phai chi chap nhan 'Nam' hoac 'Nu'!", false);
            curField = 3;
            continue;
        }
        if (!kiemTraSoDienThoaiHopLe(sv.SODT)) {
            popupCanhBao("Loi: So DT phai dung 10 chu so va bat dau bang so 0 (VD: 0912345678)!", false);
            curField = 4;
            continue;
        }

        break;
    }

    SetColorAndBG(7, 0);
    anConTro();
    return true; 
}

inline bool hienThiFormLTC(Loptinchi &ltc, bool laThemMoi) {
    int x = 24, y = 3, w = 72, h = 17;
    string tieuDe = laThemMoi ? "MO LOP TIN CHI MOI" : "SUA LOP TIN CHI";
    string huongDan = "[Up/Down]: Chon   [Enter]: Luu   [ESC]: Huy";

    string strMa = (ltc.MALOPTC > 0) ? to_string(ltc.MALOPTC) : "";
    string strHK = (ltc.HOCKY > 0) ? to_string(ltc.HOCKY) : "";
    string strNhom = (ltc.NHOM > 0) ? to_string(ltc.NHOM) : "";
    string strMin = (ltc.SOSVMIN > 0) ? to_string(ltc.SOSVMIN) : "";
    string strMax = (ltc.SOSVMAX > 0) ? to_string(ltc.SOSVMAX) : "";

    FormField fields[] = {
        { "Ma LTC (So)       :", &strMa, 6, NHAP_SO_NGUYEN, !laThemMoi, FF_TEXT, {}, 0, 0, x + 4, y + 2, x + 26, y + 2, 40 },
        { "Ma Mon Hoc        :", &ltc.MAMH, 10, NHAP_MA, false, FF_TEXT, {}, 0, 0, x + 4, y + 4, x + 26, y + 4, 40 },
        { "Nien Khoa         :", &ltc.NIENKHOA, 9, NHAP_NIEN_KHOA, false, FF_TEXT, {}, 0, 0, x + 4, y + 6, x + 26, y + 6, 40 },
        { "Hoc Ky (1, 2, 3)  :", &strHK, 1, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 8, x + 26, y + 8, 40 },
        { "Nhom (So nguyen)  :", &strNhom, 2, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 10, x + 26, y + 10, 40 },
        { "SV Toi thieu (MIN):", &strMin, 4, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 12, x + 26, y + 12, 40 },
        { "SV Toi da (MAX)   :", &strMax, 4, NHAP_SO_NGUYEN, false, FF_TEXT, {}, 0, 0, x + 4, y + 14, x + 26, y + 14, 40 }
    };
    int numFields = sizeof(fields) / sizeof(fields[0]);
    int curField = (!laThemMoi) ? 1 : 0;

    while (true) {
        if (!chayFormNhapLieu(x, y, w, h, tieuDe, huongDan, fields, numFields, curField)) {
            SetColorAndBG(7, 0);
            anConTro();
            return false;
        }

        inHoaToanBo(ltc.MAMH);
        try { ltc.MALOPTC = strMa.empty() ? 0 : stoi(strMa); } catch(...) { ltc.MALOPTC = 0; }
        try { ltc.HOCKY = strHK.empty() ? 0 : stoi(strHK); } catch(...) { ltc.HOCKY = 0; }
        try { ltc.NHOM = strNhom.empty() ? 0 : stoi(strNhom); } catch(...) { ltc.NHOM = 0; }
        try { ltc.SOSVMIN = strMin.empty() ? 0 : stoi(strMin); } catch(...) { ltc.SOSVMIN = 0; }
        try { ltc.SOSVMAX = strMax.empty() ? 0 : stoi(strMax); } catch(...) { ltc.SOSVMAX = 0; }

        if (ltc.MALOPTC <= 0) {
            popupCanhBao("Loi: Ma LTC phai la so nguyen duong lon hon 0!", false);
            curField = 0;
            continue;
        }
        if (ltc.MAMH.empty()) {
            popupCanhBao("Loi: Ma mon hoc khong duoc de trong!", false);
            curField = 1;
            continue;
        }
        if (!kiemTraNienKhoaHopLe(ltc.NIENKHOA)) {
            popupCanhBao("Loi: Nien khoa phai co dang YYYY-YYYY (VD: 2026-2027)!", false);
            curField = 2;
            continue;
        }
        if (ltc.HOCKY < 1 || ltc.HOCKY > 3) {
            popupCanhBao("Loi: Hoc ky phai la 1, 2 hoac 3!", false);
            curField = 3;
            continue;
        }
        if (ltc.NHOM < 1) {
            popupCanhBao("Loi: Nhom phai lon hon hoac bang 1!", false);
            curField = 4;
            continue;
        }
        if (ltc.SOSVMIN <= 0) {
            popupCanhBao("Loi: So sinh vien toi thieu phai lon hon 0!", false);
            curField = 5;
            continue;
        }
        if (ltc.SOSVMAX < ltc.SOSVMIN) {
            popupCanhBao("Loi: So SV toi da (MAX) phai >= So SV toi thieu (MIN)!", false);
            curField = 6;
            continue;
        }

        break;
    }

    SetColorAndBG(7, 0);
    anConTro();
    return true; 
}

// ========================================================
// 3. NHÓM MÀN HÌNH CHỨC NĂNG (GIAO DIỆN HOÀN THIỆN 100%)
// ========================================================

// 1. Màn hình Quản Lý Môn Học (Real-time Search & Custom Sort)
inline void quanLyMonHocToanDien(treeMH &t, DS_LTC &dsltc) {
    anConTro();
    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MAMH, 2: NAME, 3: LT, 4: TH
    int sortMode = 0;    // 0: MAMH, 1: NAME, 2: LT, 3: TH
    
    const string searchFieldNames[] = { "ALL", "MAMH", "NAME", "LT", "TH" };
    const string sortNames[] = { "MAMH (ID)", "NAME (A-Z)", "STC_LT", "STC_TH" };

    clrscr();

    // Cache dữ liệu gốc và mảng chỉ số hiển thị (Pipeline tìm kiếm & lọc tối ưu)
    int totalAll = 0;
    Monhoc* allMH = nullptr;
    int* displayIdx = nullptr;
    int total = 0;

    bool needReloadData = true;
    bool needResort = true;

    while (true) {
        if (needReloadData) {
            delete[] allMH;
            delete[] displayIdx;
            totalAll = demSoMonHoc(t);
            allMH = (totalAll > 0) ? new Monhoc[totalAll] : nullptr;
            displayIdx = (totalAll > 0) ? new int[totalAll] : nullptr;
            if (totalAll > 0) {
                int idx = 0;
                doMonHocVaoMang(t, allMH, idx);
            }
            needReloadData = false;
            needResort = true;
        }

        if (needResort && totalAll > 0) {
            // Sắp xếp allMH theo sortMode bằng QuickSort O(N log N)
            quickSort(allMH, 0, totalAll - 1, [sortMode](const Monhoc &a, const Monhoc &b) {
                switch (sortMode) {
                    case 1: return a.TENMH < b.TENMH;
                    case 2: return a.STC_LT > b.STC_LT;
                    case 3: return a.STC_TH > b.STC_TH;
                    case 0:
                    default:
                        return a.MAMH < b.MAMH;
                }
            });
            needResort = false;
        }

        // Lọc theo searchQuery và searchField: chỉ lưu chỉ số int (không clone struct)
        total = 0;
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        for (int i = 0; i < totalAll; i++) {
            const Monhoc &mh = allMH[i];
            if (q.empty()) {
                displayIdx[total++] = i;
                continue;
            }
            string sMAMH = mh.MAMH; for (char &ch : sMAMH) ch = tolower(static_cast<unsigned char>(ch));
            string sName = mh.TENMH; for (char &ch : sName) ch = tolower(static_cast<unsigned char>(ch));
            string sLT = to_string(mh.STC_LT);
            string sTH = to_string(mh.STC_TH);

            bool match = false;
            switch (searchField) {
                case 1: match = (sMAMH.find(q) != string::npos); break;
                case 2: match = (sName.find(q) != string::npos); break;
                case 3: match = (sLT.find(q) != string::npos); break;
                case 4: match = (sTH.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMAMH.find(q) != string::npos) ||
                            (sName.find(q) != string::npos) ||
                            (sLT.find(q) != string::npos) ||
                            (sTH.find(q) != string::npos);
                    break;
            }
            if (match) displayIdx[total++] = i;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 2, y = 4; 

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "COURSE MANAGEMENT [Total: " << totalAll << " Courses]" << string(30, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp (Search & Sort Bar)
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 7) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 20) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalAll << "  " << string(10, ' ');

        // Viền trên bảng
        SetColorAndBG(9, 0);
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(15, (char)196) << char(194) 
             << string(35, (char)196) << char(194) << string(10, (char)196) << char(194) 
             << string(10, (char)196) << char(191);
        
        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA MON HOC    " 
             << char(179) << " TEN MON HOC                       " 
             << char(179) << " STC LT   " << char(179) << " STC TH   " << char(179);
             
        // Viền giữa
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(15, (char)196) << char(197) 
             << string(35, (char)196) << char(197) << string(10, (char)196) << char(197) 
             << string(10, (char)196) << char(180);
        SetColorAndBG(7, 0);

        // Lambda vẽ 1 dòng dữ liệu đơn lẻ (tối ưu hóa triệt tiêu chớp giật)
        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                const Monhoc &mh = allMH[displayIdx[i]];
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(mh.MAMH, 14);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(mh.TENMH, 34);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(mh.STC_LT), 10);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(mh.STC_TH), 10);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(15, ' ') 
                     << char(179) << string(35, ' ') << char(179) << string(10, ' ') 
                     << char(179) << string(10, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        // Vẽ toàn bộ 16 dòng dữ liệu ban đầu
        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 82, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(15, (char)196) << char(193) 
             << string(35, (char)196) << char(193) << string(10, (char)196) << char(193) 
             << string(10, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges (Vẽ đúng 1 lần duy nhất, không vẽ lại khi di chuyển phím)
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F1] Add "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F2] Save "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F3/Enter] Edit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [DEL] Delete "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter By "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort By "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F8] Tree View "; SetBGColor(0);
        SetColor(7);
        
        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "       "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        // ==========================================
        // VÒNG LẶP ĐIỀU HƯỚNG NỘI BỘ (ZERO-FLICKER INNER LOOP)
        // Khi bấm Up/Down chỉ cập nhật đúng 2 dòng, không vẽ lại thanh chỉ dẫn
        // ==========================================
        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 82, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 82, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 82, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 82, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 5;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    needResort = true;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F8) {
                    xemCayMonHoc(t);
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F1) {
                    Monhoc mhMoi;
                    if (hienThiFormMonHoc(mhMoi, true)) {
                        if (!insertMH(t, mhMoi)) {
                            popupCanhBao("Loi: Ma mon hoc da ton tai!", false);
                        } else {
                            needReloadData = true;
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F2) {
                    ghiFileMonHoc(t);
                    popupThongBaoThanhCong("Da luu danh sach Mon hoc vao file thanh cong!");
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F3) {
                    if (total == 0) {
                        popupCanhBao("Danh sach mon hoc dang trong!", false);
                    } else {
                        string maSua = allMH[displayIdx[selectedIdx]].MAMH;
                        treeMH nodeSua = timKiemMonHoc(t, maSua);
                        if (nodeSua != NULL) {
                            hienThiFormMonHoc(nodeSua->mh, false);
                            needReloadData = true;
                        }
                        else popupCanhBao("Loi: Khong tim thay Mon hoc " + maSua, false);
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_DEL) {
                    if (total == 0) {
                        popupCanhBao("Danh sach mon hoc dang trong!", false);
                    } else {
                        string maXoa = allMH[displayIdx[selectedIdx]].MAMH;
                        string tenXoa = allMH[displayIdx[selectedIdx]].TENMH;
                        if (kiemTraMonHocDaMoLTC(dsltc, maXoa)) {
                            popupCanhBao("Loi: Mon hoc da mo Lop Tin Chi, khong the xoa!", false);
                        } else if (popupCanhBao("Chac chan xoa: " + tenXoa + " (" + maXoa + ") ?", true)) {
                            xoaNodeMonHoc(t, maXoa);
                            if (selectedIdx >= total - 1 && selectedIdx > 0) selectedIdx--;
                            needReloadData = true;
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ENTER) {
                if (total == 0) {
                    popupCanhBao("Danh sach mon hoc dang trong!", false);
                } else {
                    string maSua = allMH[displayIdx[selectedIdx]].MAMH;
                    treeMH nodeSua = timKiemMonHoc(t, maSua);
                    if (nodeSua != NULL) {
                        hienThiFormMonHoc(nodeSua->mh, false);
                        needReloadData = true;
                    }
                    else popupCanhBao("Loi: Khong tim thay Mon hoc " + maSua, false);
                }
                canTaiLaiDuLieu = true;
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allMH;
                    delete[] displayIdx;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)
    } // end while (true)
}

inline void quanLyLopToanDien(DS_LOP &dslop) {
    anConTro();
    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MALOP, 2: NAME
    int sortMode = 0;    // 0: MALOP, 1: NAME
    
    const string searchFieldNames[] = { "ALL", "MALOP", "NAME" };
    const string sortNames[] = { "MALOP (ID)", "NAME (A-Z)" };

    clrscr();

    // Cache dữ liệu gốc và mảng chỉ số hiển thị (Pipeline tìm kiếm & lọc tối ưu)
    int totalAll = 0;
    Lop* allLop = nullptr;
    int* displayIdx = nullptr;
    int total = 0;

    bool needReloadData = true;
    bool needResort = true;

    while (true) {
        if (needReloadData) {
            delete[] allLop;
            delete[] displayIdx;
            totalAll = dslop.n;
            allLop = (totalAll > 0) ? new Lop[totalAll] : nullptr;
            displayIdx = (totalAll > 0) ? new int[totalAll] : nullptr;
            for (int i = 0; i < totalAll; i++) {
                allLop[i] = dslop.nodes[i];
            }
            needReloadData = false;
            needResort = true;
        }

        if (needResort && totalAll > 0) {
            // Sắp xếp allLop theo sortMode bằng QuickSort O(N log N)
            quickSort(allLop, 0, totalAll - 1, [sortMode](const Lop &a, const Lop &b) {
                return (sortMode == 1) ? (a.TENLOP < b.TENLOP) : (a.MALOP < b.MALOP);
            });
            needResort = false;
        }

        // Lọc theo searchQuery và searchField: chỉ lưu chỉ số int (không clone struct)
        total = 0;
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        for (int i = 0; i < totalAll; i++) {
            const Lop &lop = allLop[i];
            if (q.empty()) {
                displayIdx[total++] = i;
                continue;
            }
            string sMALOP = lop.MALOP; for (char &ch : sMALOP) ch = tolower(static_cast<unsigned char>(ch));
            string sName = lop.TENLOP; for (char &ch : sName) ch = tolower(static_cast<unsigned char>(ch));

            bool match = false;
            switch (searchField) {
                case 1: match = (sMALOP.find(q) != string::npos); break;
                case 2: match = (sName.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMALOP.find(q) != string::npos) || (sName.find(q) != string::npos);
                    break;
            }
            if (match) displayIdx[total++] = i;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 6, y = 4; 

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "CLASS MANAGEMENT [Total: " << totalAll << " Classes]" << string(30, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp (Search & Sort Bar)
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 7) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 20) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalAll << "  " << string(10, ' ');
        
        // Viền trên bảng
        SetColorAndBG(9, 0); 
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(15, (char)196) << char(194) << string(35, (char)196) << char(191);
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA LOP        " << char(179) << " TEN LOP                           " << char(179);
             
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(15, (char)196) << char(197) << string(35, (char)196) << char(180);
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                const Lop &lop = allLop[displayIdx[i]];
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(lop.MALOP, 14);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(lop.TENLOP, 34);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(15, ' ') << char(179) << string(35, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 60, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(15, (char)196) << char(193) << string(35, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F1] Add "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F2] Save "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F3/Enter] Edit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [DEL] Delete "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter By "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort By "; SetBGColor(0);
        SetColor(7);
        
        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "                     "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 60, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 60, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 60, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 60, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 3;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 2;
                    selectedIdx = 0; firstVisible = 0;
                    needResort = true;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F1) {
                    Lop lopMoi;
                    lopMoi.dssv = NULL;
                    if (hienThiFormLop(lopMoi, true)) {
                        int res = themLop(dslop, lopMoi);
                        if (res == -1) popupCanhBao("Loi: Danh sach lop da day!", false);
                        else if (res == 0) popupCanhBao("Loi: Ma Lop da ton tai!", false);
                        else needReloadData = true;
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F2) {
                    ghiFileLop(dslop);
                    popupThongBaoThanhCong("Da luu danh sach Lop vao file thanh cong!");
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F3) {
                    if (total == 0) {
                        popupCanhBao("Danh sach lop dang trong!", false);
                    } else {
                        int originalIdx = timKiemLop(dslop, allLop[displayIdx[selectedIdx]].MALOP);
                        if (originalIdx != -1) {
                            hienThiFormLop(dslop.nodes[originalIdx], false);
                            needReloadData = true;
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_DEL) {
                    if (total == 0) {
                        popupCanhBao("Danh sach lop dang trong!", false);
                    } else {
                        string maXoa = allLop[displayIdx[selectedIdx]].MALOP;
                        int originalIdx = timKiemLop(dslop, maXoa);
                        if (originalIdx != -1) {
                            if (lopCoSinhVien(dslop, originalIdx)) {
                                popupCanhBao("Loi: Lop dang co sinh vien, khong the xoa!", false);
                            } else if (popupCanhBao("Chac chan xoa Lop: " + maXoa + " ?", true)) {
                                xoaLop(dslop, originalIdx);
                                if (selectedIdx >= total - 1 && selectedIdx > 0) selectedIdx--;
                                needReloadData = true;
                            }
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ENTER) {
                if (total == 0) {
                    popupCanhBao("Danh sach lop dang trong!", false);
                } else {
                    int originalIdx = timKiemLop(dslop, allLop[displayIdx[selectedIdx]].MALOP);
                    if (originalIdx != -1) {
                        hienThiFormLop(dslop.nodes[originalIdx], false);
                        needReloadData = true;
                    }
                }
                canTaiLaiDuLieu = true;
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allLop;
                    delete[] displayIdx;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)
    } // end while (true)
}

inline void quanLySinhVienToanDien(DS_LOP &dslop, DS_LTC &dsltc) {
    anConTro();
    string maLop;
    if (!popupNhapMa("--- QUAN LY SINH VIEN ---", "Nhap Ma Lop: ", maLop, NHAP_MA, 15)) return;
    
    int indexLop = timKiemLop(dslop, maLop);
    if (indexLop == -1) {
        popupCanhBao("Loi: Khong tim thay Lop " + maLop, false);
        return;
    }

    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MASV, 2: NAME, 3: PHAI, 4: PHONE
    int sortMode = 0;    // 0: TEN, 1: MASV, 2: PHAI, 3: PHONE
    
    const string searchFieldNames[] = { "ALL", "MASV", "NAME", "PHAI", "PHONE" };
    const string sortNames[] = { "TEN (Name)", "MASV (ID)", "PHAI (Gender)", "PHONE (SDT)" };

    clrscr();

    // Cache dữ liệu gốc và mảng chỉ số hiển thị (Pipeline tìm kiếm & lọc tối ưu)
    int totalAll = 0;
    Sinhvien* allSV = nullptr;
    int* displayIdx = nullptr;
    int total = 0;

    bool needReloadData = true;
    bool needResort = true;

    while (true) {
        if (needReloadData) {
            delete[] allSV;
            delete[] displayIdx;
            totalAll = 0;
            for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL; p = p->next) totalAll++;
            allSV = (totalAll > 0) ? new Sinhvien[totalAll] : nullptr;
            displayIdx = (totalAll > 0) ? new int[totalAll] : nullptr;
            int k = 0;
            for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL; p = p->next) allSV[k++] = p->sv;
            needReloadData = false;
            needResort = true;
        }

        if (needResort && totalAll > 0) {
            // Sắp xếp allSV theo sortMode bằng QuickSort O(N log N)
            quickSort(allSV, 0, totalAll - 1, [sortMode](const Sinhvien &a, const Sinhvien &b) {
                switch (sortMode) {
                    case 0: // Tên (A->Z), nếu trùng thì xét Họ
                        if (a.TEN != b.TEN) return a.TEN < b.TEN;
                        return a.HO < b.HO;
                    case 1: // Mã SV (A->Z)
                        return a.MASV < b.MASV;
                    case 2: // Phái (Nam/Nu)
                        if (a.PHAI != b.PHAI) return a.PHAI < b.PHAI;
                        return a.TEN < b.TEN;
                    case 3: // Số điện thoại
                        return a.SODT < b.SODT;
                    default:
                        return a.TEN < b.TEN;
                }
            });
            needResort = false;
        }

        // Lọc danh sách theo chuỗi tìm kiếm: chỉ lưu chỉ số int (không clone struct)
        total = 0;
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        for (int i = 0; i < totalAll; i++) {
            const Sinhvien &sv = allSV[i];
            if (q.empty()) {
                displayIdx[total++] = i;
                continue;
            }

            string sMASV = sv.MASV; for (char &ch : sMASV) ch = tolower(static_cast<unsigned char>(ch));
            string sName = sv.HO + " " + sv.TEN; for (char &ch : sName) ch = tolower(static_cast<unsigned char>(ch));
            string sPhai = sv.PHAI; for (char &ch : sPhai) ch = tolower(static_cast<unsigned char>(ch));
            string sPhone = sv.SODT; for (char &ch : sPhone) ch = tolower(static_cast<unsigned char>(ch));

            bool match = false;
            switch (searchField) {
                case 1: match = (sMASV.find(q) != string::npos); break;
                case 2: match = (sName.find(q) != string::npos); break;
                case 3: match = (sPhai.find(q) != string::npos); break;
                case 4: match = (sPhone.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMASV.find(q) != string::npos) ||
                            (sName.find(q) != string::npos) ||
                            (sPhai.find(q) != string::npos) ||
                            (sPhone.find(q) != string::npos);
                    break;
            }
            if (match) displayIdx[total++] = i;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 2, y = 4; 

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "CLASS: " << padRight(maLop, 15) << " [Total: " << totalAll << " Students]" << string(30, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 7) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 20) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalAll << "  " << string(10, ' ');
        
        // Viền trên bảng
        SetColorAndBG(9, 0); 
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(15, (char)196) << char(194) 
             << string(35, (char)196) << char(194) << string(10, (char)196) << char(194) 
             << string(15, (char)196) << char(191);
        
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA SV         " 
             << char(179) << " HO VA TEN                         " 
             << char(179) << " PHAI     " << char(179) << " SO DT         " << char(179);
             
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(15, (char)196) << char(197) 
             << string(35, (char)196) << char(197) << string(10, (char)196) << char(197) 
             << string(15, (char)196) << char(180);
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                const Sinhvien &sv = allSV[displayIdx[i]];
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(sv.MASV, 14);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(sv.HO + " " + sv.TEN, 34);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(sv.PHAI, 10);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(sv.SODT, 14);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(15, ' ') 
                     << char(179) << string(35, ' ') << char(179) << string(10, ' ') 
                     << char(179) << string(15, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 87, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(15, (char)196) << char(193) 
             << string(35, (char)196) << char(193) << string(10, (char)196) << char(193) 
             << string(15, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F1] Add "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F2] Save "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F3/Enter] Edit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [DEL] Delete "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter By "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort By "; SetBGColor(0);
        SetColor(7);
        
        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "                     "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 87, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 87, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 87, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 87, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 5;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    needResort = true;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F1) {
                    while (true) {
                        Sinhvien svMoi;
                        if (!hienThiFormSinhVien(svMoi, true)) break;
                        if (svMoi.MASV.empty()) break;
                        if (kiemTraSinhVienTonTai(dslop, svMoi.MASV)) {
                            popupCanhBao("Loi: Ma SV da ton tai trong he thong!", false);
                        } else {
                            insertSinhVien(dslop.nodes[indexLop].dssv, svMoi);
                            needReloadData = true;
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F2) {
                    ghiFileLop(dslop);
                    popupThongBaoThanhCong("Da luu danh sach Sinh vien vao file thanh cong!");
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F3) {
                    if (total == 0) {
                        popupCanhBao("Danh sach sinh vien dang trong!", false);
                    } else {
                        string maSua = allSV[displayIdx[selectedIdx]].MASV;
                        PTRSV pSua = timKiemSinhVienTrongLop(dslop.nodes[indexLop].dssv, maSua);
                        if (pSua != NULL) {
                            hienThiFormSinhVien(pSua->sv, false);
                            needReloadData = true;
                        } else {
                            popupCanhBao("Loi: SV khong ton tai trong Lop nay!", false);
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_DEL) {
                    if (total == 0) {
                        popupCanhBao("Danh sach sinh vien dang trong!", false);
                    } else {
                        string maXoa = allSV[displayIdx[selectedIdx]].MASV;
                        string tenXoa = allSV[displayIdx[selectedIdx]].TEN;
                        if (kiemTraSinhVienDaDangKyLTCNaoChua(dsltc, maXoa)) {
                            popupCanhBao("Loi: Sinh vien da dang ky Lop Tin Chi, khong the xoa!", false);
                        } else if (popupCanhBao("Chac chan xoa SV " + tenXoa + " (" + maXoa + ") ?", true)) {
                            xoaSinhVienKhoiLop(dslop.nodes[indexLop].dssv, maXoa);
                            if (selectedIdx >= total - 1 && selectedIdx > 0) selectedIdx--;
                            needReloadData = true;
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ENTER) {
                if (total == 0) {
                    popupCanhBao("Danh sach sinh vien dang trong!", false);
                } else {
                    string maSua = allSV[displayIdx[selectedIdx]].MASV;
                    PTRSV pSua = timKiemSinhVienTrongLop(dslop.nodes[indexLop].dssv, maSua);
                    if (pSua != NULL) {
                        hienThiFormSinhVien(pSua->sv, false);
                        needReloadData = true;
                    } else {
                        popupCanhBao("Loi: SV khong ton tai trong Lop nay!", false);
                    }
                }
                canTaiLaiDuLieu = true;
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allSV;
                    delete[] displayIdx;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)
    } // end while (true)
}

inline void quanLyLopTinChiToanDien(DS_LTC &dsltc, treeMH dsmh) {
    anConTro();
    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MALTC, 2: MAMH, 3: NK, 4: HK, 5: NHOM
    int sortMode = 0;    // 0: MALTC, 1: MAMH, 2: NK, 3: HK
    
    const string searchFieldNames[] = { "ALL", "MALTC", "MAMH", "NK", "HK", "NHOM" };
    const string sortNames[] = { "MALTC (ID)", "MAMH (A-Z)", "NIEN KHOA", "HOC KY" };

    clrscr();

    while (true) {
        int totalValid = demLTCHoatDong(dsltc);
        Loptinchi** allLTC = (totalValid > 0) ? new Loptinchi*[totalValid] : nullptr;
        if (totalValid > 0) {
            layDanhSachLTCHoatDong(dsltc, allLTC);
        }

        // Sắp xếp allLTC theo sortMode bằng QuickSort O(N log N)
        if (totalValid > 1) {
            quickSort(allLTC, 0, totalValid - 1, [sortMode](const Loptinchi* a, const Loptinchi* b) {
                switch (sortMode) {
                    case 1: return a->MAMH < b->MAMH;
                    case 2: return a->NIENKHOA < b->NIENKHOA;
                    case 3: return a->HOCKY < b->HOCKY;
                    case 0:
                    default:
                        return a->MALOPTC < b->MALOPTC;
                }
            });
        }

        // Lọc theo searchQuery và searchField
        Loptinchi** displayLTC = (totalValid > 0) ? new Loptinchi*[totalValid] : nullptr;
        int total = 0;
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        for (int i = 0; i < totalValid; i++) {
            Loptinchi* ltc = allLTC[i];
            if (q.empty()) {
                displayLTC[total++] = ltc;
                continue;
            }
            string sMALTC = to_string(ltc->MALOPTC);
            string sMAMH = ltc->MAMH; for (char &ch : sMAMH) ch = tolower(static_cast<unsigned char>(ch));
            string sNK = ltc->NIENKHOA; for (char &ch : sNK) ch = tolower(static_cast<unsigned char>(ch));
            string sHK = to_string(ltc->HOCKY);
            string sNhom = to_string(ltc->NHOM);

            bool match = false;
            switch (searchField) {
                case 1: match = (sMALTC.find(q) != string::npos); break;
                case 2: match = (sMAMH.find(q) != string::npos); break;
                case 3: match = (sNK.find(q) != string::npos); break;
                case 4: match = (sHK.find(q) != string::npos); break;
                case 5: match = (sNhom.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMALTC.find(q) != string::npos) ||
                            (sMAMH.find(q) != string::npos) ||
                            (sNK.find(q) != string::npos) ||
                            (sHK.find(q) != string::npos) ||
                            (sNhom.find(q) != string::npos);
                    break;
            }
            if (match) displayLTC[total++] = ltc;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 2, y = 4; 

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "CREDIT CLASS MANAGEMENT [Total: " << totalValid << " Classes]" << string(30, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 7) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 20) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalValid << "  " << string(10, ' ');

        // Viền trên bảng (STT: 5, MALTC: 8, MAMH: 13, NK: 11, HK: 5, NHOM: 6, MIN: 5, MAX: 5, DADK: 7, STATUS: 19)
        SetColorAndBG(9, 0); 
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(8, (char)196) << char(194) 
             << string(13, (char)196) << char(194) << string(11, (char)196) << char(194) 
             << string(5, (char)196) << char(194) << string(6, (char)196) << char(194) 
             << string(5, (char)196) << char(194) << string(5, (char)196) << char(194) 
             << string(7, (char)196) << char(194) << string(19, (char)196) << char(191);
        
        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA LTC " << char(179) << " MA MON HOC  " 
             << char(179) << " NIEN KHOA " << char(179) << " HK  " << char(179) << " NHOM " 
             << char(179) << " MIN " << char(179) << " MAX " << char(179) << " DA DK " << char(179) << "      STATUS       " << char(179);
             
        // Viền giữa
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(8, (char)196) << char(197) 
             << string(13, (char)196) << char(197) << string(11, (char)196) << char(197) 
             << string(5, (char)196) << char(197) << string(6, (char)196) << char(197) 
             << string(5, (char)196) << char(197) << string(5, (char)196) << char(197) 
             << string(7, (char)196) << char(197) << string(19, (char)196) << char(180);
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;
                int daDK = demSinhVienDangKy(displayLTC[i]->dssvdk);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(displayLTC[i]->MALOPTC), 8);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displayLTC[i]->MAMH, 12);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(displayLTC[i]->NIENKHOA, 11);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(displayLTC[i]->HOCKY), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(displayLTC[i]->NHOM), 6);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(displayLTC[i]->SOSVMIN), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(displayLTC[i]->SOSVMAX), 5);

                SetColorAndBG(9, 0); cout << char(179);
                if (!isSel) {
                    if (daDK < displayLTC[i]->SOSVMIN) SetColorAndBG(14, 0);
                    else if (daDK >= displayLTC[i]->SOSVMAX) SetColorAndBG(12, 0);
                    else SetColorAndBG(10, 0);
                } else {
                    SetColorAndBG(0, 14);
                }
                cout << padCenter(to_string(daDK), 7);

                SetColorAndBG(9, 0); cout << char(179);
                if (displayLTC[i]->HUYLOP) {
                    SetColorAndBG(isSel ? 4 : 12, bgCol); cout << padCenter("[DA HUY LOP]", 19);
                } else if (daDK < displayLTC[i]->SOSVMIN) {
                    string sThieu = "[THIEU SV: " + to_string(daDK) + "/" + to_string(displayLTC[i]->SOSVMIN) + "]";
                    SetColorAndBG(isSel ? 6 : 14, bgCol); cout << padCenter(sThieu, 19);
                } else if (daDK >= displayLTC[i]->SOSVMAX) {
                    SetColorAndBG(isSel ? 1 : 11, bgCol); cout << padCenter("[DA DAY SV]", 19);
                } else {
                    SetColorAndBG(isSel ? 2 : 10, bgCol); cout << padCenter("[HOP LE]", 19);
                }

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(8, ' ') 
                     << char(179) << string(13, ' ') << char(179) << string(11, ' ') 
                     << char(179) << string(5, ' ') << char(179) << string(6, ' ') 
                     << char(179) << string(5, ' ') << char(179) << string(5, ' ') 
                     << char(179) << string(7, ' ') << char(179) << string(19, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 96, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(8, (char)196) << char(193) 
             << string(13, (char)196) << char(193) << string(11, (char)196) << char(193) 
             << string(5, (char)196) << char(193) << string(6, (char)196) << char(193) 
             << string(5, (char)196) << char(193) << string(5, (char)196) << char(193) 
             << string(7, (char)196) << char(193) << string(19, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F1] Add "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F2] Save "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F3/Enter] Edit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [DEL] Delete "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter By "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort By "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F8] Course List "; SetBGColor(0);
        SetColor(7);
        
        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "                     "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 96, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 96, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 96, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 96, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 6;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F8) {
                    xemCayMonHoc(dsmh);
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F1) {
                    Loptinchi ltcMoi;
                    ltcMoi.dssvdk = NULL;
                    ltcMoi.HUYLOP = false;
                    if (hienThiFormLTC(ltcMoi, true)) {
                        if (kiemTraTrungMaLTC(dsltc, ltcMoi.MALOPTC)) {
                            popupCanhBao("Loi: Ma LTC da tung ton tai trong he thong!", false);
                        } else if (timKiemMonHoc(dsmh, ltcMoi.MAMH) == NULL) {
                            popupCanhBao("Loi: Mon hoc " + ltcMoi.MAMH + " khong ton tai!", false);
                        } else if (kiemTraLtcTrungNhom(dsltc, ltcMoi.NIENKHOA, ltcMoi.HOCKY, ltcMoi.MAMH, ltcMoi.NHOM)) {
                            popupCanhBao("Loi: Lop Tin Chi da ton tai (trung Mon, Nien khoa, Hoc ky, Nhom)!", false);
                        } else if (!insertLTC(dsltc, ltcMoi)) {
                            popupCanhBao("Loi: Danh sach Lop Tin Chi da day (toi da 10000)!", false);
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F2) {
                    ghiFileLTC(dsltc);
                    popupThongBaoThanhCong("Da luu danh sach Lop Tin Chi vao file thanh cong!");
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F3) {
                    if (total == 0) {
                        popupCanhBao("Danh sach Lop Tin Chi dang trong!", false);
                    } else {
                        int maLTC = displayLTC[selectedIdx]->MALOPTC;
                        int originalIdx = timKiemLTC(dsltc, maLTC);
                        if (originalIdx != -1) hienThiFormLTC(*(dsltc.nodes[originalIdx]), false);
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_DEL) {
                    if (total == 0) {
                        popupCanhBao("Danh sach Lop Tin Chi dang trong!", false);
                    } else {
                        int maLTC = displayLTC[selectedIdx]->MALOPTC;
                        treeMH nodeMH = timKiemMonHoc(dsmh, displayLTC[selectedIdx]->MAMH);
                        string tenMH = (nodeMH != NULL) ? nodeMH->mh.TENMH : displayLTC[selectedIdx]->MAMH;
                        if (popupCanhBao("Chac chan HUY Lop Tin Chi " + to_string(maLTC) + " (" + tenMH + ") ?", true)) {
                            huyLopTinChi(dsltc, maLTC);
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ENTER) {
                if (total == 0) {
                    popupCanhBao("Danh sach Lop Tin Chi dang trong!", false);
                } else {
                    int maLTC = displayLTC[selectedIdx]->MALOPTC;
                    int originalIdx = timKiemLTC(dsltc, maLTC);
                    if (originalIdx != -1) hienThiFormLTC(*(dsltc.nodes[originalIdx]), false);
                }
                canTaiLaiDuLieu = true;
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allLTC;
                    delete[] displayLTC;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)

        delete[] allLTC;
        delete[] displayLTC;
    } // end while (true)
}

inline void quanLyDangKyToanDien(DS_LTC &dsltc, DS_LOP &dslop) {
    anConTro();
    string maSV;
    if (!popupNhapMa("--- DANG KY LOP TIN CHI ---", "Nhap Ma Sinh Vien: ", maSV, NHAP_MA, 15)) return;
    
    if (!kiemTraSinhVienTonTai(dslop, maSV)) {
        popupCanhBao("Loi: Khong tim thay Sinh vien " + maSV + " trong he thong!", false);
        return;
    }

    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MALTC, 2: MAMH, 3: NK, 4: STATUS
    int sortMode = 0;    // 0: MALTC, 1: MAMH, 2: NK, 3: STATUS
    
    const string searchFieldNames[] = { "ALL", "MALTC", "MAMH", "NK", "STATUS" };
    const string sortNames[] = { "MALTC (ID)", "MAMH (A-Z)", "NIEN KHOA", "STATUS (DK)" };

    clrscr();

    while (true) {
        int totalValid = demLTCHoatDong(dsltc);
        Loptinchi** allLTC = (totalValid > 0) ? new Loptinchi*[totalValid] : nullptr;
        if (totalValid > 0) {
            layDanhSachLTCHoatDong(dsltc, allLTC);
        }

        // Sắp xếp allLTC theo sortMode bằng QuickSort O(N log N)
        if (totalValid > 1) {
            quickSort(allLTC, 0, totalValid - 1, [sortMode, &maSV](const Loptinchi* a, const Loptinchi* b) {
                switch (sortMode) {
                    case 1: return a->MAMH < b->MAMH;
                    case 2: return a->NIENKHOA < b->NIENKHOA;
                    case 3: {
                        bool dki = kiemTraSinhVienDaDangKyLTC(a->dssvdk, maSV);
                        bool dkj = kiemTraSinhVienDaDangKyLTC(b->dssvdk, maSV);
                        return dki && !dkj;
                    }
                    case 0:
                    default:
                        return a->MALOPTC < b->MALOPTC;
                }
            });
        }

        // Lọc theo searchQuery và searchField
        Loptinchi** displayLTC = (totalValid > 0) ? new Loptinchi*[totalValid] : nullptr;
        int total = 0;
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        for (int i = 0; i < totalValid; i++) {
            Loptinchi* ltc = allLTC[i];
            if (q.empty()) {
                displayLTC[total++] = ltc;
                continue;
            }
            bool daDK = kiemTraSinhVienDaDangKyLTC(ltc->dssvdk, maSV);
            string sMALTC = to_string(ltc->MALOPTC);
            string sMAMH = ltc->MAMH; for (char &ch : sMAMH) ch = tolower(static_cast<unsigned char>(ch));
            string sNK = ltc->NIENKHOA; for (char &ch : sNK) ch = tolower(static_cast<unsigned char>(ch));
            string sStatus = daDK ? "da dang ky registered" : "chua not registered";

            bool match = false;
            switch (searchField) {
                case 1: match = (sMALTC.find(q) != string::npos); break;
                case 2: match = (sMAMH.find(q) != string::npos); break;
                case 3: match = (sNK.find(q) != string::npos); break;
                case 4: match = (sStatus.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMALTC.find(q) != string::npos) ||
                            (sMAMH.find(q) != string::npos) ||
                            (sNK.find(q) != string::npos) ||
                            (sStatus.find(q) != string::npos);
                    break;
            }
            if (match) displayLTC[total++] = ltc;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 4, y = 4; 

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "REGISTRATION PORTAL - STUDENT: " << maSV << " - " << layTenSinhVien(dslop, maSV) << " [Active: " << totalValid << " Classes]" << string(20, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 7) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 20) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalValid << "  " << string(10, ' ');

        // Viền trên: 7 CỘT CÓ CỘT SLOTS TIẾN TRÌNH VÀ TRẠNG THÁI
        SetColorAndBG(9, 0); 
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(8, (char)196) << char(194) 
             << string(14, (char)196) << char(194) << string(11, (char)196) << char(194) 
             << string(5, (char)196) << char(194) << string(16, (char)196) << char(194) << string(16, (char)196) << char(191);
        
        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA LTC " << char(179) << " MA MON HOC   " 
             << char(179) << " NIEN KHOA " << char(179) << " HK  " << char(179) << "     SLOTS      " << char(179) << "   TRANG THAI   " << char(179);
             
        // Viền giữa
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(8, (char)196) << char(197) 
             << string(14, (char)196) << char(197) << string(11, (char)196) << char(197) 
             << string(5, (char)196) << char(197) << string(16, (char)196) << char(197) << string(16, (char)196) << char(180);
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;
                bool daDK = kiemTraSinhVienDaDangKyLTC(displayLTC[i]->dssvdk, maSV);
                int countDK = demSinhVienDangKy(displayLTC[i]->dssvdk);
                int maxSV = displayLTC[i]->SOSVMAX;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(displayLTC[i]->MALOPTC), 8);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displayLTC[i]->MAMH, 13);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(displayLTC[i]->NIENKHOA, 11);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(displayLTC[i]->HOCKY), 5);

                // Cột SLOTS: Hiển thị tỷ lệ đăng ký dạng [ XX / YY ]
                SetColorAndBG(9, 0); cout << char(179);
                string slotsText = to_string(countDK) + "/" + to_string(maxSV);
                if (!isSel) {
                    if (countDK >= maxSV) SetColorAndBG(12, 0);      // Đỏ nếu đầy
                    else if (countDK == 0) SetColorAndBG(8, 0);     // Xám nếu chưa ai
                    else SetColorAndBG(10, 0);                     // Xanh lá nếu còn chỗ
                } else {
                    SetColorAndBG(0, 14);
                }
                cout << padCenter(slotsText, 16);

                // Cột TRẠNG THÁI: Hiển thị rõ ràng ĐÃ ĐĂNG KÝ hoặc CHƯA ĐĂNG KÝ
                SetColorAndBG(9, 0); cout << char(179);
                if (daDK) {
                    SetColorAndBG(isSel ? 2 : 10, bgCol); 
                    cout << padCenter("[DA DANG KY]", 16);
                } else {
                    if (countDK >= maxSV) {
                        SetColorAndBG(isSel ? 4 : 12, bgCol);
                        cout << padCenter("[LTC DA DAY]", 16);
                    } else {
                        SetColorAndBG(isSel ? 8 : 7, bgCol);
                        cout << padCenter("Chua dang ky", 16);
                    }
                }

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(8, ' ') 
                     << char(179) << string(14, ' ') << char(179) << string(11, ' ') 
                     << char(179) << string(5, ' ') << char(179) << string(16, ' ') 
                     << char(179) << string(16, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 85, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(8, (char)196) << char(193) 
             << string(14, (char)196) << char(193) << string(11, (char)196) << char(193) 
             << string(5, (char)196) << char(193) << string(16, (char)196) << char(193) << string(16, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F1/Enter] Register "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [DEL] Cancel Reg "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F2] Save "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter By "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort By "; SetBGColor(0);
        SetColor(7);
        
        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "              "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 85, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 85, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 85, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 85, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 5;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F1) {
                    if (total == 0) {
                        popupCanhBao("Danh sach Lop Tin Chi dang trong!", false);
                    } else {
                        int maLTC = displayLTC[selectedIdx]->MALOPTC;
                        int res = dangKyLTC(dsltc, maLTC, maSV);
                        if (res == 0) popupCanhBao("Loi: LTC khong ton tai!", false);
                        else if (res == -1) popupCanhBao("Loi: Ban da dang ky LTC nay roi!", false);
                        else if (res == -2) popupCanhBao("Loi: LTC da du so luong sinh vien toi da!", false);
                        else popupThongBaoThanhCong("Dang ky thanh cong LTC " + to_string(maLTC) + "!");
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F2) {
                    ghiFileLTC(dsltc);
                    popupThongBaoThanhCong("Da luu danh sach Dang ky vao file thanh cong!");
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_DEL) {
                    if (total == 0) {
                        popupCanhBao("Danh sach Lop Tin Chi dang trong!", false);
                    } else {
                        int maLTC = displayLTC[selectedIdx]->MALOPTC;
                        if (!kiemTraSinhVienDaDangKyLTC(displayLTC[selectedIdx]->dssvdk, maSV)) {
                            popupCanhBao("Ban chua dang ky LTC " + to_string(maLTC) + " nen khong the huy!", false);
                        } else if (popupCanhBao("Chac chan HUY dang ky LTC " + to_string(maLTC) + " ?", true)) {
                            huyDangKyLTC(dsltc, maLTC, maSV);
                            popupThongBaoThanhCong("Da huy dang ky LTC " + to_string(maLTC) + "!");
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ENTER) {
                if (total == 0) {
                    popupCanhBao("Danh sach Lop Tin Chi dang trong!", false);
                } else {
                    int maLTC = displayLTC[selectedIdx]->MALOPTC;
                    int res = dangKyLTC(dsltc, maLTC, maSV);
                    if (res == 0) popupCanhBao("Loi: LTC khong ton tai!", false);
                    else if (res == -1) popupCanhBao("Loi: Ban da dang ky LTC nay roi!", false);
                    else if (res == -2) popupCanhBao("Loi: LTC da du so luong sinh vien toi da!", false);
                    else popupThongBaoThanhCong("Dang ky thanh cong LTC " + to_string(maLTC) + "!");
                }
                canTaiLaiDuLieu = true;
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allLTC;
                    delete[] displayLTC;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)

        delete[] allLTC;
        delete[] displayLTC;
    } // end while (true)
}

inline void quanLyBangDiemLTC(DS_LTC &dsltc, DS_LOP &dslop, treeMH dsmh, bool batDauBangEditMode = true) {
    anConTro();
    string nienKhoa = "", maMH = "";
    int hocKy = 0, nhom = 0;

    if (!popupNhapThongSoLTC("--- LOC LOP TIN CHI ---", nienKhoa, hocKy, maMH, nhom)) return;

    treeMH nodeMH = timKiemMonHoc(dsmh, maMH);
    if (nodeMH == NULL) {
        popupCanhBao("Loi: Khong tim thay Mon hoc " + maMH, false);
        return;
    }
    string tenMH = nodeMH->mh.TENMH;

    int indexLTC = timKiemLTCTheoThongSo(dsltc, nienKhoa, hocKy, maMH, nhom);
    if (indexLTC == -1) {
        popupCanhBao("Loi: Khong tim thay Lop tin chi phu hop voi thong so da nhap!", false);
        return;
    }

    bool isEditMode = batDauBangEditMode;
    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MASV, 2: NAME, 3: SCORE
    int sortMode = 0; // 0: TEN (A-Z), 1: MASV (A-Z), 2: DIEM (Giam), 3: DIEM (Tang)

    const string searchFieldNames[] = {"ALL", "MASV", "NAME", "SCORE"};
    const string sortNames[] = {"TEN (A-Z)", "MASV (A-Z)", "SCORE (Desc)", "SCORE (Asc)"};

    clrscr();

    while (true) {
        SinhVienDiemLTC arrSV[1000];
        int totalValid = layDanhSachSVDangKyLTC(dsltc.nodes[indexLTC], dslop, arrSV, 1000, false);

        SinhVienDiemLTC* allSV = (totalValid > 0) ? new SinhVienDiemLTC[totalValid] : nullptr;
        for (int i = 0; i < totalValid; i++) {
            allSV[i] = arrSV[i];
        }

        // Sắp xếp allSV theo sortMode bằng QuickSort O(N log N)
        if (totalValid > 1) {
            quickSort(allSV, 0, totalValid - 1, [sortMode](const SinhVienDiemLTC &a, const SinhVienDiemLTC &b) {
                switch (sortMode) {
                    case 1: return a.MASV < b.MASV;
                    case 2: return a.DIEM > b.DIEM;
                    case 3: return a.DIEM < b.DIEM;
                    case 0:
                    default:
                        if (a.TEN != b.TEN) return a.TEN < b.TEN;
                        return a.HO < b.HO;
                }
            });
        }

        // Lọc allSV theo searchQuery và searchField
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        SinhVienDiemLTC* displaySV = (totalValid > 0) ? new SinhVienDiemLTC[totalValid] : nullptr;
        int total = 0;
        for (int i = 0; i < totalValid; i++) {
            const auto &sv = allSV[i];
            if (q.empty()) {
                displaySV[total++] = sv;
                continue;
            }
            string sMASV = sv.MASV; for (char &ch : sMASV) ch = tolower(static_cast<unsigned char>(ch));
            string sName = sv.HO + " " + sv.TEN; for (char &ch : sName) ch = tolower(static_cast<unsigned char>(ch));
            string sScore = (sv.DIEM >= 0) ? to_string(sv.DIEM) : "";

            bool match = false;
            switch (searchField) {
                case 1: match = (sMASV.find(q) != string::npos); break;
                case 2: match = (sName.find(q) != string::npos); break;
                case 3: match = (sScore.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMASV.find(q) != string::npos) ||
                            (sName.find(q) != string::npos) ||
                            (sScore.find(q) != string::npos);
                    break;
            }
            if (match) displaySV[total++] = sv;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 8, y = 4;

        // Dòng 1: Tiêu đề danh sách kèm chỉ báo Mode (Edit Mode vs View Mode)
        gotoxy(x, 1); 
        if (isEditMode) {
            SetColorAndBG(0, 14); cout << " [MODE: EDIT / NHAP DIEM] ";
            SetColorAndBG(7, 0);  cout << " ";
            SetColor(11); cout << padRight(tenMH, 24);
        } else {
            SetColorAndBG(0, 10); cout << " [MODE: VIEW / XEM DIEM] ";
            SetColorAndBG(7, 0);  cout << " ";
            SetColor(11); cout << padRight(tenMH, 24);
        }
        SetColor(7);
        cout << " [NK: " << nienKhoa << " | HK: " << hocKy << " | Nhom: " << nhom << "]" << string(10, ' ');

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp (Search & Sort Bar)
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 8) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 18) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalValid << "  " << string(10, ' ');

        // Viền trên bảng (STT: 5, MASV: 14, HO: 24, TEN: 12, DIEM: 8)
        SetColorAndBG(9, 0); 
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(14, (char)196) << char(194) 
             << string(24, (char)196) << char(194) << string(12, (char)196) << char(194) << string(8, (char)196) << char(191);
        
        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << "     MASV     " 
             << char(179) << "           HO           " << char(179) << "    TEN     " << char(179) << "  DIEM  " << char(179);
             
        // Viền giữa
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(14, (char)196) << char(197) 
             << string(24, (char)196) << char(197) << string(12, (char)196) << char(197) << string(8, (char)196) << char(180);
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].MASV, 13);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].HO, 23);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].TEN, 11);

                SetColorAndBG(9, 0); cout << char(179);
                if (!isSel) {
                    SetColorAndBG(layMauDiem(displaySV[i].DIEM), 0);
                } else {
                    SetColorAndBG(0, 14);
                }
                cout << padCenter(formatDiem(displaySV[i].DIEM), 8);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(14, ' ') 
                     << char(179) << string(24, ' ') << char(179) << string(12, ' ') 
                     << char(179) << string(8, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 70, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(14, (char)196) << char(193) 
             << string(24, (char)196) << char(193) << string(12, (char)196) << char(193) << string(8, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        if (isEditMode) {
            SetBGColor(15); SetColor(0); cout << " [F2] Save "; SetBGColor(0); cout << " ";
            SetBGColor(15); SetColor(0); cout << " [F3/Enter] Input Grade "; SetBGColor(0); cout << " ";
        }
        SetBGColor(15); SetColor(0); cout << " [F4] Filter "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F6] Switch Mode "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F7] Grade Chart "; SetBGColor(0);
        SetColor(7);
        cout << string(10, ' ');

        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "                     "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 70, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 70, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 70, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 70, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 5;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F6) {
                    isEditMode = !isEditMode;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F7) {
                    if (totalValid == 0) {
                        popupCanhBao("Lop tin chi chua co sinh vien!", false);
                    } else {
                        float* grades = new float[totalValid];
                        for (int k = 0; k < totalValid; k++) grades[k] = allSV[k].DIEM;
                        string title = "GRADE DISTRIBUTION - " + tenMH + " (Nhom " + to_string(nhom) + ")";
                        popupBieuDoPhanPhoiDiem(title, grades, totalValid);
                        delete[] grades;
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F2) {
                    if (isEditMode) {
                        ghiFileLTC(dsltc);
                        popupThongBaoThanhCong("Da luu bang Diem vao file thanh cong!");
                    }
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F3) {
                    if (isEditMode) {
                        if (total == 0) {
                            popupCanhBao("Danh sach hien tai dang trong!", false);
                        } else {
                            string maSVTarget = displaySV[selectedIdx].MASV;
                            string tenSVTarget = displaySV[selectedIdx].HO + " " + displaySV[selectedIdx].TEN;
                            string strDiem = "";
                            if (popupNhapMa("--- DIEM CHO " + tenSVTarget + " ---", "Nhap Diem (0.0 - 10.0): ", strDiem, NHAP_SO_THUC, 4)) {
                                try {
                                    float d = stof(strDiem);
                                    if (kiemTraDiemHopLe(d)) {
                                        capNhatDiemSinhVien(dsltc, dsltc.nodes[indexLTC]->MALOPTC, maSVTarget, d);
                                        if (selectedIdx < total - 1) {
                                            selectedIdx++;
                                            if (selectedIdx >= firstVisible + limit) firstVisible = selectedIdx - limit + 1;
                                        }
                                    } else {
                                        popupCanhBao("Loi: Diem phai tu 0.0 den 10.0!", false);
                                    }
                                } catch(...) {
                                    popupCanhBao("Loi: Vui long nhap dung dinh dang so!", false);
                                }
                            }
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ENTER) {
                if (isEditMode) {
                    if (total == 0) {
                        popupCanhBao("Danh sach hien tai dang trong!", false);
                    } else {
                        string maSVTarget = displaySV[selectedIdx].MASV;
                        string tenSVTarget = displaySV[selectedIdx].HO + " " + displaySV[selectedIdx].TEN;
                        string strDiem = "";
                        if (popupNhapMa("--- DIEM CHO " + tenSVTarget + " ---", "Nhap Diem (0.0 - 10.0): ", strDiem, NHAP_SO_THUC, 4)) {
                            try {
                                float d = stof(strDiem);
                                if (kiemTraDiemHopLe(d)) {
                                    capNhatDiemSinhVien(dsltc, dsltc.nodes[indexLTC]->MALOPTC, maSVTarget, d);
                                    if (selectedIdx < total - 1) {
                                        selectedIdx++;
                                        if (selectedIdx >= firstVisible + limit) firstVisible = selectedIdx - limit + 1;
                                    }
                                } else {
                                    popupCanhBao("Loi: Diem phai tu 0.0 den 10.0!", false);
                                }
                            } catch(...) {
                                popupCanhBao("Loi: Vui long nhap dung dinh dang so!", false);
                            }
                        }
                    }
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allSV;
                    delete[] displaySV;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)

        delete[] allSV;
        delete[] displaySV;
    } // end while (true)
}

inline void inBangDiemMonHocToanDien(DS_LTC &dsltc, DS_LOP &dslop, treeMH dsmh) {
    quanLyBangDiemLTC(dsltc, dslop, dsmh, false);
}

// 7b. Màn hình In Danh Sách Sinh Viên Đăng Ký Lớp Tín Chỉ (Mục b của Đề bài)
// Kết xuất: Mã SV, Họ tên, Trạng thái (đã hủy)
inline void inDanhSachSVDangKyLTCToanDien(DS_LTC &dsltc, DS_LOP &dslop, treeMH dsmh) {
    anConTro();
    string nienKhoa = "", maMH = "";
    int hocKy = 0, nhom = 0;

    if (!popupNhapThongSoLTC("--- LOC LOP TIN CHI ---", nienKhoa, hocKy, maMH, nhom)) return;

    treeMH nodeMH = timKiemMonHoc(dsmh, maMH);
    if (nodeMH == NULL) {
        popupCanhBao("Loi: Khong tim thay Mon hoc " + maMH, false);
        return;
    }
    string tenMH = nodeMH->mh.TENMH;

    int indexLTC = timKiemLTCTheoThongSo(dsltc, nienKhoa, hocKy, maMH, nhom);
    if (indexLTC == -1) {
        popupCanhBao("Loi: Khong tim thay Lop tin chi phu hop!", false);
        return;
    }

    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MASV, 2: NAME, 3: STATUS
    int sortMode = 0; // 0: TEN (A-Z), 1: MASV (A-Z), 2: STATUS

    const string searchFieldNames[] = {"ALL", "MASV", "NAME", "STATUS"};
    const string sortNames[] = {"TEN (A-Z)", "MASV (A-Z)", "STATUS"};

    clrscr();

    while (true) {
        SinhVienDiemLTC arrSV[1000];
        int totalValid = layDanhSachSVDangKyLTC(dsltc.nodes[indexLTC], dslop, arrSV, 1000, true);

        SinhVienDiemLTC* allSV = (totalValid > 0) ? new SinhVienDiemLTC[totalValid] : nullptr;
        for (int i = 0; i < totalValid; i++) {
            allSV[i] = arrSV[i];
        }

        // Sắp xếp allSV theo sortMode bằng QuickSort O(N log N)
        if (totalValid > 1) {
            quickSort(allSV, 0, totalValid - 1, [sortMode](const SinhVienDiemLTC &a, const SinhVienDiemLTC &b) {
                switch (sortMode) {
                    case 1: return a.MASV < b.MASV;
                    case 2: return !a.HUYDK && b.HUYDK;
                    case 0:
                    default:
                        if (a.TEN != b.TEN) return a.TEN < b.TEN;
                        return a.HO < b.HO;
                }
            });
        }

        // Lọc allSV theo searchQuery và searchField
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        SinhVienDiemLTC* displaySV = (totalValid > 0) ? new SinhVienDiemLTC[totalValid] : nullptr;
        int total = 0;
        for (int i = 0; i < totalValid; i++) {
            const auto &sv = allSV[i];
            if (q.empty()) {
                displaySV[total++] = sv;
                continue;
            }
            string sMASV = sv.MASV; for (char &ch : sMASV) ch = tolower(static_cast<unsigned char>(ch));
            string sName = sv.HO + " " + sv.TEN; for (char &ch : sName) ch = tolower(static_cast<unsigned char>(ch));
            string sStatus = sv.HUYDK ? "da huy canceled" : "dang ky registered";

            bool match = false;
            switch (searchField) {
                case 1: match = (sMASV.find(q) != string::npos); break;
                case 2: match = (sName.find(q) != string::npos); break;
                case 3: match = (sStatus.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMASV.find(q) != string::npos) ||
                            (sName.find(q) != string::npos) ||
                            (sStatus.find(q) != string::npos);
                    break;
            }
            if (match) displaySV[total++] = sv;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 8, y = 4;

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "ENROLLED STUDENTS LIST - CLASS: " << padRight(tenMH + " (Nhom " + to_string(nhom) + ")", 35) << string(15, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp (Search & Sort Bar)
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 8) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 18) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalValid << "  " << string(10, ' ');

        // Viền trên bảng (STT: 5, MASV: 14, HO: 24, TEN: 12, STATUS: 18)
        SetColorAndBG(9, 0); 
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(14, (char)196) << char(194) 
             << string(24, (char)196) << char(194) << string(12, (char)196) << char(194) << string(18, (char)196) << char(191);
        
        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << "     MASV     " 
             << char(179) << "           HO           " << char(179) << "    TEN     " << char(179) << "    TRANG THAI    " << char(179);
             
        // Viền giữa
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(14, (char)196) << char(197) 
             << string(24, (char)196) << char(197) << string(12, (char)196) << char(197) << string(18, (char)196) << char(180);
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].MASV, 13);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].HO, 23);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].TEN, 11);

                SetColorAndBG(9, 0); cout << char(179);
                if (displaySV[i].HUYDK) {
                    SetColorAndBG(isSel ? 4 : 12, bgCol);
                    cout << padCenter("[DA HUY]", 18);
                } else {
                    SetColorAndBG(isSel ? 2 : 10, bgCol);
                    cout << padCenter("[DANG KY]", 18);
                }

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(14, ' ') 
                     << char(179) << string(24, ' ') << char(179) << string(12, ' ') 
                     << char(179) << string(18, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 80, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(14, (char)196) << char(193) 
             << string(24, (char)196) << char(193) << string(12, (char)196) << char(193) << string(18, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort "; SetBGColor(0);
        SetColor(7);
        cout << string(30, ' ');

        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "                     "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 80, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 80, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 80, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 80, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 3;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allSV;
                    delete[] displaySV;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)

        delete[] allSV;
        delete[] displaySV;
    } // end while (true)
}

inline void inDiemTBKhoaHocLopToanDien(DS_LOP &dslop, DS_LTC &dsltc, treeMH dsmh) {
    anConTro();
    string maLop;
    if (!popupNhapMa("--- DIEM TB KHOA HOC CUA LOP ---", "Nhap Ma Lop: ", maLop)) return;

    int indexLop = timKiemLop(dslop, maLop);
    if (indexLop == -1) {
        popupCanhBao("Loi: Khong tim thay Lop " + maLop, false);
        return;
    }

    int limit = 16;
    int selectedIdx = 0;
    int firstVisible = 0;
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MASV, 2: NAME, 3: GPA
    int sortMode = 0; // 0: TEN (A-Z), 1: MASV (A-Z), 2: GPA (Desc), 3: GPA (Asc)

    const string searchFieldNames[] = {"ALL", "MASV", "NAME", "GPA"};
    const string sortNames[] = {"TEN (A-Z)", "MASV (A-Z)", "GPA (Desc)", "GPA (Asc)"};

    clrscr();

    while (true) {
        SinhVienDiemTBKhoaHoc arrTB[1000];
        int totalRaw = layDiemTBKhoaHocCuaLop(dslop, indexLop, dsltc, dsmh, arrTB, 1000);

        if (totalRaw == 0) {
            popupCanhBao("Lop " + maLop + " chua co sinh vien!", false);
            return;
        }

        SinhVienDiemTBKhoaHoc* allTB = (totalRaw > 0) ? new SinhVienDiemTBKhoaHoc[totalRaw] : nullptr;
        for (int i = 0; i < totalRaw; i++) {
            allTB[i] = arrTB[i];
        }
        int totalValid = totalRaw;

        // Sắp xếp allTB theo sortMode bằng QuickSort O(N log N)
        if (totalRaw > 1) {
            quickSort(allTB, 0, totalRaw - 1, [sortMode](const SinhVienDiemTBKhoaHoc &a, const SinhVienDiemTBKhoaHoc &b) {
                switch (sortMode) {
                    case 1: return a.MASV < b.MASV;
                    case 2: return a.DIEMTB > b.DIEMTB;
                    case 3: return a.DIEMTB < b.DIEMTB;
                    case 0:
                    default:
                        if (a.TEN != b.TEN) return a.TEN < b.TEN;
                        return a.HO < b.HO;
                }
            });
        }

        // Lọc allTB theo searchQuery và searchField
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        SinhVienDiemTBKhoaHoc* displayTB = (totalRaw > 0) ? new SinhVienDiemTBKhoaHoc[totalRaw] : nullptr;
        int total = 0;
        for (int i = 0; i < totalRaw; i++) {
            const auto &sv = allTB[i];
            if (q.empty()) {
                displayTB[total++] = sv;
                continue;
            }
            string sMASV = sv.MASV; for (char &ch : sMASV) ch = tolower(static_cast<unsigned char>(ch));
            string sName = sv.HO + " " + sv.TEN; for (char &ch : sName) ch = tolower(static_cast<unsigned char>(ch));
            char bufScore[16]; snprintf(bufScore, sizeof(bufScore), "%.2f", sv.DIEMTB);
            string sGPA = bufScore;

            bool match = false;
            switch (searchField) {
                case 1: match = (sMASV.find(q) != string::npos); break;
                case 2: match = (sName.find(q) != string::npos); break;
                case 3: match = (sGPA.find(q) != string::npos); break;
                case 0:
                default:
                    match = (sMASV.find(q) != string::npos) ||
                            (sName.find(q) != string::npos) ||
                            (sGPA.find(q) != string::npos);
                    break;
            }
            if (match) displayTB[total++] = sv;
        }

        if (total == 0) {
            selectedIdx = 0;
            firstVisible = 0;
        } else {
            if (selectedIdx >= total) selectedIdx = total - 1;
            if (selectedIdx < 0) selectedIdx = 0;
            if (firstVisible > selectedIdx) firstVisible = selectedIdx;
            if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
            if (firstVisible < 0) firstVisible = 0;
        }

        int x = 8, y = 4;

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "ACADEMIC GPA SUMMARY: " << padRight(maLop + " - " + dslop.nodes[indexLop].TENLOP, 40) << string(15, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp (Search & Sort Bar)
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 8) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 18) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalValid << "  " << string(10, ' ');

        // Viền trên bảng (STT: 5, MASV: 14, HO: 24, TEN: 12, Diem TB: 12)
        SetColorAndBG(9, 0); 
        gotoxy(x, y); 
        cout << char(218) << string(5, (char)196) << char(194) << string(14, (char)196) << char(194) 
             << string(24, (char)196) << char(194) << string(12, (char)196) << char(194) << string(12, (char)196) << char(191);
        
        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << "     MASV     " 
             << char(179) << "           HO           " << char(179) << "    TEN     " << char(179) << "  Diem TB   " << char(179);
             
        // Viền giữa
        gotoxy(x, y + 2); 
        cout << char(195) << string(5, (char)196) << char(197) << string(14, (char)196) << char(197) 
             << string(24, (char)196) << char(197) << string(12, (char)196) << char(197) << string(12, (char)196) << char(180);
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisible + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                bool isSel = (i == selectedIdx);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displayTB[i].MASV, 13);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displayTB[i].HO, 23);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displayTB[i].TEN, 11);

                char bufTB[16];
                snprintf(bufTB, sizeof(bufTB), "%.2f", displayTB[i].DIEMTB);
                SetColorAndBG(9, 0); cout << char(179);
                if (!isSel) {
                    SetColorAndBG(layMauDiem(displayTB[i].DIEMTB), 0);
                } else {
                    SetColorAndBG(0, 14);
                }
                cout << padCenter(bufTB, 12);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(14, ' ') 
                     << char(179) << string(24, ' ') << char(179) << string(12, ' ') 
                     << char(179) << string(12, ' ') << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limit; r++) {
            veMotDong(r);
        }
        
        // Vẽ thanh cuộn dọc trực quan
        veThanhCuon(x + 74, y + 3, limit, total, firstVisible);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limit;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(14, (char)196) << char(193) 
             << string(24, (char)196) << char(193) << string(12, (char)196) << char(193) << string(12, (char)196) << char(217);
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F7] Grade Chart "; SetBGColor(0);
        SetColor(7);
        cout << string(20, ' ');
        
        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Row   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdx + 1) << "/" << total << "                     "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 81, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdx + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) {
                    if (selectedIdx > 0) {
                        int prevSel = selectedIdx;
                        selectedIdx--;
                        if (selectedIdx < firstVisible) {
                            firstVisible = selectedIdx;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 74, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) {
                    if (selectedIdx < total - 1) {
                        int prevSel = selectedIdx;
                        selectedIdx++;
                        if (selectedIdx >= firstVisible + limit) {
                            firstVisible = selectedIdx - limit + 1;
                            for (int r = 0; r < limit; r++) veMotDong(r);
                            veThanhCuon(x + 74, y + 3, limit, total, firstVisible);
                        } else {
                            veMotDong(prevSel - firstVisible);
                            veMotDong(selectedIdx - firstVisible);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) {
                    if (selectedIdx > 0) {
                        selectedIdx -= limit;
                        if (selectedIdx < 0) selectedIdx = 0;
                        if (selectedIdx < firstVisible) firstVisible = selectedIdx;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 74, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_RIGHT) {
                    if (selectedIdx < total - 1) {
                        selectedIdx += limit;
                        if (selectedIdx >= total) selectedIdx = (total > 0) ? total - 1 : 0;
                        if (firstVisible + limit <= selectedIdx) firstVisible = selectedIdx - limit + 1;
                        if (firstVisible < 0) firstVisible = 0;
                        for (int r = 0; r < limit; r++) veMotDong(r);
                        veThanhCuon(x + 74, y + 3, limit, total, firstVisible);
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 4;
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F7) {
                    if (totalRaw == 0) {
                        popupCanhBao("Lop chua co sinh vien de ve bieu do!", false);
                    } else {
                        float* grades = (totalRaw > 0) ? new float[totalRaw] : nullptr;
                        for (int k = 0; k < totalRaw; k++) grades[k] = allTB[k].DIEMTB;
                        string title = "CLASS GPA DISTRIBUTION - LOP: " + maLop;
                        popupBieuDoPhanPhoiDiem(title, grades, totalRaw);
                        delete[] grades;
                    }
                    canTaiLaiDuLieu = true;
                }
            } 
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == KEY_ESC) {
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdx = 0; firstVisible = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] allTB;
                    delete[] displayTB;
                    return;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdx = 0; firstVisible = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)

        delete[] allTB;
        delete[] displayTB;
    } // end while (true)
}

inline void inBangDiemTongKetToanDien(DS_LOP &dslop, DS_LTC &dsltc, treeMH dsmh) {
    anConTro();
    string maLop;
    if (!popupNhapMa("--- BANG DIEM TONG KET ---", "Nhap Ma Lop: ", maLop)) return;

    int indexLop = timKiemLop(dslop, maLop);
    if (indexLop == -1) {
        popupCanhBao("Loi: Khong tim thay Lop " + maLop, false);
        return;
    }

    int totalSV = 0;
    for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL; p = p->next) totalSV++;
    if (totalSV == 0) {
        popupCanhBao("Lop " + maLop + " chua co sinh vien!", false);
        return;
    }

    // Lấy danh sách toàn bộ môn học trong cây vào mảng
    int totalMH = demSoMonHoc(dsmh);
    if (totalMH == 0) {
        popupCanhBao("He thong chua co mon hoc nao!", false);
        return;
    }
    Monhoc* arrMH = new Monhoc[totalMH];
    int mhIdxCounter = 0;
    doMonHocVaoMang(dsmh, arrMH, mhIdxCounter);

    // Lọc ra các môn học mà sinh viên trong lớp này THỰC SỰ ĐÃ CÓ ĐIỂM
    Monhoc* arrMHCoDiem = new Monhoc[totalMH];
    int soLuongMHCoDiem = 0;
    for (int m = 0; m < totalMH; m++) {
        bool coDiem = false;
        for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL; p = p->next) {
            float diem = layDiemMonHoc(p->sv.MASV, arrMH[m].MAMH, dsltc);
            if (diem >= 0) {
                coDiem = true;
                break;
            }
        }
        if (coDiem) {
            arrMHCoDiem[soLuongMHCoDiem++] = arrMH[m];
        }
    }
    delete[] arrMH;
    arrMH = arrMHCoDiem;
    totalMH = soLuongMHCoDiem;

    if (totalMH == 0) {
        popupCanhBao("Lop " + maLop + " chua co sinh vien nao co diem bat ky mon hoc nao!", false);
        delete[] arrMH;
        return;
    }

    int limitSV = 16;
    int limitMH = 4;
    int curPageMH = 1;
    int totalPageMH = (totalMH + limitMH - 1) / limitMH;
    if (totalPageMH < 1) totalPageMH = 1;

    int selectedIdxSV = 0;
    int firstVisibleSV = 0;
    string searchQuery = "";
    int searchField = 0; // 0: ALL, 1: MASV, 2: NAME, 3: SCORE
    int sortMode = 0;    // 0: TEN (A-Z), 1: MASV (A-Z), 2: DIEM TB

    const string searchFieldNames[] = {"ALL", "MASV", "NAME", "SCORE"};
    const string sortNames[] = {"TEN (A-Z)", "MASV (A-Z)", "AVG SCORE"};

    clrscr();

    while (true) {
        Sinhvien* sortedSV = (totalSV > 0) ? new Sinhvien[totalSV] : nullptr;
        int svCount = 0;
        for (PTRSV p = dslop.nodes[indexLop].dssv; p != NULL; p = p->next) {
            sortedSV[svCount++] = p->sv;
        }

        // Sắp xếp sortedSV theo sortMode bằng QuickSort O(N log N)
        if (totalSV > 1) {
            quickSort(sortedSV, 0, totalSV - 1, [sortMode, &arrMH, totalMH, &dsltc](const Sinhvien &a, const Sinhvien &b) {
                switch (sortMode) {
                    case 1: return a.MASV < b.MASV;
                    case 2: {
                        float avg1 = -1.0f, avg2 = -1.0f;
                        int c1 = 0, c2 = 0; float s1 = 0.0f, s2 = 0.0f;
                        for (int m = 0; m < totalMH; m++) {
                            float d1 = layDiemMonHoc(a.MASV, arrMH[m].MAMH, dsltc);
                            if (d1 >= 0) { s1 += d1; c1++; }
                            float d2 = layDiemMonHoc(b.MASV, arrMH[m].MAMH, dsltc);
                            if (d2 >= 0) { s2 += d2; c2++; }
                        }
                        if (c1 > 0) avg1 = s1 / c1;
                        if (c2 > 0) avg2 = s2 / c2;
                        return avg1 > avg2;
                    }
                    case 0:
                    default:
                        if (a.TEN != b.TEN) return a.TEN < b.TEN;
                        return a.HO < b.HO;
                }
            });
        }

        // Lọc theo searchQuery và searchField
        string q = searchQuery;
        for (char &ch : q) ch = tolower(static_cast<unsigned char>(ch));

        Sinhvien* displaySV = (totalSV > 0) ? new Sinhvien[totalSV] : nullptr;
        int total = 0;
        for (int i = 0; i < totalSV; i++) {
            const auto &sv = sortedSV[i];
            if (q.empty()) {
                displaySV[total++] = sv;
                continue;
            }
            string sMASV = sv.MASV; for (char &ch : sMASV) ch = tolower(static_cast<unsigned char>(ch));
            string sName = sv.HO + " " + sv.TEN; for (char &ch : sName) ch = tolower(static_cast<unsigned char>(ch));

            bool matchScore = false;
            if (searchField == 3 || searchField == 0) {
                for (int m = 0; m < totalMH; m++) {
                    float d = layDiemMonHoc(sv.MASV, arrMH[m].MAMH, dsltc);
                    if (d >= 0) {
                        char bufD[16]; snprintf(bufD, sizeof(bufD), "%.1f", d);
                        string sd = bufD;
                        if (sd.find(q) != string::npos) { matchScore = true; break; }
                    }
                }
            }

            bool match = false;
            switch (searchField) {
                case 1: match = (sMASV.find(q) != string::npos); break;
                case 2: match = (sName.find(q) != string::npos); break;
                case 3: match = matchScore; break;
                case 0:
                default:
                    match = (sMASV.find(q) != string::npos) ||
                            (sName.find(q) != string::npos) ||
                            matchScore;
                    break;
            }
            if (match) displaySV[total++] = sv;
        }

        if (total == 0) {
            selectedIdxSV = 0;
            firstVisibleSV = 0;
        } else {
            if (selectedIdxSV >= total) selectedIdxSV = total - 1;
            if (selectedIdxSV < 0) selectedIdxSV = 0;
            if (firstVisibleSV > selectedIdxSV) firstVisibleSV = selectedIdxSV;
            if (firstVisibleSV + limitSV <= selectedIdxSV) firstVisibleSV = selectedIdxSV - limitSV + 1;
            if (firstVisibleSV < 0) firstVisibleSV = 0;
        }

        int x = 2, y = 4;

        // Dòng 1: Tiêu đề danh sách
        gotoxy(x, 1); SetColor(11); 
        cout << "BANG DIEM TONG KET: " << padRight(maLop + " - " + dslop.nodes[indexLop].TENLOP, 40)
             << " [Subjects: Page " << curPageMH << "/" << totalPageMH << "]" << string(15, ' ');
        SetColor(7);

        // Dòng 2: Thanh Toolbar Tìm kiếm & Sắp xếp (Search & Sort Bar)
        gotoxy(x, 2);
        SetColor(14); cout << "[F4] Filter: ";
        SetColorAndBG(0, 11); cout << " " << padCenter(searchFieldNames[searchField], 8) << " "; 
        SetColorAndBG(7, 0); cout << "  Search: ";
        SetColorAndBG(15, 1); cout << " " << padRight(searchQuery.empty() ? "(Type to search)" : searchQuery, 18) << " ";
        SetColorAndBG(7, 0); cout << "  ";
        SetColor(14); cout << "[F5] Sort: ";
        SetColorAndBG(0, 14); cout << " " << padCenter(sortNames[sortMode], 15) << " ";
        SetColorAndBG(7, 0); 
        cout << "  Matches: " << total << "/" << totalSV << "  " << string(10, ' ');

        int startMH = (curPageMH - 1) * limitMH;
        int endMH = startMH + limitMH;
        if (endMH > totalMH) endMH = totalMH;

        // Viền trên
        SetColorAndBG(9, 0);
        gotoxy(x, y);
        cout << char(218) << string(5, (char)196) << char(194) << string(15, (char)196) << char(194) << string(35, (char)196) << char(194);
        for(int k = 0; k < limitMH; k++) {
            cout << string(12, (char)196) << (k == limitMH - 1 ? char(191) : char(194));
        }

        // Header
        gotoxy(x, y + 1);
        cout << char(179) << " STT " << char(179) << " MA SV         " << char(179) << " HO VA TEN                         ";
        for (int k = 0; k < limitMH; k++) {
            int mhIdx = startMH + k;
            cout << char(179);
            if (mhIdx < endMH) cout << padCenter(arrMH[mhIdx].TENMH, 12);
            else cout << string(12, ' ');
        }
        cout << char(179);

        // Viền giữa
        gotoxy(x, y + 2);
        cout << char(195) << string(5, (char)196) << char(197) << string(15, (char)196) << char(197) << string(35, (char)196) << char(197);
        for(int k = 0; k < limitMH; k++) {
            cout << string(12, (char)196) << (k == limitMH - 1 ? char(180) : char(197));
        }
        SetColorAndBG(7, 0);

        auto veMotDong = [&](int r) {
            int i = firstVisibleSV + r;
            gotoxy(x, y + 3 + r);
            if (i < total) {
                bool isSel = (i == selectedIdxSV);
                WORD textCol = isSel ? 0 : 7;
                WORD bgCol = isSel ? 14 : 0;

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << padCenter(to_string(i + 1), 5);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].MASV, 14);

                SetColorAndBG(9, 0); cout << char(179);
                SetColorAndBG(textCol, bgCol); cout << " " << padRight(displaySV[i].HO + " " + displaySV[i].TEN, 34);
                
                for (int k = 0; k < limitMH; k++) {
                    int mhIdx = startMH + k;
                    SetColorAndBG(9, 0); cout << char(179);
                    if (mhIdx < endMH) {
                        float diem = layDiemMonHoc(displaySV[i].MASV, arrMH[mhIdx].MAMH, dsltc);
                        if (!isSel) {
                            SetColorAndBG(layMauDiem(diem), 0);
                        } else {
                            SetColorAndBG(0, 14);
                        }
                        cout << padCenter(formatDiem(diem), 12);
                    } else {
                        SetColorAndBG(textCol, bgCol);
                        cout << string(12, ' ');
                    }
                }
                SetColorAndBG(9, 0); cout << char(179); SetColorAndBG(7, 0);
            } else {
                SetColorAndBG(9, 0);
                cout << char(179) << string(5, ' ') << char(179) << string(15, ' ') << char(179) << string(35, ' ');
                for (int k = 0; k < limitMH; k++) {
                    cout << char(179) << string(12, ' ');
                }
                cout << char(179);
                SetColorAndBG(7, 0);
            }
        };

        for (int r = 0; r < limitSV; r++) {
            veMotDong(r);
        }

        // Vẽ thanh cuộn dọc trực quan cố định ngoài viền bảng
        veThanhCuon(x + 59 + limitMH * 13, y + 3, limitSV, total, firstVisibleSV);

        // Viền dưới khép kín
        int toaDoY = y + 3 + limitSV;
        SetColorAndBG(9, 0);
        gotoxy(x, toaDoY); 
        cout << char(192) << string(5, (char)196) << char(193) << string(15, (char)196) << char(193) << string(35, (char)196) << char(193);
        for(int k = 0; k < limitMH; k++) {
            cout << string(12, (char)196) << (k == limitMH - 1 ? char(217) : char(193));
        }
        SetColorAndBG(7, 0);
        
        // Dòng 1: Navigation Badges
        gotoxy(x, toaDoY + 2); 
        SetBGColor(15); SetColor(0); cout << " [ESC] Exit "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [Left/Right] Switch Subject "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F4] Filter "; SetBGColor(0); cout << " ";
        SetBGColor(15); SetColor(0); cout << " [F5] Sort "; SetBGColor(0);
        SetColor(7);
        cout << string(20, ' ');

        // Dòng 2: Action Details & Row Info
        gotoxy(x, toaDoY + 3);
        SetColor(14); 
        cout << "[Up/Down]: Select Student   |   [Type to Search]   |   [Backspace]: Erase   |   Row: " 
             << (total == 0 ? 0 : selectedIdxSV + 1) << "/" << total << "                     "; 
        SetColor(7);

        auto capNhatRowInfo = [&]() {
            gotoxy(x + 85, toaDoY + 3);
            SetColor(14);
            cout << padRight(to_string(total == 0 ? 0 : selectedIdxSV + 1) + "/" + to_string(total), 10);
            SetColor(7);
        };

        bool canTaiLaiDuLieu = false;
        while (!canTaiLaiDuLieu) {
            char c = _getch();
            if (c == KEY_ESC) { 
                if (!searchQuery.empty()) {
                    searchQuery.clear();
                    selectedIdxSV = 0; firstVisibleSV = 0;
                    canTaiLaiDuLieu = true;
                } else {
                    delete[] sortedSV;
                    delete[] displaySV;
                    delete[] arrMH;
                    return;
                }
            }
            else if (c == 8) { // Backspace
                if (!searchQuery.empty()) {
                    searchQuery.pop_back();
                    selectedIdxSV = 0; firstVisibleSV = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c == -32 || c == 0) {
                c = _getch();
                if (c == KEY_UP) { 
                    if (selectedIdxSV > 0) {
                        int prevSel = selectedIdxSV;
                        selectedIdxSV--;
                        if (selectedIdxSV < firstVisibleSV) {
                            firstVisibleSV = selectedIdxSV;
                            for (int r = 0; r < limitSV; r++) veMotDong(r);
                            veThanhCuon(x + 59 + limitMH * 13, y + 3, limitSV, total, firstVisibleSV);
                        } else {
                            veMotDong(prevSel - firstVisibleSV);
                            veMotDong(selectedIdxSV - firstVisibleSV);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_DOWN) { 
                    if (selectedIdxSV < total - 1) {
                        int prevSel = selectedIdxSV;
                        selectedIdxSV++;
                        if (selectedIdxSV >= firstVisibleSV + limitSV) {
                            firstVisibleSV = selectedIdxSV - limitSV + 1;
                            for (int r = 0; r < limitSV; r++) veMotDong(r);
                            veThanhCuon(x + 59 + limitMH * 13, y + 3, limitSV, total, firstVisibleSV);
                        } else {
                            veMotDong(prevSel - firstVisibleSV);
                            veMotDong(selectedIdxSV - firstVisibleSV);
                        }
                        capNhatRowInfo();
                    }
                }
                else if (c == KEY_LEFT) { 
                    if (curPageMH > 1) { 
                        curPageMH--; 
                        canTaiLaiDuLieu = true;
                    } 
                }
                else if (c == KEY_RIGHT) { 
                    if (curPageMH < totalPageMH) { 
                        curPageMH++; 
                        canTaiLaiDuLieu = true;
                    } 
                }
                else if (c == KEY_F4) {
                    searchField = (searchField + 1) % 4;
                    selectedIdxSV = 0; firstVisibleSV = 0;
                    canTaiLaiDuLieu = true;
                }
                else if (c == KEY_F5) {
                    sortMode = (sortMode + 1) % 3;
                    selectedIdxSV = 0; firstVisibleSV = 0;
                    canTaiLaiDuLieu = true;
                }
            }
            else if (c >= 32 && c <= 126 && c != ',') {
                searchQuery += c;
                selectedIdxSV = 0; firstVisibleSV = 0;
                canTaiLaiDuLieu = true;
            }
        } // end while (!canTaiLaiDuLieu)

        delete[] sortedSV;
        delete[] displaySV;
    } // end while (true)

    delete[] arrMH;
}


// ========================================================
// NHÓM TIỆN ÍCH DASHBOARD & MENU NAVIGATION
// ========================================================

inline void nhapDiemToanDien(DS_LTC &dsltc, DS_LOP &dslop, treeMH dsmh) {
    quanLyBangDiemLTC(dsltc, dslop, dsmh, true);
}



inline void veKhungMenu(int x, int y, int w, const string &title, bool isHighlight, bool isActive) {
    if (isHighlight) {
        if (isActive) SetColorAndBG(0, 14); // Nền vàng, chữ đen nổi bật
        else SetColorAndBG(15, 8);          // Nền xám, chữ trắng (khi focus menu con)
    } else {
        SetColorAndBG(7, 0);                // Nền đen, chữ trắng bình thường
    }
    
    string ngang(w - 1, (char)196);
    
    gotoxy(x, y); 
    cout << (char)218 << ngang << (char)191;
    
    gotoxy(x, y + 1); 
    cout << (char)179 << " " << padRight(title, w - 3) << " " << (char)179;
    
    gotoxy(x, y + 2); 
    cout << (char)192 << ngang << (char)217;
    
    SetColorAndBG(7, 0);
}

inline int layYSubMenu(int mainIdx, int subIdx) {
    if (mainIdx == 1) {
        // Menu 2: Lop & Sinh Vien (2 items) -> y = 10, 13
        return 10 + subIdx * 3;
    }
    if (mainIdx == 4) {
        // Menu 5: Diem & Bao Cao (5 items) -> y = 10, 13, 16, 19, 22
        return 10 + subIdx * 3;
    }
    return 10 + mainIdx * 3 + subIdx * 3;
}

inline void xoaVungSubMenu() {
    SetColorAndBG(7, 0);
    string emptyLine(45, ' ');
    for (int i = 8; i <= 26; i++) {
        gotoxy(58, i);
        cout << emptyLine; 
    }
}

inline void capNhatDongHo(int x, int y, string &lastClockStr) {
    time_t now = time(nullptr);
    tm *lt = localtime(&now);
    if (!lt) return;
    char buf[64];
    strftime(buf, sizeof(buf), "[ %d/%m/%Y | %H:%M:%S ]", lt);
    string curStr(buf);
    if (curStr != lastClockStr) {
        lastClockStr = curStr;
        gotoxy(x, y);
        SetColorAndBG(14, 0);
        cout << curStr;
        SetColorAndBG(7, 0);
    }
}

inline void veTheKPI(int x, int y, int w, string title, string bigVal, WORD valColor = 10) {
    string ngang(w - 2, (char)196);
    
    SetColorAndBG(9, 0);
    gotoxy(x, y);
    cout << (char)218;
    int titleLen = static_cast<int>(title.length());
    int padBefore = 2;
    int padAfter = (w - 2) - padBefore - titleLen;
    if (padAfter < 0) padAfter = 0;
    cout << string(padBefore, (char)196);
    SetColorAndBG(11, 0);
    cout << title;
    SetColorAndBG(9, 0);
    cout << string(padAfter, (char)196) << (char)191;
    
    gotoxy(x, y + 1); cout << (char)179;
    SetColorAndBG(valColor, 0);
    cout << padCenter(bigVal, w - 2);
    SetColorAndBG(9, 0);
    cout << (char)179;
    
    gotoxy(x, y + 2);
    cout << (char)192 << ngang << (char)217;
    SetColorAndBG(7, 0);
}

inline void layThongKeTongQuan(treeMH dsmh, const DS_LOP &dslop, const DS_LTC &dsltc,
                               int &soMon, int &soLop, int &soSV, int &soLTC, int &soLTCMo) {
    soMon = demSoMonHoc(dsmh);
    soLop = dslop.n;
    soSV = 0;
    for (int i = 0; i < dslop.n; i++) {
        PTRSV p = dslop.nodes[i].dssv;
        while (p != NULL) {
            soSV++;
            p = p->next;
        }
    }
    soLTC = dsltc.n;
    soLTCMo = 0;
    for (int i = 0; i < dsltc.n; i++) {
        if (dsltc.nodes[i] != NULL && !dsltc.nodes[i]->HUYLOP) {
            soLTCMo++;
        }
    }
}

inline void inThanhHuongDanMenu() {
    gotoxy(10, 29); SetColorAndBG(8, 0);
    cout << "[W/S/Up/Down]: Chon chuc nang   |   [A/D/Left/Right]: Chuyen cot   |   [Enter]: Chon   |   [ESC]: Thoat";
    SetColorAndBG(7, 0);
}

// Vẽ menu con
inline void veMenuCon(const int subCount[], const string subMenu[][6], int mainIdx, int curSubIdx, bool isSubFocused) {
    if (subCount[mainIdx] > 0) {
        for (int i = 0; i < subCount[mainIdx]; i++) {
            bool isHigh = (isSubFocused && i == curSubIdx);
            veKhungMenu(60, layYSubMenu(mainIdx, i), 42, subMenu[mainIdx][i], isHigh, true);
        }
    }
}

// Vẽ toàn bộ Menu khi khởi động hoặc quay về từ các màn hình con
inline void veToanBoMenu(treeMH dsmh, const DS_LOP &dslop, const DS_LTC &dsltc,
                         const string mainMenu[], const int subCount[], const string subMenu[][6],
                         int mainIndex, int subIndex, bool isSubMenuFocused, string &lastClockStr) {
    // 1. Tiêu đề chính & Đồng hồ thời gian thực
    gotoxy(10, 1);
    SetColorAndBG(11, 0);
    cout << "== CHUONG TRINH QUAN LY SINH VIEN THEO HE TIN CHI ==";
    
    capNhatDongHo(85, 1, lastClockStr);
    
    // Đường kẻ phân cách
    gotoxy(10, 2);
    SetColorAndBG(9, 0);
    cout << string(101, (char)205);
    SetColorAndBG(7, 0);
    
    // 2. 4 Thẻ KPI Tinh gọn (Chiều cao 3 dòng, thoáng đãng, sắc nét)
    int soMon = 0, soLop = 0, soSV = 0, soLTC = 0, soLTCMo = 0;
    layThongKeTongQuan(dsmh, dslop, dsltc, soMon, soLop, soSV, soLTC, soLTCMo);
    
    veTheKPI(10, 4, 23, " MON HOC (BST) ", to_string(soMon) + " Mon hoc", 14);
    veTheKPI(36, 4, 23, " LOP HOC ", to_string(soLop) + " Lop hoc", 11);
    veTheKPI(62, 4, 23, " SINH VIEN ", to_string(soSV) + " Sinh vien", 10);
    veTheKPI(88, 4, 23, " LOP TIN CHI ", to_string(soLTC) + " LTC (" + to_string(soLTCMo) + " Mo)", 13);
    
    // 3. Tiêu đề cột Menu
    gotoxy(15, 8); SetColorAndBG(14, 0); cout << "--- MENU CHINH ---";
    SetColorAndBG(7, 0);
    
    // 4. Vẽ 6 mục Menu chính (x = 15, rộng 38)
    for (int i = 0; i < 6; i++) {
        bool isHighlight = (i == mainIndex);
        bool isActive = !isSubMenuFocused;
        veKhungMenu(15, 10 + i * 3, 38, mainMenu[i], isHighlight, isActive);
    }
    
    // 5. Vẽ Menu con nếu có (x = 60, rộng 42)
    xoaVungSubMenu();
    if (subCount[mainIndex] > 0) {
        gotoxy(60, 8); SetColorAndBG(14, 0); cout << "--- MENU CON ---";
        SetColorAndBG(7, 0);
        veMenuCon(subCount, subMenu, mainIndex, subIndex, isSubMenuFocused);
    }
    
    // 6. Thanh hướng dẫn thao tác phía dưới
    inThanhHuongDanMenu();
}

inline void xuLyMenu(treeMH &dsmh, DS_LOP &dslop, DS_LTC &dsltc) {
    caiDatConsole();
    const string mainMenu[6] = {
        "1. QUAN LY MON HOC",
        "2. LOP & SINH VIEN ->",
        "3. LOP TIN CHI",
        "4. DANG KY TIN CHI",
        "5. DIEM & BAO CAO ->",
        "6. THOAT CHUONG TRINH"
    };

    const int subCount[6] = {0, 2, 0, 0, 5, 0};
    const string subMenu[6][6] = {
        {"", "", "", "", "", ""},
        {"1. Quan ly Danh sach Lop", "2. Sinh vien theo Lop", "", "", "", ""},
        {"", "", "", "", "", ""},
        {"", "", "", "", "", ""},
        {"1. Nhap Diem (LTC)", "2. In Bang Diem Mon Hoc", "3. In DS SV Dang Ky LTC", "4. Diem TB Khoa Hoc (Lop)", "5. Bang Diem Tong Ket (Lop)", ""},
        {"", "", "", "", "", ""}
    };

    int mainIndex = 0;
    int subIndex = 0;
    bool isSubMenuFocused = false; 
    string lastClockStr = "";

    SetColorAndBG(7, 0); 
    clrscr();
    veToanBoMenu(dsmh, dslop, dsltc, mainMenu, subCount, subMenu, mainIndex, subIndex, isSubMenuFocused, lastClockStr);

    while (true) {
        // Cập nhật đồng hồ thời gian thực mượt mà không chớp giật
        while (!_kbhit()) {
            capNhatDongHo(85, 1, lastClockStr);
            Sleep(50);
        }

        char c = _getch();
        if (c == -32 || c == 0) { 
            c = _getch();
            if (c == KEY_UP) c = 'w';
            else if (c == KEY_DOWN) c = 's';
            else if (c == KEY_LEFT) c = 'a';
            else if (c == KEY_RIGHT) c = 'd';
        }
        else if (c == KEY_ESC) {
            mainIndex = 5;
            c = KEY_ENTER;
        }
        c = tolower(static_cast<unsigned char>(c));

        int oldMainIndex = mainIndex;
        int oldSubIndex = subIndex;
        bool oldSubFocused = isSubMenuFocused;

        if (c == 'w') {
            if (isSubMenuFocused) {
                if (subIndex > 0) subIndex--;
            } else {
                mainIndex = (mainIndex - 1 + 6) % 6;
            }
        }
        else if (c == 's') {
            if (isSubMenuFocused) {
                if (subIndex < subCount[mainIndex] - 1) subIndex++;
            } else {
                mainIndex = (mainIndex + 1) % 6;
            }
        }
        else if (c == 'd') {
            if (!isSubMenuFocused && subCount[mainIndex] > 0) {
                isSubMenuFocused = true;
                subIndex = 0;
            }
        }
        else if (c == 'a') {
            if (isSubMenuFocused) {
                isSubMenuFocused = false;
            }
        }
        else if (c == KEY_ENTER) {
            if (!isSubMenuFocused && subCount[mainIndex] > 0) {
                isSubMenuFocused = true;
                subIndex = 0;
                veKhungMenu(15, 10 + mainIndex * 3, 38, mainMenu[mainIndex], true, false);
                veKhungMenu(60, layYSubMenu(mainIndex, 0), 42, subMenu[mainIndex][0], true, true);
                continue;
            }

            SetColorAndBG(7, 0); 
            clrscr();

            if (mainIndex == 0) {
                quanLyMonHocToanDien(dsmh, dsltc);
            } 
            else if (mainIndex == 1) {
                if (subIndex == 0) quanLyLopToanDien(dslop);
                else if (subIndex == 1) quanLySinhVienToanDien(dslop, dsltc);
            }
            else if (mainIndex == 2) {
                quanLyLopTinChiToanDien(dsltc, dsmh);
            }
            else if (mainIndex == 3) {
                quanLyDangKyToanDien(dsltc, dslop);
            }
            else if (mainIndex == 4) {
                if (subIndex == 0) nhapDiemToanDien(dsltc, dslop, dsmh);
                else if (subIndex == 1) inBangDiemMonHocToanDien(dsltc, dslop, dsmh);
                else if (subIndex == 2) inDanhSachSVDangKyLTCToanDien(dsltc, dslop, dsmh);
                else if (subIndex == 3) inDiemTBKhoaHocLopToanDien(dslop, dsltc, dsmh);
                else if (subIndex == 4) inBangDiemTongKetToanDien(dslop, dsltc, dsmh);
            }
            else if (mainIndex == 5) {
                if (popupCanhBao("Ban co muon luu thay doi truoc khi thoat?", true)) {
                    luuTatCaDuLieu(dsmh, dslop, dsltc);
                    popupThongBaoThanhCong("Da luu toan bo du lieu he thong thanh cong!");
                }
                clrscr(); gotoxy(10, 5); SetColor(11);
                cout << "Tam biet!";
                SetColor(7); gotoxy(10, 7);
                hienConTro();
                return; 
            }

            // Vẽ lại toàn bộ Dashboard sau khi thoát màn hình con (cập nhật KPI mới nếu có thay đổi!)
            SetColorAndBG(7, 0); 
            clrscr();
            lastClockStr = "";
            veToanBoMenu(dsmh, dslop, dsltc, mainMenu, subCount, subMenu, mainIndex, subIndex, isSubMenuFocused, lastClockStr);
            continue;
        }

        // ==========================================
        // CẬP NHẬT CỤC BỘ (SELECTIVE REDRAW) - TRIỆT TIÊU 100% CHỚP GIẬT & DELAY
        // ==========================================
        if (mainIndex != oldMainIndex) {
            // 1. Vẽ lại item cũ trong menu chính: bình thường
            veKhungMenu(15, 10 + oldMainIndex * 3, 38, mainMenu[oldMainIndex], false, false);
            // 2. Vẽ lại item mới trong menu chính: sáng vàng
            veKhungMenu(15, 10 + mainIndex * 3, 38, mainMenu[mainIndex], true, true);
            // 3. Xóa menu con cũ và vẽ menu con mới (nếu có)
            xoaVungSubMenu();
            subIndex = 0;
            isSubMenuFocused = false;
            if (subCount[mainIndex] > 0) {
                gotoxy(60, 8); SetColorAndBG(14, 0); cout << "--- MENU CON ---";
                SetColorAndBG(7, 0);
                veMenuCon(subCount, subMenu, mainIndex, 0, false);
            }
        }
        else if (isSubMenuFocused != oldSubFocused) {
            if (isSubMenuFocused) {
                // Focus vào menu con: menu chính chuyển sang màu xám/trắng
                veKhungMenu(15, 10 + mainIndex * 3, 38, mainMenu[mainIndex], true, false);
                // Sub-item 0 sáng vàng
                veKhungMenu(60, layYSubMenu(mainIndex, subIndex), 42, subMenu[mainIndex][subIndex], true, true);
            } else {
                // Thoát menu con về menu chính: menu chính sáng vàng
                veKhungMenu(15, 10 + mainIndex * 3, 38, mainMenu[mainIndex], true, true);
                // Sub-item cũ trở về bình thường
                veKhungMenu(60, layYSubMenu(mainIndex, oldSubIndex), 42, subMenu[mainIndex][oldSubIndex], false, true);
            }
        }
        else if (isSubMenuFocused && subIndex != oldSubIndex) {
            // Chỉ di chuyển giữa các sub-item: vẽ lại 2 sub-item
            veKhungMenu(60, layYSubMenu(mainIndex, oldSubIndex), 42, subMenu[mainIndex][oldSubIndex], false, true);
            veKhungMenu(60, layYSubMenu(mainIndex, subIndex), 42, subMenu[mainIndex][subIndex], true, true);
        }
    }
}

#endif // UI_H
