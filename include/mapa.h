#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace rpg {

/** @brief enumeracao dos tipos de sala possiveis no mapa procedural */
enum class TipoSala {
    VAZIA,
    MONSTRO,
    TESOURO,
    LOJA,
    CHEFE,
    SAIDA
};

/** @brief estrutura de dados de um inimigo gerado no mapa */
struct InimigoSala {
    std::string nome;     /**< identificacao do monstro */
    std::string classe;   /**< arquetipo de combate */
    int nivel;            /**< forca base */
    int hp;               /**< vida maxima */
    int dano;             /**< poder de ataque */
    int xp_recompensa;    /**< experiencia ao derrotar */
    int ouro_recompensa;  /**< dinheiro ao derrotar */
    bool derrotado;       /**< flag de controle de morte */

    /** @brief construtor padrao para instanciar monstros na sala */
    InimigoSala(const std::string& nome, const std::string& classe,
                int nivel, int hp, int dano, int xp, int ouro)
        : nome(nome), classe(classe), nivel(nivel), hp(hp), dano(dano),
          xp_recompensa(xp), ouro_recompensa(ouro), derrotado(false) {}
};

/** @brief pacote de recompensas achado em um bau */
struct Tesouro {
    std::string nome_item; /**< identificador do loot */
    std::string tipo_item; /**< categoria do equipamento */
    int ouro;              /**< dinheiro encontrado */
    bool coletado;         /**< flag de controle de coleta */

    /** @brief construtor padrao de baus */
    Tesouro(const std::string& item, const std::string& tipo, int ouro)
        : nome_item(item), tipo_item(tipo), ouro(ouro), coletado(false) {}
};

/**
 * @brief nodo de navegacao dentro do mapa de exploracao
 * * armazena monstros, itens e conexoes para outras salas.
 */
class Sala {
private:
    int id;                            /**< identificador unico local */
    std::string nome;                  /**< titulo de exibicao */
    std::string descricao;             /**< lore do ambiente */
    TipoSala tipo;                     /**< categoria do conteudo */
    std::vector<int> saidas;           /**< ids das salas conectadas */
    std::vector<InimigoSala> inimigos; /**< fila de ameacas */
    std::vector<Tesouro> tesouros;     /**< fila de recompensas */
    bool visitada;                     /**< controle de nevoa de guerra */
    bool limpa;                        /**< indicador de sala segura */

public:
    /**
     * @brief construtor de ambiente
     * @param id identificador de array
     * @param nome titulo
     * @param desc lore
     * @param tipo enumerador do tipo
     */
    Sala(int id, const std::string& nome, const std::string& desc, TipoSala tipo);

    /** @brief le id @return valor inteiro */
    int getId() const;
    /** @brief le nome @return string do nome */
    std::string getNome() const;
    /** @brief le descricao @return string da lore */
    std::string getDescricao() const;
    /** @brief le o tipo @return categoria */
    TipoSala getTipo() const;
    /** @brief checa exploracao @return verdadeiro se ja entrou */
    bool foiVisitada() const;
    /** @brief checa seguranca @return verdadeiro se nao ha ameacas */
    bool estaLimpa() const;

    /** @brief cria uma ponte para outra sala @param id_sala destino */
    void adicionarSaida(int id_sala);
    /** @brief joga um monstro na sala @param inimigo dados do bot */
    void adicionarInimigo(const InimigoSala& inimigo);
    /** @brief joga um bau na sala @param tesouro dados do loot */
    void adicionarTesouro(const Tesouro& tesouro);

    /** @brief acessa conexoes @return lista de ids vizinhos */
    const std::vector<int>& getSaidas() const;
    /** @brief acessa lista de combate @return ponteiro dos inimigos */
    std::vector<InimigoSala>& getInimigos();
    /** @brief acessa lista de baus @return ponteiro dos tesouros */
    std::vector<Tesouro>& getTesouros();

    /** @brief retira nevoa de guerra */
    void marcarVisitada();
    /** @brief sinaliza eliminacao dos perigos */
    void marcarLimpa();
    /** @brief imprime as informacoes da sala atual */
    void exibirInfo() const;
};

/**
 * @brief gerador e controlador do labirinto procedural
 */
class Mapa {
private:
    std::map<int, std::shared_ptr<Sala>> salas; /**< lista geral de ambientes */
    int sala_atual_id;                          /**< ponteiro de posicao do heroi */
    int proximo_id;                             /**< auto incremento de geracao */
    int nivel_dificuldade;                      /**< multiplicador de status dos bots */

    /** @brief roleta magica de probabilidade @return tipo sorteado */
    TipoSala sorteiarTipoSala();
    /** @brief fabrica monstros aleatorios @param nivel_sala fator de multiplicacao @return dados da criatura */
    InimigoSala gerarInimigo(int nivel_sala);
    /** @brief fabrica baus aleatorios @param nivel_sala fator de multiplicacao @return dados do premio */
    Tesouro gerarTesouro(int nivel_sala);
    /** @brief rotula o ambiente @param tipo categoria @param indice contador @return nome gerado */
    std::string gerarNomeSala(TipoSala tipo, int indice);

public:
    Mapa();

    /**
     * @brief limpa a memoria e cria um novo labirinto
     * @param nivel forca base (padrao = 1)
     */
    void gerarAndar(int nivel = 1);

    /** @brief le onde o jogador esta @return ponteiro do ambiente atual */
    std::shared_ptr<Sala> getSalaAtual() const;
    /** @brief busca ambiente especifico @param id local @return ponteiro */
    std::shared_ptr<Sala> getSala(int id) const;

    /**
     * @brief avanca o heroi de comodo
     * @param id_sala identificador de destino
     * @return verdadeiro se a porta existir e for cruzada
     */
    bool moverPara(int id_sala);

    /** @brief imprime o minimapa geral */
    void exibirMapa() const;
    /** @brief imprime a lore do comodo */
    void exibirSalaAtual() const;

    /** @brief le o perigo global @return valor multiplicador */
    int getNivelDificuldade() const;
    /** @brief altera perigo global @param nivel novo patamar */
    void setNivelDificuldade(int nivel);
};

} // namespace rpg