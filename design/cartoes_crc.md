# Cartões CRC - Jogo PDS

## Classe: Jogador
- **Responsabilidades:**
  1. Armazenar posição, velocidade e estado (no chão, no ar, grande, pequeno).
  2. Atualizar a posição do personagem com base nas entradas e na gravidade.
  3. Gerenciar vidas, pontuação e estado atual de power-ups.
  4. Tratar o recebimento de dano e tempo de invulnerabilidade.
  5. Atualizar e selecionar o quadro de animação ativo.
- **Colaboradores:** Entidade, GerenciadorColisao, GerenciadorInput.

## Classe: Inimigo
- **Responsabilidades:**
  1. Armazenar o tipo, posição e direção do movimento do inimigo.
  2. Realizar a movimentação de patrulha alternando a direção em obstáculos.
  3. Detectar a área de colisão do topo da cabeça vs. colisão lateral.
  4. Gerenciar a animação de eliminação e desativação da entidade.
  5. Definir a pontuação concedida ao jogador ao ser derrotado.
- **Colaboradores:** Entidade, GerenciadorColisao, Jogador.

## Classe: GerenciadorColisao
- **Responsabilidades:**
  1. Verificar interseções entre a caixa de colisão do jogador e as plataformas.
  2. Detectar e resolver colisões entre Jogador e Inimigos.
  3. Tratar a colisão do Jogador com itens coletáveis (moedas e power-ups).
  4. Impedir que entidades atravessem paredes e pisos sólidos.
  5. Notificar as entidades sobre os pontos exatos do impacto.
- **Colaboradores:** Jogador, Inimigo, Mapa, Item.