#pragma once
#include "personagem.h"
#include "batalha.h"
#include "quest.h"
#include "crafting.h"
#include "mapa.h"
#include "arquivo.h"
#include "observer.h"
#include <memory>
#include <string>

namespace rpg {

/**
 * @brief interface visual orientada a linha de comando.
 * * coordena todas as interacoes do usuario invocando as rotinas do back-end.
 */
class MenuPrincipal {
private:
    std::unique_ptr<Personagem> personagem_atual; /**< memoria persistente do jogador corrente */
    std::unique_ptr<Mapa> mapa;                   /**< ambiente geografico do jogo */
    GerenciadorQuests gerenciador_quests;         /**< engine de resolucao missao */
    SistemaCrafting sistema_crafting;             /**< engine quimico metalurgica */
    SistemaArquivo sistema_arquivo;               /**< interfaceamento com json hdd */
    LoggerBatalha logger;                         /**< printador serial passivo */
    NotificadorNivel notificador_nivel;           /**< printador secundario estelar */
    bool jogo_em_execucao;                        /**< loop de tela principal rodando */
    int ouro_jogador;                             /**< cash wallet basica externa */

    /** @brief subrotina de nascimento e dados rpg base */
    void menuCriarPersonagem();
    
    /** * @brief injecao instanciadora usando factory
     * @param nome digitacao livre
     * @param classe enum string 
     * @param raca enum string
     * @return pointer do construtor
     */
    std::unique_ptr<Personagem> criarPersonagemComClasse(
        const std::string& nome, const std::string& classe, const std::string& raca);

    /** @brief painel base apos o login e alocacoes pre missao */
    void menuPrincipalJogo();
    /** @brief loop de visualizacao das stats numericas */
    void menuPersonagem();
    /** @brief loop de logistica peso carga equip */
    void menuInventario();
    /** @brief loop de biblioteca magica descritiva */
    void menuHabilidades();
    /** @brief disparador estatico vs cpu em loop */
    void menuBatalha();
    /** @brief sub sistema de transicao por salas andares comodos */
    void menuExplorar();
    /** @brief visualizador com status tracking printador */
    void menuQuests();
    /** @brief painel mercador combinador inventario vs memory map */
    void menuCrafting();
    /** @brief pass thru json conversor para escrita no c disco */
    void menuSalvar();
    /** @brief seletor string load array json back in engine */
    void menuCarregar();

    /** @brief mock arena cpu artificial engatilhada strategy agressivo auto */
    void batalharContraBot();
    /** @brief engine porrada dual pass in loop (descontinuado opcional demo) */
    void batalharContraJogador();
    /** @brief logica de comodo read node vector arrays id e prints */
    void explorarSala();
    /** @brief looper colhedor de ouro flags clean up room flags e loot pop */
    void interagirComSala();
    /** @brief call pop back memory transfer wallet menos price value item factory up */
    void comprarItem(const std::string& nome_item, int preco);

    /** @brief design cli asteriscos barras e tracinhos @param titulo print central */
    void exibirCabecalho(const std::string& titulo) const;
    /** @brief line divisor de visualizacao quebra string */
    void exibirSeparador() const;
    /** @brief sanitizador read fail try catch limits min limits max in console cin @param prompt cout value @param min chao valid @param max teto valid @return seguro numerico */
    int lerInteiro(const std::string& prompt, int min, int max) const;
    /** @brief string block loop getline with dump discard pre space @param prompt o display in stream @return full space valid char array */
    std::string lerString(const std::string& prompt) const;
    /** @brief blocking block cin loop with stream empty discard cin max ignore wait break block in console */
    void aguardarEnter() const;
    /** @brief exec block console clean up in multios compiler using cross tag macro ifdef */
    void limparTela() const;

    /** @brief hook and boot startup memory mock db data sets from memory address array pointer build startup and registry log observer push backs */
    void inicializarSistemas();

public:
    /** @brief default constructor boots default false set true boot default config engine up startup loop false out */
    MenuPrincipal();
    ~MenuPrincipal();

    /** @brief thread freeze root lock and trigger up while play game exec true start up flow root to branch loop menu logic */
    void executar();
};

} // namespace rpg