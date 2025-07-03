#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <ctime>
#include <fstream>

using namespace std;

enum class Status {
    NEW,
    READY,
    ON_PROGRESS,
    RFT,
    CLOSED,
    NEED_INFO
};

struct Date {
    int day;
    int month;
    int year;
};

struct HistoryEntry {
    string releaseVersion;
    string changeDescription;
    string changedBy;
    time_t timestamp;
};

struct HistoryNode {
    HistoryEntry data;
    HistoryNode* next;
};

struct Task {
    string namaTugas;
    Status statusTugas;
    int idTugas;
    Date creationDate;
    Date closedDate;
    string assignedUsername;
    HistoryNode* headHistory = nullptr;
};

struct TaskNode {
    Task data;
    TaskNode* next;
};

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