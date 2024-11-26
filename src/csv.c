#include "csv.h"
#include "split.h"
#include <stdio.h>
#include <string.h>
#define tamaño_array 450

struct archivo_csv {
	FILE *archivo_direccion;
	char separador;
};

///////////////////////////////////////////////////////////////////////////
////////////////////////FUNCIONES AUXILIARES///////////////////////////////
///////////////////////////////////////////////////////////////////////////

size_t aplicar_funciones_en_linea_csv(struct Partes *columnas_array,
				      size_t columnas_a_leer,
				      bool (*funciones[])(const char *, void *),
				      void *ctx[])
{
	size_t columnas_leidas_correctamente = 0;
	for (size_t indice_columna = 0; indice_columna < columnas_a_leer;
	     indice_columna++) {
		if (funciones[indice_columna] == NULL)
			return columnas_leidas_correctamente;
		char *contenido_columna_actual =
			columnas_array->string[indice_columna];
		bool estado = funciones[indice_columna](
			contenido_columna_actual, ctx[indice_columna]);
		if (!estado)
			return columnas_leidas_correctamente;
		columnas_leidas_correctamente++;
	}
	return columnas_leidas_correctamente;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

struct archivo_csv *abrir_archivo_csv(const char *nombre_archivo,
				      char separador)
{
	struct archivo_csv *archivo =
		(struct archivo_csv *)malloc(sizeof(struct archivo_csv));
	if (archivo == NULL)
		return NULL;

	archivo->archivo_direccion = fopen(nombre_archivo, "r");
	if (archivo->archivo_direccion == NULL) {
		free(archivo);
		return NULL;
	}
	archivo->separador = separador;
	return archivo;
}

size_t leer_linea_csv(struct archivo_csv *archivo, size_t columnas,
		      bool (*funciones[])(const char *, void *), void *ctx[])
{
	if (archivo == NULL || archivo->archivo_direccion == NULL)
		return 0;
	char array[tamaño_array];
	if (!fgets(array, tamaño_array, archivo->archivo_direccion))
		return 0;
	size_t array_longitud = strlen(array);
	if (array[array_longitud - 1] == '\n')
		array[array_longitud - 1] = '\0';
	struct Partes *array_dividido =
		dividir_string(array, archivo->separador);
	if (!array_dividido)
		return 0;
	size_t columnas_a_leer = (array_dividido->cantidad < columnas) ?
					 array_dividido->cantidad :
					 columnas;

	size_t columnas_leidas_correctamente = aplicar_funciones_en_linea_csv(
		array_dividido, columnas_a_leer, funciones, ctx);
	liberar_partes(array_dividido);
	return columnas_leidas_correctamente;
}

void cerrar_archivo_csv(struct archivo_csv *archivo)
{
	fclose(archivo->archivo_direccion);
	free(archivo);
}
