#include <gtest/gtest.h>
#include "../include/personagem.h"
#include "../include/inventario.h"
#include "../include/item.h"

using namespace rpg;

// TESTES DE ENTIDADE E PERSONAGEM

TEST(PersonagemTest, CriacaoEAtributosIniciais) {
    auto guerreiro = PersonagemFactory::criarGuerreiro("Arthur", "humano");
    
    EXPECT_EQ(guerreiro->getNome(), "Arthur");
    EXPECT_EQ(guerreiro->getClasse(), "guerreiro");
    EXPECT_EQ(guerreiro->getNivel(), 1);
    EXPECT_TRUE(guerreiro->estaVivo());
}

TEST(PersonagemTest, ReceberDanoECurar) {
    auto mago = PersonagemFactory::criarMago("Merlin", "elfo");
    int hp_inicial = mago->getHpMaximo();
    
    // teste de dano
    mago->receberDano(20);
    EXPECT_EQ(mago->getHp(), hp_inicial - 20);
    
    // teste de cura parcial
    mago->curar(10);
    EXPECT_EQ(mago->getHp(), hp_inicial - 10);
    
    // teste de cura além do limite não podeultrapassar o HP max
    mago->curar(500);
    EXPECT_EQ(mago->getHp(), hp_inicial);
}

TEST(PersonagemTest, MorteDoPersonagem) {
    auto ladrao = PersonagemFactory::criarLadrao("Loki", "hobbit");
    
    ladrao->receberDano(9999); // dano massivo
    
    EXPECT_EQ(ladrao->getHp(), 0);
    EXPECT_FALSE(ladrao->estaVivo());
}

TEST(PersonagemTest, GanhoDeXpESubidaDeNivel) {
    auto clerigo = PersonagemFactory::criarClerigo("Luz", "humano");
    int nivel_inicial = clerigo->getNivel();
    int xp_necessario = clerigo->getXpProximoNivel();
    
    clerigo->ganharXp(xp_necessario + 10); // passa do limite do nivel 1
    
    EXPECT_GT(clerigo->getNivel(), nivel_inicial); // verifica se o nivel é maior que o inicial
}

// TESTES DE INVENTARIO E ITENS 

TEST(InventarioTest, AdicionarItemLimitePeso) {
    Inventario inv(10, 10.0f); // capacidade 10, peso max de 10kg
    
    auto espada_pesada = std::make_shared<Arma>("Espadao", "Pesada", 8.0f, 100, 30, "espada");
    auto bigorna = std::make_shared<ItemEspecial>("Bigorna", "Muito pesada", 5.0f, 10, "peso", 0);
    
    // adiciona o primeiro item (8kg) 
    EXPECT_TRUE(inv.adicionarItem(espada_pesada));
    
    // tenta adicionar o segundo item (5kg) - deve falhar pois passa dos 10kg
    EXPECT_FALSE(inv.adicionarItem(bigorna));
}

TEST(InventarioTest, EquiparArmaVerificacao) {
    Inventario inv(5, 50.0f);
    auto espada = std::make_shared<Arma>("Excalibur", "Lendaria", 3.0f, 1000, 50, "espada");
    auto pocao = std::make_shared<Pocao>("Cura", "Cura hp", 0.5f, 20, 50, "hp");
    
    inv.adicionarItem(espada);
    inv.adicionarItem(pocao);
    
    // Deve equipar a arma com sucesso
    EXPECT_TRUE(inv.equiparArma("Excalibur"));
    
    // Não deve equipar uma poção como arma
    EXPECT_FALSE(inv.equiparArma("Cura"));
}

// ENTRADA DO GTEST 
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}