#include "include/observer.h"
#include <iostream>
#include <algorithm>

namespace rpg {

// inicializacao da instancia unica do singleton
std::unique_ptr<GerenciadorEventos> GerenciadorEventos::instancia = nullptr;

GerenciadorEventos& GerenciadorEventos::getInstance() {
    if (!instancia) {
        // cria a instancia so na primeira vez que for chamado
        instancia.reset(new GerenciadorEventos());
    }
    return *instancia;
}

void GerenciadorEventos::registrarObserver(IObserver* observer) {
    if (observer) {
        observers.push_back(observer);
    }
}

void GerenciadorEventos::removerObserver(IObserver* observer) {
    auto it = std::find(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

void GerenciadorEventos::publicarEvento(const Evento& evento) {
    // salva o evento no historico antes de notificar os observers
    historico_eventos.push_back(evento);

    // notifica todos os observers registrados
    for (auto* observer : observers) {
        observer->onEvento(evento);
    }
}

const std::vector<Evento>& GerenciadorEventos::getHistorico() const {
    return historico_eventos;
}

void GerenciadorEventos::limparHistorico() {
    historico_eventos.clear();
}

// --- logger de batalha ---

void LoggerBatalha::onEvento(const Evento& evento) {
    // so loga os eventos relevantes para combate
    switch (evento.tipo) {
        case TipoEvento::DANO_CAUSADO:
            std::cout << "[batalha] " << evento.nome_origem
                      << " causou " << evento.valor << " de dano.\n";
            break;
        case TipoEvento::CURA_RECEBIDA:
            std::cout << "[batalha] " << evento.nome_origem
                      << " recuperou " << evento.valor << " de hp.\n";
            break;
        case TipoEvento::PERSONAGEM_MORREU:
            std::cout << "[batalha] " << evento.nome_origem << " foi derrotado!\n";
            break;
        case TipoEvento::HABILIDADE_USADA:
            std::cout << "[batalha] " << evento.nome_origem
                      << " usou " << evento.mensagem << ".\n";
            break;
        case TipoEvento::BATALHA_INICIADA:
            std::cout << "[batalha] " << evento.mensagem << "\n";
            break;
        case TipoEvento::BATALHA_ENCERRADA:
            std::cout << "[batalha] batalha encerrada. vencedor: "
                      << evento.nome_origem << "\n";
            break;
        default:
            break;
    }
}

// --- notificador de nivel ---

void NotificadorNivel::onEvento(const Evento& evento) {
    if (evento.tipo == TipoEvento::NIVEL_SUBIU) {
        std::cout << "[nivel] " << evento.nome_origem
                  << " subiu para o nivel " << evento.valor << "!\n";
        std::cout << "[nivel] atributos aumentados e hp restaurado.\n";
    }

    if (evento.tipo == TipoEvento::QUEST_COMPLETADA) {
        std::cout << "[quest] missao concluida: " << evento.mensagem << "\n";
        std::cout << "[quest] recompensas recebidas!\n";
    }
}

} // namespace rpg
