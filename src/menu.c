#include "utils.h"
#include "menu.h"
#include "../extra/ansi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void destruir_opcion(void *opcion_void)
{
	opcion_t *opcion = (opcion_t *)opcion_void;
	if (!opcion)
		return;
	free(opcion->descripcion);
	free(opcion);
}

bool imprimir_opcion_menu(char *letra, void *valor, void *ctx)
{
	opcion_t *opcion_a_imprimir = (opcion_t *)valor;
	printf(ANSI_COLOR_MAGENTA ANSI_COLOR_BOLD "(%s) " ANSI_COLOR_RESET,
	       letra);
	printf(ANSI_COLOR_WHITE "%s\n" ANSI_COLOR_RESET,
	       opcion_a_imprimir->descripcion);
	return true;
}

menu_t *menu_crear()
{
	return (menu_t *)hash_crear(4);
}

size_t menu_cantidad(menu_t *menu)
{
	return hash_cantidad((hash_t *)menu);
}

bool menu_agregar_opcion(menu_t *menu, char *letra, const char *descripcion,
			 bool (*accion)(void *), void *contexto)
{
	if (!letra || !descripcion)
		return false;
	opcion_t *nueva_opcion = malloc(sizeof(opcion_t));
	if (!nueva_opcion)
		return false;

	nueva_opcion->descripcion = copiar(descripcion);
	if (!nueva_opcion->descripcion) {
		free(nueva_opcion);
		return false;
	}

	nueva_opcion->funcion = accion;
	nueva_opcion->contexto = contexto;
	opcion_t *opcion_anterior = NULL;
	if (!hash_insertar((hash_t *)menu, letra, nueva_opcion,
			   (void **)&opcion_anterior)) {
		free(nueva_opcion->descripcion);
		free(nueva_opcion);
		return false;
	}
	if (opcion_anterior) {
		free(opcion_anterior->descripcion);
		free(opcion_anterior);
	}
	return true;
}

bool menu_contiene(menu_t *menu, char *identificador)
{
	return hash_contiene((hash_t *)menu, identificador);
}

size_t menu_mostrar(menu_t *menu)
{
	if (hash_cantidad((hash_t *)menu) > 0)
		printf("\n" ANSI_COLOR_CYAN ANSI_COLOR_BOLD
		       "Opciones disponibles:\n" ANSI_COLOR_RESET);
	return hash_iterar((hash_t *)menu, imprimir_opcion_menu, NULL);
}

bool menu_ejecutar_opcion(menu_t *menu, char *identificador)
{
	if (!menu || !identificador)
		return false;
	opcion_t *opcion =
		(opcion_t *)hash_buscar((hash_t *)menu, identificador);
	if (!opcion) {
		return false;
	}
	if (!opcion->funcion)
		return false;
	return opcion->funcion(opcion->contexto);
}

void menu_destruir(menu_t *menu)
{
	hash_destruir_todo((hash_t *)menu, destruir_opcion);
}
