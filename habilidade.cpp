#include "include/habilidade.h"
#include <iostream>
#include <algorithm>

namespace rpg {

// --- habilidade base ---

Habilidade::Habilidade(const std::string& nome, const std::string& desc,
                       int custo_mana, int cooldown, int nivel)
    : nome(nome), descricao(desc), custo_mana(custo_mana),
      cooldown_maximo(cooldown), cooldown_atual(0), nivel_habilidade(nivel) {}

std::string Habilidade::getNome() const { return nome; }
std::string Habilidade::getDescricao() const { return descricao; }
int Habilidade::getCustoMana() const { return custo_mana; }
int Habilidade::getCooldownAtual() const { return cooldown_atual; }
int Habilidade::getNivelHabilidade() const { return nivel_habilidade; }

bool Habilidade::estaDisponivel() const {
    return cooldown_atual == 0;
}

void Habilidade::reduzirCooldown() {
    if (cooldown_atual > 0) {
        cooldown_atual--;
    }
}

void Habilidade::exibirInfo() const {
    std::cout << "  [" << getTipo() << "] " << nome
              << " | custo: " << custo_mana << " mana"
              << " | cooldown: " << cooldown_maximo << " turnos";
    if (cooldown_atual > 0) {
        std::cout << " (disponivel em " << cooldown_atual << " turnos)";
    } else {
        std::cout << " (disponivel)";
    }
    std::cout << "\n    " << descricao << "\n";
}

// --- habilidade ofensiva ---

HabilidadeOfensiva::HabilidadeOfensiva(const std::string& nome, const std::string& desc,
                                       int custo_mana, int cooldown,
                                       float mult_dano, int nivel)
    : Habilidade(nome, desc, custo_mana, cooldown, nivel),
      multiplicador_dano(mult_dano) {}

int HabilidadeOfensiva::executar(int atributo_base) {
    // calcula o dano com base no atributo do personagem e no multiplicador
    int dano = static_cast<int>(atributo_base * multiplicador_dano);
    dano += nivel_habilidade * 5; // cada nivel da habilidade acrescenta 5 de dano
    cooldown_atual = cooldown_maximo;
    std::cout << "  habilidade '" << nome << "' causou " << dano << " de dano.\n";
    return dano;
}

std::string HabilidadeOfensiva::getTipo() const { return "ofensiva"; }

// --- habilidade defensiva ---

HabilidadeDefensiva::HabilidadeDefensiva(const std::string& nome, const std::string& desc,
                                         int custo_mana, int cooldown,
                                         float mult_defesa, int nivel)
    : Habilidade(nome, desc, custo_mana, cooldown, nivel),
      multiplicador_defesa(mult_defesa) {}

int HabilidadeDefensiva::executar(int atributo_base) {
    // retorna a quantidade de dano que sera absorvida neste turno
    int absorcao = static_cast<int>(atributo_base * multiplicador_defesa);
    absorcao += nivel_habilidade * 3;
    cooldown_atual = cooldown_maximo;
    std::cout << "  habilidade '" << nome << "' absorve " << absorcao << " de dano.\n";
    return absorcao;
}

std::string HabilidadeDefensiva::getTipo() const { return "defensiva"; }

// --- habilidade de suporte ---

HabilidadeSuporte::HabilidadeSuporte(const std::string& nome, const std::string& desc,
                                     int custo_mana, int cooldown,
                                     int cura, int nivel)
    : Habilidade(nome, desc, custo_mana, cooldown, nivel),
      quantidade_cura(cura) {}

int HabilidadeSuporte::executar(int atributo_base) {
    // a cura e calculada com base no atributo de sabedoria ou inteligencia
    int cura_total = quantidade_cura + static_cast<int>(atributo_base * 0.5f);
    cura_total += nivel_habilidade * 8;
    cooldown_atual = cooldown_maximo;
    std::cout << "  habilidade '" << nome << "' restaura " << cura_total << " de hp.\n";
    return cura_total;
}

std::string HabilidadeSuporte::getTipo() const { return "suporte"; }

} // namespace rpg
