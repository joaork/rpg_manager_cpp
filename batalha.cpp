#include "include/batalha.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <random>
#include <algorithm>

namespace rpg {

static std::mt19937 rng_batalha(std::random_device{}());

// --- estrategia agressiva: sempre ataca, usa habilidades ofensivas ---

AcaoBatalha EstrategiaAgressiva::escolherAcao(
    const Personagem& personagem, const Personagem& oponente) {
    (void)oponente;
    // usa habilidade se disponivel, caso contrario ataca
    for (int i = 0; i < 10; i++) {
        if (personagem.getMana() > 20) return AcaoBatalha::USAR_HABILIDADE;
    }
    return AcaoBatalha::ATACAR;
}

int EstrategiaAgressiva::escolherHabilidade(const Personagem& personagem) {
    (void)personagem; // diz ao compilador que o parâmetro é ignorado intencionalmente
    return 0;
}

// --- estrategia defensiva: cura quando hp estiver baixo ---

AcaoBatalha EstrategiaDefensiva::escolherAcao(
    const Personagem& personagem, const Personagem& oponente) {
    (void)oponente;
    // prioriza cura quando o hp cai abaixo de 40%
    if (personagem.getHp() < personagem.getHpMaximo() * 0.4) {
        auto& inv = const_cast<Personagem&>(personagem).getInventario();
        if (inv.buscarItem("pocao de cura maior") || inv.buscarItem("pocao de cura menor")) {
            return AcaoBatalha::USAR_ITEM;
        }
        return AcaoBatalha::DEFENDER;
    }
    return AcaoBatalha::ATACAR;
}

int EstrategiaDefensiva::escolherHabilidade(const Personagem& personagem) {
    (void)personagem; // diz ao compilador que o parâmetro é ignorado intencionalmente
    return 0;
}

// --- estrategia aleatoria ---

AcaoBatalha EstrategiaAleatoria::escolherAcao(
    const Personagem& personagem, const Personagem& oponente) {
    (void)oponente;
    std::uniform_int_distribution<int> dist(0, 3);
    int escolha = dist(rng_batalha);

    switch (escolha) {
        case 0: return AcaoBatalha::ATACAR;
        case 1: return (personagem.getMana() > 15)
                    ? AcaoBatalha::USAR_HABILIDADE : AcaoBatalha::ATACAR;
        case 2: return AcaoBatalha::DEFENDER;
        default: return AcaoBatalha::ATACAR;
    }
}

int EstrategiaAleatoria::escolherHabilidade(const Personagem& personagem) {
    (void)personagem; // diz ao compilador que o parâmetro é ignorado intencionalmente
    return 0;
}

// --- batalha ---

Batalha::Batalha(Personagem& j1, Personagem& j2, bool j2_e_bot,
                 std::unique_ptr<EstrategiaBatalha> estrategia)
    : jogador_um(j1), jogador_dois(j2),
      jogador_dois_e_bot(j2_e_bot),
      estrategia_bot(std::move(estrategia)),
      turno_atual(0),
      defesa_temporaria_j1(0), defesa_temporaria_j2(0) {

    // usa estrategia agressiva como padrao se nenhuma for fornecida
    if (j2_e_bot && !estrategia_bot) {
        estrategia_bot = std::make_unique<EstrategiaAgressiva>();
    }
}

void Batalha::exibirEstadoBatalha() const {
    std::cout << "\n---------- turno " << turno_atual << " ----------\n";
    std::cout << jogador_um.getNome()
              << " hp: " << jogador_um.getHp() << "/" << jogador_um.getHpMaximo()
              << " | mana: " << jogador_um.getMana() << "\n";
    std::cout << jogador_dois.getNome()
              << " hp: " << jogador_dois.getHp() << "/" << jogador_dois.getHpMaximo()
              << " | mana: " << jogador_dois.getMana() << "\n";
    std::cout << "-----------------------------------\n";
}

AcaoBatalha Batalha::solicitarAcaoHumano(Personagem& personagem) {
    std::cout << "\n" << personagem.getNome() << ", escolha sua acao:\n";
    std::cout << "  1. atacar\n";
    std::cout << "  2. usar habilidade\n";
    std::cout << "  3. usar item\n";
    std::cout << "  4. defender (-50% dano recebido este turno)\n";
    std::cout << "  5. fugir\n";
    std::cout << "opcao: ";

    int opcao = 1;
    std::cin >> opcao;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        opcao = 1;
    }

    switch (opcao) {
        case 1: return AcaoBatalha::ATACAR;
        case 2: return AcaoBatalha::USAR_HABILIDADE;
        case 3: return AcaoBatalha::USAR_ITEM;
        case 4: return AcaoBatalha::DEFENDER;
        case 5: return AcaoBatalha::FUGIR;
        default: return AcaoBatalha::ATACAR;
    }
}

