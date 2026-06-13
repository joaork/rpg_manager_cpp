#include "include/menu.h"
#include <iostream>

int main() {
    try {
        rpg::MenuPrincipal menu;
        menu.executar();
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << '\n';
        return 1;
    }
    return 0;
}