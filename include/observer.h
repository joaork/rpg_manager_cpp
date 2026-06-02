#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace rpg {

/** @brief lista de incidentes notificaveis */
enum class TipoEvento {
    NIVEL_SUBIU,
    PERSONAGEM_MORREU,
    DANO_CAUSADO,
    CURA_RECEBIDA,
    ITEM_COLETADO,
    HABILIDADE_USADA,
    BATALHA_INICIADA,
    BATALHA_ENCERRADA,
    QUEST_COMPLETADA
};

/**
 * @brief carrega informacoes sobre o que aconteceu no sistema
 */
struct Evento {
    TipoEvento tipo;         /**< categoria do acontecimento */
    std::string nome_origem; /**< agente responsavel */
    std::string mensagem;    /**< lore ou texto do log */
    int valor;               /**< dados numericos pertinentes */

    /**
     * @brief constroi o pacote de notificacao
     * @param tipo enumeracao da acao
     * @param origem string causadora
     * @param msg string descritiva
     * @param val valor gerado (padrao = 0)
     */
    Evento(TipoEvento tipo, const std::string& origem, const std::string& msg, int val = 0)
        : tipo(tipo), nome_origem(origem), mensagem(msg), valor(val) {}
};

/**
 * @brief contrato para agentes ouvintes
 */
class IObserver {
public:
    virtual ~IObserver() = default;
    /** @brief acionado sempre que um evento ocorrer @param evento pacote gerado */
    virtual void onEvento(const Evento& evento) = 0;
};

/**
 * @brief singleton que centraliza transmissoes de acoes
 */
class GerenciadorEventos {
private:
    static std::unique_ptr<GerenciadorEventos> instancia; /**< pivo estatico */

    std::vector<IObserver*> observers;     /**< lista de assinantes */
    std::vector<Evento> historico_eventos; /**< registro continuo de logs */

    GerenciadorEventos() = default;

public:
    GerenciadorEventos(const GerenciadorEventos&) = delete;
    GerenciadorEventos& operator=(const GerenciadorEventos&) = delete;

    /** @brief le o pivo principal @return referencia do gerenciador */
    static GerenciadorEventos& getInstance();

    /** @brief adiciona um ouvinte na lista @param observer classe logadora */
    void registrarObserver(IObserver* observer);
    /** @brief remove ouvinte da lista @param observer classe logadora */
    void removerObserver(IObserver* observer);
    /** @brief dispara o alerta para os inscritos @param evento pacote recem gerado */
    void publicarEvento(const Evento& evento);

    /** @brief le as escritas @return fila de acoes anteriores */
    const std::vector<Evento>& getHistorico() const;
    /** @brief esvazia a fila logica */
    void limparHistorico();
};

/** @brief classe que traduz eventos de porrada em texto de console */
class LoggerBatalha : public IObserver {
public:
    void onEvento(const Evento& evento) override;
};

/** @brief classe que monitora crescimento de personagens */
class NotificadorNivel : public IObserver {
public:
    void onEvento(const Evento& evento) override;
};

} // namespace rpg