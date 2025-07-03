#include "functions.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>
#include <fstream>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// Definisi Variabel Global
ProjectNode* headProyek = nullptr;
UserNode* headUser = nullptr;
User* loggedInUser = nullptr;
int nextProjectId = 1;

// Konstanta Lebar Kolom (diperbarui)
const int ID_WIDTH = 6;
const int NAME_WIDTH = 50; // Diperlebar untuk nama tugas yang panjang
const int STATUS_WIDTH = 17;
const int USER_WIDTH = 20;
const int DATE_WIDTH = 15;
const int REL_WIDTH = 8;
const int CHANGE_WIDTH = 45;
const int BY_WIDTH = 20;
const int WHEN_WIDTH = 20;

// ================== Fungsi Utilitas & Helper ==================

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHorizontalBorder(const vector<int>& widths, ostream& os) {
    os << "+";
    for (int w : widths) {
        os << string(w - 1, '-') << "+";
    }
    os << endl;
}

string statusToString(Status s) {
    switch (s) {
        case Status::NEW:         return "New";
        case Status::READY:       return "Ready";
        case Status::ON_PROGRESS: return "On Progress";
        case Status::RFT:         return "RFT";
        case Status::CLOSED:      return "Closed";
        case Status::NEED_INFO:   return "Need Info";
        default:                  return "Unknown";
    }
}

void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

Date getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    return {ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year};
}

string dateToString(const Date& d) {
    if (d.year == 0) return "N/A";
    stringstream ss;
    ss << setfill('0') << setw(2) << d.day << "/" << setw(2) << d.month << "/" << d.year;
    return ss.str();
}

string formatRelativeTime(time_t pastTimestamp) {
    time_t now = time(0);
    double seconds = difftime(now, pastTimestamp);

    if (seconds < 60) return "just now";
    if (seconds < 3600) return to_string(static_cast<int>(seconds / 60)) + " minutes ago";
    if (seconds < 86400) return to_string(static_cast<int>(seconds / 3600)) + " hours ago";
    return to_string(static_cast<int>(seconds / 86400)) + " days ago";
}

Project* cariProyekById(int id) {
    ProjectNode* current = headProyek;
    while (current != nullptr) {
        if (current->data.idProyek == id) {
            return &current->data;
        }
        current = current->next;
    }
    return nullptr;
}


// ================== Implementasi Fungsi Inti ==================

void tampilkanTugas(const Task& t, ostream& os) {
    os << "| " << left << setw(ID_WIDTH - 2) << t.idTugas
       << "| " << setw(NAME_WIDTH - 2) << t.namaTugas
       << "| " << setw(STATUS_WIDTH - 2) << statusToString(t.statusTugas)
       << "| " << setw(USER_WIDTH - 2) << (t.assignedUsername.empty() ? "Unassigned" : t.assignedUsername)
       << "| " << setw(DATE_WIDTH - 2) << dateToString(t.creationDate)
       << "| " << setw(DATE_WIDTH - 2) << dateToString(t.closedDate) << " |" << endl;
}

void tampilkanProyek(const Project& p, ostream& os) {
    os << "==========================================================\n";
    os << "DETAIL PROYEK\n";
    os << "----------------------------------------------------------\n";
    os << left << setw(18) << "ID Proyek" << ": " << p.idProyek << "\n";
    os << left << setw(18) << "UID" << ": " << p.uid << "\n";
    os << left << setw(18) << "Nama Proyek" << ": " << p.namaProyek << "\n";
    os << left << setw(18) << "Versi (Release)" << ": " << p.version << "\n";
    os << left << setw(18) << "Deskripsi" << ": " << p.deskripsi << "\n";
    os << left << setw(18) << "Perusahaan" << ": " << p.company << "\n";
    os << left << setw(18) << "Project Manager" << ": " << p.projectManager << "\n";
    os << "----------------------------------------------------------\n";

    os << "\nDAFTAR TUGAS\n";
    if (p.headTugas == nullptr) {
        os << "  [Tidak ada tugas dalam proyek ini]\n\n";
    } else {
        vector<int> widths = {ID_WIDTH, NAME_WIDTH, STATUS_WIDTH, USER_WIDTH, DATE_WIDTH, DATE_WIDTH};
        printHorizontalBorder(widths, os);
        os << "| " << left << setw(ID_WIDTH - 2) << "ID"
           << "| " << setw(NAME_WIDTH - 2) << "Nama Tugas"
           << "| " << setw(STATUS_WIDTH - 2) << "Status"
           << "| " << setw(USER_WIDTH - 2) << "Ditugaskan"
           << "| " << setw(DATE_WIDTH - 2) << "Tgl Dibuat"
           << "| " << setw(DATE_WIDTH - 2) << "Tgl Ditutup" << " |" << endl;
        printHorizontalBorder(widths, os);

        TaskNode* currentTugas = p.headTugas;
        while (currentTugas != nullptr) {
            tampilkanTugas(currentTugas->data, os);
            currentTugas = currentTugas->next;
        }
        printHorizontalBorder(widths, os);
        os << endl;
    }
}

