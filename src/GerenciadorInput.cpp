/**
 * @file GerenciadorInput.cpp
 * @brief Implementação dos métodos da classe GerenciadorInput.
 */

#include "GerenciadorInput.hpp"
#include <allegro5/allegro.h> // Incluído caso utilize Allegro para gerenciar eventos de teclado

// Exemplo de implementação padrão para verificar se uma tecla está pressionada
bool GerenciadorInput::isTeclaPressionada(int codigoTecla) const {
    // Implementação utilizando o estado do teclado do Allegro ou lógica customizada
    // Exemplo genérico: alState guarda o estado atual das teclas
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);
    return al_key_down(&keyState, codigoTecla);
}