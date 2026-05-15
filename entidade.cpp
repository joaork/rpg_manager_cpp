#include "include/entidade.h"
#include <stdexcept>
#include <algorithm>

namespace rpg {

Entidade::Entidade(const std::string& nome, int hp_maximo, int nivel)
    : hp_maximo(hp_maximo), nivel(nivel) {
    setNome(nome);
    setHpMaximo(hp_maximo);
    this->hp = hp_maximo;
    this->nivel = nivel;
}

std::string Entidade::getNome() const { return nome; }
int Entidade::getHp() const { return hp; }
int Entidade::getHpMaximo() const { return hp_maximo; }
int Entidade::getNivel() const { return nivel; }

void Entidade::setNome(const std::string& valor) {
    // nao permite nomes vazios ou apenas espacos
    if (valor.empty() || valor.find_first_not_of(' ') == std::string::npos) {
        throw std::invalid_argument("nome nao pode ser vazio");
    }
    nome = valor;
}

void Entidade::setHp(int valor) {
    // hp nao pode ser negativo nem ultrapassar o maximo
    hp = std::max(0, std::min(valor, hp_maximo));
}

void Entidade::setHpMaximo(int valor) {
    if (valor <= 0) {
        throw std::invalid_argument("hp maximo precisa ser positivo");
    }
    hp_maximo = valor;
}

void Entidade::setNivel(int valor) {
    if (valor < 1) {
        throw std::invalid_argument("nivel minimo e 1");
    }
    nivel = valor;
}

bool Entidade::estaVivo() const {
    return hp > 0;
}

void Entidade::receberDano(int quantidade) {
    if (quantidade < 0) {
        throw std::invalid_argument("dano nao pode ser negativo");
    }
    setHp(hp - quantidade);

    if (!estaVivo()) {
        std::cout << nome << " foi derrotado.\n";
    } else {
        std::cout << nome << " recebeu " << quantidade << " de dano. hp: "
                  << hp << "/" << hp_maximo << "\n";
    }
}

void Entidade::curar(int quantidade) {
    if (quantidade <= 0) {
        throw std::invalid_argument("quantidade de cura precisa ser positiva");
    }
    if (!estaVivo()) {
        std::cout << nome << " esta caido e nao pode ser curado.\n";
        return;
    }
    setHp(hp + quantidade);
    std::cout << nome << " recuperou " << quantidade << " de hp. hp: "
              << hp << "/" << hp_maximo << "\n";
}

std::ostream& operator<<(std::ostream& os, const Entidade& e) {
    os << e.nome << " | nivel " << e.nivel << " | hp: " << e.hp << "/" << e.hp_maximo;
    return os;
}

} // namespace rpg
