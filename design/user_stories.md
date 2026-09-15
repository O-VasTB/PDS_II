# User Stories - Jogo PDS

## US01: Movimentação Básica
**Descrição:** Como jogador, quero ser capaz de me mover no eixo X livremente e no eixo Y com saltos.
**Critérios de Aceitação:**
- Pressionar A\D move o personagem no eixo X.
- Pressionar espaço o personagem pular se este estiver tocando o chão.
- O personagem deve ter uma animação para demonstrar que esta andando e/ou pulando.
- Na ausência de upgrades temporários, o personagem é incapaz de dar saltos duplos.

## US02: Coleta de Moedas
**Descrição:** Como jogador, quero coletar moedas espalhadas pela fase para converter em itens ao final de cada fase e melhorar a pontuação.
**Critérios de Aceitação:**
- Moedas devem ser geradas aleatoriamente pelo mapa
- A moeda desaparece imediatamente após a colisão com o jogador.
- O HUD do personagem é atualizado instantaneamente ao coletar uma moeda.
- Quando uma moeda é coletada um som de confirmação deve ser tocado.
- Upgrades podem ser comprados com as moedas coletadas.