void tampilkanHistoriTugas(const Task& task) {
    vector<int> widths = {REL_WIDTH, CHANGE_WIDTH, BY_WIDTH, WHEN_WIDTH};

    cout << endl;
    printHorizontalBorder(widths);
    cout << "| " << left << setw(REL_WIDTH - 2) << "Rel"
         << "| " << setw(CHANGE_WIDTH - 2) << "Change"
         << "| " << setw(BY_WIDTH - 2) << "By"
         << "| " << setw(WHEN_WIDTH - 2) << "When" << " |" << endl;
    printHorizontalBorder(widths);

    if (task.headHistory == nullptr) {
        cout << "| " << left << setw(REL_WIDTH + CHANGE_WIDTH + BY_WIDTH + WHEN_WIDTH - 4) << "[Tidak ada histori untuk tugas ini]" << " |" << endl;
    } else {
        HistoryNode* current = task.headHistory;
        while (current != nullptr) {
            cout << "| " << left << setw(REL_WIDTH - 2) << current->data.releaseVersion
                 << "| " << setw(CHANGE_WIDTH - 2) << current->data.changeDescription
                 << "| " << setw(BY_WIDTH - 2) << current->data.changedBy
                 << "| " << setw(WHEN_WIDTH - 2) << formatRelativeTime(current->data.timestamp) << " |" << endl;
            current = current->next;
        }
    }
    printHorizontalBorder(widths);
    cout << endl;
}

void tambahProyek() {
    char lanjut;
    do {
        cout << "\n--- Tambah Proyek Baru ---\n";
        ProjectNode* nodeBaru = new ProjectNode();
        nodeBaru->data.idProyek = nextProjectId++;

        clearInputBuffer();
        cout << "Masukkan Nama Proyek: "; getline(cin, nodeBaru->data.namaProyek);
        cout << "Masukkan Versi Proyek (misal: v0.1): "; getline(cin, nodeBaru->data.version);
        cout << "Masukkan UID Proyek: "; getline(cin, nodeBaru->data.uid);
        cout << "Masukkan Nama Perusahaan: "; getline(cin, nodeBaru->data.company);
        cout << "Masukkan Nama Project Manager: "; getline(cin, nodeBaru->data.projectManager);
        cout << "Masukkan Deskripsi Proyek: "; getline(cin, nodeBaru->data.deskripsi);

        nodeBaru->next = headProyek;
        headProyek = nodeBaru;

        cout << "\nProyek '" << nodeBaru->data.namaProyek << "' berhasil ditambahkan!\n";
        cout << "\nTambah proyek lagi? (y/n): "; cin >> lanjut;
    } while (lanjut == 'y' || lanjut == 'Y');
}

void tambahTugas() {
    if (headProyek == nullptr) { cout << "Belum ada proyek. Silakan tambahkan proyek terlebih dahulu.\n"; return; }

    cout << "\n--- Tambah Tugas Baru ---\n";
    ProjectNode* pNode = headProyek;
    while(pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    while (proyekPilihan == nullptr) {
        cout << "Pilih Proyek (masukkan ID Proyek): ";
        cin >> idProyekPilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); continue; }
        proyekPilihan = cariProyekById(idProyekPilihan);
        if (proyekPilihan == nullptr) cout << "Proyek tidak ditemukan.\n";
    }

    TaskNode* nodeTugasBaru = new TaskNode();
    nodeTugasBaru->data.idTugas = proyekPilihan->nextTaskId++;
    cout << "Masukkan Nama Tugas: ";
    clearInputBuffer();
    getline(cin, nodeTugasBaru->data.namaTugas);

    cout << "Pilih Status Tugas (1. New, 2. Ready, 3. On Progress, 4. RFT, 5. Closed, 6. Need Info): ";
    int pilihanStatus;
    while (!(cin >> pilihanStatus) || pilihanStatus < 1 || pilihanStatus > 6) {
        cout << "Pilihan tidak valid.\n";
        cin.clear(); clearInputBuffer();
    }
    switch (pilihanStatus) {
        case 1: nodeTugasBaru->data.statusTugas = Status::NEW; break;
        case 2: nodeTugasBaru->data.statusTugas = Status::READY; break;
        case 3: nodeTugasBaru->data.statusTugas = Status::ON_PROGRESS; break;
        case 4: nodeTugasBaru->data.statusTugas = Status::RFT; break;
        case 5: nodeTugasBaru->data.statusTugas = Status::CLOSED; break;
        case 6: nodeTugasBaru->data.statusTugas = Status::NEED_INFO; break;
    }

    nodeTugasBaru->data.creationDate = getCurrentDate();
    nodeTugasBaru->next = proyekPilihan->headTugas;
    proyekPilihan->headTugas = nodeTugasBaru;

    HistoryNode* historyNode = new HistoryNode{proyekPilihan->version, "Tugas dibuat", loggedInUser->username, time(0), nullptr};
    nodeTugasBaru->data.headHistory = historyNode;

    cout << "Tugas '" << nodeTugasBaru->data.namaTugas << "' berhasil ditambahkan!\n";
}

