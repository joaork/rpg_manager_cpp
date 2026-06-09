#include "include/mapa.h"
#include <iostream>
#include <random>
#include <algorithm>

namespace rpg {

static std::mt19937 rng_mapa(std::random_device{}());

// --- sala ---

Sala::Sala(int id, const std::string& nome, const std::string& desc, TipoSala tipo)
    : id(id), nome(nome), descricao(desc), tipo(tipo),
      visitada(false), limpa(tipo != TipoSala::MONSTRO && tipo != TipoSala::CHEFE) {}

int Sala::getId() const { return id; }
std::string Sala::getNome() const { return nome; }
std::string Sala::getDescricao() const { return descricao; }
TipoSala Sala::getTipo() const { return tipo; }
bool Sala::foiVisitada() const { return visitada; }
bool Sala::estaLimpa() const { return limpa; }

void Sala::adicionarSaida(int id_sala) { saidas.push_back(id_sala); }
void Sala::adicionarInimigo(const InimigoSala& inimigo) { inimigos.push_back(inimigo); }
void Sala::adicionarTesouro(const Tesouro& tesouro) { tesouros.push_back(tesouro); }

const std::vector<int>& Sala::getSaidas() const { return saidas; }
std::vector<InimigoSala>& Sala::getInimigos() { return inimigos; }
std::vector<Tesouro>& Sala::getTesouros() { return tesouros; }

void Sala::marcarVisitada() { visitada = true; }
void Sala::marcarLimpa() { limpa = true; }

void Sala::exibirInfo() const {
    std::cout << "\n[sala " << id << "] " << nome << "\n";
    std::cout << "  " << descricao << "\n";

    std::string tipo_str;
    switch (tipo) {
        case TipoSala::VAZIA:    tipo_str = "vazia"; break;
        case TipoSala::MONSTRO:  tipo_str = "monstro"; break;
        case TipoSala::TESOURO:  tipo_str = "tesouro"; break;
        case TipoSala::LOJA:     tipo_str = "loja"; break;
        case TipoSala::CHEFE:    tipo_str = "chefe"; break;
        case TipoSala::SAIDA:    tipo_str = "saida"; break;
    }
    std::cout << "  tipo: " << tipo_str;
    if (visitada) std::cout << " | [visitada]";
    if (limpa) std::cout << " | [limpa]";
    std::cout << "\n";

    if (!saidas.empty()) {
        std::cout << "  saidas para: ";
        for (int i = 0; i < static_cast<int>(saidas.size()); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << "sala " << saidas[i];
        }
        std::cout << "\n";
    }

    // exibe inimigos ativos na sala
    bool tem_inimigo_vivo = false;
    for (const auto& ini : inimigos) {
        if (!ini.derrotado) {
            if (!tem_inimigo_vivo) {
                std::cout << "  inimigos:\n";
                tem_inimigo_vivo = true;
            }
            std::cout << "    - " << ini.nome << " (nivel " << ini.nivel
                      << ") hp: " << ini.hp << "\n";
        }
    }

    // exibe tesouros nao coletados
    bool tem_tesouro = false;
    for (const auto& t : tesouros) {
        if (!t.coletado) {
            if (!tem_tesouro) {
                std::cout << "  tesouros:\n";
                tem_tesouro = true;
            }
            if (!t.nome_item.empty()) {
                std::cout << "    - " << t.nome_item << "\n";
            }
            if (t.ouro > 0) {
                std::cout << "    - " << t.ouro << " de ouro\n";
            }
        }
    }
}

// --- mapa ---

Mapa::Mapa() : sala_atual_id(0), proximo_id(0), nivel_dificuldade(1) {}

TipoSala Mapa::sorteiarTipoSala() {
    std::uniform_int_distribution<int> dist(0, 9);
    int sorteio = dist(rng_mapa);

    // distribuicao de probabilidade dos tipos de sala
    if (sorteio <= 4) return TipoSala::MONSTRO;  // 50%
    if (sorteio <= 6) return TipoSala::VAZIA;     // 20%
    if (sorteio <= 7) return TipoSala::TESOURO;   // 10%
    if (sorteio <= 8) return TipoSala::LOJA;      // 10%
    return TipoSala::VAZIA;                        // 10%
}

InimigoSala Mapa::gerarInimigo(int nivel_sala) {
    // lista de tipos de inimigos disponiveis
    static const std::vector<std::pair<std::string, std::string>> tipos_inimigos = {
        {"goblin feroz", "ladrao"},
        {"orc guardiao", "guerreiro"},
        {"esqueleto arcano", "mago"},
        {"bandido sombrio", "ladrao"},
        {"troll da montanha", "guerreiro"},
        {"bruxa maldita", "mago"},
        {"lobisomem", "guerreiro"},
        {"vampiro ancioe", "mago"}
    };

    std::uniform_int_distribution<int> dist(0, tipos_inimigos.size() - 1);
    auto& tipo = tipos_inimigos[dist(rng_mapa)];

    int hp = 50 + nivel_sala * 20 + nivel_dificuldade * 10;
    int dano = 10 + nivel_sala * 5 + nivel_dificuldade * 3;
    int xp = 30 + nivel_sala * 15;
    int ouro = 10 + nivel_sala * 8;

    return InimigoSala(tipo.first, tipo.second, nivel_sala, hp, dano, xp, ouro);
}

Tesouro Mapa::gerarTesouro(int nivel_sala) {
    // lista de itens possiveis em tesouros
    static const std::vector<std::pair<std::string, std::string>> itens_tesouro = {
        {"pocao de cura menor", "pocao"},
        {"pocao de cura maior", "pocao"},
        {"pocao de mana", "pocao"},
        {"adaga", "arma"},
        {"couro curtido", "armadura"},
        {"amuleto de forca", "especial"}
    };

    std::uniform_int_distribution<int> dist(0, itens_tesouro.size() - 1);
    auto& item = itens_tesouro[dist(rng_mapa)];

    std::uniform_int_distribution<int> ouro_dist(10, 30 + nivel_sala * 10);
    int ouro = ouro_dist(rng_mapa);

    return Tesouro(item.first, item.second, ouro);
}

std::string Mapa::gerarNomeSala(TipoSala tipo, int indice) {
    switch (tipo) {
        case TipoSala::VAZIA:
            return "corredor " + std::to_string(indice);
        case TipoSala::MONSTRO:
            return "covil dos inimigos " + std::to_string(indice);
        case TipoSala::TESOURO:
            return "camara do tesouro " + std::to_string(indice);
        case TipoSala::LOJA:
            return "loja do andarilho " + std::to_string(indice);
        case TipoSala::CHEFE:
            return "sala do guardiao";
        case TipoSala::SAIDA:
            return "saida do dungeon";
        default:
            return "sala " + std::to_string(indice);
    }
}

void Mapa::gerarAndar(int nivel) {
    nivel_dificuldade = nivel;
    salas.clear();
    proximo_id = 0;

    // cria a sala inicial sempre como sala vazia
    auto sala_inicial = std::make_shared<Sala>(
        proximo_id++, "entrada do dungeon",
        "a entrada principal do dungeon.", TipoSala::VAZIA);
    salas[sala_inicial->getId()] = sala_inicial;
    sala_atual_id = sala_inicial->getId();

    // gera entre 8 e 12 salas de conteudo
    std::uniform_int_distribution<int> qtd_salas(8, 12);
    int num_salas = qtd_salas(rng_mapa);

    for (int i = 0; i < num_salas; i++) {
        TipoSala tipo = sorteiarTipoSala();
        auto sala = std::make_shared<Sala>(
            proximo_id, gerarNomeSala(tipo, i + 1),
            "uma sala com caracteristicas proprias do dungeon.", tipo);

        // adiciona inimigos nas salas de monstro
        if (tipo == TipoSala::MONSTRO) {
            std::uniform_int_distribution<int> qtd_inimigos(1, 2);
            for (int j = 0; j < qtd_inimigos(rng_mapa); j++) {
                sala->adicionarInimigo(gerarInimigo(nivel));
            }
        }

        // adiciona tesouros nas salas de tesouro
        if (tipo == TipoSala::TESOURO) {
            sala->adicionarTesouro(gerarTesouro(nivel));
        }

        // conecta a sala com a anterior
        int id_anterior = proximo_id - 1;
        sala->adicionarSaida(id_anterior);
        salas[id_anterior]->adicionarSaida(proximo_id);

        salas[proximo_id] = sala;
        proximo_id++;
    }

    // sala do chefe sempre no final
    auto sala_chefe = std::make_shared<Sala>(
        proximo_id, "sala do guardiao",
        "uma sala enorme com pilares de pedra. no centro, um ser poderoso aguarda.", TipoSala::CHEFE);

    // o chefe e significativamente mais forte que os inimigos normais
    InimigoSala chefe("guardiao das sombras", "guerreiro", nivel + 2,
                      200 + nivel * 50, 30 + nivel * 10, 300, 200);
    sala_chefe->adicionarInimigo(chefe);
    sala_chefe->adicionarSaida(proximo_id - 1);
    salas[proximo_id - 1]->adicionarSaida(proximo_id);
    salas[proximo_id] = sala_chefe;
    proximo_id++;

    // sala de saida logo apos o chefe
    auto sala_saida = std::make_shared<Sala>(
        proximo_id, "saida do dungeon",
        "uma escada de pedra que leva de volta ao mundo de cima.", TipoSala::SAIDA);
    sala_saida->adicionarSaida(proximo_id - 1);
    salas[proximo_id - 1]->adicionarSaida(proximo_id);
    salas[proximo_id] = sala_saida;

    std::cout << "dungeon gerado com " << salas.size() << " salas.\n";
}

std::shared_ptr<Sala> Mapa::getSalaAtual() const {
    auto it = salas.find(sala_atual_id);
    return (it != salas.end()) ? it->second : nullptr;
}

std::shared_ptr<Sala> Mapa::getSala(int id) const {
    auto it = salas.find(id);
    return (it != salas.end()) ? it->second : nullptr;
}

bool Mapa::moverPara(int id_sala) {
    auto sala_atual = getSalaAtual();
    if (!sala_atual) return false;

    const auto& saidas = sala_atual->getSaidas();
    bool pode_mover = std::find(saidas.begin(), saidas.end(), id_sala) != saidas.end();

    if (!pode_mover) {
        std::cout << "nao e possivel ir para a sala " << id_sala
                  << " a partir daqui.\n";
        return false;
    }

    sala_atual_id = id_sala;
    auto nova_sala = getSalaAtual();
    if (nova_sala) {
        nova_sala->marcarVisitada();
        std::cout << "voce se move para: " << nova_sala->getNome() << "\n";
        nova_sala->exibirInfo();
    }
    return true;
}

void Mapa::exibirMapa() const {
    std::cout << "\n--- mapa do dungeon ---\n";
    for (const auto& [id, sala] : salas) {
        std::string icone = " ";
        if (id == sala_atual_id) icone = "*";
        else if (sala->foiVisitada()) icone = "v";

        std::cout << "  [" << icone << "] sala " << id
                  << ": " << sala->getNome();
        if (!sala->estaLimpa()) std::cout << " [!]";
        std::cout << "\n";
    }
    std::cout << "  * = posicao atual | v = visitada | [!] = inimigos\n";
}

void Mapa::exibirSalaAtual() const {
    auto sala = getSalaAtual();
    if (sala) {
        sala->exibirInfo();
    }
}

int Mapa::getNivelDificuldade() const { return nivel_dificuldade; }
void Mapa::setNivelDificuldade(int nivel) { nivel_dificuldade = std::max(1, nivel); }

} // namespace rpg
