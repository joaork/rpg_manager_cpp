#include "include/crafting.h"
#include <iostream>
#include <algorithm>

namespace rpg {

// --- receita ---

Receita::Receita(const std::string& id, const std::string& nome,
                 const std::string& resultado, const std::string& tipo_resultado)
    : id(id), nome(nome), item_resultado(resultado), tipo_resultado(tipo_resultado) {}

std::string Receita::getId() const { return id; }
std::string Receita::getNome() const { return nome; }
std::string Receita::getItemResultado() const { return item_resultado; }
std::string Receita::getTipoResultado() const { return tipo_resultado; }
const std::vector<Ingrediente>& Receita::getIngredientes() const { return ingredientes; }

void Receita::adicionarIngrediente(const std::string& nome, int quantidade) {
    ingredientes.emplace_back(nome, quantidade);
}

bool Receita::podeSerFeita(const Inventario& inv) const {
    // verifica se todos os ingredientes estao disponiveis no inventario
    for (const auto& ing : ingredientes) {
        if (!inv.buscarItem(ing.nome_item)) {
            return false;
        }
    }
    return true;
}

void Receita::exibirInfo() const {
    std::cout << "  [receita] " << nome << " -> " << item_resultado << "\n";
    std::cout << "    ingredientes: ";
    for (int i = 0; i < static_cast<int>(ingredientes.size()); i++) {
        if (i > 0) std::cout << " + ";
        std::cout << ingredientes[i].quantidade << "x " << ingredientes[i].nome_item;
    }
    std::cout << "\n";
}

// --- sistema de crafting ---

void SistemaCrafting::registrarReceita(std::shared_ptr<Receita> receita) {
    receitas[receita->getId()] = receita;
}

bool SistemaCrafting::fabricar(const std::string& id_receita, Inventario& inventario) {
    auto it = receitas.find(id_receita);
    if (it == receitas.end()) {
        std::cout << "receita '" << id_receita << "' nao encontrada.\n";
        return false;
    }

    auto& receita = it->second;

    if (!receita->podeSerFeita(inventario)) {
        std::cout << "ingredientes insuficientes para fabricar " << receita->getNome() << ".\n";
        return false;
    }

    // remove os ingredientes do inventario
    for (const auto& ing : receita->getIngredientes()) {
        inventario.removerItem(ing.nome_item);
    }

    // cria o item resultante e adiciona ao inventario
    std::string tipo = receita->getTipoResultado();
    std::string resultado = receita->getItemResultado();

    if (tipo == "arma") {
        inventario.adicionarItem(ItemFactory::criarArma(resultado));
    } else if (tipo == "armadura") {
        inventario.adicionarItem(ItemFactory::criarArmadura(resultado));
    } else if (tipo == "pocao") {
        inventario.adicionarItem(ItemFactory::criarPocao(resultado));
    } else if (tipo == "especial") {
        inventario.adicionarItem(ItemFactory::criarItemEspecial(resultado));
    }

    std::cout << "item fabricado com sucesso: " << resultado << "\n";
    return true;
}

void SistemaCrafting::listarReceitas() const {
    if (receitas.empty()) {
        std::cout << "  nenhuma receita conhecida.\n";
        return;
    }
    std::cout << "\n--- receitas de crafting ---\n";
    for (const auto& [id, r] : receitas) {
        r->exibirInfo();
    }
}

void SistemaCrafting::listarReceitasDisponiveis(const Inventario& inventario) const {
    std::cout << "\n--- receitas disponiveis com seu inventario ---\n";
    bool alguma = false;
    for (const auto& [id, r] : receitas) {
        if (r->podeSerFeita(inventario)) {
            r->exibirInfo();
            alguma = true;
        }
    }
    if (!alguma) {
        std::cout << "  nenhuma receita pode ser fabricada com os itens atuais.\n";
    }
}

std::shared_ptr<Receita> SistemaCrafting::getReceita(const std::string& id) const {
    auto it = receitas.find(id);
    return (it != receitas.end()) ? it->second : nullptr;
}

void SistemaCrafting::carregarReceitasPadrao() {
    // combina duas pocoes menores para criar uma maior
    auto r1 = std::make_shared<Receita>(
        "r001", "pocao de cura maior", "pocao de cura maior", "pocao");
    r1->adicionarIngrediente("pocao de cura menor", 2);
    registrarReceita(r1);

    // forja uma adaga com componentes basicos
    auto r2 = std::make_shared<Receita>(
        "r002", "forja de adaga", "adaga", "arma");
    r2->adicionarIngrediente("pocao de cura menor", 1); // material como placeholder
    registrarReceita(r2);

    // combina armadura de couro com pocao para criar cota de malha
    auto r3 = std::make_shared<Receita>(
        "r003", "cota de malha reforçada", "cota de malha", "armadura");
    r3->adicionarIngrediente("couro curtido", 1);
    r3->adicionarIngrediente("pocao de mana", 1);
    registrarReceita(r3);

    // receita de elixir de vida
    auto r4 = std::make_shared<Receita>(
        "r004", "elixir de vida", "elixir de vida", "pocao");
    r4->adicionarIngrediente("pocao de cura maior", 1);
    r4->adicionarIngrediente("pocao de mana", 1);
    registrarReceita(r4);
}

} // namespace rpg