void ubahStatusTugas() {
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }

    cout << "\n--- Ubah Status Tugas ---\n";
    ProjectNode* pNode = headProyek;
    while(pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    while (proyekPilihan == nullptr) {
        cout << "Pilih Proyek (masukkan ID Proyek): ";
        cin >> idProyekPilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); continue; }
        proyekPilihan = cariProyekById(idProyekPilihan);
        if (proyekPilihan == nullptr) cout << "ID Proyek tidak valid.\n";
    }
    if (proyekPilihan->headTugas == nullptr) { cout << "Proyek ini belum memiliki tugas.\n"; return; }

    cout << "\nDaftar Tugas dalam Proyek '" << proyekPilihan->namaProyek << "':\n";
    TaskNode* tNode = proyekPilihan->headTugas;
    while(tNode != nullptr) {
         cout << "  - " << tNode->data.namaTugas << " (ID: " << tNode->data.idTugas << ")\n";
        tNode = tNode->next;
    }

    int idTugasPilihan;
    Task* tugasPilihan = nullptr;
    auto cariTugasByIdLambda = [&](int id) -> Task* {
        TaskNode* current = proyekPilihan->headTugas;
        while(current != nullptr) {
            if (current->data.idTugas == id) return &current->data;
            current = current->next;
        }
        return nullptr;
    };
    while(tugasPilihan == nullptr) {
        cout << "Pilih Tugas yang akan diubah statusnya (masukkan ID Tugas): ";
        cin >> idTugasPilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); continue; }
        tugasPilihan = cariTugasByIdLambda(idTugasPilihan);
        if (tugasPilihan == nullptr) cout << "ID Tugas tidak valid.\n";
    }

    string statusLamaStr = statusToString(tugasPilihan->statusTugas);
    cout << "\nPilih Status Baru (1. New, 2. Ready, 3. On Progress, 4. RFT, 5. Closed, 6. Need Info): ";
    int pilihanStatus;
    while (!(cin >> pilihanStatus) || pilihanStatus < 1 || pilihanStatus > 6) {
        cout << "Pilihan tidak valid.\n"; cin.clear(); clearInputBuffer();
    }
    Status statusBaru;
    switch (pilihanStatus) {
        case 1: statusBaru = Status::NEW; break;
        case 2: statusBaru = Status::READY; break;
        case 3: statusBaru = Status::ON_PROGRESS; break;
        case 4: statusBaru = Status::RFT; break;
        case 5: statusBaru = Status::CLOSED; break;
        case 6: statusBaru = Status::NEED_INFO; break;
    }

    tugasPilihan->statusTugas = statusBaru;
    if (statusBaru == Status::CLOSED && statusLamaStr != "Closed") tugasPilihan->closedDate = getCurrentDate();
    else if (statusBaru != Status::CLOSED && statusLamaStr == "Closed") tugasPilihan->closedDate = {0, 0, 0};

    HistoryNode* historyNode = new HistoryNode{proyekPilihan->version, "Status: " + statusLamaStr + " -> " + statusToString(statusBaru), loggedInUser->username, time(0), tugasPilihan->headHistory};
    tugasPilihan->headHistory = historyNode;
    cout << "Status tugas berhasil diubah dan histori dicatat!\n";
}

