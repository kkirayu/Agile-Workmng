#include "functions.h"
#include <iostream>

int main() {
    UserNode* adminUser = new UserNode();
    adminUser->data.username = "admin";
    adminUser->data.password = "admin123";
    adminUser->next = headUser;
    headUser = adminUser;

    tampilkanMainMenu();

    bersihkanMemori();

    cout << "Terima kasih telah menggunakan program ini!\n";

    return 0;
}