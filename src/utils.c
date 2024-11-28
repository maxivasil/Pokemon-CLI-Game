#include "utils.h"
#include "../extra/ansi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

char *copiar(const char *s)
{
	size_t len = strlen(s) + 1;
	char *copia = malloc(len);
	if (copia) {
		memcpy(copia, s, len);
	}
	return copia;
}

size_t generar_posicion_random(size_t limite)
{
    return (size_t)rand() % limite;
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

// --- Función para obtener colores ANSI ---
char* obtener_color_ansi(char* color) {
    switch (color[0]) { // Usamos el primer carácter del nombre para el switch
		case 'A':  // AZUL O AMARILLO
            if (strcmp(color, "AZUL") == 0) return ANSI_COLOR_BLUE;
			if (strcmp(color, "AMARILLO") == 0) return ANSI_COLOR_YELLOW;
            break;
		case 'B':  // AZUL O AMARILLO
            if (strcmp(color, "BLANCO") == 0) return ANSI_COLOR_BLACK;
            break;
		case 'C':  // AZUL O AMARILLO
            if (strcmp(color, "CYAN") == 0) return ANSI_COLOR_CYAN;
            break;
        case 'M':  // MAGENTA
            if (strcmp(color, "MAGENTA") == 0) return ANSI_COLOR_MAGENTA;
            break;
		case 'N':  // AZUL O AMARILLO
            if (strcmp(color, "NEGRO") == 0) return ANSI_COLOR_BLACK;
            break;
		case 'R':  // ROJO
            if (strcmp(color, "ROJO") == 0) return ANSI_COLOR_RED;
            break;
		case 'V':  // VERDE
            if (strcmp(color, "VERDE") == 0) return ANSI_COLOR_GREEN;
            break;
    }
    return ANSI_COLOR_RESET; //si no se encuentra el color
}