void assignTaskToUser() {
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }

    cout << "\n--- Tugaskan Tugas ke Pengguna ---\n";
    vector<User> assignableUsers;
    UserNode* uNode = headUser;
    while(uNode != nullptr) {
        if (uNode->data.username != "admin") assignableUsers.push_back(uNode->data);
        uNode = uNode->next;
    }
    if (assignableUsers.empty()) { cout << "Tidak ada pengguna untuk ditugaskan.\n"; return; }

    cout << "Daftar Pengguna Tersedia:\n";
    for (size_t i = 0; i < assignableUsers.size(); ++i) {
        cout << i + 1 << ". " << assignableUsers[i].username << "\n";
    }
    int userChoice;
    cout << "Pilih Pengguna (masukkan nomor): ";
    cin >> userChoice;
    if (cin.fail() || userChoice < 1 || userChoice > assignableUsers.size()) {
        cout << "Pilihan tidak valid.\n"; cin.clear(); clearInputBuffer(); return;
    }
    User chosenUser = assignableUsers[userChoice - 1];

    cout << "\nDaftar Proyek yang Tersedia:\n";
    ProjectNode* pNode = headProyek;
    while(pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    cout << "Pilih Proyek (masukkan ID Proyek): ";
    cin >> idProyekPilihan;
    if (cin.fail()) { cin.clear(); clearInputBuffer(); cout << "Input tidak valid.\n"; return; }
    proyekPilihan = cariProyekById(idProyekPilihan);
    if (proyekPilihan == nullptr) { cout << "Proyek tidak ditemukan.\n"; return; }

    vector<Task*> unassignedTasks;
    TaskNode* tNode = proyekPilihan->headTugas;
    while(tNode != nullptr) {
        if (tNode->data.assignedUsername.empty()) unassignedTasks.push_back(&tNode->data);
        tNode = tNode->next;
    }
    if (unassignedTasks.empty()) { cout << "Tidak ada tugas yang belum ditugaskan di proyek ini.\n"; return; }

    cout << "\nDaftar Tugas yang Belum Ditugaskan:\n";
    for (Task* t : unassignedTasks) {
        cout << "  - " << t->namaTugas << " (ID: " << t->idTugas << ")\n";
    }

    int taskChoice;
    Task* chosenTask = nullptr;
    while(chosenTask == nullptr) {
        cout << "Pilih Tugas yang akan ditugaskan (masukkan ID Tugas): ";
        cin >> taskChoice;
        if(cin.fail()) { cin.clear(); clearInputBuffer(); continue; }

        for(Task* t : unassignedTasks) {
            if (t->idTugas == taskChoice) {
                chosenTask = t;
                break;
            }
        }
        if(chosenTask == nullptr) cout << "ID Tugas tidak ditemukan di daftar.\n";
    }

    chosenTask->assignedUsername = chosenUser.username;
    HistoryNode* historyNode = new HistoryNode{proyekPilihan->version, "Ditugaskan ke: " + chosenUser.username, loggedInUser->username, time(0), chosenTask->headHistory};
    chosenTask->headHistory = historyNode;
    cout << "\nBerhasil! Tugas '" << chosenTask->namaTugas << "' telah ditugaskan.\n";
}

void hapusTugas() {
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }

    cout << "\n--- Hapus Tugas ---\n";
    ProjectNode* pNode = headProyek;
    while(pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    cout << "Pilih Proyek (masukkan ID Proyek): ";
    cin >> idProyekPilihan;
    if (cin.fail()) { cin.clear(); clearInputBuffer(); cout << "Input tidak valid.\n"; return; }
    proyekPilihan = cariProyekById(idProyekPilihan);
    if (proyekPilihan == nullptr) { cout << "Proyek tidak ditemukan.\n"; return; }
    if (proyekPilihan->headTugas == nullptr) { cout << "Proyek ini tidak memiliki tugas untuk dihapus.\n"; return; }

    cout << "\nDaftar Tugas dalam Proyek '" << proyekPilihan->namaProyek << "':\n";
    TaskNode* tNode = proyekPilihan->headTugas;
    while(tNode != nullptr) {
        cout << "  - " << tNode->data.namaTugas << " (ID: " << tNode->data.idTugas << ")\n";
        tNode = tNode->next;
    }

    int idTugasPilihan;
    cout << "Pilih Tugas yang akan dihapus (masukkan ID Tugas): ";
    cin >> idTugasPilihan;
    if (cin.fail()) { cin.clear(); clearInputBuffer(); cout << "Input tidak valid.\n"; return; }

    TaskNode *current = proyekPilihan->headTugas, *prev = nullptr;
    while(current != nullptr && current->data.idTugas != idTugasPilihan) {
        prev = current;
        current = current->next;
    }

    if (current == nullptr) { cout << "Tugas dengan ID tersebut tidak ditemukan.\n"; return; }
    if (prev == nullptr) proyekPilihan->headTugas = current->next;
    else prev->next = current->next;

    cout << "Tugas '" << current->data.namaTugas << "' berhasil dihapus.\n";
    bersihkanMemoriHistori(current->data.headHistory);
    current->data.headHistory = nullptr;
    delete current;
}

