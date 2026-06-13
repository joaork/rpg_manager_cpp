#pragma once
#include "item.h"
#include "inventario.h"
#include <vector>
#include <string>
#include <map>
#include <memory>

namespace rpg {

/** @brief materia prima exigida pelo caldeirao */
struct Ingrediente {
    std::string nome_item; /**< nome generico de quem procurar */
    int quantidade;        /**< peso em copias exigido */

    /** @brief constroi o requerimento */
    Ingrediente(const std::string& nome, int qtd)
        : nome_item(nome), quantidade(qtd) {}
};

/** @brief bula ou diagrama de formacao de novos loots */
class Receita {
private:
    std::string id;                       /**< hash de referencia */
    std::string nome;                     /**< titulo visivel no menu */
    std::vector<Ingrediente> ingredientes;/**< formula quimica / lista de compras */
    std::string item_resultado;           /**< que nome tera a saida final */
    std::string tipo_resultado;           /**< para qual factory chamar depois */

public:
    /**
     * @brief cadastra o blueprint
     * @param id registro unico
     * @param nome titulo
     * @param resultado nome para o novo loot
     * @param tipo_resultado classe base
     */
    Receita(const std::string& id, const std::string& nome,
            const std::string& resultado, const std::string& tipo_resultado);

    std::string getId() const;
    std::string getNome() const;
    std::string getItemResultado() const;
    std::string getTipoResultado() const;
    const std::vector<Ingrediente>& getIngredientes() const;

    /** @brief cadastra na lista uma exigencia nova @param nome nome do item @param quantidade soma */
    void adicionarIngrediente(const std::string& nome, int quantidade);
    
    /** @brief analisa se a mochila do heroi bate com o diagrama @param inv classe inventario com estado de itens @return verdadeiro se possivel */
    bool podeSerFeita(const Inventario& inv) const;
    
    /** @brief exibe como produzir na tela */
    void exibirInfo() const;
};

/** @brief orquestra a interacao da bagagem com a forja */
class SistemaCrafting {
private:
    std::map<std::string, std::shared_ptr<Receita>> receitas; /**< livros de diagrama globais */

public:
    /** @brief cataloga novo conhecimento no sistema @param receita pointer pro objeto instanciado */
    void registrarReceita(std::shared_ptr<Receita> receita);

    /**
     * @brief queima os ingredientes e invoca factory pro resultado
     * @param id_receita key string
     * @param inventario referencia real para subtrair e somar os pointers
     * @return condicional se a transacao do mercado deu certo
     */
    bool fabricar(const std::string& id_receita, Inventario& inventario);

    /** @brief dump cego do dicionario de forja */
    void listarReceitas() const;
    /** @brief filtra os prints testando o inv antes @param inventario classe mochila */
    void listarReceitasDisponiveis(const Inventario& inventario) const;

    /** @brief pega o diagrama @param id hash local @return memoria ponteiro */
    std::shared_ptr<Receita> getReceita(const std::string& id) const;

    /** @brief chama o mock default de testes na memoria */
    void carregarReceitasPadrao();
};

} // namespace rpg