int Batalha::solicitarHabilidadeHumano(Personagem& personagem) {
    personagem.listarHabilidades();
    std::cout << "escolha a habilidade (numero): ";
    int idx = 1;
    std::cin >> idx;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        idx = 1;
    }
    return idx - 1;
}

ResultadoTurno Batalha::executarAcao(Personagem& atacante, Personagem& alvo,
                                     AcaoBatalha acao, int indice_habilidade) {
    ResultadoTurno resultado;
    resultado.acao = acao;
    resultado.nome_atacante = atacante.getNome();
    resultado.nome_alvo = alvo.getNome();
    resultado.dano_causado = 0;
    resultado.dano_recebido = 0;
    resultado.fugiu = false;

    switch (acao) {
        case AcaoBatalha::ATACAR: {
            int dano = atacante.atacar();

            // aplica reducao de defesa se o alvo estiver defendendo
            bool alvo_e_j2 = (&alvo == &jogador_dois);
            int defesa_temp = alvo_e_j2 ? defesa_temporaria_j2 : defesa_temporaria_j1;

            int dano_real = std::max(1, dano - defesa_temp);
            alvo.receberDano(dano_real);
            resultado.dano_causado = dano_real;

            // reseta a defesa temporaria apos receber o golpe
            if (alvo_e_j2) defesa_temporaria_j2 = 0;
            else defesa_temporaria_j1 = 0;

            break;
        }

        case AcaoBatalha::USAR_HABILIDADE: {
            // calcula o atributo base dependendo da classe
            int atributo = 20; // valor padrao
            int resultado_hab = atacante.usarHabilidade(indice_habilidade, atributo);

            if (resultado_hab > 0) {
                bool alvo_e_j2 = (&alvo == &jogador_dois);
                int defesa_temp = alvo_e_j2 ? defesa_temporaria_j2 : defesa_temporaria_j1;
                int dano_real = std::max(0, resultado_hab - defesa_temp);
                if (dano_real > 0) {
                    alvo.receberDano(dano_real);
                    resultado.dano_causado = dano_real;
                } else {
                    // pode ser uma habilidade de cura ou defesa
                    atacante.curar(resultado_hab);
                }
            }
            break;
        }

        case AcaoBatalha::USAR_ITEM: {
            auto& inv = atacante.getInventario();
            // tenta usar qualquer pocao disponivel
            int cura = 0;
            if (inv.buscarItem("pocao de cura maior")) {
                cura = inv.usarPocao("pocao de cura maior");
            } else if (inv.buscarItem("pocao de cura menor")) {
                cura = inv.usarPocao("pocao de cura menor");
            } else if (inv.buscarItem("elixir de vida")) {
                cura = inv.usarPocao("elixir de vida");
            } else {
                std::cout << atacante.getNome() << " nao tem pocoes para usar.\n";
                // faz ataque basico como fallback
                int dano = atacante.atacar();
                alvo.receberDano(std::max(1, dano));
                resultado.dano_causado = dano;
            }

            if (cura > 0) {
                atacante.curar(cura);

                GerenciadorEventos::getInstance().publicarEvento(
                    Evento(TipoEvento::CURA_RECEBIDA, atacante.getNome(), "pocao", cura));
            }
            break;
        }

        case AcaoBatalha::DEFENDER: {
            // postura defensiva reduz o dano do proximo ataque
            bool atacante_e_j1 = (&atacante == &jogador_um);
            int bonus_defesa = 15;

            if (atacante_e_j1) {
                defesa_temporaria_j1 = bonus_defesa;
            } else {
                defesa_temporaria_j2 = bonus_defesa;
            }

            std::cout << atacante.getNome() << " assumiu postura defensiva.\n";
            resultado.descricao = "postura defensiva ativa";
            break;
        }

        case AcaoBatalha::FUGIR: {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(rng_batalha) > 0.5f) {
                std::cout << atacante.getNome() << " conseguiu fugir da batalha!\n";
                resultado.fugiu = true;
            } else {
                std::cout << atacante.getNome() << " tentou fugir mas falhou!\n";
                // falhar na fuga deixa o personagem vulneravel
                int dano = alvo.atacar();
                atacante.receberDano(dano);
                resultado.dano_recebido = dano;
            }
            break;
        }
    }

    return resultado;
}