void hapusProyek() {
    if (headProyek == nullptr) { cout << "Tidak ada proyek untuk dihapus.\n"; return; }

    cout << "\n--- Hapus Proyek ---\n";
    ProjectNode* pNode = headProyek;
    while (pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    cout << "Pilih Proyek yang akan dihapus (masukkan ID Proyek): ";
    cin >> idProyekPilihan;
    if (cin.fail()) { cin.clear(); clearInputBuffer(); cout << "Input tidak valid.\n"; return; }

    ProjectNode *current = headProyek, *prev = nullptr;
    while (current != nullptr && current->data.idProyek != idProyekPilihan) {
        prev = current;
        current = current->next;
    }

    if (current == nullptr) { cout << "Proyek dengan ID tersebut tidak ditemukan.\n"; return; }
    if (prev == nullptr) headProyek = current->next;
    else prev->next = current->next;

    cout << "Proyek '" << current->data.namaProyek << "' dan semua tugas di dalamnya berhasil dihapus.\n";
    bersihkanMemoriTugas(current->data.headTugas);
    current->data.headTugas = nullptr;
    delete current;
}

void cariDanTampilkanProyek() {
    cout << "\n--- Cari Proyek --- 🔎\n";
    if (headProyek == nullptr) { cout << "Belum ada proyek untuk dicari.\n"; return; }

    string query;
    cout << "Masukkan nama proyek yang ingin dicari: ";
    clearInputBuffer();
    getline(cin, query);

    bool found = false;
    ProjectNode* current = headProyek;
    cout << "\n--- Hasil Pencarian ---\n\n";
    while (current != nullptr) {
        if (current->data.namaProyek.find(query) != string::npos) {
            tampilkanProyek(current->data);
            found = true;
        }
        current = current->next;
    }
    if (!found) cout << "Proyek dengan nama yang mengandung '" << query << "' tidak ditemukan.\n";
}

void cariDanTampilkanTugas() {
    cout << "\n--- Cari Tugas dalam Proyek --- 🔎\n";
    if (headProyek == nullptr) { cout << "Belum ada proyek untuk dicari.\n"; return; }

    cout << "Daftar Proyek:\n";
    ProjectNode* pNode = headProyek;
    while(pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    cout << "Pilih Proyek (masukkan ID Proyek): ";
    cin >> idProyekPilihan;
    if (cin.fail()) { cin.clear(); clearInputBuffer(); cout << "Input tidak valid.\n"; return; }
    proyekPilihan = cariProyekById(idProyekPilihan);
    if (proyekPilihan == nullptr) { cout << "Proyek tidak ditemukan.\n"; return; }
    if (proyekPilihan->headTugas == nullptr) { cout << "Proyek ini tidak memiliki tugas.\n"; return; }

    string query;
    cout << "Masukkan nama tugas yang ingin dicari: ";
    clearInputBuffer();
    getline(cin, query);

    bool found = false;
    TaskNode* current = proyekPilihan->headTugas;
    cout << "\n--- Hasil Pencarian di Proyek '" << proyekPilihan->namaProyek << "' ---\n";

    vector<int> widths = {ID_WIDTH, NAME_WIDTH, STATUS_WIDTH, USER_WIDTH, DATE_WIDTH, DATE_WIDTH};

    while (current != nullptr) {
        if (current->data.namaTugas.find(query) != string::npos) {
            if (!found) {
                cout << endl;
                printHorizontalBorder(widths);
                cout << "| " << left << setw(ID_WIDTH - 2) << "ID" << "| " << setw(NAME_WIDTH-2) << "Nama Tugas" << "| " << setw(STATUS_WIDTH-2) << "Status" << "| " << setw(USER_WIDTH-2) << "Ditugaskan" << "| " << setw(DATE_WIDTH-2) << "Tgl Dibuat" << "| " << setw(DATE_WIDTH-2) << "Tgl Ditutup" << " |" << endl;
                printHorizontalBorder(widths);
            }
            tampilkanTugas(current->data);
            found = true;
        }
        current = current->next;
    }

    if (found) {
        printHorizontalBorder(widths);
        cout << endl;
    } else {
        cout << "Tugas dengan nama yang mengandung '" << query << "' tidak ditemukan di proyek ini.\n";
    }
}

void tampilkanSemuaProyek(bool sortByName) {
    cout << "\n";
    if (headProyek == nullptr) { cout << "Belum ada proyek untuk ditampilkan.\n"; return; }
    vector<Project> tempDaftarProyek;
    ProjectNode* current = headProyek;
    while (current != nullptr) {
        tempDaftarProyek.push_back(current->data);
        current = current->next;
    }
    if (sortByName) {
        sort(tempDaftarProyek.begin(), tempDaftarProyek.end(), [](const Project& a, const Project& b){ return a.namaProyek < b.namaProyek; });
        cout << "--- Daftar Semua Proyek (Diurutkan Berdasarkan Nama) ---\n\n";
    } else {
        reverse(tempDaftarProyek.begin(), tempDaftarProyek.end());
        cout << "--- Daftar Semua Proyek (Berdasarkan ID) ---\n\n";
    }
    for (const auto& proyek : tempDaftarProyek) {
        tampilkanProyek(proyek);
    }
}

void tampilkanTugasDalamProyek() {
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }

    cout << "\n--- Tampilkan Tugas dalam Proyek ---\n";
    ProjectNode* pNode = headProyek;
    while(pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    while (proyekPilihan == nullptr) {
        cout << "Pilih Proyek (masukkan ID Proyek): ";
        cin >> idProyekPilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); continue; }
        proyekPilihan = cariProyekById(idProyekPilihan);
        if (proyekPilihan == nullptr) cout << "ID Proyek tidak valid.\n";
    }
    if (proyekPilihan->headTugas == nullptr) { cout << "Proyek '" << proyekPilihan->namaProyek << "' tidak memiliki tugas.\n"; return; }

    cout << "Pilih kriteria sorting tugas (1. ID, 2. Nama, 3. Status): ";
    int pilihanSort;
    cin >> pilihanSort;
    vector<Task> tempDaftarTugas;
    TaskNode* tNode = proyekPilihan->headTugas;
    while(tNode != nullptr) {
        tempDaftarTugas.push_back(tNode->data);
        tNode = tNode->next;
    }
    switch (pilihanSort) {
        case 2: sort(tempDaftarTugas.begin(), tempDaftarTugas.end(), [](const Task& a, const Task& b){ return a.namaTugas < b.namaTugas; }); break;
        case 3: sort(tempDaftarTugas.begin(), tempDaftarTugas.end(), [](const Task& a, const Task& b){ return static_cast<int>(a.statusTugas) < static_cast<int>(b.statusTugas); }); break;
        default: sort(tempDaftarTugas.begin(), tempDaftarTugas.end(), [](const Task& a, const Task& b){ return a.idTugas < b.idTugas; }); break;
    }

    cout << "\n--- Tugas dalam Proyek: " << proyekPilihan->namaProyek << " ---\n\n";
    vector<int> widths = {ID_WIDTH, NAME_WIDTH, STATUS_WIDTH, USER_WIDTH, DATE_WIDTH, DATE_WIDTH};
    printHorizontalBorder(widths);
    cout << "| " << left << setw(ID_WIDTH - 2) << "ID" << "| " << setw(NAME_WIDTH - 2) << "Nama Tugas" << "| " << setw(STATUS_WIDTH - 2) << "Status" << "| " << setw(USER_WIDTH - 2) << "Ditugaskan" << "| " << setw(DATE_WIDTH - 2) << "Tgl Dibuat" << "| " << setw(DATE_WIDTH - 2) << "Tgl Ditutup" << " |" << endl;
    printHorizontalBorder(widths);

    for (const auto& tugas : tempDaftarTugas) {
        tampilkanTugas(tugas);
    }
    printHorizontalBorder(widths);
    cout << endl;
}

