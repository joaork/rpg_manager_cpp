#include "include/personagem.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <random>
#include <stdexcept>

namespace rpg {

// gerador de numeros aleatorios compartilhado
static std::mt19937 rng(std::random_device{}());

// --- personagem base ---

Personagem::Personagem(const std::string& nome, std::unique_ptr<Raca> raca,
                       const std::string& classe, int hp_max, int mana_max, int nivel)
    : Entidade(nome, hp_max, nivel),
      raca(std::move(raca)),
      classe_rpg(classe),
      xp(0),
      xp_para_proximo_nivel(100),
      mana(mana_max),
      mana_maxima(mana_max) {
    aplicarBonusRaca();
}

Personagem::~Personagem() {
    std::cout << "personagem " << getNome() << " removido da memoria.\n";
}

void Personagem::aplicarBonusRaca() {
    if (!raca) return;
    // soma os bonus da raca nos atributos base do personagem
    setHpMaximo(getHpMaximo() + raca->getBonusHp());
    setHp(getHpMaximo());
    mana_maxima += raca->getBonusInteligencia();
    mana = mana_maxima;
}

void Personagem::subirNivel() {
    setNivel(getNivel() + 1);
    int novo_hp = getHpMaximo() + 20;
    setHpMaximo(novo_hp);
    setHp(novo_hp); // restaura o hp ao subir de nivel
    mana_maxima += 10;
    mana = mana_maxima;

    xp_para_proximo_nivel = static_cast<int>(xp_para_proximo_nivel * 1.5);

    // publica o evento de subida de nivel para os observers
    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::NIVEL_SUBIU, getNome(),
               "subiu de nivel", getNivel())
    );
}

std::string Personagem::getRaca() const {
    return raca ? raca->getNome() : "sem raca";
}
std::string Personagem::getClasse() const { return classe_rpg; }
int Personagem::getXp() const { return xp; }
int Personagem::getXpProximoNivel() const { return xp_para_proximo_nivel; }
int Personagem::getMana() const { return mana; }
int Personagem::getManaMaxima() const { return mana_maxima; }
Inventario& Personagem::getInventario() { return inventario; }
const Inventario& Personagem::getInventario() const { return inventario; }

void Personagem::ganharXp(int quantidade) {
    if (quantidade <= 0) return;
    xp += quantidade;
    std::cout << getNome() << " ganhou " << quantidade << " de xp. total: "
              << xp << "/" << xp_para_proximo_nivel << "\n";

    // sobe de nivel repetidamente enquanto houver xp suficiente
    while (xp >= xp_para_proximo_nivel) {
        xp -= xp_para_proximo_nivel;
        subirNivel();
    }
}

int Personagem::usarHabilidade(int indice, int atributo_base) {
    if (indice < 0 || indice >= static_cast<int>(habilidades.size())) {
        std::cout << "habilidade invalida.\n";
        return 0;
    }

    auto& hab = habilidades[indice];

    if (!hab->estaDisponivel()) {
        std::cout << "habilidade '" << hab->getNome() << "' em cooldown por mais "
                  << hab->getCooldownAtual() << " turno(s).\n";
        return 0;
    }

    if (mana < hab->getCustoMana()) {
        std::cout << "mana insuficiente para usar '" << hab->getNome()
                  << "'. custo: " << hab->getCustoMana()
                  << " | mana atual: " << mana << "\n";
        return 0;
    }

    mana -= hab->getCustoMana();

    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::HABILIDADE_USADA, getNome(), hab->getNome())
    );

    return hab->executar(atributo_base);
}

void Personagem::adicionarHabilidade(std::shared_ptr<Habilidade> habilidade) {
    habilidades.push_back(habilidade);
}

void Personagem::listarHabilidades() const {
    if (habilidades.empty()) {
        std::cout << "  nenhuma habilidade disponivel.\n";
        return;
    }
    for (int i = 0; i < static_cast<int>(habilidades.size()); i++) {
        std::cout << "  " << (i + 1) << ". ";
        habilidades[i]->exibirInfo();
    }
}

void Personagem::atualizarCooldowns() {
    for (auto& hab : habilidades) {
        hab->reduzirCooldown();
    }
}

bool Personagem::operator<(const Personagem& outro) const {
    return getNivel() < outro.getNivel();
}

bool Personagem::operator==(const Personagem& outro) const {
    return getNome() == outro.getNome() && classe_rpg == outro.classe_rpg;
}

void Personagem::exibirStatus() const {
    std::cout << "\n=== " << getNome() << " ===\n";
    std::cout << "  classe: " << classe_rpg;
    if (raca) std::cout << " | raca: " << raca->getNome();
    std::cout << " | nivel: " << getNivel() << "\n";
    std::cout << "  hp: " << getHp() << "/" << getHpMaximo()
              << " | mana: " << mana << "/" << mana_maxima
              << " | xp: " << xp << "/" << xp_para_proximo_nivel << "\n";
}

