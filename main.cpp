#include "functions.h"
#include "seeder.h" // <-- Tambahkan include ini
#include <iostream>

int main() {
    // Inisialisasi pengguna admin default
    UserNode* adminUser = new UserNode();
    adminUser->data.username = "admin";
    adminUser->data.password = "admin123";
    adminUser->next = headUser;
    headUser = adminUser;

    // Muat dummy data
    loadDummyData();

    // Tampilkan menu utama
    tampilkanMainMenu();

    // Bersihkan memori
    bersihkanMemori();

    std::cout << "Thank you for using this program!\n";

    return 0;
}