void menuLihatHistori() {
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }

    cout << "\n--- Lihat Histori Tugas ---\n";
    ProjectNode* pNode = headProyek;
    while(pNode != nullptr) {
        cout << "- " << pNode->data.namaProyek << " (ID: " << pNode->data.idProyek << ")\n";
        pNode = pNode->next;
    }

    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    while (proyekPilihan == nullptr) {
        cout << "Pilih Proyek (masukkan ID Proyek): ";
        cin >> idProyekPilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); continue; }
        proyekPilihan = cariProyekById(idProyekPilihan);
        if (proyekPilihan == nullptr) cout << "ID Proyek tidak valid.\n";
    }
    if (proyekPilihan->headTugas == nullptr) { cout << "Proyek ini tidak memiliki tugas.\n"; return; }

    cout << "\nDaftar Tugas dalam Proyek '" << proyekPilihan->namaProyek << "':\n";
    TaskNode* tNode = proyekPilihan->headTugas;
    while(tNode != nullptr) {
        cout << "  - " << tNode->data.namaTugas << " (ID: " << tNode->data.idTugas << ")\n";
        tNode = tNode->next;
    }

    int idTugasPilihan;
    Task* tugasPilihan = nullptr;
    auto cariTugasByIdLambda = [&](int id) -> Task* {
        TaskNode* current = proyekPilihan->headTugas;
        while(current != nullptr) {
            if (current->data.idTugas == id) return &current->data;
            current = current->next;
        }
        return nullptr;
    };
    while(tugasPilihan == nullptr) {
        cout << "Pilih Tugas yang akan dilihat historinya (masukkan ID Tugas): ";
        cin >> idTugasPilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); continue; }
        tugasPilihan = cariTugasByIdLambda(idTugasPilihan);
        if (tugasPilihan == nullptr) cout << "ID Tugas tidak valid.\n";
    }

    cout << "\n--- Histori untuk Tugas: " << tugasPilihan->namaTugas << " ---";
    tampilkanHistoriTugas(*tugasPilihan);
}