std::string Personagem::serializar() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"nome\": \"" << getNome() << "\",\n";
    oss << "  \"classe\": \"" << classe_rpg << "\",\n";
    oss << "  \"raca\": \"" << getRaca() << "\",\n";
    oss << "  \"nivel\": " << getNivel() << ",\n";
    oss << "  \"hp\": " << getHp() << ",\n";
    oss << "  \"hp_maximo\": " << getHpMaximo() << ",\n";
    oss << "  \"mana\": " << mana << ",\n";
    oss << "  \"mana_maxima\": " << mana_maxima << ",\n";
    oss << "  \"xp\": " << xp << "\n";
    oss << "}";
    return oss.str();
}

// --- guerreiro ---

Guerreiro::Guerreiro(const std::string& nome, std::unique_ptr<Raca> raca, int nivel)
    : Personagem(nome, std::move(raca), "guerreiro", 150 + (nivel - 1) * 20, 30, nivel),
      forca(20 + (nivel - 1) * 3), defesa(10 + (nivel - 1) * 2), modo_berserk(false) {

    // guerreiros conhecem golpe poderoso e postura defensiva desde o inicio
    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "golpe poderoso", "ataque fisico devastador que ignora parte da defesa",
        15, 3, 2.5f));
    adicionarHabilidade(std::make_shared<HabilidadeDefensiva>(
        "postura defensiva", "assume postura de defesa absorvendo o proximo dano",
        10, 4, 1.5f));
    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "frenesi", "serie de golpes rapidos causando dano multiplas vezes",
        25, 5, 3.0f));
}

int Guerreiro::getForca() const { return forca; }
int Guerreiro::getDefesa() const { return defesa; }

int Guerreiro::atacar() {
    int dano_base = forca;

    // berserk ativa quando o hp cai abaixo de 30% e aumenta o dano em 50%
    modo_berserk = (getHp() < getHpMaximo() * 0.3);
    if (modo_berserk) {
        dano_base = static_cast<int>(dano_base * 1.5);
        std::cout << getNome() << " entrou em berserk! ";
    }

    // a arma equipada adiciona seu dano base ao ataque
    auto arma = inventario.getArmaEquipada();
    if (arma) {
        dano_base += arma->getDanoBase();
    }

    std::cout << getNome() << " golpeou com forca causando " << dano_base << " de dano.\n";

    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::DANO_CAUSADO, getNome(), "ataque fisico", dano_base));

    return dano_base;
}

void Guerreiro::receberDanoComDefesa(int dano) {
    // a defesa do guerreiro e a defesa da armadura reduzem o dano
    int defesa_total = defesa;
    auto armadura = inventario.getArmaduraEquipada();
    if (armadura) {
        defesa_total += armadura->getDefesa();
    }

    int dano_real = std::max(1, dano - defesa_total);
    receberDano(dano_real);
}

void Guerreiro::exibirStatus() const {
    Personagem::exibirStatus();
    std::cout << "  forca: " << forca << " | defesa: " << defesa;
    if (modo_berserk) std::cout << " | [modo berserk ativo]";
    std::cout << "\n";
}

std::string Guerreiro::serializar() const {
    std::ostringstream oss;
    oss << Personagem::serializar();
    // adiciona atributos especificos do guerreiro
    oss << "\n\"forca\": " << forca << ", \"defesa\": " << defesa;
    return oss.str();
}

// --- mago ---

Mago::Mago(const std::string& nome, std::unique_ptr<Raca> raca, int nivel)
    : Personagem(nome, std::move(raca), "mago", 80 + (nivel - 1) * 10, 120, nivel),
      inteligencia(30 + (nivel - 1) * 4), poder_magico(25 + (nivel - 1) * 3) {

    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "bola de fogo", "lanca uma esfera de fogo que explode no alvo",
        20, 2, 2.8f));
    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "raio de gelo", "lanca um feixe de gelo que tambem retarda o alvo",
        25, 3, 3.2f));
    adicionarHabilidade(std::make_shared<HabilidadeSuporte>(
        "escudo magico", "cria uma barreira de energia que absorve dano",
        30, 4, 40));
    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "meteoro", "invoca um meteoro que causa dano massivo em area",
        50, 6, 5.0f));
}

int Mago::getInteligencia() const { return inteligencia; }
int Mago::getPodMagico() const { return poder_magico; }

