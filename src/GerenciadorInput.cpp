/**
 * @file GerenciadorInput.cpp
 * @brief Implementação dos métodos da classe GerenciadorInput utilizando a alib.
 */

#include "GerenciadorInput.hpp"
#include "alib.h"

void GerenciadorInput::processarEntradas() {
    // Processamento de entradas utilizando as funções disponibilizadas pela alib
    // (Adicione aqui a lógica de leitura de eventos caso a alib gerencie filas ou estados)
}

bool GerenciadorInput::isTeclaPressionada(int codigoTecla) const {
    // Delega a verificação da tecla para a alib ou utiliza a estrutura padrão suportada por ela
    return alib::isTeclaPressionada(codigoTecla); // Ajuste conforme a assinatura exata das funções da alib no projeto
}