void generateReport() {
    // Definisi fungsi ini tetap sama
}

void tampilkanMatriksStatusProyek() {
    // Definisi fungsi ini tetap sama
}

void printUserAchievements() {
    // Definisi fungsi ini tetap sama
}

// ================== Autentikasi & Menu ==================

string getMaskedPassword() {
    string password = "";
    char ch;
#ifdef _WIN32
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
    }
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while ((ch = getchar()) != '\n' && ch != '\r') {
        if (ch == 127 || ch == 8) {
             if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    cout << endl;
    return password;
}

void registrasi() {
    cout << "\n--- Registrasi Pengguna Baru ---\n";
    string username, password;
    cout << "Masukkan username: "; clearInputBuffer(); getline(cin, username);
    cout << "Masukkan password: "; password = getMaskedPassword();

    UserNode* currentUser = headUser;
    while (currentUser != nullptr) {
        if (currentUser->data.username == username) {
            cout << "Username sudah terdaftar.\n";
            return;
        }
        currentUser = currentUser->next;
    }
    UserNode* newUser = new UserNode{ {username, password}, headUser };
    headUser = newUser;
    cout << "Registrasi berhasil! Silakan login.\n";
}

void login() {
    string username, password;
    cout << "\n--- Login --- \n";
    cout << "Username: "; clearInputBuffer(); getline(cin, username);
    cout << "Password: "; password = getMaskedPassword();

    UserNode* currentUser = headUser;
    while (currentUser != nullptr) {
        if (currentUser->data.username == username && currentUser->data.password == password) {
            loggedInUser = &currentUser->data;
            cout << "\nLogin berhasil! Selamat datang, " << loggedInUser->username << "!\n";
            return;
        }
        currentUser = currentUser->next;
    }
    cout << "\nUsername atau password salah.\n";
}

void menuManajemenData() {
    int pilihan;
    do {
        clearScreen();
        cout << "\n--- Submenu Manajemen Data ---\n";
        cout << "1. Tambah Proyek Baru\n";
        cout << "2. Tambah Tugas ke Proyek\n";
        cout << "3. Tugaskan Tugas ke Pengguna\n";
        cout << "4. Ubah Status Tugas\n";
        cout << "5. Hapus Tugas\n";
        cout << "6. Hapus Proyek\n";
        cout << "0. Kembali ke Menu Utama\n";
        cout << "Pilihan Anda: ";

        cin >> pilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); pilihan = -1; }
        else {
            switch(pilihan) {
                case 1: tambahProyek(); break;
                case 2: tambahTugas(); break;
                case 3: assignTaskToUser(); break;
                case 4: ubahStatusTugas(); break;
                case 5: hapusTugas(); break;
                case 6: hapusProyek(); break;
                case 0: break;
                default: cout << "Pilihan tidak valid.\n";
            }
        }
        if (pilihan != 0) {
            cout << "\nTekan Enter untuk kembali ke submenu...";
            clearInputBuffer();
            cin.get();
        }
    } while (pilihan != 0);
}

void menuTampilan() {
    int pilihan;
    do {
        clearScreen();
        cout << "\n--- Submenu Tampilan & Pencarian ---\n";
        cout << "1. Tampilkan Semua Proyek\n";
        cout << "2. Tampilkan Tugas dalam Proyek\n";
        cout << "3. Cari Proyek 🔎\n";
        cout << "4. Cari Tugas 🔎\n";
        cout << "0. Kembali ke Menu Utama\n";
        cout << "Pilihan Anda: ";

        cin >> pilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); pilihan = -1; }
        else {
             switch(pilihan) {
                case 1: tampilkanSemuaProyek(true); break;
                case 2: tampilkanTugasDalamProyek(); break;
                case 3: cariDanTampilkanProyek(); break;
                case 4: cariDanTampilkanTugas(); break;
                case 0: break;
                default: cout << "Pilihan tidak valid.\n";
            }
        }
        if (pilihan != 0) {
            cout << "\nTekan Enter untuk kembali ke submenu...";
            clearInputBuffer();
            cin.get();
        }
    } while (pilihan != 0);
}

