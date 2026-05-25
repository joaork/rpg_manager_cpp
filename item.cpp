#include "include/item.h"
#include <stdexcept>
#include <sstream>

namespace rpg {

// --- item base ---

Item::Item(const std::string& nome, const std::string& desc, float peso,
           int valor, TipoItem tipo)
    : nome(nome), descricao(desc), peso(peso), valor(valor), tipo(tipo) {}

std::string Item::getNome() const { return nome; }
std::string Item::getDescricao() const { return descricao; }
float Item::getPeso() const { return peso; }
int Item::getValor() const { return valor; }
TipoItem Item::getTipo() const { return tipo; }

bool Item::operator==(const Item& outro) const {
    // dois itens sao iguais se tiverem o mesmo nome
    return nome == outro.nome;
}

std::string Item::operator+(const Item& outro) const {
    // concatena as descricoes dos dois itens
    return descricao + " + " + outro.descricao;
}

bool Item::operator<(const Item& outro) const {
    // ordena por valor monetario
    return valor < outro.valor;
}

std::ostream& operator<<(std::ostream& os, const Item& item) {
    os << "[" << item.nome << "] " << item.descricao
       << " | peso: " << item.peso << "kg | valor: " << item.valor << "g";
    return os;
}

void Item::exibirInfo() const {
    std::cout << *this << "\n";
}

// --- arma ---

Arma::Arma(const std::string& nome, const std::string& desc, float peso, int valor,
           int dano_base, const std::string& tipo_arma)
    : Item(nome, desc, peso, valor, TipoItem::ARMA),
      dano_base(dano_base), tipo_arma(tipo_arma) {}

int Arma::getDanoBase() const { return dano_base; }
std::string Arma::getTipoArma() const { return tipo_arma; }

void Arma::exibirInfo() const {
    std::cout << "[arma] " << nome << " (" << tipo_arma << ")"
              << " | dano: +" << dano_base
              << " | peso: " << peso << "kg"
              << " | valor: " << valor << "g\n";
}

std::string Arma::serializar() const {
    std::ostringstream oss;
    oss << "ARMA|" << nome << "|" << descricao << "|"
        << peso << "|" << valor << "|" << dano_base << "|" << tipo_arma;
    return oss.str();
}

// --- armadura ---

Armadura::Armadura(const std::string& nome, const std::string& desc, float peso, int valor,
                   int defesa, const std::string& tipo_armadura)
    : Item(nome, desc, peso, valor, TipoItem::ARMADURA),
      defesa(defesa), tipo_armadura(tipo_armadura) {}

int Armadura::getDefesa() const { return defesa; }
std::string Armadura::getTipoArmadura() const { return tipo_armadura; }

void Armadura::exibirInfo() const {
    std::cout << "[armadura] " << nome << " (" << tipo_armadura << ")"
              << " | defesa: +" << defesa
              << " | peso: " << peso << "kg"
              << " | valor: " << valor << "g\n";
}

std::string Armadura::serializar() const {
    std::ostringstream oss;
    oss << "ARMADURA|" << nome << "|" << descricao << "|"
        << peso << "|" << valor << "|" << defesa << "|" << tipo_armadura;
    return oss.str();
}

// --- pocao ---

Pocao::Pocao(const std::string& nome, const std::string& desc, float peso, int valor,
             int quantidade_cura, const std::string& tipo_pocao)
    : Item(nome, desc, peso, valor, TipoItem::POCAO),
      quantidade_cura(quantidade_cura), tipo_pocao(tipo_pocao) {}

int Pocao::getQuantidadeCura() const { return quantidade_cura; }
std::string Pocao::getTipoPocao() const { return tipo_pocao; }

void Pocao::exibirInfo() const {
    std::cout << "[pocao] " << nome << " (" << tipo_pocao << ")"
              << " | cura: " << quantidade_cura
              << " | peso: " << peso << "kg"
              << " | valor: " << valor << "g\n";
}

std::string Pocao::serializar() const {
    std::ostringstream oss;
    oss << "POCAO|" << nome << "|" << descricao << "|"
        << peso << "|" << valor << "|" << quantidade_cura << "|" << tipo_pocao;
    return oss.str();
}

// --- item especial ---

ItemEspecial::ItemEspecial(const std::string& nome, const std::string& desc, float peso,
                           int valor, const std::string& efeito, int poder)
    : Item(nome, desc, peso, valor, TipoItem::ITEM_ESPECIAL),
      efeito(efeito), poder(poder) {}

std::string ItemEspecial::getEfeito() const { return efeito; }
int ItemEspecial::getPoder() const { return poder; }

void ItemEspecial::exibirInfo() const {
    std::cout << "[especial] " << nome
              << " | efeito: " << efeito << " (poder " << poder << ")"
              << " | peso: " << peso << "kg"
              << " | valor: " << valor << "g\n";
}

std::string ItemEspecial::serializar() const {
    std::ostringstream oss;
    oss << "ESPECIAL|" << nome << "|" << descricao << "|"
        << peso << "|" << valor << "|" << efeito << "|" << poder;
    return oss.str();
}

// --- item factory ---

std::unique_ptr<Arma> ItemFactory::criarArma(const std::string& tipo) {
    if (tipo == "espada longa")
        return std::make_unique<Arma>("espada longa", "uma espada afiada de aco", 3.5f, 150, 25, "espada");
    if (tipo == "machado de batalha")
        return std::make_unique<Arma>("machado de batalha", "machado pesado de duas maos", 6.0f, 200, 35, "machado");
    if (tipo == "arco longo")
        return std::make_unique<Arma>("arco longo", "arco de madeira resistente de longa distancia", 1.5f, 120, 20, "arco");
    if (tipo == "adaga")
        return std::make_unique<Arma>("adaga", "faca curta para ataques rapidos", 0.5f, 80, 15, "adaga");
    if (tipo == "cajado magico")
        return std::make_unique<Arma>("cajado magico", "cajado que amplifica o poder magico", 2.0f, 180, 18, "cajado");
    if (tipo == "lanca")
        return std::make_unique<Arma>("lanca", "arma de haste com bom alcance", 2.5f, 130, 22, "lanca");

    // tipo generico se nao encontrar nos catalogos
    return std::make_unique<Arma>(tipo, "arma generica", 2.0f, 100, 10, "generica");
}

std::unique_ptr<Armadura> ItemFactory::criarArmadura(const std::string& tipo) {
    if (tipo == "cota de malha")
        return std::make_unique<Armadura>("cota de malha", "armadura de aneis metalicos entrelacados", 8.0f, 200, 15, "media");
    if (tipo == "armadura de placas")
        return std::make_unique<Armadura>("armadura de placas", "armadura pesada de placas de aco", 15.0f, 400, 30, "pesada");
    if (tipo == "couro curtido")
        return std::make_unique<Armadura>("couro curtido", "armadura leve de couro resistente", 3.0f, 100, 8, "leve");
    if (tipo == "manto arcano")
        return std::make_unique<Armadura>("manto arcano", "manto encantado que repele magias", 1.0f, 250, 10, "magica");
    if (tipo == "escudo de aco")
        return std::make_unique<Armadura>("escudo de aco", "escudo redondo de aco temperado", 5.0f, 150, 12, "escudo");

    return std::make_unique<Armadura>(tipo, "armadura generica", 5.0f, 100, 5, "generica");
}

std::unique_ptr<Pocao> ItemFactory::criarPocao(const std::string& tipo) {
    if (tipo == "pocao de cura menor")
        return std::make_unique<Pocao>("pocao de cura menor", "restaura uma quantidade pequena de hp", 0.3f, 30, 30, "hp");
    if (tipo == "pocao de cura maior")
        return std::make_unique<Pocao>("pocao de cura maior", "restaura uma quantidade maior de hp", 0.3f, 80, 100, "hp");
    if (tipo == "pocao de mana")
        return std::make_unique<Pocao>("pocao de mana", "restaura pontos de mana gastos", 0.3f, 60, 50, "mana");
    if (tipo == "elixir de vida")
        return std::make_unique<Pocao>("elixir de vida", "restaura hp ao maximo e remove penalidades", 0.5f, 200, 200, "hp");
    if (tipo == "antidoto")
        return std::make_unique<Pocao>("antidoto", "remove efeitos de veneno", 0.2f, 40, 0, "cura_status");

    return std::make_unique<Pocao>(tipo, "pocao generica", 0.3f, 30, 20, "hp");
}

std::unique_ptr<ItemEspecial> ItemFactory::criarItemEspecial(const std::string& tipo) {
    if (tipo == "amuleto de forca")
        return std::make_unique<ItemEspecial>("amuleto de forca", "aumenta a forca do portador", 0.1f, 500, "buff_forca", 10);
    if (tipo == "anel de sabedoria")
        return std::make_unique<ItemEspecial>("anel de sabedoria", "aumenta a inteligencia do portador", 0.1f, 500, "buff_inteligencia", 10);
    if (tipo == "botas de mercurio")
        return std::make_unique<ItemEspecial>("botas de mercurio", "aumentam drasticamente a agilidade", 1.0f, 400, "buff_agilidade", 15);
    if (tipo == "pedra filosofal")
        return std::make_unique<ItemEspecial>("pedra filosofal", "amplifica todos os atributos do portador", 0.5f, 2000, "buff_todos", 20);

    return std::make_unique<ItemEspecial>(tipo, "item especial misterioso", 0.5f, 300, "efeito_desconhecido", 5);
}

} // namespace rpg
