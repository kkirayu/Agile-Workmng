#include "functions.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>
#include <chrono>

// ================== Definisi Variabel Global ==================
ProjectNode* headProyek = nullptr;
UserNode* headUser = nullptr;
User* loggedInUser = nullptr;
int nextProjectId = 1;

// ================== Implementasi Fungsi ==================

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

void tampilkanTugas(const Task& t) {
    cout << "    ID Tugas: " << t.idTugas << "\n";
    cout << "    Nama Tugas: " << t.namaTugas << "\n";
    cout << "    Status: " << statusToString(t.statusTugas) << "\n";
    if (!t.assignedUsername.empty()) {
        cout << "    Ditugaskan kepada: " << t.assignedUsername << "\n";
    }
    cout << "    Tanggal Dibuat: " << t.creationDate.day << "/" << t.creationDate.month << "/" << t.creationDate.year << "\n";
    if (t.statusTugas == Status::CLOSED) {
        cout << "    Tanggal Ditutup: " << t.closedDate.day << "/" << t.closedDate.month << "/" << t.closedDate.year << "\n";
    }
    cout << "    -------------------------\n";
}

void tampilkanProyek(const Project& p) {
    cout << "\n--- Detail Proyek --- \n";
    cout << "ID Proyek: " << p.idProyek << "\n";
    cout << "Nama Proyek: " << p.namaProyek << "\n";
    cout << "Daftar Tugas:\n";
    TaskNode* currentTugas = p.headTugas;
    if (currentTugas == nullptr) {
        cout << "  [Tidak ada tugas dalam proyek ini]\n";
    } else {
        while (currentTugas != nullptr) {
            tampilkanTugas(currentTugas->data);
            currentTugas = currentTugas->next;
        }
    }
    cout << "-------------------------\n";
}

void tambahProyek() {
    char lanjut;
    do {
        cout << "\n--- Tambah Proyek Baru ---\n";
        ProjectNode* nodeBaru = new ProjectNode();
        nodeBaru->data.idProyek = nextProjectId++;
        nodeBaru->data.headTugas = nullptr;
        cout << "Masukkan Nama Proyek: ";
        clearInputBuffer();
        getline(cin, nodeBaru->data.namaProyek);
        nodeBaru->next = headProyek;
        headProyek = nodeBaru;
        cout << "Proyek '" << nodeBaru->data.namaProyek << "' (ID: " << nodeBaru->data.idProyek << ") berhasil ditambahkan!\n";
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
            cout << index++ << ". " << currentProyekNode->data.namaProyek << " (ID: " << currentProyekNode->data.idProyek << ")\n";
            currentProyekNode = currentProyekNode->next;
        }
        int idProyekPilihan;
        Project* proyekPilihan = nullptr;
        while (proyekPilihan == nullptr) {
            cout << "Pilih Proyek (masukkan ID Proyek): ";
            cin >> idProyekPilihan;
            if (cin.fail()) {
                cout << "Input tidak valid. Masukkan angka.\n";
                cin.clear(); clearInputBuffer(); continue;
            }
            proyekPilihan = cariProyekById(idProyekPilihan);
            if (proyekPilihan == nullptr) {
                cout << "Proyek dengan ID " << idProyekPilihan << " tidak ditemukan. Coba lagi.\n";
            }
        }
        TaskNode* nodeTugasBaru = new TaskNode();
        nodeTugasBaru->data.idTugas = proyekPilihan->nextTaskId++;
        cout << "Masukkan Nama Tugas untuk Proyek '" << proyekPilihan->namaProyek << "': ";
        clearInputBuffer();
        getline(cin, nodeTugasBaru->data.namaTugas);
        cout << "Pilih Status Tugas (1. New, 2. Ready, 3. On Progress, 4. RFT, 5. Closed, 6. Need Info): ";
        int pilihanStatus;
        while (!(cin >> pilihanStatus) || pilihanStatus < 1 || pilihanStatus > 6) {
            cout << "Pilihan tidak valid. Masukkan angka antara 1 dan 6.\n";
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
        nodeTugasBaru->data.closedDate = {0, 0, 0};
        nodeTugasBaru->data.assignedUsername = ""; // Inisialisasi tugas sebagai belum ditugaskan
        nodeTugasBaru->next = proyekPilihan->headTugas;
        proyekPilihan->headTugas = nodeTugasBaru;
        cout << "Tugas '" << nodeTugasBaru->data.namaTugas << "' berhasil ditambahkan ke proyek '" << proyekPilihan->namaProyek << "'!\n";
        cout << "\nTambah tugas lagi? (y/n): ";
        cin >> lanjut;
    } while (lanjut == 'y' || lanjut == 'Y');
}

