#include "pokedex.h"
#include "abb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../extra/ansi.h"


pokedex_t* pokedex_crear(int (*comparador)(void *, void *)) {
    return (pokedex_t*)abb_crear(comparador);
}

void pokedex_destruir(pokedex_t *pokedex) {
    abb_destruir((abb_t*)pokedex);
}

void pokedex_destruir_todo(pokedex_t* pokedex, void (*destructor)(void *)) {
    abb_destruir_todo((abb_t*)pokedex, destructor);
}

bool pokedex_insertar(pokedex_t* pokedex, pokemon_t* pokemon) {
    return abb_insertar((abb_t*)pokedex, pokemon);
}
bool pokedex_quitar(pokedex_t* pokedex, pokemon_t* buscado, pokemon_t** encontrado) {
    return abb_quitar((abb_t*)pokedex, buscado, (void**)encontrado);
}

void* pokedex_obtener(pokedex_t* pokedex, pokemon_t* pokemon) {
    return abb_obtener((abb_t*)pokedex,pokemon);
}

size_t pokedex_cantidad(pokedex_t* pokedex){
    return abb_cantidad((abb_t*)pokedex);
}

size_t pokedex_iterar(pokedex_t* pokedex,  bool (*f)(void *, void *), void* ctx) {
    printf("Pokemones conocidos:\n");
    return abb_iterar_inorden((abb_t*)pokedex, f, ctx);
}

size_t pokedex_vectorizar(pokedex_t* pokedex, pokemon_t** vector,size_t tamaño){
    return abb_vectorizar_inorden((abb_t*)pokedex, (void**)vector, tamaño);
}