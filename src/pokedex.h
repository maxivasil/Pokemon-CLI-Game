#ifndef POKEDEX_H_
#define POKEDEX_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "csv.h"

typedef struct pokedex pokedex_t;
typedef struct pokemon {
    char* nombre;
    size_t puntos;
    char* color;
    char* patron_movimiento;
}pokemon_t;

/**
 * Crea una pokedex_t.
 */
pokedex_t *pokedex_crear();

/**
 * Destruye la pokedex aplicando el destructor a los pokemones del usuario.
 * No devuelve nada.
 */
void pokedex_destruir(pokedex_t *pokedex);

/**
 * Inserta el pokemon. Devuelve true si pudo o false si no pudo.
 */
bool pokedex_insertar(pokedex_t *pokedex, pokemon_t *pokemon);

/**
 * Inserta todos los pokemones de un archivo csv en la pokedex. Recibe un 
 * puntero a un struct archivo_csv y el puntero a la pokedex. No devuelve nada.
 */
void pokedex_insertar_desde_archivo(struct archivo_csv* archivo, pokedex_t* pokedex);

/**
 * Busca un pokemon en la pokedex. Si lo encuentra lo devuelve. Caso contrario
 * devuelve NULL.
 */
void *pokedex_obtener(pokedex_t *pokedex, pokemon_t *pokemon);

/**
 * Devuelve la cantidad de pokemones en la pokedex.
 */
size_t pokedex_cantidad(pokedex_t *pokedex);

/**
 * Recorre los pokemones de la pokedex y aplica la función f a cada uno.
 * Si la función f devuelve false, se deja de iterar.
 * Devuelve la cantidad de veces que fue invocada la función f.
 */
size_t pokedex_iterar(pokedex_t *pokedex, bool (*f)(void *, void *), void *ctx);

/**
 * Imprime por pantalla los pokemones de la pokedex. Devuelve false
 * si no hay pokemones en la pokedex o el ctx es nulo. Sino devuelve true.
 */
bool pokedex_imprimir(void* ctx);

#endif // POKEDEX_H_
