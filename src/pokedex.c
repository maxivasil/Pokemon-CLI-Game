#include "pokedex.h"
#include "abb.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../extra/ansi.h"

bool leer_nombre(const char *str, void *ctx)
{
	char *nuevo = malloc(strlen(str) + 1);
	if (nuevo == NULL)
		return false;
	strcpy(nuevo, str);
	*(char **)ctx = nuevo;
	return true;
}

bool leer_int(const char *str, void *ctx)
{
	return sscanf(str, "%d", (int *)ctx) == 1;
}

char *obtener_color_ansi(char *color)
{
	switch (color[0]) {
	case 'A':
		if (strcmp(color, "AZUL") == 0)
			return ANSI_COLOR_BLUE;
		if (strcmp(color, "AMARILLO") == 0)
			return ANSI_COLOR_YELLOW;
		break;
	case 'B':
		if (strcmp(color, "BLANCO") == 0)
			return ANSI_COLOR_BLACK;
		break;
	case 'C':
		if (strcmp(color, "CYAN") == 0)
			return ANSI_COLOR_CYAN;
		break;
	case 'M':
		if (strcmp(color, "MAGENTA") == 0)
			return ANSI_COLOR_MAGENTA;
		break;
	case 'N':
		if (strcmp(color, "NEGRO") == 0)
			return ANSI_COLOR_BLACK;
		break;
	case 'R':
		if (strcmp(color, "ROJO") == 0)
			return ANSI_COLOR_RED;
		break;
	case 'V':
		if (strcmp(color, "VERDE") == 0)
			return ANSI_COLOR_GREEN;
		break;
	}
	return ANSI_COLOR_RESET;
}

bool imprimir_pokemon(void *elemento, void *ctx)
{
	if (!elemento)
		return false;

	pokemon_t *pokemon = (pokemon_t *)elemento;

	char *nombre = pokemon->nombre;
	size_t puntos = pokemon->puntos;
	char *color = pokemon->color;
	char *patron = pokemon->patron_movimiento;

	printf(ANSI_COLOR_BOLD "%s %s " ANSI_COLOR_RESET "(%s) " ANSI_COLOR_BOLD
			       "→ %ld puntos\n" ANSI_COLOR_RESET,
	       color, nombre, patron, puntos);
	return true;
}

int comparar_nombre_pokemon(void *poke1, void *poke2)
{
	pokemon_t *pokemon1 = (pokemon_t *)poke1;
	pokemon_t *pokemon2 = (pokemon_t *)poke2;

	return strcmp(pokemon1->nombre, pokemon2->nombre);
}

pokedex_t *pokedex_crear()
{
	return (pokedex_t *)abb_crear(comparar_nombre_pokemon);
}

void pokedex_destruir(pokedex_t *pokedex)
{
	abb_destruir_todo((abb_t *)pokedex, destruir_pokemon);
}

bool pokedex_insertar(pokedex_t *pokedex, pokemon_t *pokemon)
{
	if (!pokemon)
		return false;
	return abb_insertar((abb_t *)pokedex, pokemon);
}

bool pokedex_insertar_desde_archivo(struct archivo_csv *archivo,
				    pokedex_t *pokedex)
{
	if (!archivo || !pokedex)
		return false;
	bool (*funciones[])(const char *, void *) = { leer_nombre, leer_int,
						      leer_nombre,
						      leer_nombre };
	char *nombre = NULL;
	size_t puntos = 0;
	char *color = NULL;
	char *patron_mov = NULL;

	void *punteros[] = { &nombre, &puntos, &color, &patron_mov };

	while (leer_linea_csv(archivo, 5, funciones, punteros) > 0) {
		pokemon_t *pokemon_leido = malloc(sizeof(struct pokemon));
		if (!pokemon_leido) {
			free(nombre);
			free(color);
			free(patron_mov);
			return false;
		}

		pokemon_leido->nombre = nombre;
		pokemon_leido->puntos = (size_t)puntos;

		if (patron_mov) {
			size_t len = strlen(patron_mov);
			if (len > 0 && patron_mov[len - 1] == '\n')
				patron_mov[len - 1] = '\0';
		}
		pokemon_leido->color = obtener_color_ansi(color);
		free(color);
		pokemon_leido->patron_movimiento = patron_mov;
		pokedex_insertar(pokedex, pokemon_leido);
	}
	return true;
}

void *pokedex_obtener(pokedex_t *pokedex, pokemon_t *pokemon)
{
	return abb_obtener((abb_t *)pokedex, pokemon);
}

size_t pokedex_cantidad(pokedex_t *pokedex)
{
	return abb_cantidad((abb_t *)pokedex);
}

size_t pokedex_iterar(pokedex_t *pokedex, bool (*f)(void *, void *), void *ctx)
{
	return abb_iterar_inorden((abb_t *)pokedex, f, ctx);
}

bool pokedex_imprimir(void *ctx)
{
	if (!ctx)
		return false;
	pokedex_t *pokedex = (pokedex_t *)ctx;
	if (pokedex_cantidad(pokedex) == 0) {
		printf("No hay pokemones conocidos :(\n");
		return false;
	}
	printf("Pokemones conocidos:\n");
	abb_iterar_inorden((abb_t *)pokedex, imprimir_pokemon, NULL);
	return true;
}
