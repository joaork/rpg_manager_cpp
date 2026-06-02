#pragma once
#include "personagem.h"
#include "observer.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace rpg {

/** @brief acoes taticas no menu da luta */
enum class AcaoBatalha {
    ATACAR,
    USAR_HABILIDADE,
    USAR_ITEM,
    DEFENDER,
    FUGIR
};

/** @brief pacote do log de troca de golpes */
struct ResultadoTurno {
    AcaoBatalha acao;          /**< decisao do turno */
    std::string nome_atacante; /**< quem fez a acao */
    std::string nome_alvo;     /**< quem recebeu */
    int dano_causado;          /**< sangramento total */
    int dano_recebido;         /**< troco da fugas falhas */
    std::string descricao;     /**< texto de apoio */
    bool fugiu;                /**< desfecho evasivo */
};

/** @brief interface de controle de comportamentos do pc */
class EstrategiaBatalha {
public:
    virtual ~EstrategiaBatalha() = default;
    /** @brief computa a acao recomendada */
    virtual AcaoBatalha escolherAcao(const Personagem& personagem, const Personagem& oponente) = 0;
    /** @brief decide qual tecnica avancada disparar */
    virtual int escolherHabilidade(const Personagem& personagem) = 0;
};

/** @brief ia que so avanca com forca bruta */
class EstrategiaAgressiva : public EstrategiaBatalha {
public:
    AcaoBatalha escolherAcao(const Personagem& personagem, const Personagem& oponente) override;
    int escolherHabilidade(const Personagem& personagem) override;
};

/** @brief ia que preza pela propria vida e pocoes */
class EstrategiaDefensiva : public EstrategiaBatalha {
public:
    AcaoBatalha escolherAcao(const Personagem& personagem, const Personagem& oponente) override;
    int escolherHabilidade(const Personagem& personagem) override;
};

/** @brief ia erratica e dependente do random */
class EstrategiaAleatoria : public EstrategiaBatalha {
public:
    AcaoBatalha escolherAcao(const Personagem& personagem, const Personagem& oponente) override;
    int escolherHabilidade(const Personagem& personagem) override;
};

/** @brief lida com o laco infinito da luta entre duas entidades */
class Batalha {
private:
    Personagem& jogador_um;                            /**< desafiante primario */
    Personagem& jogador_dois;                          /**< desafiante secundario */
    bool jogador_dois_e_bot;                           /**< flag de intelgencia artificial */
    std::unique_ptr<EstrategiaBatalha> estrategia_bot; /**< cerebro da ia ativa */
    std::vector<ResultadoTurno> log_batalha;           /**< cronologia dos ataques */
    int turno_atual;                                   /**< contador ciclico */
    int defesa_temporaria_j1;                          /**< mitigacao extra em acao de def */
    int defesa_temporaria_j2;                          /**< mitigacao extra do pc */

    /** @brief compila logica bruta de hp e mp */
    ResultadoTurno executarAcao(Personagem& atacante, Personagem& alvo,
                                AcaoBatalha acao, int indice_habilidade = -1);

    /** @brief imprime as opcoes no prompt */
    AcaoBatalha solicitarAcaoHumano(Personagem& personagem);
    /** @brief imprime lista de skills e le indice */
    int solicitarHabilidadeHumano(Personagem& personagem);

    /** @brief joga no ar a vida dos atuantes */
    void exibirEstadoBatalha() const;

public:
    /**
     * @brief constroi o coliseu
     * @param j1 heroi
     * @param j2 antagonista
     * @param j2_e_bot logico de ativacao do padrao strategy
     * @param estrategia pointer do comportamento da ia
     */
    Batalha(Personagem& j1, Personagem& j2, bool j2_e_bot = false,
            std::unique_ptr<EstrategiaBatalha> estrategia = nullptr);

    /** @brief trava o console ate resolver a luta @return o lutador nao morto */
    Personagem* iniciar();

    /** @brief descarrega os socos na tela */
    void exibirLog() const;
    /** @brief checa a rodada corrente @return numero do ciclo */
    int getTurnoAtual() const;
};

} // namespace rpg