#ifndef UTILS_H_
#define UTILS_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *copiar(const char *s);

int generar_posicion_random(int limite);

bool leer_nombre(const char *str, void *ctx);

bool leer_int(const char *str, void *ctx);

char* obtener_color_ansi(char* color);

void destruir_pokemon(void *pokemon_void);

#endif // UTILS_H_
