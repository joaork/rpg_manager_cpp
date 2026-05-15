#pragma once
#include <string>
#include <iostream>

namespace rpg {

/**
 * @brief classe base abstrata para todos os seres do jogo
 * * define os atributos e comportamentos comuns a qualquer entidade,
 * servindo de base para personagens e possiveis monstros.
 */
class Entidade {
protected:
    std::string nome; /**< nome da entidade */
    int hp;           /**< pontos de vida atuais */
    int hp_maximo;    /**< limite maximo de pontos de vida */
    int nivel;        /**< nivel atual da entidade */

public:
    /**
     * @brief construtor padrao da entidade
     * @param nome nome do ser
     * @param hp_maximo valor maximo de hp
     * @param nivel nivel inicial (padrao = 1)
     */
    Entidade(const std::string& nome, int hp_maximo, int nivel = 1);
    
    /** @brief destrutor virtual padrao */
    virtual ~Entidade() = default;

    /** @brief obtem o nome da entidade @return string com o nome */
    std::string getNome() const;
    /** @brief obtem o hp atual @return valor do hp */
    int getHp() const;
    /** @brief obtem o hp maximo @return valor maximo de hp */
    int getHpMaximo() const;
    /** @brief obtem o nivel atual @return valor do nivel */
    int getNivel() const;

    /** @brief altera o nome @param nome novo nome */
    void setNome(const std::string& nome);
    /** @brief altera o hp atual @param valor novo hp */
    void setHp(int valor);
    /** @brief altera o hp maximo @param valor novo hp maximo */
    void setHpMaximo(int valor);
    /** @brief altera o nivel @param valor novo nivel */
    void setNivel(int valor);

    /**
     * @brief checa se a entidade tem hp maior que zero
     * @return true se viva, false se morta
     */
    bool estaVivo() const;

    /**
     * @brief subtrai hp com base no dano
     * @param quantidade valor do dano
     */
    void receberDano(int quantidade);

    /**
     * @brief adiciona hp ate o limite maximo
     * @param quantidade valor da cura
     */
    void curar(int quantidade);

    /**
     * @brief sobrecarga de operador para impressao no console
     * @param os stream de saida
     * @param e entidade a ser impressa
     * @return stream formatada
     */
    friend std::ostream& operator<<(std::ostream& os, const Entidade& e);

    /** @brief metodo puro para exibir os dados detalhados */
    virtual void exibirStatus() const = 0;
    
    /** @brief metodo puro para calcular e executar um ataque @return dano gerado */
    virtual int atacar() = 0;
};

} // namespace rpg