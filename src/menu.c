#include "utils.h"
#include "menu.h"
#include "../extra/ansi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * Mostrar las opciones disponibles al usuario.
 * Capturar y validar la opción ingresada.
 * Ejecutar la acción correspondiente según la entrada.
 * 
 * Clave: la opción del menú (ej., "P", "J", etc.).
 * Valor: un struct que contiene:
 * La descripción de la opción.
 * El puntero a la función correspondiente.
 * Un puntero al contexto (si es necesario).
 */

void destruir_opcion(void *opcion_void){
    opcion_t *opcion = (opcion_t *)opcion_void;
    if(!opcion)
        return;
    free(opcion->descripcion); //libero la descripcion q reserve memoria
    free(opcion);
}

//opcion_t

menu_t* menu_crear() {
    return (menu_t*)hash_crear(4); // creo el hash con capacidad 4, pues voy a tener 4 opciones
}

size_t menu_cantidad(menu_t* menu) {
   return hash_cantidad((hash_t*)menu);
}

bool menu_agregar_opcion(menu_t* menu, char* letra, const char* descripcion, bool (*accion)(void*), void* contexto) {
    opcion_t* nueva_opcion = malloc(sizeof(opcion_t)); //me creo el struct de la nueva opcion a realizar
    if (!nueva_opcion)
        return false;

    nueva_opcion->descripcion = copiar(descripcion); 
    if (!nueva_opcion->descripcion) { // Validación adicional
        free(nueva_opcion);
        return false;
    }

    nueva_opcion->funcion = accion;
    nueva_opcion->contexto = contexto;
    opcion_t* opcion_anterior = NULL;
    if (!hash_insertar((hash_t*)menu, letra, nueva_opcion, (void**)&opcion_anterior)) {
        free(nueva_opcion->descripcion); // ver si esto esta bien !!
        free(nueva_opcion);
        return false;
    }
    if (opcion_anterior){ //si habia una antes
        free(opcion_anterior->descripcion);
        free(opcion_anterior);
    }
    return true;
}

// Función de iteración para imprimir las opciones del menú
bool imprimir_opcion_menu(char* letra, void* valor, void* ctx) {
    opcion_t* opcion_a_imprimir = (opcion_t*)valor;

    // Imprimir la letra con un color y la descripción con otro
    printf(ANSI_COLOR_MAGENTA ANSI_COLOR_BOLD "(%s) " ANSI_COLOR_RESET, letra);
    printf(ANSI_COLOR_WHITE "%s\n" ANSI_COLOR_RESET, opcion_a_imprimir->descripcion);

    return true; // sigo iterando
}

void menu_mostrar(menu_t* menu) {
    printf("\n" ANSI_COLOR_CYAN ANSI_COLOR_BOLD  "Opciones disponibles:\n" ANSI_COLOR_RESET);
    hash_iterar((hash_t*)menu, imprimir_opcion_menu, NULL);
}

// Función para manejar la selección de opciones y ejecutar la acción correspondiente
bool menu_ejecutar_opcion(menu_t* menu, char* letra) {
    if (!menu || !letra)
        return false;
    opcion_t* opcion = (opcion_t*)hash_buscar((hash_t*)menu, letra);
    if (!opcion) {
        return false;
    }
    if (!opcion->funcion) 
        return false;
    return opcion->funcion(opcion->contexto);
}

void menu_destruir_todo(menu_t* menu) {
    hash_destruir_todo((hash_t*)menu, destruir_opcion);
}
