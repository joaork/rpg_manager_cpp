#include "include/menu.h"
#include <iostream>
#include <limits>

namespace rpg {

MenuPrincipal::MenuPrincipal() : jogo_em_execucao(true), ouro_jogador(100) {
    inicializarSistemas();
}

MenuPrincipal::~MenuPrincipal() = default;

void MenuPrincipal::inicializarSistemas() {
    GerenciadorEventos::getInstance().registrarObserver(&logger);
    GerenciadorEventos::getInstance().registrarObserver(&notificador_nivel);
    gerenciador_quests.carregarQuestsPadrao();
    sistema_crafting.carregarReceitasPadrao();
    mapa = std::make_unique<Mapa>();
    mapa->gerarAndar(1);
}

void MenuPrincipal::limparTela() const {
    // comando para limpeza visual de console
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void MenuPrincipal::aguardarEnter() const {
    std::cout << "\nPressione Enter para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int MenuPrincipal::lerInteiro(const std::string& prompt, int min, int max) const {
    int valor;
    while (true) {
        std::cout << prompt;
        if (std::cin >> valor && valor >= min && valor <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return valor;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida. Digite um numero entre " << min << " e " << max << ".\n";
    }
}

std::string MenuPrincipal::lerString(const std::string& prompt) const {
    std::string valor;
    std::cout << prompt;
    std::getline(std::cin, valor);
    return valor;
}

void MenuPrincipal::executar() {
    while (jogo_em_execucao) {
        limparTela();
        std::cout << "=== RPG MANAGER ===\n";
        std::cout << "1. Novo Jogo\n";
        std::cout << "2. Carregar Jogo\n";
        std::cout << "3. Sair\n";
        
        int opcao = lerInteiro("Escolha uma opcao: ", 1, 3);
        
        if (opcao == 1) {
            menuCriarPersonagem();
            if (personagem_atual) menuPrincipalJogo();
        } else if (opcao == 2) {
            menuCarregar();
            if (personagem_atual) menuPrincipalJogo();
        } else {
            jogo_em_execucao = false;
        }
    }
}

void MenuPrincipal::menuCriarPersonagem() {
    limparTela();
    std::cout << "--- Criacao de Personagem ---\n";
    std::string nome = lerString("Nome do heroi: ");
    
    std::cout << "\nRacas disponiveis:\n1. Humano\n2. Elfo\n3. Anao\n4. Orc\n5. Hobbit\n";
    int raca_op = lerInteiro("Escolha a raca (1-5): ", 1, 5);
    std::string racas[] = {"humano", "elfo", "anao", "orc", "hobbit"};
    
    std::cout << "\nClasses disponiveis:\n1. Guerreiro\n2. Mago\n3. Arqueiro\n4. Ladrao\n5. Clerigo\n";
    int classe_op = lerInteiro("Escolha a classe (1-5): ", 1, 5);
    std::string classes[] = {"guerreiro", "mago", "arqueiro", "ladrao", "clerigo"};

    if (classes[classe_op-1] == "guerreiro") personagem_atual = PersonagemFactory::criarGuerreiro(nome, racas[raca_op-1]);
    else if (classes[classe_op-1] == "mago") personagem_atual = PersonagemFactory::criarMago(nome, racas[raca_op-1]);
    else if (classes[classe_op-1] == "arqueiro") personagem_atual = PersonagemFactory::criarArqueiro(nome, racas[raca_op-1]);
    else if (classes[classe_op-1] == "ladrao") personagem_atual = PersonagemFactory::criarLadrao(nome, racas[raca_op-1]);
    else if (classes[classe_op-1] == "clerigo") personagem_atual = PersonagemFactory::criarClerigo(nome, racas[raca_op-1]);
    
    std::cout << "\nPersonagem criado com sucesso!\n";
    aguardarEnter();
}

void MenuPrincipal::menuPrincipalJogo() {
    bool voltando = false;
    while (!voltando && personagem_atual->estaVivo()) {
        limparTela();
        std::cout << "=== MENU PRINCIPAL ===\n";
        std::cout << "Heroi: " << personagem_atual->getNome() << " | Nivel: " << personagem_atual->getNivel() << "\n";
        std::cout << "1. Status do Personagem\n";
        std::cout << "2. Inventario\n";
        std::cout << "3. Explorar Mapa\n";
        std::cout << "4. Salvar Jogo\n";
        std::cout << "5. Voltar ao Menu Inicial\n";
        
        int opcao = lerInteiro("Opcao: ", 1, 5);
        
        switch (opcao) {
            case 1: 
                limparTela(); 
                personagem_atual->exibirStatus(); 
                aguardarEnter(); 
                break;
            case 2: 
                limparTela(); 
                personagem_atual->getInventario().listarItens(); 
                aguardarEnter(); 
                break;
            case 3: 
                menuExplorar(); 
                break;
            case 4: 
                menuSalvar(); 
                break;
            case 5: 
                voltando = true; 
                break;
        }
    }
}

void MenuPrincipal::menuExplorar() {
    bool explorando = true;

    while (explorando && personagem_atual->estaVivo()) {
        limparTela();
        std::cout << "=========================================\n";
        std::cout << "          EXPLORACAO DE DUNGEON          \n";
        std::cout << "=========================================\n";
        
        auto sala_atual = mapa->getSalaAtual();
        if (!sala_atual) break;
        
        // exibe as informações básicas da sala (lore, saídas conhecidas)
        mapa->exibirSalaAtual();

        if (!sala_atual->estaLimpa()) {
            auto& inimigos = sala_atual->getInimigos();
            bool combate_ocorreu = false;

            for (auto& ini : inimigos) {
                if (!ini.derrotado) {
                    std::cout << "\n[!] ATENCAO: Um " << ini.nome << " (Classe: " 
                              << ini.classe << " | Nivel: " << ini.nivel << ") bloqueia o seu caminho!\n";
                    aguardarEnter();

                    // instancia um personagem temporario para a batalha usando o Factory do projeto
                    std::unique_ptr<Personagem> inimigo_bot;
                    
                    if (ini.classe == "mago") {
                        inimigo_bot = PersonagemFactory::criarMago(ini.nome, "elfo");
                    } else if (ini.classe == "ladrao") {
                        inimigo_bot = PersonagemFactory::criarLadrao(ini.nome, "hobbit");
                    } else if (ini.classe == "clerigo") {
                        inimigo_bot = PersonagemFactory::criarClerigo(ini.nome, "humano");
                    } else { // guerreiro ou fallback
                        inimigo_bot = PersonagemFactory::criarGuerreiro(ini.nome, "orc");
                    }

                    // sincroniza os atributos gerados proceduralmente pelo mapa no objeto de batalha
                    inimigo_bot->setNivel(ini.nivel);
                    inimigo_bot->setHpMaximo(ini.hp);
                    inimigo_bot->setHp(ini.hp);

                    // inicializa o loop de batalha por turnos com a estrategia agressiva padrao
                    Batalha loop_combate(*personagem_atual, *inimigo_bot, true, std::make_unique<EstrategiaAgressiva>());
                    Personagem* vencedor = loop_combate.iniciar();

                    // processa o desfecho da batalha
                    if (vencedor == personagem_atual.get()) {
                        std::cout << "\n[SUCESSO] Voce derrotou o " << ini.nome << "!\n";
                        
                        // concede as recompensas específicas registradas na geracao da sala
                        personagem_atual->ganharXp(ini.xp_recompensa);
                        ouro_jogador += ini.ouro_recompensa;
                        std::cout << "[RECOMPENSA] +" << ini.ouro_recompensa << " moedas de ouro adicionadas ao inventario.\n";
                        
                        ini.derrotado = true;
                        combate_ocorreu = true;
                        aguardarEnter();
                    } else {
                        std::cout << "\n[DERROTA] Voce sucumbiu em batalha...\n";
                        aguardarEnter();
                        return; // retorna ao menu inicial pois o personagem morreu
                    }
                }
            }

            // se passamos por todos os inimigos da lista e os derrotamos, a sala agora esta segura
            if (combate_ocorreu) {
                sala_atual->marcarLimpa();
                std::cout << "\n[INFO] Todas as ameacas da sala foram eliminadas! O ambiente agora esta seguro.\n";
                aguardarEnter();
                continue; // reinicia o loop para atualizar a exibicao da sala limpa
            }
        }

        auto& tesouros = sala_atual->getTesouros();
        for (auto& t : tesouros) {
            if (!t.coletado) {
                std::cout << "\n=========================================\n";
                std::cout << "  [BAU] Voce encontrou um bau de tesouro!  \n";
                std::cout << "=========================================\n";
                
                if (t.ouro > 0) {
                    ouro_jogador += t.ouro;
                    std::cout << "  * Ouro encontrado: +" << t.ouro << "g\n";
                }
                
                if (!t.nome_item.empty()) {
                    std::shared_ptr<Item> item_gerado = nullptr;
                    
                    // identifica o tipo do item e invoca a respectiva factory correspondente
                    if (t.tipo_item == "arma") item_gerado = ItemFactory::criarArma(t.nome_item);
                    else if (t.tipo_item == "armadura") item_gerado = ItemFactory::criarArmadura(t.nome_item);
                    else if (t.tipo_item == "pocao") item_gerado = ItemFactory::criarPocao(t.nome_item);
                    else if (t.tipo_item == "especial") item_gerado = ItemFactory::criarItemEspecial(t.nome_item);

                    if (item_gerado) {
                        std::cout << "  * Item encontrado: " << item_gerado->getNome() << "\n";
                        personagem_atual->getInventario().adicionarItem(item_gerado);
                    }
                }
                t.coletado = true;
                aguardarEnter();
            }
        }

        const auto& saidas = sala_atual->getSaidas();

        std::cout << "\n--- ACOES DISPONIVEIS ---\n";
        std::cout << "0. Voltar para o acampamento (Menu Principal)\n";
        std::cout << "1. Consultar o mapa do andar\n";
        
        int index_opcao = 2;
        for (int id_saida : saidas) {
            std::cout << index_opcao << ". Mover-se para a Sala " << id_saida << "\n";
            index_opcao++;
        }

        int opcao = lerInteiro("\nEscolha para onde ir: ", 0, index_opcao - 1);

        if (opcao == 0) {
            explorando = false;
        } else if (opcao == 1) {
            limparTela();
            mapa->exibirMapa();
            aguardarEnter();
        } else {
            int id_destino = saidas[opcao - 2];
            limparTela();
            mapa->moverPara(id_destino);
            aguardarEnter();
        }
    }
}

void MenuPrincipal::menuSalvar() {
    std::string nome_save = lerString("Digite o nome do save: ");
    sistema_arquivo.salvarPersonagem(*personagem_atual, nome_save);
    aguardarEnter();
}

void MenuPrincipal::menuCarregar() {
    auto saves = sistema_arquivo.listarSaves();
    if (saves.empty()) {
        std::cout << "Nenhum save encontrado.\n";
        aguardarEnter();
        return;
    }
    std::cout << "Saves disponiveis:\n";
    for (size_t i = 0; i < saves.size(); ++i) {
        std::cout << i + 1 << ". " << saves[i] << "\n";
    }
    int op = lerInteiro("Escolha o save para carregar (0 para cancelar): ", 0, saves.size());
    if (op > 0) {
        personagem_atual = sistema_arquivo.carregarPersonagem(saves[op - 1]);
    }
}

} // namespace rpg