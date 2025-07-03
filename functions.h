#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structures.h"
#include <vector>
#include <iostream>
#include <string>

// Deklarasi Fungsi

// Menu dan UI
void clearScreen();
void tampilkanMainMenu();
void tampilkanMenuUtama();

// Autentikasi Pengguna
void login();
void registrasi();
string getMaskedPassword();

// Logika Inti (Proyek & Tugas)
void tambahProyek();
void tambahTugas();
void ubahStatusTugas();
void assignTaskToUser();
void tampilkanSemuaProyek(bool sortByName = false);
void tampilkanTugasDalamProyek();
void generateReport();
void tampilkanMatriksStatusProyek();
void menuLihatHistori();
void printUserAchievements();

// Fungsi Utilitas
void printHorizontalBorder(const vector<int>& widths, ostream& os = cout);
string statusToString(Status s);
void clearInputBuffer();
Date getCurrentDate();
void tampilkanTugas(const Task& t, ostream& os = cout);
void tampilkanProyek(const Project& p, ostream& os = cout);
Project* cariProyekById(int id);
bool bandingkanNamaProyek(const Project& a, const Project& b);
bool bandingkanNamaTugas(const Task& a, const Task& b);
bool bandingkanStatusTugas(const Task& a, const Task& b);
bool bandingkanIdTugas(const Task& a, const Task& b);

// Manajemen Memori
void bersihkanMemori();
void bersihkanMemoriTugas(TaskNode* head);
void bersihkanMemoriPengguna();
void bersihkanMemoriHistori(HistoryNode* head);

#endif // FUNCTIONS_H