void assignTaskToUser() {
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
        cout << "Tidak ada pengguna yang tersedia untuk ditugaskan.\n";
        return;
    }
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
    if (headProyek == nullptr) { cout << "Belum ada proyek.\n"; return; }
    cout << "\nDaftar Proyek yang Tersedia:\n";
    ProjectNode* currentProyekNode = headProyek;
    int index = 1;
    while(currentProyekNode != nullptr) {
        cout << index++ << ". " << currentProyekNode->data.namaProyek << " (ID: " << currentProyekNode->data.idProyek << ")\n";
        currentProyekNode = currentProyekNode->next;
    }
    int idProyekPilihan;
    Project* proyekPilihan = nullptr;
    cout << "Pilih Proyek (masukkan ID Proyek): ";
    cin >> idProyekPilihan;
    if (cin.fail()) { cin.clear(); clearInputBuffer(); cout << "Input ID tidak valid.\n"; return; }
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
        cout << i + 1 << ". " << unassignedTasks[i]->namaTugas << " (ID: " << unassignedTasks[i]->idTugas << ")\n";
    }
    int taskChoice;
    cout << "Pilih Tugas yang akan ditugaskan (masukkan nomor): ";
    cin >> taskChoice;
    if (cin.fail() || taskChoice < 1 || taskChoice > unassignedTasks.size()) {
        cout << "Pilihan tidak valid.\n"; cin.clear(); clearInputBuffer(); return;
    }
    Task* chosenTask = unassignedTasks[taskChoice - 1];
    chosenTask->assignedUsername = chosenUser.username;
    cout << "\nBerhasil! Tugas '" << chosenTask->namaTugas << "' telah ditugaskan kepada pengguna '" << chosenUser.username << "'.\n";
}

