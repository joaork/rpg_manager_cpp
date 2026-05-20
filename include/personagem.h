#pragma once
#include "entidade.h"
#include "raca.h"
#include "inventario.h"
#include "habilidade.h"
#include "observer.h"
#include <vector>
#include <memory>
#include <string>

namespace rpg {

/**
 * @brief heroi jogavel principal que contem sistemas de rpg
 * * expande a entidade basica com inventario, racas, classes
 * experiencia e sistema de mana.
 */
class Personagem : public Entidade {
protected:
    std::unique_ptr<Raca> raca;                            /**< associacao da linhagem */
    std::string classe_rpg;                                /**< profissao escolhida */
    int xp;                                                /**< progresso para evoluir */
    int xp_para_proximo_nivel;                             /**< meta para subir de nivel */
    int mana;                                              /**< pontos magicos atuais */
    int mana_maxima;                                       /**< teto magico do personagem */
    Inventario inventario;                                 /**< colecao de itens e equipamentos */
    std::vector<std::shared_ptr<Habilidade>> habilidades;  /**< conjunto de ataques ativos */

    /** @brief soma as vantagens inatas da raca ao heroi */
    void aplicarBonusRaca();

    /** @brief avanca de estagio e aprimora forca e status maximos */
    void subirNivel();

public:
    /**
     * @brief define os tracos basicos de criacao
     * @param nome titulo pessoal
     * @param raca alocacao dinamicada da especie
     * @param classe nome da carreira de batalha
     * @param hp_max pontos de vida
     * @param mana_max pontos de conjuracao
     * @param nivel inicio basico de experiencia
     */
    Personagem(const std::string& nome, std::unique_ptr<Raca> raca,
               const std::string& classe, int hp_max, int mana_max, int nivel = 1);

    virtual ~Personagem();

    /** @brief le a string raca @return nome correspondente */
    std::string getRaca() const;
    /** @brief le a classe @return categoria profissional */
    std::string getClasse() const;
    /** @brief le a xp @return pontuacao adquirida */
    int getXp() const;
    /** @brief le a meta xp @return alvo evolutivo */
    int getXpProximoNivel() const;
    /** @brief le a mana @return contagem de magia disponivel */
    int getMana() const;
    /** @brief le a mana maxima @return topo de capacidade magica */
    int getManaMaxima() const;
    
    /** @brief acesso editavel a mochila @return referencia de inventario */
    Inventario& getInventario();
    /** @brief acesso fixo a mochila @return referencia imutavel de inventario */
    const Inventario& getInventario() const;

    /**
     * @brief consolida recompensas de batalha
     * @param quantidade valor concedido pelo sistema
     */
    void ganharXp(int quantidade);

    /**
     * @brief valida mana e cooldown antes do ataque
     * @param indice ponteiro vetorial da magia
     * @param atributo_base valor primario da classe
     * @return resultado numerico do efeito
     */
    int usarHabilidade(int indice, int atributo_base);
    
    /** @brief expande biblioteca de golpes @param habilidade memoria dinamica da skill */
    void adicionarHabilidade(std::shared_ptr<Habilidade> habilidade);
    
    /** @brief lanca as informacoes no terminal visual */
    void listarHabilidades() const;

    /** @brief corre todas as magias tirando um turno de espera */
    void atualizarCooldowns();

    /**
     * @brief operador relacional nivelado
     * @param outro entidade alvo
     * @return logico comparativo de inferioridade
     */
    bool operator<(const Personagem& outro) const;
    
    /**
     * @brief operador relacional igualitario
     * @param outro entidade alvo
     * @return logico de coincidencia de chave primaria
     */
    bool operator==(const Personagem& outro) const;

    /** @brief lista ficha inteira e detalhada */
    void exibirStatus() const override;

    /** @brief transforma objeto em json @return texto para persistencia */
    virtual std::string serializar() const;
    /** @brief extrai json para as variaveis @param dados formato textual recuperado */
    virtual void deserializar(const std::string& dados);
};

/** @brief combate corpo a corpo focado no poder muscular */
class Guerreiro : public Personagem {
private:
    int forca;         /**< multiplica danos fisicos */
    int defesa;        /**< absorve danos recebidos */
    bool modo_berserk; /**< ativado no desespero para golpear pesado */

public:
    Guerreiro(const std::string& nome, std::unique_ptr<Raca> raca, int nivel = 1);

    int getForca() const;
    int getDefesa() const;

    int atacar() override;
    
    /** @brief calculo mitigatorio @param dano valor hostil emitido */
    void receberDanoComDefesa(int dano);
    void exibirStatus() const override;
    std::string serializar() const override;
};

/** @brief magia destrutiva com alta capacidade mas pouca resistencia */
class Mago : public Personagem {
private:
    int inteligencia; /**< potencializa licoes magicas */
    int poder_magico; /**< impacto basico das magias */

public:
    Mago(const std::string& nome, std::unique_ptr<Raca> raca, int nivel = 1);

    int getInteligencia() const;
    int getPodMagico() const;

    int atacar() override;
    void exibirStatus() const override;
    std::string serializar() const override;
};

/** @brief controle critico em alta mobilidade */
class Arqueiro : public Personagem {
private:
    int agilidade;        /**< garante furtividade extra */
    int precisao;         /**< acertos solidos e consistentes */
    float chance_critico; /**< possibilidade de escalar dano bruto */

public:
    Arqueiro(const std::string& nome, std::unique_ptr<Raca> raca, int nivel = 1);

    int getAgilidade() const;
    float getChanceCritico() const;

    int atacar() override;
    void exibirStatus() const override;
    std::string serializar() const override;
};

/** @brief evasivo e especialista em armadilhas e criticos duplos */
class Ladrao : public Personagem {
private:
    int agilidade;        /**< flexibilidade hostil */
    float chance_critico; /**< margem estendida de sorte */
    bool furtivo;         /**< invisibilidade passiva temporal */

public:
    Ladrao(const std::string& nome, std::unique_ptr<Raca> raca, int nivel = 1);

    int getAgilidade() const;
    float getChanceCritico() const;
    bool isFurtivo() const;
    
    /** @brief garante um turno oculto livre de agravos */
    void ativarFurtividade();

    int atacar() override;
    void exibirStatus() const override;
    std::string serializar() const override;
};

/** @brief conjurador voltado para a saude da equipe e fogo sagrado */
class Clerigo : public Personagem {
private:
    int sabedoria;  /**< amplifica conexao com habilidades misticas */
    int poder_cura; /**< valor estatico base das magias bondosas */

public:
    Clerigo(const std::string& nome, std::unique_ptr<Raca> raca, int nivel = 1);

    int getSabedoria() const;
    int getPodCura() const;

    int atacar() override;
    
    /** @brief remete a logica de cura a companheiros @param alvo destino da aura */
    int curarAliado(Personagem& alvo);
    void exibirStatus() const override;
    std::string serializar() const override;
};

/**
 * @brief centraliza a geracao alocada em metodos organizadores
 */
class PersonagemFactory {
public:
    static std::unique_ptr<Guerreiro> criarGuerreiro(const std::string& nome, const std::string& raca);
    static std::unique_ptr<Mago> criarMago(const std::string& nome, const std::string& raca);
    static std::unique_ptr<Arqueiro> criarArqueiro(const std::string& nome, const std::string& raca);
    static std::unique_ptr<Ladrao> criarLadrao(const std::string& nome, const std::string& raca);
    static std::unique_ptr<Clerigo> criarClerigo(const std::string& nome, const std::string& raca);
};

} // namespace rpg