Personagem* Batalha::iniciar() {
    GerenciadorEventos::getInstance().publicarEvento(
        Evento(TipoEvento::BATALHA_INICIADA, "",
               jogador_um.getNome() + " vs " + jogador_dois.getNome()));

    std::cout << "\n============================\n";
    std::cout << "  BATALHA: " << jogador_um.getNome()
              << " vs " << jogador_dois.getNome() << "\n";
    std::cout << "============================\n";

    // loop de batalha que continua ate um dos personagens cair
    while (jogador_um.estaVivo() && jogador_dois.estaVivo()) {
        turno_atual++;
        exibirEstadoBatalha();

        // atualiza cooldowns no inicio de cada turno
        jogador_um.atualizarCooldowns();
        jogador_dois.atualizarCooldowns();

        // turno do jogador 1
        std::cout << "\n>> vez de " << jogador_um.getNome() << "\n";
        AcaoBatalha acao_j1;
        int hab_j1 = 0;

        acao_j1 = solicitarAcaoHumano(jogador_um);
        if (acao_j1 == AcaoBatalha::USAR_HABILIDADE) {
            hab_j1 = solicitarHabilidadeHumano(jogador_um);
        }

        auto resultado_j1 = executarAcao(jogador_um, jogador_dois, acao_j1, hab_j1);
        log_batalha.push_back(resultado_j1);

        if (resultado_j1.fugiu) break;
        if (!jogador_dois.estaVivo()) break;

        // turno do jogador 2 (humano ou bot)
        std::cout << "\n>> vez de " << jogador_dois.getNome() << "\n";
        AcaoBatalha acao_j2;
        int hab_j2 = 0;

        if (jogador_dois_e_bot && estrategia_bot) {
            // o bot escolhe a acao automaticamente pela estrategia
            acao_j2 = estrategia_bot->escolherAcao(jogador_dois, jogador_um);
            if (acao_j2 == AcaoBatalha::USAR_HABILIDADE) {
                hab_j2 = estrategia_bot->escolherHabilidade(jogador_dois);
            }

            // exibe a acao do bot
            switch (acao_j2) {
                case AcaoBatalha::ATACAR:
                    std::cout << jogador_dois.getNome() << " [bot] escolheu atacar.\n"; break;
                case AcaoBatalha::USAR_HABILIDADE:
                    std::cout << jogador_dois.getNome() << " [bot] escolheu usar habilidade.\n"; break;
                case AcaoBatalha::DEFENDER:
                    std::cout << jogador_dois.getNome() << " [bot] escolheu defender.\n"; break;
                default:
                    std::cout << jogador_dois.getNome() << " [bot] escolheu atacar.\n";
                    acao_j2 = AcaoBatalha::ATACAR;
            }
        } else {
            acao_j2 = solicitarAcaoHumano(jogador_dois);
            if (acao_j2 == AcaoBatalha::USAR_HABILIDADE) {
                hab_j2 = solicitarHabilidadeHumano(jogador_dois);
            }
        }

        auto resultado_j2 = executarAcao(jogador_dois, jogador_um, acao_j2, hab_j2);
        log_batalha.push_back(resultado_j2);

        if (resultado_j2.fugiu) break;
    }

    // determina o vencedor
    Personagem* vencedor = nullptr;

    if (jogador_um.estaVivo() && !jogador_dois.estaVivo()) {
        vencedor = &jogador_um;
    } else if (!jogador_um.estaVivo() && jogador_dois.estaVivo()) {
        vencedor = &jogador_dois;
    } else if (jogador_um.estaVivo()) {
        // pode ter havido uma fuga
        vencedor = &jogador_um;
    }

    if (vencedor) {
        std::cout << "\n============================\n";
        std::cout << "  vencedor: " << vencedor->getNome() << "!\n";
        std::cout << "============================\n";

        // recompensa de xp pela batalha
        int xp_ganho = 50 * turno_atual;
        vencedor->ganharXp(xp_ganho);

        GerenciadorEventos::getInstance().publicarEvento(
            Evento(TipoEvento::BATALHA_ENCERRADA, vencedor->getNome(), "batalha encerrada"));
    } else {
        std::cout << "\n empate ou ambos os personagens fugiram.\n";
    }

    return vencedor;
}

void Batalha::exibirLog() const {
    std::cout << "\n--- log da batalha ---\n";
    for (const auto& r : log_batalha) {
        std::cout << r.nome_atacante;
        switch (r.acao) {
            case AcaoBatalha::ATACAR:
                std::cout << " atacou " << r.nome_alvo
                          << " causando " << r.dano_causado << " de dano.\n"; break;
            case AcaoBatalha::DEFENDER:
                std::cout << " entrou em postura defensiva.\n"; break;
            case AcaoBatalha::FUGIR:
                std::cout << (r.fugiu ? " fugiu da batalha.\n" : " tentou fugir mas falhou.\n");
                break;
            default:
                std::cout << " realizou uma acao.\n";
        }
    }
}

int Batalha::getTurnoAtual() const { return turno_atual; }

} // namespace rpg
