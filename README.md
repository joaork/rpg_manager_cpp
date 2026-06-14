# RPG Manager
Este projeto foi desenvolvido para avaliação da disciplina de Programação Orientada a Objetos do curso de Engenharia de Computação da Universidade Tecnológica Federal do Paraná (UTFPR).

## Funcionalidades

* **Sistema de Personagens:** Criação de heróis com escolhas de classes (Guerreiro, Mago, Arqueiro, Ladrão, Clérigo) e raças (Humano, Elfo, Anão, Orc, Hobbit), cada uma com atributos e habilidades únicas utilizando Polimorfismo.
* **Inventário e Equipamentos:** Gerenciamento dinâmico de itens, controle de peso/capacidade, e mecânica de equipar/desequipar armas e armaduras com reflexo imediato nos atributos do herói.
* **Combate por Turnos:** Sistema de batalha com o padrão *Strategy*, permitindo que inimigos automatizados tomem decisões em tempo de execução (Estratégias Agressiva, Defensiva e Aleatória).
* **Geração de Mapa Procedural:** Exploração de masmorras geradas dinamicamente com salas de monstros, chefes e baús de tesouro.
* **Sistema de Crafting:** Combinação de itens do inventário para forjar novos equipamentos ou poções através de um sistema de verificação de receitas.
* **Gerenciador de Missões (Quests):** Rastreamento de objetivos em tempo real com entrega automática de recompensas de ouro e XP.
* **Persistência de Dados:** Salvamento e carregamento do estado completo do jogador (atributos, itens e equipamentos) utilizando serialização em formato de texto estruturado.

## Padrões de Projeto
**Factory Method:** Centraliza a instanciação de Personagens e Itens genéricos baseados em *strings*, escondendo a complexidade de alocação de memória do código cliente.
**Observer:** Implementado no `GerenciadorEventos` para desacoplar a lógica central do jogo das saídas de terminal (`LoggerBatalha` e `NotificadorNivel` atuam como ouvintes passivos).
**Strategy:** Utilizado na injeção de dependência de comportamentos de Inteligência Artificial para os inimigos nas batalhas.
**Singleton:** Garante que exista apenas um canal de barramento de eventos rodando durante a execução do jogo.

* **Linguagem:** C++
* **Testes Unitários:** Google Test (gtest) cobrindo regras críticas de negócio e restrições matemáticas.
* **Compilador Testado:** GCC/G++ (via w64devkit/MinGW)

## Compilação e execução

Certifique-se de ter um compilador C++ (como o `g++`) instalado e configurado nas variáveis de ambiente.

1. Clone o repositório e acesse a pasta do projeto:
```bash
git clone https://github.com/joaork/rpg_manager_cpp.git
cd rpg_manager_cpp

```


2. Crie a pasta de saída (se não existir):
```bash
mkdir output

```


3. Compile todos os arquivos fonte juntos:
```bash
g++ -Wall -Wextra -g3 *.cpp -o output\main.exe

```


4. Execute o jogo:
```bash
.\output\main.exe

```



## Testes

Para executá-los:

1. Navegue até o diretório de testes.
2. Link o código-fonte ao framework GTest durante a compilação (requer libgtest instalada no ambiente).
3. Execute o binário gerado para validar a integridade dos módulos de `Personagem` e `Inventario`.
