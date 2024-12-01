#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "pokedex.h"
#include "../extra/ansi.h"

char *copiar(const char *s)
{
	size_t len = strlen(s) + 1;
	char *copia = malloc(len);
	if (copia)
		memcpy(copia, s, len);
	return copia;
}

int generar_posicion_random(int limite)
{
    return (int)rand() % limite;
}

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

char* obtener_color_ansi(char* color) {
    switch (color[0]) {
		case 'A':
            if (strcmp(color, "AZUL") == 0) return ANSI_COLOR_BLUE;
			if (strcmp(color, "AMARILLO") == 0) return ANSI_COLOR_YELLOW;
            break;
		case 'B':
            if (strcmp(color, "BLANCO") == 0) return ANSI_COLOR_BLACK;
            break;
		case 'C':
            if (strcmp(color, "CYAN") == 0) return ANSI_COLOR_CYAN;
            break;
        case 'M':
            if (strcmp(color, "MAGENTA") == 0) return ANSI_COLOR_MAGENTA;
            break;
		case 'N':
            if (strcmp(color, "NEGRO") == 0) return ANSI_COLOR_BLACK;
            break;
		case 'R':
            if (strcmp(color, "ROJO") == 0) return ANSI_COLOR_RED;
            break;
		case 'V':
            if (strcmp(color, "VERDE") == 0) return ANSI_COLOR_GREEN;
            break;
    }
    return ANSI_COLOR_RESET;
}

void destruir_pokemon(void *pokemon_void) {
    if (!pokemon_void) 
        return;
    pokemon_t *pokemon = (pokemon_t *)pokemon_void;
    free(pokemon->nombre);
    free(pokemon->patron_movimiento);
    free(pokemon);
}