#pragma once
#include <string>
#include <iostream>
#include <memory>

namespace rpg {

/** @brief categorias padronizadas de itens */
enum class TipoItem {
    ARMA,
    ARMADURA,
    POCAO,
    ITEM_ESPECIAL
};

/**
 * @brief classe base para objetos fisicos do jogo
 */
class Item {
protected:
    std::string nome;      /**< titulo do objeto */
    std::string descricao; /**< lore e explicacao */
    float peso;            /**< massa em kg */
    int valor;             /**< preco em ouro */
    TipoItem tipo;         /**< categoria logica */

public:
    /**
     * @brief construtor base de objetos
     * @param nome titulo
     * @param desc lore
     * @param peso kg
     * @param valor ouro
     * @param tipo categoria
     */
    Item(const std::string& nome, const std::string& desc, float peso, int valor, TipoItem tipo);
    virtual ~Item() = default;

    /** @brief le nome @return string */
    std::string getNome() const;
    /** @brief le descricao @return string */
    std::string getDescricao() const;
    /** @brief le peso @return float */
    float getPeso() const;
    /** @brief le valor @return preco */
    int getValor() const;
    /** @brief le categoria @return tipo enumerado */
    TipoItem getTipo() const;

    /**
     * @brief compara igualdade pelo nome
     * @param outro objeto de comparacao
     * @return logico de igualdade
     */
    bool operator==(const Item& outro) const;

    /**
     * @brief junta as descricoes
     * @param outro item somado
     * @return string concatenada
     */
    std::string operator+(const Item& outro) const;

    /**
     * @brief checa valor financeiro
     * @param outro item alvo
     * @return verdadeiro se este for mais barato
     */
    bool operator<(const Item& outro) const;

    /** @brief stream do item */
    friend std::ostream& operator<<(std::ostream& os, const Item& item);

    /** @brief lanca no console */
    virtual void exibirInfo() const;
    /** @brief converte para texto @return formato para arquivo */
    virtual std::string serializar() const = 0;
};

/** @brief item de ataque corporal ou a distancia */
class Arma : public Item {
private:
    int dano_base;         /**< pontuacao extra no ataque */
    std::string tipo_arma; /**< ex: espada, arco */

public:
    Arma(const std::string& nome, const std::string& desc, float peso, int valor,
         int dano_base, const std::string& tipo_arma);

    int getDanoBase() const;
    std::string getTipoArma() const;

    void exibirInfo() const override;
    std::string serializar() const override;
};

/** @brief equipamento protetivo */
class Armadura : public Item {
private:
    int defesa;                 /**< mitigacao de golpes */
    std::string tipo_armadura;  /**< ex: couro, placas */

public:
    Armadura(const std::string& nome, const std::string& desc, float peso, int valor,
             int defesa, const std::string& tipo_armadura);

    int getDefesa() const;
    std::string getTipoArmadura() const;

    void exibirInfo() const override;
    std::string serializar() const override;
};

/** @brief consumivel para status */
class Pocao : public Item {
private:
    int quantidade_cura;    /**< valor recuperado */
    std::string tipo_pocao; /**< ex: hp, mana */

public:
    Pocao(const std::string& nome, const std::string& desc, float peso, int valor,
          int quantidade_cura, const std::string& tipo_pocao);

    int getQuantidadeCura() const;
    std::string getTipoPocao() const;

    void exibirInfo() const override;
    std::string serializar() const override;
};

/** @brief itens com comportamentos unicos */
class ItemEspecial : public Item {
private:
    std::string efeito; /**< chave de ativacao do efeito */
    int poder;          /**< intensidade da magia */

public:
    ItemEspecial(const std::string& nome, const std::string& desc, float peso, int valor,
                 const std::string& efeito, int poder);

    std::string getEfeito() const;
    int getPoder() const;

    void exibirInfo() const override;
    std::string serializar() const override;
};

/** @brief instanciador logico de loot do jogo */
class ItemFactory {
public:
    static std::unique_ptr<Arma> criarArma(const std::string& tipo);
    static std::unique_ptr<Armadura> criarArmadura(const std::string& tipo);
    static std::unique_ptr<Pocao> criarPocao(const std::string& tipo);
    static std::unique_ptr<ItemEspecial> criarItemEspecial(const std::string& tipo);
};

} // namespace rpg