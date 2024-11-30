#ifndef POKEDEX_H_
#define POKEDEX_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "csv.h"

typedef struct pokedex pokedex_t;
typedef struct jugador jugador_t;
typedef struct pokemon {
    char* nombre;
    size_t puntos;
    char* color;
    char* patron_movimiento;
    int x;  // Cambiado a size_t
    int y;  // Cambiado a size_t
    size_t iteracion;
}pokemon_t;

/**
 * Crea una pokedex_t.
 */
pokedex_t *pokedex_crear();

/**
 * Destruye la pokedex aplicando el destructor a los pokemones del usuario.
 */
void pokedex_destruir_todo(pokedex_t *pokedex);

/**
 * Inserta el pokemon. Devuelve true si pudo o false si no pudo.
 */
bool pokedex_insertar(pokedex_t *pokedex, pokemon_t *pokemon);

void pokedex_insertar_desde_archivo(struct archivo_csv* archivo, pokedex_t* pokedex);

/**
 * Quita el pokemon buscado de la pokedex. Si lo encuentra y encontrado no es NULL,
 * almacena el puntero.
 *
 * Devuelve true si pudo quitar el pokemon.
 */
bool pokedex_quitar(pokedex_t *pokedex, pokemon_t *buscado, pokemon_t **encontrado);

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
 * Recorre los pokemones de la pokedex en el orden específico y aplica la función f a
 * cada uno.
 *
 * Si la función f devuelve false, se deja de iterar.
 *
 * Devuelve la cantidad de veces que fue invocada la función f.
 */
size_t pokedex_iterar(pokedex_t *pokedex, bool (*f)(void *, void *), void *ctx);

bool pokedex_imprimir(void* ctx);

/**
 * Rellena el vector de punteros con los pokemones de la pokedex siguiendo un orden
 * dado. Tamaño indica la capacidad del vector.
 *
 * Devuelve la cantidad de pokemones guardados en el vector.
 */
size_t pokedex_vectorizar(pokedex_t *pokedex, pokemon_t **pokemones, size_t tamaño);

#endif // POKEDEX_H_
