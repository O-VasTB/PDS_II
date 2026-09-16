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

## US03: Interação e Derrota de Inimigos
**Descrição:** Como jogador, quero que inimigos sejam derrotados, no momento de contato, ao pular na cabeça deles.
**Critérios de Aceitação:**
- Pular sobre a cabeça do inimigo o elimina no contato e dá um pequeno impulso ao jogador.
- Colidir lateralmente com o inimigo faz o jogador perder uma vida ou perder o power-up/diminuir de tamanho, caso tenha algum item.
- O inimigo se move em patrulha entre os limites da plataforma, deslocando-se somente no eixo x.
- Ao ser derrotado, o inimigo emite um som e toca uma animação de eliminação.

## US04: Sistema de Vidas e Game Over
**Descrição:** Como jogador, quero que exista um limite de vidas, que podem ser portadas, para haver desafio durante a partida.
**Critérios de Aceitação:**
- O jogador inicia a partida com 3 vidas.
- Ao cair em buracos ou perder todos os power-up carregados, perde 1 vida e reinicia a fase do ponto de salvamento mais próximo.
- Se o número de vidas chegar a 0, exibe a tela de Game Over.
- A tela de Game Over permite reiniciar a partida do início da fase ou voltar ao menu principal.

## US05: Power-ups (Cogumelo / Flor)
**Descrição:** Como jogador, quero coletar itens para aumentar minha resistência e habilidades.
**Critérios de Aceitação:**
- Atingir blocos "?" por baixo faz o power-up surgir.
- O Cogumelo aumenta o tamanho do jogador e permite absorver um golpe.
- O visual do personagem muda para a forma "Super".
- O item desliza pelas plataformas até atingir o jogador ou um obstáculo.

## US06: Menu Principal e Pausa
**Descrição:** Como jogador, quero um menu principal para iniciar e um menu de pausa durante o jogo.
**Critérios de Aceitação:**
- O menu principal oferece opções para "Iniciar Jogo", "Instruções" e "Sair".
- Pressionar ESC ou P pausa a física e movimentação do jogo.
- O menu de pausa permite retomar a partida ou voltar ao menu principal.
- O áudio de fundo entra em estado de pausa junto com a física.
