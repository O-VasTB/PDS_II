#include "Jogo.hpp"

int main() {
    const int larguraTela = 1280;
    const int alturaTela = 720;
    InitWindow(larguraTela, alturaTela, "Jogo Trabalho Prático PDSII");

    SetTargetFPS(60);

    Jogo jogo;

    while (!WindowShouldClose()) {
        

        jogo.atualizar();


        BeginDrawing();
        ClearBackground(RAYWHITE);

        jogo.desenhar();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}