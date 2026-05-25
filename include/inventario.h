#pragma once
#include "item.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

namespace rpg {

/**
 * @brief gerencia os itens e a carga de um personagem
 * * possui limites de slots e de peso total.
 */
class Inventario {
private:
    std::vector<std::shared_ptr<Item>> itens; /**< lista de itens armazenados */
    int capacidade_maxima;                    /**< limite de espacos (slots) */
    float peso_maximo;                        /**< limite de carga em kg */
    float peso_atual;                         /**< carga atual armazenada */

    std::shared_ptr<Arma> arma_equipada;         /**< slot da arma principal */
    std::shared_ptr<Armadura> armadura_equipada; /**< slot de protecao corporal */

public:
    /**
     * @brief constroi o inventario com limites
     * @param capacidade total de itens permitidos (padrao = 20)
     * @param peso_max carga suportada em kg (padrao = 50.0f)
     */
    explicit Inventario(int capacidade = 20, float peso_max = 50.0f);

    /**
     * @brief tenta inserir um item respeitando limites
     * @param item ponteiro para o item a ser guardado
     * @return verdadeiro se inserido com sucesso
     */
    bool adicionarItem(std::shared_ptr<Item> item);

    /**
     * @brief busca e deleta um item pelo nome
     * @param nome_item string de busca
     * @return verdadeiro se encontrou e removeu
     */
    bool removerItem(const std::string& nome_item);

    /**
     * @brief pesquisa um item sem remove-lo
     * @param nome_item string de busca
     * @return ponteiro do item ou nulo se nao achar
     */
    std::shared_ptr<Item> buscarItem(const std::string& nome_item) const;

    /**
     * @brief define uma arma como ativa
     * @param nome_arma nome do equipamento
     * @return verdadeiro se equipou
     */
    bool equiparArma(const std::string& nome_arma);

    /**
     * @brief define uma armadura como ativa
     * @param nome_armadura nome do equipamento
     * @return verdadeiro se equipou
     */
    bool equiparArmadura(const std::string& nome_armadura);

    /** @brief retira a arma do slot ativo */
    void desequiparArma();
    /** @brief retira a armadura do slot ativo */
    void desequiparArmadura();

    /**
     * @brief consome uma pocao e aplica o efeito
     * @param nome_pocao string de busca
     * @return valor de cura ou recuperacao retornado
     */
    int usarPocao(const std::string& nome_pocao);

    /** @brief le a arma ativa @return ponteiro para arma */
    std::shared_ptr<Arma> getArmaEquipada() const;
    /** @brief le a armadura ativa @return ponteiro para armadura */
    std::shared_ptr<Armadura> getArmaduraEquipada() const;
    /** @brief le a carga atual @return valor em kg */
    float getPesoAtual() const;
    /** @brief le a carga suportada @return limite em kg */
    float getPesoMaximo() const;
    /** @brief le o uso de slots @return quantidade de itens */
    int getQuantidadeItens() const;
    /** @brief le limite de slots @return capacidade maxima */
    int getCapacidadeMaxima() const;
    /** @brief checa se nao cabem mais itens @return logico de lotacao */
    bool estaCheia() const;

    /** @brief lista os itens visualmente no terminal */
    void listarItens() const;

    /**
     * @brief insere item via operador
     * @param item ponteiro do item
     * @return referencia do proprio inventario
     */
    Inventario& operator+=(std::shared_ptr<Item> item);

    /** @brief exporta bagagem @return string serializada */
    std::string serializar() const;
    /** @brief importa bagagem @param dados string json formatada */
    void deserializar(const std::string& dados);
};

} // namespace rpg