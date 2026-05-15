#pragma once
#include <string>
#include <memory>

namespace rpg {

/**
 * @brief classe base para as racas do jogo
 * * cada raca concede bonus diferentes nos atributos base do personagem.
 */
class Raca {
protected:
    std::string nome;         /**< nome da raca */
    int bonus_hp;             /**< pontos de vida adicionais */
    int bonus_forca;          /**< forca adicional para combate corpo a corpo */
    int bonus_inteligencia;   /**< magia e mana adicionais */
    int bonus_agilidade;      /**< velocidade e evasao adicionais */
    std::string descricao;    /**< breve lore da raca */

public:
    /**
     * @brief construtor da raca
     * @param nome nome da raca
     * @param b_hp bonus de hp
     * @param b_for bonus de forca
     * @param b_int bonus de inteligencia
     * @param b_agi bonus de agilidade
     * @param desc descricao da historia da raca
     */
    Raca(const std::string& nome, int b_hp, int b_for, int b_int, int b_agi, const std::string& desc);
    
    /** @brief destrutor virtual padrao */
    virtual ~Raca() = default;

    /** @brief retorna o nome @return string do nome */
    std::string getNome() const;
    /** @brief retorna o bonus de hp @return valor do bonus */
    int getBonusHp() const;
    /** @brief retorna o bonus de forca @return valor do bonus */
    int getBonusForca() const;
    /** @brief retorna o bonus de inteligencia @return valor do bonus */
    int getBonusInteligencia() const;
    /** @brief retorna o bonus de agilidade @return valor do bonus */
    int getBonusAgilidade() const;
    /** @brief retorna a lore @return texto de descricao */
    std::string getDescricao() const;

    /** @brief exibe as estatisticas da raca no console */
    virtual void exibirInfo() const;
};

/** @brief raca equilibrada sem pontos fracos evidentes */
class Humano : public Raca {
public:
    Humano();
};

/** @brief raca magica e agil porem fragil fisicamente */
class Elfo : public Raca {
public:
    Elfo();
};

/** @brief raca resistente e forte porem lenta */
class Anao : public Raca {
public:
    Anao();
};

/** @brief raca com forca brutal acima da media */
class Orc : public Raca {
public:
    Orc();
};

/** @brief raca furtiva com altas chances de acerto critico */
class Hobbit : public Raca {
public:
    Hobbit();
};

/**
 * @brief instancia a raca correta a partir de uma string
 * @param nome texto contendo a raca desejada
 * @return ponteiro inteligente para a raca alocada
 */
std::unique_ptr<Raca> criarRaca(const std::string& nome);

} // namespace rpg