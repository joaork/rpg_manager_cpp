#include "include/inventario.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sstream>

namespace rpg {

Inventario::Inventario(int capacidade, float peso_max)
    : capacidade_maxima(capacidade), peso_maximo(peso_max), peso_atual(0.0f),
      arma_equipada(nullptr), armadura_equipada(nullptr) {}

bool Inventario::adicionarItem(std::shared_ptr<Item> item) {
    if (!item) return false;

    if (static_cast<int>(itens.size()) >= capacidade_maxima) {
        std::cout << "inventario cheio. nao e possivel adicionar " << item->getNome() << ".\n";
        return false;
    }

    if (peso_atual + item->getPeso() > peso_maximo) {
        std::cout << "peso excedido. nao e possivel carregar " << item->getNome() << ".\n";
        return false;
    }

    itens.push_back(item);
    peso_atual += item->getPeso();
    std::cout << item->getNome() << " adicionado ao inventario.\n";
    return true;
}

bool Inventario::removerItem(const std::string& nome_item) {
    auto it = std::find_if(itens.begin(), itens.end(),
        [&nome_item](const std::shared_ptr<Item>& i) {
            return i->getNome() == nome_item;
        });

    if (it == itens.end()) {
        std::cout << "item '" << nome_item << "' nao encontrado no inventario.\n";
        return false;
    }

    peso_atual -= (*it)->getPeso();
    itens.erase(it);
    std::cout << nome_item << " removido do inventario.\n";
    return true;
}

std::shared_ptr<Item> Inventario::buscarItem(const std::string& nome_item) const {
    auto it = std::find_if(itens.begin(), itens.end(),
        [&nome_item](const std::shared_ptr<Item>& i) {
            return i->getNome() == nome_item;
        });

    return (it != itens.end()) ? *it : nullptr;
}

bool Inventario::equiparArma(const std::string& nome_arma) {
    auto item = buscarItem(nome_arma);
    if (!item) {
        std::cout << "arma '" << nome_arma << "' nao encontrada no inventario.\n";
        return false;
    }

    if (item->getTipo() != TipoItem::ARMA) {
        std::cout << "'" << nome_arma << "' nao e uma arma.\n";
        return false;
    }

    // desequipa a arma anterior se houver
    if (arma_equipada) {
        std::cout << "desequipando " << arma_equipada->getNome() << ".\n";
    }

    arma_equipada = std::dynamic_pointer_cast<Arma>(item);
    std::cout << nome_arma << " equipada com sucesso.\n";
    return true;
}

bool Inventario::equiparArmadura(const std::string& nome_armadura) {
    auto item = buscarItem(nome_armadura);
    if (!item) {
        std::cout << "armadura '" << nome_armadura << "' nao encontrada no inventario.\n";
        return false;
    }

    if (item->getTipo() != TipoItem::ARMADURA) {
        std::cout << "'" << nome_armadura << "' nao e uma armadura.\n";
        return false;
    }

    if (armadura_equipada) {
        std::cout << "desequipando " << armadura_equipada->getNome() << ".\n";
    }

    armadura_equipada = std::dynamic_pointer_cast<Armadura>(item);
    std::cout << nome_armadura << " equipada com sucesso.\n";
    return true;
}

void Inventario::desequiparArma() {
    if (!arma_equipada) {
        std::cout << "nenhuma arma equipada no momento.\n";
        return;
    }
    std::cout << arma_equipada->getNome() << " desequipada.\n";
    arma_equipada = nullptr;
}

void Inventario::desequiparArmadura() {
    if (!armadura_equipada) {
        std::cout << "nenhuma armadura equipada no momento.\n";
        return;
    }
    std::cout << armadura_equipada->getNome() << " desequipada.\n";
    armadura_equipada = nullptr;
}

int Inventario::usarPocao(const std::string& nome_pocao) {
    auto item = buscarItem(nome_pocao);
    if (!item) {
        std::cout << "pocao '" << nome_pocao << "' nao encontrada.\n";
        return 0;
    }

    if (item->getTipo() != TipoItem::POCAO) {
        std::cout << "'" << nome_pocao << "' nao e uma pocao.\n";
        return 0;
    }

    auto pocao = std::dynamic_pointer_cast<Pocao>(item);
    int cura = pocao->getQuantidadeCura();

    // remove a pocao do inventario apos usar
    removerItem(nome_pocao);
    return cura;
}

std::shared_ptr<Arma> Inventario::getArmaEquipada() const { return arma_equipada; }
std::shared_ptr<Armadura> Inventario::getArmaduraEquipada() const { return armadura_equipada; }
float Inventario::getPesoAtual() const { return peso_atual; }
float Inventario::getPesoMaximo() const { return peso_maximo; }
int Inventario::getQuantidadeItens() const { return static_cast<int>(itens.size()); }
int Inventario::getCapacidadeMaxima() const { return capacidade_maxima; }
bool Inventario::estaCheia() const { return static_cast<int>(itens.size()) >= capacidade_maxima; }

void Inventario::listarItens() const {
    if (itens.empty()) {
        std::cout << "  inventario vazio.\n";
        return;
    }

    std::cout << "  peso: " << peso_atual << "/" << peso_maximo << "kg"
              << " | slots: " << itens.size() << "/" << capacidade_maxima << "\n";

    if (arma_equipada)    std::cout << "  [equipada] ";
    if (armadura_equipada) std::cout << "  [equipada] ";

    std::cout << "\n  --- itens ---\n";
    for (int i = 0; i < static_cast<int>(itens.size()); i++) {
        std::cout << "  " << (i + 1) << ". ";
        itens[i]->exibirInfo();

        // indica o status de equipamento
        if (arma_equipada && *itens[i] == *arma_equipada) {
            std::cout << "     [equipado]\n";
        } else if (armadura_equipada && *itens[i] == *armadura_equipada) {
            std::cout << "     [equipado]\n";
        }
    }
}

Inventario& Inventario::operator+=(std::shared_ptr<Item> item) {
    adicionarItem(item);
    return *this;
}

std::string Inventario::serializar() const {
    std::ostringstream oss;
    oss << itens.size();
    for (const auto& item : itens) {
        oss << "\n" << item->serializar();
    }
    if (arma_equipada) {
        oss << "\nEQUIP_ARMA:" << arma_equipada->getNome();
    }
    if (armadura_equipada) {
        oss << "\nEQUIP_ARMADURA:" << armadura_equipada->getNome();
    }
    return oss.str();
}

} // namespace rpg
