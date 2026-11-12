# PDS_II
Super PDS Bros

Jogo de plataforma 2D feito em C++11 com a biblioteca Raylib, para a disciplina de PDS II. O jogador atravessa fases pulando, atirando e desviando de inimigos, e enfrenta um boss na última fase.

Como compilar e jogar (Windows)

Requisitos: MinGW (g++ e mingw32-make). A Raylib já vem estática em lib/libraylib.a.

mingw32-make          # compila e gera build/jogo_executavel.exe
mingw32-make run      # compila e abre o jogo
mingw32-make test     # compila e roda os testes (doctest)
mingw32-make clean    # apaga a pasta build

Rode o jogo sempre a partir da raiz do projeto, pois os caminhos de Sprites/ e design/ são relativos:

.\build\jogo_executavel.exe
Controles
Ação	Teclas
Mover	Setas ou A / D
Pular	Espaço ou W
Atirar	Z
Pausar / continuar	P ou Esc
Navegar no menu	Setas ou W / S
Confirmar	Enter
Tela cheia	F11
Como jogar
Pise nos inimigos ou atire neles para derrotá-los. Tocar neles de lado tira vida (ou encolhe o jogador).
Chegue à bandeira antes do tempo acabar. Sobrar tempo dá pontos de bônus.
Moedas valem pontos, e 100 moedas dão uma vida extra.
O cogumelo faz o jogador crescer, e a estrela dá invencibilidade por alguns segundos.
A última fase é a arena do boss. Ele atira bolas de fogo alternando entre alto (passe por baixo) e baixo (pule por cima), e fica mais rápido conforme perde vida.
O recorde é salvo em design/data/recorde.txt.
Estrutura do projeto
src/            código-fonte (.cpp)
include/        cabeçalhos (.hpp)
design/fases/   mapas das fases (fase1.txt, fase2.txt, ...)
design/data/    recorde salvo
Sprites/        imagens (PNG) do jogo
lib/            Raylib estática
tests/          testes automatizados
Makefile
Principais classes
Classe	Papel
Jogo	Janela, laço principal, estados (menu, jogando, pausado...) e câmera
EntityData	Classe base de todas as entidades (posição, vida, desenho de sprites)
Jogador	Movimento, pulo, tiro, vidas, pontos e estados Pequeno/Grande
Inimigo	Andarilho e Escudo (patrulham); Boss e TiroBoss ficam no mesmo arquivo
Item	Classe abstrata: Moeda, Cogumelo e Estrela com efeitos próprios
Mapa	Lê as fases, desenha os tiles e as plataformas móveis e frágeis
GerenciadorColisao / GerenciadorInput	Colisões e teclado
Hud	Vidas, moedas, pontos, fase e tempo
Criando fases

Cada fase é um arquivo de texto em design/fases/faseN.txt. O jogo conta as fases em sequência (fase1, fase2, ...) e a última é tratada como a fase do boss. Todas as linhas devem ter o mesmo comprimento, com exatamente um @ e ao menos um F.

# Símbolo	Significado
.	Vazio
"#"	Chão
=	Bloco / plataforma fixa
F	Bandeira (fim da fase)
@	Início do jogador
g	Andarilho
k	Escudo
B	Boss
o	Moeda
m	Cogumelo
s	Estrela
h	Plataforma móvel horizontal (uma sequência de h forma uma plataforma)
v	Plataforma móvel vertical (uma sequência de v forma uma plataforma)
~	Plataforma frágil: some meio segundo depois de pisada e volta depois

Cada tile tem 32 × 32 px.

# Sprites

As imagens ficam em Sprites/. Se um arquivo não for encontrado, o jogo usa um desenho simples no lugar e avisa no terminal.

Arquivo	Uso
Mauro_1.png, Mauro_2.png, Jogador pequeno e grande
Inimigo_1.png, Inimigo_2.png	Andarilho e Escudo
boss.png, Ataque_boss.png	Boss e seu tiro
Tiro_Mauro.png	Tiro do jogador
Moeda.png, Cogumelo.png	Itens
Chao-1.png, Chao-2.png, Bloco.png, Plataforma_move.png	Cenário
Titulo.png, Fundo_fase.png, Fundo_boss.png	Tela de título e fundos

# Equipe

Eduardo Rodrigues - 2026020471
Mateus Paulo - 2026019970
Felipe Alves - 2026020331
Lucas Gabriel - 2026066650
Rafael Alves - 2026019821