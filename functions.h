#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structures.h"

// ================== Deklarasi Fungsi ==================

// Fungsi Menu dan UI
void tampilkanMainMenu();
void tampilkanMenuUtama();

// Fungsi Autentikasi Pengguna
void login();
void registrasi();

// Fungsi Logika Inti (Proyek & Tugas)
void tambahProyek();
void tambahTugas();
void ubahStatusTugas();
void assignTaskToUser();
void tampilkanSemuaProyek(bool sortByName = false);
void tampilkanTugasDalamProyek();
void generateReport();

// Fungsi Utilitas
string statusToString(Status s);
void clearInputBuffer();
Date getCurrentDate();
void tampilkanTugas(const Task& t);
void tampilkanProyek(const Project& p);
Project* cariProyekById(int id);
bool bandingkanNamaProyek(const Project& a, const Project& b);
bool bandingkanNamaTugas(const Task& a, const Task& b);
bool bandingkanStatusTugas(const Task& a, const Task& b);
bool bandingkanIdTugas(const Task& a, const Task& b);

// Fungsi Manajemen Memori
void bersihkanMemori();
void bersihkanMemoriTugas(TaskNode* head);
void bersihkanMemoriPengguna();

#endif // FUNCTIONS_H