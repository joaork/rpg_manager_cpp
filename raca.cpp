#include "include/raca.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace rpg {

Raca::Raca(const std::string& nome, int b_hp, int b_for, int b_int, int b_agi, const std::string& desc)
    : nome(nome), bonus_hp(b_hp), bonus_forca(b_for),
      bonus_inteligencia(b_int), bonus_agilidade(b_agi), descricao(desc) {}

std::string Raca::getNome() const { return nome; }
int Raca::getBonusHp() const { return bonus_hp; }
int Raca::getBonusForca() const { return bonus_forca; }
int Raca::getBonusInteligencia() const { return bonus_inteligencia; }
int Raca::getBonusAgilidade() const { return bonus_agilidade; }
std::string Raca::getDescricao() const { return descricao; }

void Raca::exibirInfo() const {
    std::cout << "raca: " << nome << "\n";
    std::cout << "  " << descricao << "\n";
    std::cout << "  bonus hp: +" << bonus_hp
              << " | forca: +" << bonus_forca
              << " | inteligencia: +" << bonus_inteligencia
              << " | agilidade: +" << bonus_agilidade << "\n";
}

// humanos sao genericos e equilibrados em todos os atributos
Humano::Humano() : Raca("humano", 20, 5, 5, 5,
    "equilibrado em todos os atributos sem grandes pontos fracos") {}

// elfos ganham bastante em inteligencia e agilidade, mas perdem em hp e forca
Elfo::Elfo() : Raca("elfo", 0, 0, 15, 10,
    "inteligentes e ageis, preferem magia e arcos a combate corpo a corpo") {}

// anoes sao tanques naturais com muita forca e resistencia
Anao::Anao() : Raca("anao", 40, 10, 0, -5,
    "resistentes e fortes, mas um pouco lentos para reagir") {}

// orcs tem forca bruta acima de qualquer outra raca
Orc::Orc() : Raca("orc", 30, 20, -10, 0,
    "guerreiros natos com forca brutal, mas limitados em magia") {}

// hobbits sao pequenos e rapidos, otimos para ladroes e arqueiros
Hobbit::Hobbit() : Raca("hobbit", -10, 0, 5, 20,
    "pequenos e furtivos, com alta agilidade para fuga e golpes criticos") {}

// cria a raca correta a partir de um nome em string
std::unique_ptr<Raca> criarRaca(const std::string& nome) {
    std::string nome_lower = nome;
    std::transform(nome_lower.begin(), nome_lower.end(), nome_lower.begin(), ::tolower);

    if (nome_lower == "humano")  return std::make_unique<Humano>();
    if (nome_lower == "elfo")    return std::make_unique<Elfo>();
    if (nome_lower == "anao")    return std::make_unique<Anao>();
    if (nome_lower == "orc")     return std::make_unique<Orc>();
    if (nome_lower == "hobbit")  return std::make_unique<Hobbit>();

    throw std::invalid_argument("raca desconhecida: " + nome);
}

} // namespace rpg
