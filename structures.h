#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <ctime>
#include <fstream> 

using namespace std;

// Enum untuk status tugas
enum class Status {
    NEW,
    READY,
    ON_PROGRESS,
    RFT,
    CLOSED,
    NEED_INFO
};

// Struct untuk menyimpan tanggal
struct Date {
    int day;
    int month;
    int year;
};

// Struct untuk histori perubahan
struct HistoryEntry {
    string releaseVersion;
    string changeDescription;
    string changedBy;
    time_t timestamp;
};

// Node untuk linked list Histori
struct HistoryNode {
    HistoryEntry data;
    HistoryNode* next;
};

// Struct untuk tugas
struct Task {
    string namaTugas;
    Status statusTugas;
    int idTugas;
    Date creationDate;
    Date closedDate;
    string assignedUsername;
    HistoryNode* headHistory = nullptr; 
};

// Node untuk linked list Tugas
struct TaskNode {
    Task data;
    TaskNode* next;
};

// Struct untuk proyek
struct Project {
    string namaProyek;
    int idProyek;
    string deskripsi;
    string company;
    string uid;
    string projectManager;
    string version; 
    int nextTaskId = 1;
    TaskNode* headTugas = nullptr;
};

// Node untuk linked list Proyek
struct ProjectNode {
    Project data;
    ProjectNode* next;
};

// Struct untuk pengguna
struct User {
    string username;
    string password;
};

// Node untuk linked list Pengguna
struct UserNode {
    User data;
    UserNode* next;
};

// Deklarasi Variabel Global
extern ProjectNode* headProyek;
extern UserNode* headUser;
extern User* loggedInUser;
extern int nextProjectId;

#endif // STRUCTURES_H