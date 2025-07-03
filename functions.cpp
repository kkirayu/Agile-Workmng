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
#include <conio.h> // Untuk _getch() di Windows
#else
#include <termios.h> // Untuk pengaturan terminal di Linux/macOS
#include <unistd.h>  // Untuk STDIN_FILENO
#endif


// Definisi Variabel Global
ProjectNode* headProyek = nullptr;
UserNode* headUser = nullptr;
User* loggedInUser = nullptr;
int nextProjectId = 1;

// Konstanta Lebar Kolom (termasuk padding)
const int ID_WIDTH = 6;
const int NAME_WIDTH = 35;
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
    // Bekerja di Linux dan macOS
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
    ss << d.day << "/" << d.month << "/" << d.year;
    return ss.str();
}

string formatRelativeTime(time_t pastTimestamp) {
    time_t now = time(0);
    double seconds = difftime(now, pastTimestamp);

    if (seconds < 60) {
        return "just now";
    } else if (seconds < 3600) {
        int minutes = static_cast<int>(seconds / 60);
        return to_string(minutes) + " minutes ago";
    } else if (seconds < 86400) {
        int hours = static_cast<int>(seconds / 3600);
        return to_string(hours) + " hours ago";
    } else {
        int days = static_cast<int>(seconds / 86400);
        return to_string(days) + " days ago";
    }
}

// ================== Implementasi Fungsi Inti ==================

void tampilkanTugas(const Task& t, ostream& os) {
    os << "| " << left << setw(ID_WIDTH - 2) << t.idTugas << " ";
    os << "| " << left << setw(NAME_WIDTH - 2) << t.namaTugas << " ";
    os << "| " << left << setw(STATUS_WIDTH - 2) << statusToString(t.statusTugas) << " ";
    os << "| " << left << setw(USER_WIDTH - 2) << (t.assignedUsername.empty() ? "Unassigned" : t.assignedUsername) << " ";
    os << "| " << left << setw(DATE_WIDTH - 2) << dateToString(t.creationDate) << " ";
    os << "| " << left << setw(DATE_WIDTH - 2) << dateToString(t.closedDate) << " |" << endl;
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

    os << "DAFTAR TUGAS\n";
    if (p.headTugas == nullptr) {
        os << "  [Tidak ada tugas dalam proyek ini]\n\n";
    } else {
        vector<int> widths = {ID_WIDTH, NAME_WIDTH, STATUS_WIDTH, USER_WIDTH, DATE_WIDTH, DATE_WIDTH};
        printHorizontalBorder(widths, os);
        os << "| " << left << setw(ID_WIDTH - 2) << "ID" << " ";
        os << "| " << left << setw(NAME_WIDTH - 2) << "Nama Tugas" << " ";
        os << "| " << left << setw(STATUS_WIDTH - 2) << "Status" << " ";
        os << "| " << left << setw(USER_WIDTH - 2) << "Ditugaskan" << " ";
        os << "| " << left << setw(DATE_WIDTH - 2) << "Tgl Dibuat" << " ";
        os << "| " << left << setw(DATE_WIDTH - 2) << "Tgl Ditutup" << " |" << endl;
        printHorizontalBorder(widths, os);

        TaskNode* currentTugas = p.headTugas;
        while (currentTugas != nullptr) {
            tampilkanTugas(currentTugas->data, os);
            currentTugas = currentTugas->next;
        }
        printHorizontalBorder(widths, os);
    }
}

