#include "include/arquivo.h"
#include <filesystem>
#include <iostream>

namespace rpg {

SistemaArquivo::SistemaArquivo(const std::string& dir) : diretorio_saves(dir) {
    // requer C++17 para std::filesystem
    if (!std::filesystem::exists(diretorio_saves)) {
        std::filesystem::create_directories(diretorio_saves);
    }
}

std::string SistemaArquivo::getDiretorio() const {
    return diretorio_saves;
}

bool SistemaArquivo::salvarPersonagem(const Personagem& personagem, const std::string& nome_arquivo) const {
    std::string caminho = diretorio_saves + nome_arquivo + ".json";
    std::ofstream arquivo(caminho);
    
    if (!arquivo.is_open()) {
        std::cout << "Erro ao abrir arquivo para salvar: " << caminho << "\n";
        return false;
    }

    arquivo << personagem.serializar();
    arquivo.close();
    std::cout << "Jogo salvo com sucesso em " << caminho << "\n";
    return true;
}

std::unique_ptr<Personagem> SistemaArquivo::carregarPersonagem(const std::string& nome_arquivo) const {
    std::string caminho = diretorio_saves + nome_arquivo + ".json";
    std::ifstream arquivo(caminho);
    
    if (!arquivo.is_open()) {
        std::cout << "Save nao encontrado: " << caminho << "\n";
        return nullptr;
    }

    // carrega todo o texto do JSON para a string
    std::string conteudo((std::istreambuf_iterator<char>(arquivo)), std::istreambuf_iterator<char>());
    arquivo.close();
    
    // extrai os dados principais usando nossa função auxiliar
    std::string nome = extrairValor(conteudo, "nome");
    std::string classe = extrairValor(conteudo, "classe");
    std::string raca = extrairValor(conteudo, "raca");
    
    // validação de segurança caso o save esteja corrompido
    if (nome.empty() || classe.empty() || raca.empty()) {
        std::cout << "Erro: Arquivo de save corrompido ou formato invalido.\n";
        return nullptr;
    }

    // factory para instanciar a classe correta
    std::unique_ptr<Personagem> p;
    if (classe == "guerreiro") p = PersonagemFactory::criarGuerreiro(nome, raca);
    else if (classe == "mago") p = PersonagemFactory::criarMago(nome, raca);
    else if (classe == "arqueiro") p = PersonagemFactory::criarArqueiro(nome, raca);
    else if (classe == "ladrao") p = PersonagemFactory::criarLadrao(nome, raca);
    else if (classe == "clerigo") p = PersonagemFactory::criarClerigo(nome, raca);
    else p = PersonagemFactory::criarGuerreiro(nome, "humano"); // fallback de seguranca
    
    // extrai e aplica os atributos numéricos (nível e HP)
    std::string nivel_str = extrairValor(conteudo, "nivel");
    std::string hp_max_str = extrairValor(conteudo, "hp_maximo");
    std::string hp_str = extrairValor(conteudo, "hp");

    if (!nivel_str.empty()) p->setNivel(std::stoi(nivel_str));
    if (!hp_max_str.empty()) p->setHpMaximo(std::stoi(hp_max_str));
    if (!hp_str.empty()) p->setHp(std::stoi(hp_str));

    std::cout << "Jogo carregado com sucesso! Bem-vindo de volta, " << nome << ".\n";
    return p;
}

std::vector<std::string> SistemaArquivo::listarSaves() const {
    std::vector<std::string> saves;
    for (const auto& entry : std::filesystem::directory_iterator(diretorio_saves)) {
        if (entry.path().extension() == ".json") {
            saves.push_back(entry.path().stem().string());
        }
    }
    return saves;
}

bool SistemaArquivo::removerSave(const std::string& nome_arquivo) const {
    std::string caminho = diretorio_saves + nome_arquivo + ".json";
    return std::filesystem::remove(caminho);
}

bool SistemaArquivo::saveExiste(const std::string& nome_arquivo) const {
    std::string caminho = diretorio_saves + nome_arquivo + ".json";
    return std::filesystem::exists(caminho);
}

bool SistemaArquivo::salvarJogoCompleto(const Personagem& personagem, const std::string& dados_mapa,
                                        const std::string& dados_quests, const std::string& nome_arquivo) const {
    (void)dados_mapa;
    (void)dados_quests;
    return salvarPersonagem(personagem, nome_arquivo);
}

std::string SistemaArquivo::extrairValor(const std::string& json, const std::string& chave) const {
    // procura a chave no formato JSON gerado
    std::string chave_busca = "\"" + chave + "\": ";
    size_t pos_chave = json.find(chave_busca);
    if (pos_chave == std::string::npos) return "";

    size_t pos_inicio = pos_chave + chave_busca.length();
    bool is_string = (json[pos_inicio] == '"');
    
    if (is_string) {
        // se for string, pega o que está entre as aspas
        pos_inicio++; 
        size_t pos_fim = json.find('"', pos_inicio);
        if (pos_fim != std::string::npos) {
            return json.substr(pos_inicio, pos_fim - pos_inicio);
        }
    } else {
        // se for número, lê até a vírgula ou quebra de linha
        size_t pos_fim = json.find_first_of(",\n\r}", pos_inicio);
        if (pos_fim != std::string::npos) {
            return json.substr(pos_inicio, pos_fim - pos_inicio);
        }
    }
    return "";
}

} // namespace rpg