void ubahStatusTugas() {
    if (headProyek == nullptr) {
        cout << "Belum ada proyek.\n";
        return;
    }
    cout << "\n--- Ubah Status Tugas ---\n";
    cout << "Daftar Proyek yang Tersedia:\n";
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
        cout << "Proyek '" << proyekPilihan->namaProyek << "' belum memiliki tugas.\n";
        return;
    }
    cout << "\nDaftar Tugas dalam Proyek '" << proyekPilihan->namaProyek << "':\n";
    TaskNode* currentTugasNode = proyekPilihan->headTugas;
    while(currentTugasNode != nullptr) {
        cout << "  - " << currentTugasNode->data.namaTugas << " (ID: " << currentTugasNode->data.idTugas << ")"
             << " - Status: " << statusToString(currentTugasNode->data.statusTugas) << "\n";
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
    cout << "\nPilih Status Baru untuk Tugas '" << tugasPilihan->namaTugas << "' (1-6): ";
    int pilihanStatus;
    while (!(cin >> pilihanStatus) || pilihanStatus < 1 || pilihanStatus > 6) {
        cout << "Pilihan tidak valid.\n";
        cin.clear(); clearInputBuffer();
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
    if (statusBaru == Status::CLOSED && tugasPilihan->statusTugas != Status::CLOSED) {
        tugasPilihan->closedDate = getCurrentDate();
    } else if (statusBaru != Status::CLOSED && tugasPilihan->statusTugas == Status::CLOSED) {
        tugasPilihan->closedDate = {0, 0, 0};
    }
    tugasPilihan->statusTugas = statusBaru;
    cout << "Status tugas '" << tugasPilihan->namaTugas << "' berhasil diubah menjadi '" << statusToString(statusBaru) << "'!\n";
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
        cout << "\n--- Daftar Semua Proyek (Diurutkan Berdasarkan Nama) ---\n";
    } else {
        reverse(tempDaftarProyek.begin(), tempDaftarProyek.end());
        cout << "\n--- Daftar Semua Proyek (Berdasarkan ID) ---\n";
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
    cout << "Daftar Proyek yang Tersedia:\n";
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
    for (const auto& tugas : tempDaftarTugas) {
        tampilkanTugas(tugas);
    }
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
        cout << "Input tidak valid. Pilih 1 atau 2.\n";
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
    cout << "\n--- Ringkasan Laporan ---\n";
    cout << "Total Tugas Baru: " << newTasks << "\n";
    cout << "Total Tugas Sedang Dikerjakan (On Progress): " << onProgressTasks << "\n";
    cout << "Total Tugas Selesai (Closed): " << closedTasks << "\n";
    cout << "-------------------------\n";
}

void registrasi() {
    cout << "\n--- Registrasi Pengguna Baru ---\n";
    string username, password;
    cout << "Masukkan username: ";
    clearInputBuffer();
    getline(cin, username);
    cout << "Masukkan password: ";
    getline(cin, password);
    UserNode* currentUser = headUser;
    while (currentUser != nullptr) {
        if (currentUser->data.username == username) {
            cout << "Username sudah terdaftar. Silakan gunakan username lain.\n";
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
    getline(cin, password);
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

void bersihkanMemoriTugas(TaskNode* head) {
    TaskNode* currentTugas = head;
    while (currentTugas != nullptr) {
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
        cout << "\n===== Menu Manajemen Proyek =====\n";
        bool isAdmin = (loggedInUser != nullptr && loggedInUser->username == "admin");
        int nomorMenu = 1;
        int assignOpt = 0;
        if (isAdmin) {
            cout << nomorMenu++ << ". Tambah Proyek Baru\n";
            cout << nomorMenu++ << ". Tambah Tugas ke Proyek\n";
            assignOpt = nomorMenu++;
            cout << assignOpt << ". Tugaskan Tugas ke Pengguna\n";
        }
        int ubahStatusOpt = nomorMenu++;
        int tampilIdOpt = nomorMenu++;
        int tampilNamaOpt = nomorMenu++;
        int tampilTugasOpt = nomorMenu++;
        int laporanOpt = nomorMenu++;
        int logoutOpt = nomorMenu++;
        cout << ubahStatusOpt << ". Ubah Status Tugas\n";
        cout << tampilIdOpt << ". Tampilkan Semua Proyek (Berdasarkan ID)\n";
        cout << tampilNamaOpt << ". Tampilkan Semua Proyek (Urut Nama)\n";
        cout << tampilTugasOpt << ". Tampilkan Tugas dalam Proyek (Dengan Opsi Sorting)\n";
        cout << laporanOpt << ". Buat Laporan Mingguan/Bulanan\n";
        cout << logoutOpt << ". Logout\n";
        cout << "Pilihan Anda: ";
        cin >> pilihan;
        if (cin.fail()) {
            cout << "Input tidak valid. Masukkan angka.\n";
            cin.clear(); clearInputBuffer(); pilihan = -1; continue;
        }
        if (isAdmin && pilihan == 1) {
            tambahProyek();
        } else if (isAdmin && pilihan == 2) {
            tambahTugas();
        } else if (isAdmin && pilihan == assignOpt) {
            assignTaskToUser();
        } else if (pilihan == ubahStatusOpt) {
            ubahStatusTugas();
        } else if (pilihan == tampilIdOpt) {
            tampilkanSemuaProyek(false);
        } else if (pilihan == tampilNamaOpt) {
            tampilkanSemuaProyek(true);
        } else if (pilihan == tampilTugasOpt) {
            tampilkanTugasDalamProyek();
        } else if (pilihan == laporanOpt) {
            generateReport();
        } else if (pilihan == logoutOpt) {
            loggedInUser = nullptr;
            cout << "\nAnda telah logout.\n";
            break;
        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    } while (loggedInUser != nullptr);
}

void tampilkanMainMenu() {
    int pilihan;
    do {
        cout << "\n===== Selamat Datang di Sistem Manajemen Proyek =====\n";
        cout << "1. Login\n";
        cout << "2. Registrasi\n";
        cout << "0. Keluar\n";
        cout << "Pilihan Anda: ";
        cin >> pilihan;
        if (cin.fail()) {
            cout << "Input tidak valid. Masukkan angka.\n";
            cin.clear(); clearInputBuffer(); pilihan = -1; continue;
        }
        switch (pilihan) {
            case 1:
                login();
                if (loggedInUser != nullptr) {
                    tampilkanMenuUtama();
                }
                break;
            case 2: registrasi(); break;
            case 0: break;
            default: cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    } while (pilihan != 0);
}