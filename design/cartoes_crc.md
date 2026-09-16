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

## Classe: Mapa
- **Responsabilidades:**
  1. Carregar a estrutura e matriz de blocos (chão, tijolos, canos).
  2. Armazenar as propriedades físicas de cada bloco (sólido, destruível, transponível).
  3. Desenhar na tela apenas os blocos visíveis dentro da câmera.
  4. Alterar o estado de blocos atingidos (quebrar tijolo, ativar bloco '?').
  5. Gerenciar os limites de largura e altura da fase.
- **Colaboradores:** GerenciadorColisao, GerenciadorRenderizacao

## Classe: GerenciadorRenderizacao
- **Responsabilidades:**
  1. Inicializar e manter a janela principal do jogo via biblioteca gráfica.
  2. Controlar a taxa de atualização da tela (60 FPS).
  3. Controlar a posição da câmera (View) para seguir o jogador.
  4. Renderizar plano de fundo, mapa, inimigos, jogador e HUD.
  5. Exibir os elementos de texto da interface (vidas, moedas, pontuação).
- **Colaboradores:** Jogador, Mapa, HUD

## Classe: Jogo
- **Responsabilidades:**
  1. Inicializar todos os sub-sistemas (Renderização, Áudio, Recursos).
  2. Manter o laço principal (Game Loop) executando (Eventos -> Update -> Render).
  3. Gerenciar as transições de telas e estados (Menu, Fase, Pausa, Game Over).
  4. Processar eventos globais do sistema (fechar janela, redimensionar).
  5. Controlar o tempo decorrido (Delta Time) para garantir física fluida.
- **Colaboradores:** GerenciadorRenderizacao, GerenciadorColisao, Jogador, Mapa