void tampilkanHistoriTugas(const Task& task) {
    vector<int> widths = {REL_WIDTH, CHANGE_WIDTH, BY_WIDTH, WHEN_WIDTH};

    printHorizontalBorder(widths);
    cout << "| " << left << setw(REL_WIDTH - 2) << "Rel" << " ";
    cout << "| " << left << setw(CHANGE_WIDTH - 2) << "Change" << " ";
    cout << "| " << left << setw(BY_WIDTH - 2) << "By" << " ";
    cout << "| " << left << setw(WHEN_WIDTH - 2) << "When" << " |" << endl;
    printHorizontalBorder(widths);

    if (task.headHistory == nullptr) {
        cout << "| " << left << setw(REL_WIDTH + CHANGE_WIDTH + BY_WIDTH + WHEN_WIDTH - 4) << "[Tidak ada histori untuk tugas ini]" << " |" << endl;
    } else {
        HistoryNode* current = task.headHistory;
        while (current != nullptr) {
            cout << "| " << left << setw(REL_WIDTH - 2) << current->data.releaseVersion << " ";
            cout << "| " << left << setw(CHANGE_WIDTH - 2) << current->data.changeDescription << " ";
            cout << "| " << left << setw(BY_WIDTH - 2) << current->data.changedBy << " ";
            cout << "| " << left << setw(WHEN_WIDTH - 2) << formatRelativeTime(current->data.timestamp) << " |" << endl;
            current = current->next;
        }
    }
    printHorizontalBorder(widths);
}

void tambahProyek() {
    char lanjut;
    do {
        cout << "\n--- Tambah Proyek Baru ---\n";
        ProjectNode* nodeBaru = new ProjectNode();
        nodeBaru->data.idProyek = nextProjectId++;

        clearInputBuffer();
        cout << "Masukkan Nama Proyek: ";
        getline(cin, nodeBaru->data.namaProyek);

        cout << "Masukkan Versi Proyek (misal: v0.1): ";
        getline(cin, nodeBaru->data.version);

        cout << "Masukkan UID Proyek: ";
        getline(cin, nodeBaru->data.uid);

        cout << "Masukkan Nama Perusahaan: ";
        getline(cin, nodeBaru->data.company);

        cout << "Masukkan Nama Project Manager: ";
        getline(cin, nodeBaru->data.projectManager);

        cout << "Masukkan Deskripsi Proyek: ";
        getline(cin, nodeBaru->data.deskripsi);

        nodeBaru->next = headProyek;
        headProyek = nodeBaru;

        cout << "\nProyek '" << nodeBaru->data.namaProyek << "' berhasil ditambahkan!\n";
        cout << "\nTambah proyek lagi? (y/n): ";
        cin >> lanjut;
    } while (lanjut == 'y' || lanjut == 'Y');
}

