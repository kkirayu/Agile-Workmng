#include "functions.h"
#include <iostream>

int main() {
    // Inisialisasi pengguna admin default
    UserNode* adminUser = new UserNode();
    adminUser->data.username = "admin";
    adminUser->data.password = "admin123";
    adminUser->next = headUser;
    headUser = adminUser;

    // Tampilkan menu utama untuk memulai interaksi pengguna
    tampilkanMainMenu();

    // Bersihkan semua memori yang dialokasikan sebelum program berakhir
    bersihkanMemori();

    cout << "Terima kasih telah menggunakan program ini!\n";

    return 0;
}