#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace rpg {

/** @brief flag indicadora do andamento da missao */
enum class EstadoQuest {
    NAO_INICIADA,
    EM_ANDAMENTO,
    COMPLETADA,
    FALHOU
};

/** @brief pacote ganho ao terminar o servico */
struct RecompensaQuest {
    int xp;                      /**< crescimento entregue */
    int ouro;                    /**< dinheiro entregue */
    std::string item_recompensa; /**< loot ganho se existir */

    /** @brief construtor padrao */
    RecompensaQuest(int xp = 0, int ouro = 0, const std::string& item = "")
        : xp(xp), ouro(ouro), item_recompensa(item) {}
};

/** @brief etapa singular da missao maior */
struct ObjetivoQuest {
    std::string descricao;    /**< lore da tarefa */
    int progresso_atual;      /**< contador atual */
    int progresso_necessario; /**< teto da tarefa */
    bool completado;          /**< flag logica de terminada */

    /** @brief construtor da rotina */
    ObjetivoQuest(const std::string& desc, int necessario)
        : descricao(desc), progresso_atual(0),
          progresso_necessario(necessario), completado(false) {}

    /** @brief incrementa os dados @param valor pulo do loop */
    void atualizar(int valor = 1) {
        progresso_atual = std::min(progresso_atual + valor, progresso_necessario);
        if (progresso_atual >= progresso_necessario) {
            completado = true;
        }
    }
};

/**
 * @brief centraliza as operacoes de uma missao do jogo
 */
class Quest {
private:
    std::string id;                       /**< codigo hash referencial */
    std::string titulo;                   /**< cabecalho da lenda */
    std::string descricao;                /**< lore profunda */
    EstadoQuest estado;                   /**< indicador temporal */
    RecompensaQuest recompensa;           /**< pacote de premio */
    std::vector<ObjetivoQuest> objetivos; /**< lista de etapas */

public:
    /**
     * @brief monta a missao
     * @param id identificador curto
     * @param titulo cabecalho
     * @param desc lore
     * @param recompensa pacote de premicao
     */
    Quest(const std::string& id, const std::string& titulo,
          const std::string& desc, const RecompensaQuest& recompensa);

    std::string getId() const;
    std::string getTitulo() const;
    std::string getDescricao() const;
    EstadoQuest getEstado() const;
    const RecompensaQuest& getRecompensa() const;

    /** @brief empilha uma nova tarefa @param obj estrutura montada */
    void adicionarObjetivo(const ObjetivoQuest& obj);
    /** @brief avanca o tracker de uma tarefa especifica @param indice array local @param valor salto */
    void atualizarObjetivo(int indice, int valor = 1);
    /** @brief checa a conclusao integral @return logico de progresso 100% */
    bool todosObjetivosCompletos() const;

    /** @brief troca a flag do estado */
    void iniciar();
    /** @brief encerra e despeja os lucros */
    void completar();
    /** @brief joga no erro e tranca a recompensa */
    void falhar();

    /** @brief lanca o sumario na tela */
    void exibirInfo() const;
};

/**
 * @brief centraliza e monitora todas as chamadas de missoes
 */
class GerenciadorQuests {
private:
    std::map<std::string, std::shared_ptr<Quest>> quests; /**< base de dados total */
    std::vector<std::string> quests_ativas;               /**< as que estao rodando */
    std::vector<std::string> quests_completadas;          /**< historico do que passou */

public:
    /** @brief injeta uma missao criada no banco global @param quest endereco de memoria */
    void registrarQuest(std::shared_ptr<Quest> quest);
    
    /** @brief lanca uma missao em tracking @param id codigo hash @return logico de sucesso */
    bool iniciarQuest(const std::string& id);
    
    /** @brief avanca barramentos na missao ativa @param id_quest codigo hash @param indice_objetivo array da quest @param valor somador @return flag booleana de sucesso */
    bool atualizarProgresso(const std::string& id_quest, int indice_objetivo, int valor = 1);
    
    /** @brief checa se chegou no fim e limpa a lista de ativas @param id_quest codigo da quest @return verdadeiro se fechou ela agora */
    bool verificarCompletar(const std::string& id_quest);

    /** @brief puxa os dados estaticos de uma missao @param id codigo base @return ponteiro de memoria */
    std::shared_ptr<Quest> getQuest(const std::string& id) const;
    /** @brief lista o hash das missoes running @return vetor de strings */
    std::vector<std::string> getQuestsAtivas() const;
    /** @brief lista o hash de missoes finitas @return vetor de strings */
    std::vector<std::string> getQuestsCompletadas() const;

    /** @brief dump de todas no terminal */
    void listarQuests() const;
    /** @brief dump apenas do backlog atual */
    void listarQuestsAtivas() const;

    /** @brief inicializa o mock de testes pre-pronto */
    void carregarQuestsPadrao();
};

} // namespace rpg