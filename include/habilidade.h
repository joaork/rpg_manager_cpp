#pragma once
#include <string>
#include <memory>
#include <iostream>

namespace rpg {

/**
 * @brief classe abstrata para habilidades ativas de combate
 * * gerencia custos de mana e tempo de recarga (cooldown).
 */
class Habilidade {
protected:
    std::string nome;       /**< titulo da habilidade */
    std::string descricao;  /**< funcionamento do efeito */
    int custo_mana;         /**< mana consumida ao utilizar */
    int cooldown_maximo;    /**< turnos necessarios para recarregar */
    int cooldown_atual;     /**< turnos restantes para liberar uso */
    int nivel_habilidade;   /**< grau de forca da habilidade */

public:
    /**
     * @brief constroi os parametros de mana e tempo
     * @param nome titulo da habilidade
     * @param desc descricao de uso
     * @param custo_mana exigencia de mana
     * @param cooldown espera maxima
     * @param nivel forca inerente (padrao = 1)
     */
    Habilidade(const std::string& nome, const std::string& desc,
               int custo_mana, int cooldown, int nivel = 1);
               
    virtual ~Habilidade() = default;

    /** @brief retorna titulo @return string de nome */
    std::string getNome() const;
    /** @brief retorna efeito @return string descritiva */
    std::string getDescricao() const;
    /** @brief retorna exigencia de mana @return valor do custo */
    int getCustoMana() const;
    /** @brief retorna espera atual @return turnos restantes */
    int getCooldownAtual() const;
    /** @brief retorna grau de forca @return nivel */
    int getNivelHabilidade() const;

    /**
     * @brief checa se a recarga chegou a zero
     * @return verdadeiro se puder usar
     */
    bool estaDisponivel() const;

    /** @brief decrementa os turnos de espera em um */
    void reduzirCooldown();

    /**
     * @brief executa a formula de sucesso da habilidade
     * @param atributo_base poder de forca ou inteligencia do personagem
     * @return valor final gerado de dano ou cura
     */
    virtual int executar(int atributo_base) = 0;
    
    /** @brief define categoria @return classificacao da habilidade */
    virtual std::string getTipo() const = 0;
    
    /** @brief lista os dados no formato de menu */
    virtual void exibirInfo() const;
};

/** @brief habilidade feita para subtrair hp do adversario */
class HabilidadeOfensiva : public Habilidade {
private:
    float multiplicador_dano; /**< potencializa o dano base */

public:
    HabilidadeOfensiva(const std::string& nome, const std::string& desc,
                       int custo_mana, int cooldown, float mult_dano, int nivel = 1);

    int executar(int atributo_base) override;
    std::string getTipo() const override;
};

/** @brief habilidade focada em prevencao de dano e protecao */
class HabilidadeDefensiva : public Habilidade {
private:
    float multiplicador_defesa; /**< fator de mitigacao de ataque */

public:
    HabilidadeDefensiva(const std::string& nome, const std::string& desc,
                        int custo_mana, int cooldown, float mult_defesa, int nivel = 1);

    int executar(int atributo_base) override;
    std::string getTipo() const override;
};

/** @brief habilidade focada em restauracao de status e saude */
class HabilidadeSuporte : public Habilidade {
private:
    int quantidade_cura; /**< valor padrao somado ao escalonamento */

public:
    HabilidadeSuporte(const std::string& nome, const std::string& desc,
                      int custo_mana, int cooldown, int cura, int nivel = 1);

    int executar(int atributo_base) override;
    std::string getTipo() const override;
};

} // namespace rpg