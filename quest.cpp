#include "include/quest.h"
#include <iostream>
#include <algorithm>

namespace rpg {

// --- quest ---

Quest::Quest(const std::string& id, const std::string& titulo,
             const std::string& desc, const RecompensaQuest& recompensa)
    : id(id), titulo(titulo), descricao(desc),
      estado(EstadoQuest::NAO_INICIADA), recompensa(recompensa) {}

std::string Quest::getId() const { return id; }
std::string Quest::getTitulo() const { return titulo; }
std::string Quest::getDescricao() const { return descricao; }
EstadoQuest Quest::getEstado() const { return estado; }
const RecompensaQuest& Quest::getRecompensa() const { return recompensa; }

void Quest::adicionarObjetivo(const ObjetivoQuest& obj) {
    objetivos.push_back(obj);
}

void Quest::atualizarObjetivo(int indice, int valor) {
    if (indice >= 0 && indice < static_cast<int>(objetivos.size())) {
        objetivos[indice].atualizar(valor);
    }
}

bool Quest::todosObjetivosCompletos() const {
    return std::all_of(objetivos.begin(), objetivos.end(),
        [](const ObjetivoQuest& o) { return o.completado; });
}

void Quest::iniciar() {
    if (estado == EstadoQuest::NAO_INICIADA) {
        estado = EstadoQuest::EM_ANDAMENTO;
        std::cout << "quest iniciada: " << titulo << "\n";
        std::cout << "  " << descricao << "\n";
    }
}

void Quest::completar() {
    estado = EstadoQuest::COMPLETADA;
    std::cout << "quest completada: " << titulo << "\n";
    std::cout << "  recompensa: " << recompensa.xp << " xp, "
              << recompensa.ouro << " de ouro";
    if (!recompensa.item_recompensa.empty()) {
        std::cout << ", " << recompensa.item_recompensa;
    }
    std::cout << "\n";
}

void Quest::falhar() {
    estado = EstadoQuest::FALHOU;
    std::cout << "quest falhou: " << titulo << "\n";
}

void Quest::exibirInfo() const {
    std::string status;
    switch (estado) {
        case EstadoQuest::NAO_INICIADA:  status = "nao iniciada"; break;
        case EstadoQuest::EM_ANDAMENTO:  status = "em andamento"; break;
        case EstadoQuest::COMPLETADA:    status = "completada"; break;
        case EstadoQuest::FALHOU:        status = "falhou"; break;
    }

    std::cout << "\n[quest] " << titulo << " [" << status << "]\n";
    std::cout << "  " << descricao << "\n";
    std::cout << "  recompensa: " << recompensa.xp << " xp | "
              << recompensa.ouro << " ouro\n";

    if (!objetivos.empty()) {
        std::cout << "  objetivos:\n";
        for (const auto& obj : objetivos) {
            std::cout << "    " << (obj.completado ? "[x]" : "[ ]")
                      << " " << obj.descricao
                      << " (" << obj.progresso_atual << "/"
                      << obj.progresso_necessario << ")\n";
        }
    }
}

// --- gerenciador de quests ---

void GerenciadorQuests::registrarQuest(std::shared_ptr<Quest> quest) {
    quests[quest->getId()] = quest;
}

bool GerenciadorQuests::iniciarQuest(const std::string& id) {
    auto it = quests.find(id);
    if (it == quests.end()) {
        std::cout << "quest '" << id << "' nao encontrada.\n";
        return false;
    }

    if (it->second->getEstado() != EstadoQuest::NAO_INICIADA) {
        std::cout << "quest ja foi iniciada ou finalizada.\n";
        return false;
    }

    it->second->iniciar();
    quests_ativas.push_back(id);
    return true;
}

bool GerenciadorQuests::atualizarProgresso(const std::string& id_quest,
                                           int indice_objetivo, int valor) {
    auto it = quests.find(id_quest);
    if (it == quests.end()) return false;
    if (it->second->getEstado() != EstadoQuest::EM_ANDAMENTO) return false;

    it->second->atualizarObjetivo(indice_objetivo, valor);
    return true;
}

bool GerenciadorQuests::verificarCompletar(const std::string& id_quest) {
    auto it = quests.find(id_quest);
    if (it == quests.end()) return false;

    if (it->second->todosObjetivosCompletos()) {
        it->second->completar();

        // move da lista de ativas para completadas
        auto pos = std::find(quests_ativas.begin(), quests_ativas.end(), id_quest);
        if (pos != quests_ativas.end()) {
            quests_ativas.erase(pos);
        }
        quests_completadas.push_back(id_quest);
        return true;
    }
    return false;
}

std::shared_ptr<Quest> GerenciadorQuests::getQuest(const std::string& id) const {
    auto it = quests.find(id);
    return (it != quests.end()) ? it->second : nullptr;
}

std::vector<std::string> GerenciadorQuests::getQuestsAtivas() const { return quests_ativas; }
std::vector<std::string> GerenciadorQuests::getQuestsCompletadas() const { return quests_completadas; }

void GerenciadorQuests::listarQuests() const {
    if (quests.empty()) {
        std::cout << "  nenhuma quest disponivel.\n";
        return;
    }
    for (const auto& [id, q] : quests) {
        q->exibirInfo();
    }
}

void GerenciadorQuests::listarQuestsAtivas() const {
    if (quests_ativas.empty()) {
        std::cout << "  nenhuma quest em andamento.\n";
        return;
    }
    for (const auto& id : quests_ativas) {
        auto it = quests.find(id);
        if (it != quests.end()) it->second->exibirInfo();
    }
}

void GerenciadorQuests::carregarQuestsPadrao() {
    // quest de tutorial: matar o primeiro inimigo
    auto q1 = std::make_shared<Quest>(
        "q001", "primeiro sangue",
        "derrote seu primeiro inimigo no dungeon e prove seu valor.",
        RecompensaQuest(100, 50, "pocao de cura menor"));
    q1->adicionarObjetivo(ObjetivoQuest("derrotar um inimigo", 1));
    registrarQuest(q1);

    // quest de exploracao
    auto q2 = std::make_shared<Quest>(
        "q002", "explorador iniciante",
        "explore pelo menos 5 salas diferentes do dungeon.",
        RecompensaQuest(200, 100, "botas de mercurio"));
    q2->adicionarObjetivo(ObjetivoQuest("explorar salas", 5));
    registrarQuest(q2);

    // quest de coleta
    auto q3 = std::make_shared<Quest>(
        "q003", "coleta de recursos",
        "encontre e colete 3 pocoes durante a exploracao.",
        RecompensaQuest(150, 75, ""));
    q3->adicionarObjetivo(ObjetivoQuest("coletar pocoes", 3));
    registrarQuest(q3);

    // quest de combate avancado
    auto q4 = std::make_shared<Quest>(
        "q004", "guerreiro experiente",
        "derrote 10 inimigos no total para se tornar um veterano.",
        RecompensaQuest(500, 300, "amuleto de forca"));
    q4->adicionarObjetivo(ObjetivoQuest("derrotar inimigos", 10));
    registrarQuest(q4);

    // quest do chefe
    auto q5 = std::make_shared<Quest>(
        "q005", "o chefe do dungeon",
        "encontre e derrote o guardiao do dungeon para escapar.",
        RecompensaQuest(1000, 500, "pedra filosofal"));
    q5->adicionarObjetivo(ObjetivoQuest("encontrar o chefe", 1));
    q5->adicionarObjetivo(ObjetivoQuest("derrotar o chefe", 1));
    registrarQuest(q5);
}

} // namespace rpg
