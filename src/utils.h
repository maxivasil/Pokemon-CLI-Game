#ifndef UTILS_H_
#define UTILS_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ANCHO_TABLERO 32
#define ALTO_TABLERO 15
#define SEGUNDOS_DE_JUEGO 40
#define CANTIDAD_POKEMONES_A_AGREGAR 7

size_t max(size_t a, size_t b);

size_t min(size_t a, size_t b);

char *copiar(const char *s);

size_t generar_posicion_random(size_t limite);

bool leer_nombre(const char *str, void *ctx);

bool leer_int(const char *str, void *ctx);

char* obtener_color_ansi(char* color);

#endif // UTILS_H_
