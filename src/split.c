#include "split.h"
#include <stdlib.h>
#include <string.h>

/**
 * Copia un substring en una nueva ubicación de memoria.
 */
char *copiar_substring(const char *substring, size_t longitud_substring)
{
	char *copia = malloc(longitud_substring + 1);
	if (!copia) {
		return NULL;
	}
	strncpy(copia, substring, longitud_substring);
	copia[longitud_substring] = '\0';
	return copia;
}

/**
 * Agrega una nueva parte al struct Partes, redimensionando el array de strings.
 */
int agregar_parte(struct Partes *resultado, const char *caracter_inicial,
		  size_t longitud)
{
	char **temp = realloc(resultado->string,
			      (resultado->cantidad + 1) * sizeof(char *));
	if (!temp) {
		return -1;
	}
	resultado->string = temp;
	resultado->string[resultado->cantidad] =
		copiar_substring(caracter_inicial, longitud);
	if (!resultado->string[resultado->cantidad]) {
		return -1;
	}
	resultado->cantidad++;
	return 0;
}

struct Partes *dividir_string(const char *string, char separador)
{
	struct Partes *resultado = malloc(sizeof(struct Partes));
	if (!resultado) {
		return NULL;
	}

	resultado->cantidad = 0;
	resultado->string = NULL;

	if (string == NULL) {
		return resultado;
	}

	const char *caracter_inicial = string;
	const char *caracter_actual = string;

	while (*caracter_actual) {
		if (*caracter_actual == separador) {
			size_t longitud =
				(size_t)(caracter_actual - caracter_inicial);
			if (agregar_parte(resultado, caracter_inicial,
					  longitud) == -1) {
				liberar_partes(resultado);
				return NULL;
			}
			caracter_inicial = caracter_actual + 1;
		}
		caracter_actual++;
	}

	if (agregar_parte(resultado, caracter_inicial,
			  strlen(caracter_inicial)) == -1) {
		liberar_partes(resultado);
		return NULL;
	}

	return resultado;
}

void liberar_partes(struct Partes *partes)
{
	if (!partes) {
		return;
	}

	for (size_t i = 0; i < partes->cantidad; i++) {
		free(partes->string[i]);
	}

	free(partes->string);
	free(partes);
}