int Mago::atacar() {
    // mago precisa de mana para atacar com eficiencia
    if (getMana() < 10) {
        int dano_minimo = 5;
        std::cout << getNome() << " esta sem mana e usou um ataque basico de "
                  << dano_minimo << " de dano.\n";
        return dano_minimo;
    }

    // consome mana para cada ataque magico basico
    //int mana_atual = getMana();
    // reduz mana pelo metodo de acesso (via mana publico nao e possivel fora da classe)
    // aqui calculamos direto
    int dano = static_cast<int>(inteligencia * 2.0);

    auto arma = inventario.getArmaEquipada();
    if (arma) {
        dano += arma->getDanoBase(); // cajado amplifica o dano magico
    }

    std::cout << getNome() << " lancou uma magia causando " << dano << " de dano.\n";

    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::DANO_CAUSADO, getNome(), "magia", dano));

    return dano;
}

void Mago::exibirStatus() const {
    Personagem::exibirStatus();
    std::cout << "  inteligencia: " << inteligencia
              << " | poder magico: " << poder_magico << "\n";
}

std::string Mago::serializar() const {
    std::ostringstream oss;
    oss << Personagem::serializar();
    oss << "\n\"inteligencia\": " << inteligencia << ", \"poder_magico\": " << poder_magico;
    return oss.str();
}

// --- arqueiro ---

Arqueiro::Arqueiro(const std::string& nome, std::unique_ptr<Raca> raca, int nivel)
    : Personagem(nome, std::move(raca), "arqueiro", 110 + (nivel - 1) * 15, 50, nivel),
      agilidade(22 + (nivel - 1) * 3), precisao(18 + (nivel - 1) * 2),
      chance_critico(0.25f + (nivel - 1) * 0.02f) {

    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "chuva de flechas", "dispara multiplas flechas em area",
        20, 3, 2.0f));
    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "tiro certeiro", "mira com precisao causando dano elevado",
        15, 2, 2.5f));
    adicionarHabilidade(std::make_shared<HabilidadeDefensiva>(
        "esquiva", "aumenta a mobilidade para evitar o proximo ataque",
        10, 3, 0.8f));
}

int Arqueiro::getAgilidade() const { return agilidade; }
float Arqueiro::getChanceCritico() const { return chance_critico; }

int Arqueiro::atacar() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    bool critico = dist(rng) <= chance_critico;

    int dano;
    if (critico) {
        dano = static_cast<int>(agilidade * 2.5 + precisao);
        std::cout << "acerto critico! " << getNome() << " disparou uma flecha certeira de "
                  << dano << " de dano.\n";
    } else {
        dano = static_cast<int>(agilidade + precisao * 0.5);
        std::cout << getNome() << " disparou uma flecha causando " << dano << " de dano.\n";
    }

    auto arma = inventario.getArmaEquipada();
    if (arma) {
        dano += arma->getDanoBase();
    }

    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::DANO_CAUSADO, getNome(), critico ? "critico" : "normal", dano));

    return dano;
}

void Arqueiro::exibirStatus() const {
    Personagem::exibirStatus();
    std::cout << "  agilidade: " << agilidade
              << " | precisao: " << precisao
              << " | chance critico: " << static_cast<int>(chance_critico * 100) << "%\n";
}

std::string Arqueiro::serializar() const {
    std::ostringstream oss;
    oss << Personagem::serializar();
    oss << "\n\"agilidade\": " << agilidade << ", \"precisao\": " << precisao;
    return oss.str();
}

// --- ladrao ---

Ladrao::Ladrao(const std::string& nome, std::unique_ptr<Raca> raca, int nivel)
    : Personagem(nome, std::move(raca), "ladrao", 100 + (nivel - 1) * 12, 60, nivel),
      agilidade(25 + (nivel - 1) * 4), chance_critico(0.35f + (nivel - 1) * 0.03f),
      furtivo(false) {

    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "golpe furtivo", "ataque surpresa a partir da sombra causando dano critico",
        20, 3, 4.0f));
    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "veneno", "envenena o alvo causando dano ao longo do tempo",
        15, 4, 1.5f));
    adicionarHabilidade(std::make_shared<HabilidadeDefensiva>(
        "sombras", "funde-se com as sombras reduzindo o dano recebido",
        20, 3, 1.2f));
}

int Ladrao::getAgilidade() const { return agilidade; }
float Ladrao::getChanceCritico() const { return chance_critico; }
bool Ladrao::isFurtivo() const { return furtivo; }

void Ladrao::ativarFurtividade() {
    furtivo = true;
    std::cout << getNome() << " entrou no modo furtivo. proximo ataque sera critico.\n";
}

