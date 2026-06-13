#pragma once
#include "personagem.h"
#include <string>
#include <vector>
#include <memory>
#include <fstream>

namespace rpg {

/**
 * @brief sistema de persistencia de dados
 * * gerencia a escrita e leitura dos arquivos fisicos de save json.
 */
class SistemaArquivo {
private:
    std::string diretorio_saves; /**< pasta alvo no disco rigido */

    /** @brief trata caracteres invalidos @param str texto sujo @return texto limpo */
    std::string escaparString(const std::string& str) const;
    /** @brief encontra valor na string json @param json texto completo @param chave campo procurado @return valor cortado */
    std::string extrairValor(const std::string& json, const std::string& chave) const;

public:
    /**
     * @brief construtor com definicao de destino
     * @param dir caminho relativo para a pasta de saves (padrao = saves/)
     */
    explicit SistemaArquivo(const std::string& dir = "saves/");

    /**
     * @brief grava os dados do heroi no disco
     * @param personagem entidade a ser serializada
     * @param nome_arquivo string com nome do save
     * @return verdadeiro se a operacao de i/o teve sucesso
     */
    bool salvarPersonagem(const Personagem& personagem, const std::string& nome_arquivo) const;

    /**
     * @brief remonta a classe a partir do disco
     * @param nome_arquivo string com nome do save
     * @return ponteiro instanciado ou nulo se houver falha
     */
    std::unique_ptr<Personagem> carregarPersonagem(const std::string& nome_arquivo) const;

    /** @brief le a pasta no disco @return array de nomes de saves */
    std::vector<std::string> listarSaves() const;

    /** @brief apaga do disco @param nome_arquivo string de identificacao @return verdadeiro se apagado */
    bool removerSave(const std::string& nome_arquivo) const;

    /** @brief verifica o disco @param nome_arquivo string de identificacao @return verdadeiro se encontrado */
    bool saveExiste(const std::string& nome_arquivo) const;

    /**
     * @brief backup global do estado (funcao expansivel)
     * @param personagem heroi
     * @param dados_mapa arvore json do ambiente
     * @param dados_quests arvore json das missoes
     * @param nome_arquivo nome fisico no disco
     * @return verdadeiro se gravou por completo
     */
    bool salvarJogoCompleto(const Personagem& personagem,
                            const std::string& dados_mapa,
                            const std::string& dados_quests,
                            const std::string& nome_arquivo) const;

    /** @brief le caminho alvo @return pasta raiz dos backups */
    std::string getDiretorio() const;
};

} // namespace rpg