void tambahTugas() {
    char lanjut;
    do {
        if (headProyek == nullptr) {
            cout << "Belum ada proyek. Silakan tambahkan proyek terlebih dahulu.\n";
            return;
        }
        cout << "\n--- Tambah Tugas Baru ---\n";
        cout << "Daftar Proyek yang Tersedia:\n";
        ProjectNode* currentProyekNode = headProyek;
        int index = 1;
        while(currentProyekNode != nullptr) {
            cout << left << setw(4) << to_string(index++) + "."
                 << setw(30) << currentProyekNode->data.namaProyek
                 << "(ID: " << currentProyekNode->data.idProyek << ")\n";
            currentProyekNode = currentProyekNode->next;
        }
        int idProyekPilihan;
        Project* proyekPilihan = nullptr;
        while (proyekPilihan == nullptr) {
            cout << "Pilih Proyek (masukkan ID Proyek): ";
            cin >> idProyekPilihan;
            if (cin.fail()) {
                cout << "Input tidak valid.\n";
                cin.clear(); clearInputBuffer(); continue;
            }
            proyekPilihan = cariProyekById(idProyekPilihan);
            if (proyekPilihan == nullptr) {
                cout << "Proyek tidak ditemukan.\n";
            }
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

        HistoryNode* historyNode = new HistoryNode();
        historyNode->data.releaseVersion = proyekPilihan->version;
        historyNode->data.changeDescription = "Tugas dibuat";
        historyNode->data.changedBy = loggedInUser->username;
        historyNode->data.timestamp = time(0);
        historyNode->next = nodeTugasBaru->data.headHistory;
        nodeTugasBaru->data.headHistory = historyNode;

        cout << "Tugas '" << nodeTugasBaru->data.namaTugas << "' berhasil ditambahkan!\n";
        cout << "\nTambah tugas lagi? (y/n): ";
        cin >> lanjut;
    } while (lanjut == 'y' || lanjut == 'Y');
}

void ubahStatusTugas() {
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }

    cout << "\n--- Ubah Status Tugas ---\n";
    ProjectNode* currentProyekNode = headProyek;
    while(currentProyekNode != nullptr) {
        cout << "- " << currentProyekNode->data.namaProyek << " (ID: " << currentProyekNode->data.idProyek << ")\n";
        currentProyekNode = currentProyekNode->next;
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

    if (proyekPilihan->headTugas == nullptr) {
        cout << "Proyek ini belum memiliki tugas.\n";
        return;
    }

    cout << "\nDaftar Tugas dalam Proyek '" << proyekPilihan->namaProyek << "':\n";
    TaskNode* currentTugasNode = proyekPilihan->headTugas;
    while(currentTugasNode != nullptr) {
         cout << "  - " << currentTugasNode->data.namaTugas << " (ID: " << currentTugasNode->data.idTugas << ")\n";
        currentTugasNode = currentTugasNode->next;
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

    if (statusBaru == Status::CLOSED && statusLamaStr != "Closed") {
        tugasPilihan->closedDate = getCurrentDate();
    } else if (statusBaru != Status::CLOSED && statusLamaStr == "Closed") {
        tugasPilihan->closedDate = {0, 0, 0};
    }

    HistoryNode* historyNode = new HistoryNode();
    historyNode->data.releaseVersion = proyekPilihan->version;
    historyNode->data.changeDescription = "Status: " + statusLamaStr + " -> " + statusToString(statusBaru);
    historyNode->data.changedBy = loggedInUser->username;
    historyNode->data.timestamp = time(0);

    historyNode->next = tugasPilihan->headHistory;
    tugasPilihan->headHistory = historyNode;

    cout << "Status tugas berhasil diubah dan histori dicatat!\n";
}

void assignTaskToUser() {
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }

    cout << "\n--- Tugaskan Tugas ke Pengguna ---\n";
    vector<User> assignableUsers;
    UserNode* currentUserNode = headUser;
    while(currentUserNode != nullptr) {
        if (currentUserNode->data.username != "admin") {
            assignableUsers.push_back(currentUserNode->data);
        }
        currentUserNode = currentUserNode->next;
    }
    if (assignableUsers.empty()) {
        cout << "Tidak ada pengguna untuk ditugaskan.\n";
        return;
    }
    cout << "Daftar Pengguna Tersedia:\n";
    for (size_t i = 0; i < assignableUsers.size(); ++i) {
        cout << left << setw(4) << to_string(i + 1) + "." << assignableUsers[i].username << "\n";
    }
    int userChoice;
    cout << "Pilih Pengguna (masukkan nomor): ";
    cin >> userChoice;
    if (cin.fail() || userChoice < 1 || userChoice > assignableUsers.size()) {
        cout << "Pilihan tidak valid.\n"; cin.clear(); clearInputBuffer(); return;
    }
    User chosenUser = assignableUsers[userChoice - 1];

    cout << "\nDaftar Proyek yang Tersedia:\n";
    ProjectNode* currentProyekNode = headProyek;
    while(currentProyekNode != nullptr) {
        cout << "- " << currentProyekNode->data.namaProyek << " (ID: " << currentProyekNode->data.idProyek << ")\n";
        currentProyekNode = currentProyekNode->next;
    }
    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    cout << "Pilih Proyek (masukkan ID Proyek): ";
    cin >> idProyekPilihan;
    if (cin.fail()) { cin.clear(); clearInputBuffer(); cout << "Input tidak valid.\n"; return; }
    proyekPilihan = cariProyekById(idProyekPilihan);
    if (proyekPilihan == nullptr) { cout << "Proyek tidak ditemukan.\n"; return; }

    vector<Task*> unassignedTasks;
    TaskNode* currentTugasNode = proyekPilihan->headTugas;
    while(currentTugasNode != nullptr) {
        if (currentTugasNode->data.assignedUsername.empty()) {
            unassignedTasks.push_back(&currentTugasNode->data);
        }
        currentTugasNode = currentTugasNode->next;
    }
    if (unassignedTasks.empty()) {
        cout << "Tidak ada tugas yang belum ditugaskan di proyek ini.\n";
        return;
    }
    cout << "\nDaftar Tugas yang Belum Ditugaskan:\n";
    for (size_t i = 0; i < unassignedTasks.size(); ++i) {
        cout << "  - " << unassignedTasks[i]->namaTugas << " (ID: " << unassignedTasks[i]->idTugas << ")\n";
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

    HistoryNode* historyNode = new HistoryNode();
    historyNode->data.releaseVersion = proyekPilihan->version;
    historyNode->data.changeDescription = "Ditugaskan ke: " + chosenUser.username;
    historyNode->data.changedBy = loggedInUser->username;
    historyNode->data.timestamp = time(0);
    historyNode->next = chosenTask->headHistory;
    chosenTask->headHistory = historyNode;

    cout << "\nBerhasil! Tugas '" << chosenTask->namaTugas << "' telah ditugaskan.\n";
}

void menuLihatHistori() {
    if (headProyek == nullptr) {
        cout << "Belum ada proyek.\n";
        return;
    }

    cout << "\n--- Lihat Histori Tugas ---\n";
    ProjectNode* currentProyekNode = headProyek;
    while(currentProyekNode != nullptr) {
        cout << "- " << currentProyekNode->data.namaProyek << " (ID: " << currentProyekNode->data.idProyek << ")\n";
        currentProyekNode = currentProyekNode->next;
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

    if (proyekPilihan->headTugas == nullptr) {
        cout << "Proyek ini tidak memiliki tugas.\n";
        return;
    }

    cout << "\nDaftar Tugas dalam Proyek '" << proyekPilihan->namaProyek << "':\n";
    TaskNode* currentTugasNode = proyekPilihan->headTugas;
    while(currentTugasNode != nullptr) {
        cout << "  - " << currentTugasNode->data.namaTugas << " (ID: " << currentTugasNode->data.idTugas << ")\n";
        currentTugasNode = currentTugasNode->next;
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

    cout << "\n--- Histori untuk Tugas: " << tugasPilihan->namaTugas << " ---\n";
    tampilkanHistoriTugas(*tugasPilihan);
}

bool bandingkanNamaProyek(const Project& a, const Project& b) { return a.namaProyek < b.namaProyek; }
bool bandingkanNamaTugas(const Task& a, const Task& b) { return a.namaTugas < b.namaTugas; }
bool bandingkanStatusTugas(const Task& a, const Task& b) { return static_cast<int>(a.statusTugas) < static_cast<int>(b.statusTugas); }
bool bandingkanIdTugas(const Task& a, const Task& b) { return a.idTugas < b.idTugas; }

void tampilkanSemuaProyek(bool sortByName) {
    if (headProyek == nullptr) {
        cout << "Belum ada proyek untuk ditampilkan.\n";
        return;
    }
    vector<Project> tempDaftarProyek;
    ProjectNode* current = headProyek;
    while (current != nullptr) {
        tempDaftarProyek.push_back(current->data);
        current = current->next;
    }
    if (sortByName) {
        sort(tempDaftarProyek.begin(), tempDaftarProyek.end(), bandingkanNamaProyek);
        cout << "\n--- Daftar Semua Proyek (Diurutkan Berdasarkan Nama) ---\n\n";
    } else {
        reverse(tempDaftarProyek.begin(), tempDaftarProyek.end());
        cout << "\n--- Daftar Semua Proyek (Berdasarkan ID) ---\n\n";
    }
    for (const auto& proyek : tempDaftarProyek) {
        tampilkanProyek(proyek);
    }
}

void tampilkanTugasDalamProyek() {
    if (headProyek == nullptr) {
        cout << "Belum ada proyek.\n";
        return;
    }
    cout << "\n--- Tampilkan Tugas dalam Proyek ---\n";
    cout << "Daftar Proyek:\n";
    ProjectNode* currentProyekNode = headProyek;
    while(currentProyekNode != nullptr) {
        cout << "- " << currentProyekNode->data.namaProyek << " (ID: " << currentProyekNode->data.idProyek << ")\n";
        currentProyekNode = currentProyekNode->next;
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
    if (proyekPilihan->headTugas == nullptr) {
        cout << "Proyek '" << proyekPilihan->namaProyek << "' tidak memiliki tugas.\n";
        return;
    }
    cout << "Pilih kriteria sorting tugas (1. ID, 2. Nama, 3. Status): ";
    int pilihanSort;
    cin >> pilihanSort;
    vector<Task> tempDaftarTugas;
    TaskNode* currentTugasNode = proyekPilihan->headTugas;
    while(currentTugasNode != nullptr) {
        tempDaftarTugas.push_back(currentTugasNode->data);
        currentTugasNode = currentTugasNode->next;
    }
    switch (pilihanSort) {
        case 2: sort(tempDaftarTugas.begin(), tempDaftarTugas.end(), bandingkanNamaTugas); break;
        case 3: sort(tempDaftarTugas.begin(), tempDaftarTugas.end(), bandingkanStatusTugas); break;
        default: sort(tempDaftarTugas.begin(), tempDaftarTugas.end(), bandingkanIdTugas); break;
    }

    cout << "\n--- Tugas dalam Proyek: " << proyekPilihan->namaProyek << " ---\n";
    vector<int> widths = {ID_WIDTH, NAME_WIDTH, STATUS_WIDTH, USER_WIDTH, DATE_WIDTH, DATE_WIDTH};
    printHorizontalBorder(widths);
    cout << "| " << left << setw(ID_WIDTH - 2) << "ID" << " ";
    cout << "| " << left << setw(NAME_WIDTH - 2) << "Nama Tugas" << " ";
    cout << "| " << left << setw(STATUS_WIDTH - 2) << "Status" << " ";
    cout << "| " << left << setw(USER_WIDTH - 2) << "Ditugaskan" << " ";
    cout << "| " << left << setw(DATE_WIDTH - 2) << "Tgl Dibuat" << " ";
    cout << "| " << left << setw(DATE_WIDTH - 2) << "Tgl Ditutup" << " |" << endl;
    printHorizontalBorder(widths);

    for (const auto& tugas : tempDaftarTugas) {
        tampilkanTugas(tugas);
    }
    printHorizontalBorder(widths);
}

void generateReport() {
    cout << "\n--- Laporan Data Tugas ---\n";
    if (headProyek == nullptr) {
        cout << "Tidak ada proyek untuk membuat laporan.\n";
        return;
    }
    cout << "Pilih jenis laporan (1. Mingguan, 2. Bulanan): ";
    int pilihanReport;
    cin >> pilihanReport;
    if (cin.fail() || (pilihanReport != 1 && pilihanReport != 2)) {
        cout << "Input tidak valid.\n";
        cin.clear(); clearInputBuffer(); return;
    }
    Date today = getCurrentDate();
    cout << "\nLaporan Per Tanggal: " << today.day << "/" << today.month << "/" << today.year << "\n";
    int closedTasks = 0;
    int onProgressTasks = 0;
    int newTasks = 0;
    ProjectNode* currentProyek = headProyek;
    while (currentProyek != nullptr) {
        TaskNode* currentTugas = currentProyek->data.headTugas;
        while (currentTugas != nullptr) {
            Task t = currentTugas->data;
            bool includeInReport = false;
            if (pilihanReport == 1) { // Laporan Mingguan
                time_t now = time(0);
                tm taskTime = {0};
                taskTime.tm_mday = t.creationDate.day;
                taskTime.tm_mon = t.creationDate.month - 1;
                taskTime.tm_year = t.creationDate.year - 1900;
                time_t taskTimestamp = mktime(&taskTime);
                double seconds = difftime(now, taskTimestamp);
                if (seconds >= 0 && seconds <= (7 * 24 * 60 * 60)) {
                    includeInReport = true;
                }
                if (t.statusTugas == Status::CLOSED) {
                    tm closedTime = {0};
                    closedTime.tm_mday = t.closedDate.day;
                    closedTime.tm_mon = t.closedDate.month - 1;
                    closedTime.tm_year = t.closedDate.year - 1900;
                    time_t closedTimestamp = mktime(&closedTime);
                    seconds = difftime(now, closedTimestamp);
                    if (seconds >= 0 && seconds <= (7 * 24 * 60 * 60)) {
                        includeInReport = true;
                    }
                }
            } else { // Laporan Bulanan
                if (t.creationDate.month == today.month && t.creationDate.year == today.year) {
                    includeInReport = true;
                }
                if (t.statusTugas == Status::CLOSED && t.closedDate.month == today.month && t.closedDate.year == today.year) {
                    includeInReport = true;
                }
            }
            if (includeInReport) {
                if (t.statusTugas == Status::CLOSED) {
                    closedTasks++;
                } else if (t.statusTugas == Status::ON_PROGRESS) {
                    onProgressTasks++;
                } else if (t.statusTugas == Status::NEW) {
                    newTasks++;
                }
            }
            currentTugas = currentTugas->next;
        }
        currentProyek = currentProyek->next;
    }

    cout << "\n--- Ringkasan Laporan " << (pilihanReport == 1 ? "Mingguan" : "Bulanan") << " ---\n";
    vector<int> widths = {35, 10};
    printHorizontalBorder(widths);
    cout << "| " << left << setw(widths[0]-2) << "Kategori" << "| " << setw(widths[1]-2) << "Jumlah" << " |" << endl;
    printHorizontalBorder(widths);
    cout << "| " << left << setw(widths[0]-2) << "Total Tugas Baru" << "| " << setw(widths[1]-2) << newTasks << " |" << endl;
    cout << "| " << left << setw(widths[0]-2) << "Total Tugas Sedang Dikerjakan" << "| " << setw(widths[1]-2) << onProgressTasks << " |" << endl;
    cout << "| " << left << setw(widths[0]-2) << "Total Tugas Selesai" << "| " << setw(widths[1]-2) << closedTasks << " |" << endl;
    printHorizontalBorder(widths);
}

void tampilkanMatriksStatusProyek() {
    cout << "\n--- Laporan Matriks: Status Tugas per Proyek ---\n";
    if (headProyek == nullptr) {
        cout << "Belum ada proyek untuk ditampilkan.\n";
        return;
    }

    map<int, map<Status, int>> projectStatusCounts;
    map<int, string> projectNames;

    ProjectNode* currentProyek = headProyek;
    while (currentProyek != nullptr) {
        projectNames[currentProyek->data.idProyek] = currentProyek->data.namaProyek;
        projectStatusCounts[currentProyek->data.idProyek] = {
            {Status::NEW, 0}, {Status::READY, 0}, {Status::ON_PROGRESS, 0},
            {Status::RFT, 0}, {Status::CLOSED, 0}, {Status::NEED_INFO, 0}
        };

        TaskNode* currentTugas = currentProyek->data.headTugas;
        while (currentTugas != nullptr) {
            projectStatusCounts[currentProyek->data.idProyek][currentTugas->data.statusTugas]++;
            currentTugas = currentTugas->next;
        }
        currentProyek = currentProyek->next;
    }

    const int PROYEK_COL_WIDTH = 25;
    const int STATUS_COL_WIDTH = 13;
    vector<Status> statusOrder = {Status::NEW, Status::READY, Status::ON_PROGRESS, Status::RFT, Status::NEED_INFO, Status::CLOSED};

    vector<int> widths;
    widths.push_back(PROYEK_COL_WIDTH);
    for(size_t i = 0; i < statusOrder.size(); ++i) widths.push_back(STATUS_COL_WIDTH);

    printHorizontalBorder(widths);
    cout << "| " << left << setw(PROYEK_COL_WIDTH - 2) << "Nama Proyek" << " ";
    for (const auto& status : statusOrder) {
        cout << "| " << setw(STATUS_COL_WIDTH - 2) << statusToString(status) << " ";
    }
    cout << "|" << endl;
    printHorizontalBorder(widths);

    for (const auto& pair : projectNames) {
        int projectId = pair.first;
        string projectName = pair.second;
        cout << "| " << left << setw(PROYEK_COL_WIDTH - 2) << projectName << " ";
        for (const auto& status : statusOrder) {
            cout << "| " << setw(STATUS_COL_WIDTH - 2) << projectStatusCounts[projectId][status] << " ";
        }
        cout << "|" << endl;
    }
    printHorizontalBorder(widths);
}

void printUserAchievements() {
    if (loggedInUser == nullptr) {
        cout << "Anda harus login untuk mencetak pencapaian.\n";
        return;
    }

    cout << "\n--- Cetak Pencapaian Pengguna ---\n";
    string filename;
    cout << "Masukkan nama file untuk menyimpan pencapaian (contoh: pencapaian_saya.txt): ";
    clearInputBuffer();
    getline(cin, filename);

    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error: Gagal membuka file untuk ditulis.\n";
        return;
    }

    Date today = getCurrentDate();
    outFile << "Laporan Pencapaian untuk: " << loggedInUser->username << "\n";
    outFile << "Tanggal Laporan: " << dateToString(today) << "\n";
    outFile << "==========================================================\n\n";

    int totalTasks = 0;
    int closedTasks = 0;

    ProjectNode* currentProyek = headProyek;
    while (currentProyek != nullptr) {
        Project userProject = currentProyek->data;
        userProject.headTugas = nullptr;

        TaskNode* originalTugas = currentProyek->data.headTugas;
        TaskNode* tail = nullptr;
        bool hasUserTasks = false;

        while(originalTugas != nullptr) {
            if (originalTugas->data.assignedUsername == loggedInUser->username) {
                hasUserTasks = true;
                totalTasks++;
                if (originalTugas->data.statusTugas == Status::CLOSED) {
                    closedTasks++;
                }
                TaskNode* newTaskNode = new TaskNode{originalTugas->data, nullptr};
                if (userProject.headTugas == nullptr) {
                    userProject.headTugas = newTaskNode;
                    tail = newTaskNode;
                } else {
                    tail->next = newTaskNode;
                    tail = newTaskNode;
                }
            }
            originalTugas = originalTugas->next;
        }

        if (hasUserTasks) {
            tampilkanProyek(userProject, outFile);
            bersihkanMemoriTugas(userProject.headTugas);
        }
        currentProyek = currentProyek->next;
    }

    outFile << "\n--- Ringkasan ---\n";
    outFile << "Total tugas yang ditugaskan: " << totalTasks << "\n";
    outFile << "Tugas yang diselesaikan: " << closedTasks << "\n";
    if (totalTasks > 0) {
        double completionRate = (static_cast<double>(closedTasks) / totalTasks) * 100.0;
        outFile << "Tingkat Penyelesaian: " << fixed << setprecision(2) << completionRate << "%\n";
    }

    outFile.close();
    cout << "Pencapaian berhasil dicetak ke " << filename << "\n";
}

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
        }
        else {
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
    cout << "Masukkan username: ";
    clearInputBuffer();
    getline(cin, username);

    cout << "Masukkan password: ";
    password = getMaskedPassword();

    UserNode* currentUser = headUser;
    while (currentUser != nullptr) {
        if (currentUser->data.username == username) {
            cout << "Username sudah terdaftar.\n";
            return;
        }
        currentUser = currentUser->next;
    }
    UserNode* newUser = new UserNode();
    newUser->data.username = username;
    newUser->data.password = password;
    newUser->next = headUser;
    headUser = newUser;
    cout << "Registrasi berhasil! Silakan login.\n";
}

void login() {
    string username, password;
    cout << "\n--- Login --- \n";
    cout << "Username: ";
    clearInputBuffer();
    getline(cin, username);

    cout << "Password: ";
    password = getMaskedPassword();

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

void tampilkanMenuUtama() {
    int pilihan;
    do {
        clearScreen();

        cout << "\n===== Menu Manajemen Proyek =====\n";
        bool isAdmin = (loggedInUser != nullptr && loggedInUser->username == "admin");

        if (isAdmin) {
            cout << "1. Tambah Proyek Baru\n";
            cout << "2. Tambah Tugas ke Proyek\n";
            cout << "3. Tugaskan Tugas ke Pengguna\n";
            cout << "4. Ubah Status Tugas\n";
            cout << "5. 📜 Lihat Histori Tugas\n";
            cout << "6. Tampilkan Semua Proyek (by ID)\n";
            cout << "7. Tampilkan Semua Proyek (by Name)\n";
            cout << "8. Tampilkan Tugas dalam Proyek\n";
            cout << "9. Buat Laporan Mingguan/Bulanan\n";
            cout << "10. 📊 Tampilkan Laporan Matriks\n";
            cout << "11. Logout\n";
        } else { // Regular user menu
            cout << "1. Ubah Status Tugas\n";
            cout << "2. 📜 Lihat Histori Tugas\n";
            cout << "3. Tampilkan Semua Proyek (by ID)\n";
            cout << "4. Tampilkan Semua Proyek (by Name)\n";
            cout << "5. Tampilkan Tugas dalam Proyek\n";
            cout << "6. 🖨️ Cetak Pencapaian Saya ke File\n";
            cout << "7. Logout\n";
        }
        cout << "Pilihan Anda: ";

        cin >> pilihan;
        if (cin.fail()) {
            cout << "Input tidak valid.\n";
            cin.clear(); clearInputBuffer(); pilihan = -1; continue;
        }

        if (isAdmin) {
            switch(pilihan) {
                case 1: tambahProyek(); break;
                case 2: tambahTugas(); break;
                case 3: assignTaskToUser(); break;
                case 4: ubahStatusTugas(); break;
                case 5: menuLihatHistori(); break;
                case 6: tampilkanSemuaProyek(false); break;
                case 7: tampilkanSemuaProyek(true); break;
                case 8: tampilkanTugasDalamProyek(); break;
                case 9: generateReport(); break;
                case 10: tampilkanMatriksStatusProyek(); break;
                case 11:
                    loggedInUser = nullptr;
                    cout << "\nAnda telah logout.\n";
                    break;
                default: cout << "Pilihan tidak valid.\n";
            }
        } else { // Regular user
             switch(pilihan) {
                case 1: ubahStatusTugas(); break;
                case 2: menuLihatHistori(); break;
                case 3: tampilkanSemuaProyek(false); break;
                case 4: tampilkanSemuaProyek(true); break;
                case 5: tampilkanTugasDalamProyek(); break;
                case 6: printUserAchievements(); break;
                case 7:
                    loggedInUser = nullptr;
                    cout << "\nAnda telah logout.\n";
                    break;
                default: cout << "Pilihan tidak valid.\n";
            }
        }

        if (loggedInUser != nullptr) {
            cout << "\nTekan Enter untuk kembali ke menu...";
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
            cin.clear(); clearInputBuffer(); pilihan = -1; continue;
        }
        switch (pilihan) {
            case 1:
                login();
                if (loggedInUser != nullptr) {
                    cout << "\nTekan Enter untuk melanjutkan...";
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