int Ladrao::atacar() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    bool critico = furtivo || dist(rng) <= chance_critico;

    int dano;
    if (critico) {
        dano = static_cast<int>(agilidade * 3.0);
        if (furtivo) {
            std::cout << "golpe pelas costas! ";
            furtivo = false;
        } else {
            std::cout << "acerto critico! ";
        }
        std::cout << getNome() << " causou " << dano << " de dano.\n";
    } else {
        dano = agilidade;
        std::cout << getNome() << " realizou um ataque rapido de " << dano << " de dano.\n";
    }

    auto arma = inventario.getArmaEquipada();
    if (arma) {
        dano += arma->getDanoBase();
    }

    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::DANO_CAUSADO, getNome(), critico ? "critico" : "normal", dano));

    return dano;
}

void Ladrao::exibirStatus() const {
    Personagem::exibirStatus();
    std::cout << "  agilidade: " << agilidade
              << " | chance critico: " << static_cast<int>(chance_critico * 100) << "%";
    if (furtivo) std::cout << " | [furtivo ativo]";
    std::cout << "\n";
}

std::string Ladrao::serializar() const {
    std::ostringstream oss;
    oss << Personagem::serializar();
    oss << "\n\"agilidade\": " << agilidade;
    return oss.str();
}

// --- clerigo ---

Clerigo::Clerigo(const std::string& nome, std::unique_ptr<Raca> raca, int nivel)
    : Personagem(nome, std::move(raca), "clerigo", 120 + (nivel - 1) * 15, 100, nivel),
      sabedoria(25 + (nivel - 1) * 3), poder_cura(30 + (nivel - 1) * 4) {

    adicionarHabilidade(std::make_shared<HabilidadeSuporte>(
        "cura sagrada", "canaliza energia divina para curar ferimentos",
        20, 2, 50));
    adicionarHabilidade(std::make_shared<HabilidadeOfensiva>(
        "luz sagrada", "lanca um feixe de energia sagrada no inimigo",
        25, 3, 2.0f));
    adicionarHabilidade(std::make_shared<HabilidadeSuporte>(
        "bencao", "abencoa o alvo restaurando uma grande quantidade de hp",
        40, 5, 100));
    adicionarHabilidade(std::make_shared<HabilidadeDefensiva>(
        "escudo divino", "cria um escudo sagrado que absorve dano magico",
        30, 4, 1.8f));
}

int Clerigo::getSabedoria() const { return sabedoria; }
int Clerigo::getPodCura() const { return poder_cura; }

int Clerigo::atacar() {
    int dano = static_cast<int>(sabedoria * 1.5);
    std::cout << getNome() << " invocou a luz sagrada causando " << dano << " de dano.\n";

    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::DANO_CAUSADO, getNome(), "luz sagrada", dano));

    return dano;
}

int Clerigo::curarAliado(Personagem& alvo) {
    if (getMana() < 20) {
        std::cout << getNome() << " nao tem mana suficiente para curar.\n";
        return 0;
    }

    int cura = poder_cura + static_cast<int>(sabedoria * 0.5);
    alvo.curar(cura);

    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::CURA_RECEBIDA, alvo.getNome(), "cura do clerigo", cura));

    return cura;
}

void Clerigo::exibirStatus() const {
    Personagem::exibirStatus();
    std::cout << "  sabedoria: " << sabedoria
              << " | poder de cura: " << poder_cura << "\n";
}

std::string Clerigo::serializar() const {
    std::ostringstream oss;
    oss << Personagem::serializar();
    oss << "\n\"sabedoria\": " << sabedoria << ", \"poder_cura\": " << poder_cura;
    return oss.str();
}

// --- personagem factory ---

std::unique_ptr<Guerreiro> PersonagemFactory::criarGuerreiro(
    const std::string& nome, const std::string& raca) {
    return std::make_unique<Guerreiro>(nome, criarRaca(raca));
}

std::unique_ptr<Mago> PersonagemFactory::criarMago(
    const std::string& nome, const std::string& raca) {
    return std::make_unique<Mago>(nome, criarRaca(raca));
}

std::unique_ptr<Arqueiro> PersonagemFactory::criarArqueiro(
    const std::string& nome, const std::string& raca) {
    return std::make_unique<Arqueiro>(nome, criarRaca(raca));
}

std::unique_ptr<Ladrao> PersonagemFactory::criarLadrao(
    const std::string& nome, const std::string& raca) {
    return std::make_unique<Ladrao>(nome, criarRaca(raca));
}

std::unique_ptr<Clerigo> PersonagemFactory::criarClerigo(
    const std::string& nome, const std::string& raca) {
    return std::make_unique<Clerigo>(nome, criarRaca(raca));
}

void Personagem::deserializar(const std::string& dados) {
    // deserializacao do JSON/Texto aqui
    (void)dados; 
}

} // namespace rpg