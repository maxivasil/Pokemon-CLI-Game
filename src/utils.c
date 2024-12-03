#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "pokedex.h"

char *copiar(const char *s)
{
	size_t len = strlen(s) + 1;
	char *copia = malloc(len);
	if (copia)
		memcpy(copia, s, len);
	return copia;
}

void destruir_pokemon(void *pokemon_void)
{
	if (!pokemon_void)
		return;
	pokemon_t *pokemon = (pokemon_t *)pokemon_void;
	if (pokemon->nombre)
		free(pokemon->nombre);
	if (pokemon->patron_movimiento)
		free(pokemon->patron_movimiento);
	free(pokemon);
}