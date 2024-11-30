#ifndef UTILS_H_
#define UTILS_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "ctype.h"

#define ANCHO_TABLERO 32
#define ALTO_TABLERO 15
#define SEGUNDOS_DE_JUEGO 30
#define CANTIDAD_POKEMONES_A_AGREGAR 7

int max(int a, int b);

int min(int a, int b);

char *copiar(const char *s);

int generar_posicion_random(int limite);

bool leer_nombre(const char *str, void *ctx);

bool leer_int(const char *str, void *ctx);

char* obtener_color_ansi(char* color);

void destruir_pokemon(void *pokemon_void);

#endif // UTILS_H_
