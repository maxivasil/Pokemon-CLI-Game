#include "pokedex.h"
#include "abb.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../extra/ansi.h"


bool imprimir_pokemon(void *elemento, void *ctx)
{
	if (!elemento)
		return false;

	pokemon_t *pokemon = (pokemon_t *)elemento;

	char *nombre = pokemon->nombre;
    size_t puntos = pokemon->puntos;
    char *color = pokemon->color;
    char *patron = pokemon->patron_movimiento;

    printf(ANSI_COLOR_BOLD "%s %s " ANSI_COLOR_RESET "(%s) " ANSI_COLOR_BOLD "→ %ld puntos\n" ANSI_COLOR_RESET, color, nombre, patron,puntos);  // Imprimir con color
    return true;
}

int comparar_nombre_pokemon(void *poke1, void *poke2)
{
	pokemon_t *pokemon1 = (pokemon_t *)poke1;
	pokemon_t *pokemon2 = (pokemon_t *)poke2;

	return strcmp(pokemon1->nombre, pokemon2->nombre);
}

void mezclar_vector(pokemon_t** vector, size_t n) {
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t) rand() % (i + 1); // genera un índice aleatorio entre 0 e i
        pokemon_t* temp = vector[i];
        vector[i] = vector[j];
        vector[j] = temp;
    }
}


pokedex_t* pokedex_crear() {
    return (pokedex_t*)abb_crear(comparar_nombre_pokemon);
}

void pokedex_destruir_todo(pokedex_t* pokedex) {
    abb_destruir_todo((abb_t*)pokedex, destruir_pokemon);
}

bool pokedex_insertar(pokedex_t* pokedex, pokemon_t* pokemon) {
    return abb_insertar((abb_t*)pokedex, pokemon);
}

void pokedex_insertar_desde_archivo(struct archivo_csv* archivo, pokedex_t* pokedex) {
    	bool (*funciones[])(const char *, void *) = { leer_nombre,
						      leer_int, leer_nombre, leer_nombre};
	char *nombre = NULL;
	size_t puntos = 0;
	char *color= NULL;
	char *patron_mov = NULL;

	void *punteros[] = { &nombre, &puntos, &color, &patron_mov};

	while (leer_linea_csv(archivo, 5, funciones, punteros) > 0) {
		pokemon_t *pokemon_leido = malloc(sizeof(struct pokemon));
		if (!pokemon_leido) {
			free(nombre); 
            free(color);  //VER SI ESTO HACE FALTA 
            free(patron_mov);
			break;
		}

		pokemon_leido->nombre = nombre;
		pokemon_leido->puntos = (size_t)puntos;

		if (patron_mov) {
            size_t len = strlen(patron_mov);
            if (len > 0 && patron_mov[len - 1] == '\n') {
                patron_mov[len - 1] = '\0'; // saco el '\n'
            }
        }
        pokemon_leido->color = obtener_color_ansi(color);
		free(color);

		pokemon_leido->patron_movimiento = patron_mov;
		pokemon_leido->x = generar_posicion_random(ANCHO_TABLERO);
		pokemon_leido->y = generar_posicion_random(ALTO_TABLERO);
		pokemon_leido->iteracion = 0;
		pokedex_insertar(pokedex, pokemon_leido);
	}
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
    
    return abb_iterar_inorden((abb_t*)pokedex, f, ctx);
}

void pokedex_imprimir(void* ctx) {
    pokedex_t* pokedex = (pokedex_t*)ctx; 
    printf("Pokemones conocidos:\n");
    abb_iterar_inorden((abb_t*)pokedex, imprimir_pokemon, NULL);
}

size_t pokedex_vectorizar(pokedex_t* pokedex, pokemon_t** vector,size_t tamaño){
    size_t vectorizados = abb_vectorizar_inorden((abb_t*)pokedex, (void**)vector, tamaño);
    mezclar_vector(vector, vectorizados);
    return vectorizados;
}