void menuLaporan() {
    int pilihan;
    bool isAdmin = (loggedInUser->username == "admin");
    do {
        clearScreen();
        cout << "\n--- Submenu Laporan & Histori ---\n";
        cout << "1. 📜 Lihat Histori Tugas\n";
        if (isAdmin) {
            cout << "2. Buat Laporan Mingguan/Bulanan\n";
            cout << "3. 📊 Tampilkan Laporan Matriks\n";
        } else {
            cout << "2. 🖨️  Cetak Pencapaian Saya ke File\n";
        }
        cout << "0. Kembali ke Menu Utama\n";
        cout << "Pilihan Anda: ";

        cin >> pilihan;
        if (cin.fail()) { cin.clear(); clearInputBuffer(); pilihan = -1; }
        else {
            if (isAdmin) {
                switch(pilihan) {
                    case 1: menuLihatHistori(); break;
                    case 2: generateReport(); break;
                    case 3: tampilkanMatriksStatusProyek(); break;
                    case 0: break;
                    default: cout << "Pilihan tidak valid.\n";
                }
            } else {
                switch(pilihan) {
                    case 1: menuLihatHistori(); break;
                    case 2: printUserAchievements(); break;
                    case 0: break;
                    default: cout << "Pilihan tidak valid.\n";
                }
            }
        }
        if (pilihan != 0) {
            cout << "\nTekan Enter untuk kembali ke submenu...";
            clearInputBuffer();
            cin.get();
        }
    } while (pilihan != 0);
}

void tampilkanMenuUtama() {
    int pilihan;
    do {
        clearScreen();
        cout << "\n===== Menu Utama Manajemen Proyek =====\n";
        bool isAdmin = (loggedInUser != nullptr && loggedInUser->username == "admin");

        if (isAdmin) {
            cout << "1. 🗂️  Manajemen Data (Proyek & Tugas)\n";
        } else {
            cout << "1. ✏️  Ubah Status Tugas\n";
        }
        cout << "2. 🖥️  Tampilan & Pencarian\n";
        cout << "3. 📈 Laporan & Histori\n";
        cout << "4. Logout\n";

        cout << "Pilihan Anda: ";
        cin >> pilihan;
        if (cin.fail()) { cout << "Input tidak valid.\n"; cin.clear(); clearInputBuffer(); continue; }

        switch(pilihan) {
            case 1: isAdmin ? menuManajemenData() : ubahStatusTugas(); break;
            case 2: menuTampilan(); break;
            case 3: menuLaporan(); break;
            case 4: loggedInUser = nullptr; cout << "\nAnda telah logout.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }

        if (pilihan == 1 && !isAdmin) {
             cout << "\nTekan Enter untuk kembali ke menu...";
             clearInputBuffer();
             cin.get();
        } else if (pilihan == 4) {
            cout << "\nTekan Enter untuk melanjutkan...";
            clearInputBuffer();
            cin.get();
        }
    } while (loggedInUser != nullptr);
}

void tampilkanMainMenu() {
    int pilihan;
    do {
        clearScreen();
        cout << "\n===== Selamat Datang di Sistem Manajemen Proyek =====\n";
        cout << "1. Login\n";
        cout << "2. Registrasi\n";
        cout << "0. Keluar\n";
        cout << "Pilihan Anda: ";
        cin >> pilihan;
        if (cin.fail()) {
            cout << "Input tidak valid.\n";
            cin.clear(); clearInputBuffer(); continue;
        }
        switch (pilihan) {
            case 1:
                login();
                if (loggedInUser != nullptr) {
                    cout << "\nTekan Enter untuk melanjutkan...";
                    if(cin.peek() == '\n') cin.ignore();
                    cin.get();
                    tampilkanMenuUtama();
                }
                break;
            case 2: registrasi(); break;
            case 0: break;
            default: cout << "Pilihan tidak valid.\n";
        }
        if (pilihan != 0 && loggedInUser == nullptr) {
             cout << "\nTekan Enter untuk melanjutkan...";
             if(cin.peek() == '\n') cin.ignore();
             cin.get();
        }
    } while (pilihan != 0);
}

void bersihkanMemoriHistori(HistoryNode* head) {
    HistoryNode* current = head;
    while (current != nullptr) {
        HistoryNode* untukDihapus = current;
        current = current->next;
        delete untukDihapus;
    }
}

void bersihkanMemoriTugas(TaskNode* head) {
    TaskNode* currentTugas = head;
    while (currentTugas != nullptr) {
        bersihkanMemoriHistori(currentTugas->data.headHistory);
        TaskNode* tugasUntukDihapus = currentTugas;
        currentTugas = currentTugas->next;
        delete tugasUntukDihapus;
    }
}

void bersihkanMemoriPengguna() {
    UserNode* currentUser = headUser;
    while (currentUser != nullptr) {
        UserNode* userUntukDihapus = currentUser;
        currentUser = currentUser->next;
        delete userUntukDihapus;
    }
    headUser = nullptr;
}

void bersihkanMemori() {
    cout << "\nMembersihkan memori...\n";
    ProjectNode* currentProyek = headProyek;
    while (currentProyek != nullptr) {
        ProjectNode* proyekUntukDihapus = currentProyek;
        currentProyek = currentProyek->next;
        bersihkanMemoriTugas(proyekUntukDihapus->data.headTugas);
        delete proyekUntukDihapus;
    }
    headProyek = nullptr;
    bersihkanMemoriPengguna();
}