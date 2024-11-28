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

void destruir_accion(void *accion_void){
    accion_t *accion = (accion_t *)accion_void;
    if(!accion)
        return;
    free(accion->descripcion); //libero la descripcion q reserve memoria
    free(accion);
}

//accion_t

menu_t* menu_crear() {
    return (menu_t*)hash_crear(4); // creo el hash con capacidad 4, pues voy a tener 4 opciones
}

size_t menu_cantidad(menu_t* menu) {
   return hash_cantidad((hash_t*)menu);
}

bool menu_agregar_opcion(menu_t* menu, char* letra, const char* descripcion, void (*accion)(void*), void* contexto) {
    accion_t* nueva_accion = malloc(sizeof(accion_t)); //me creo el struct de la nueva accion a realizar
    if (!nueva_accion)
        return false;

    nueva_accion->descripcion = copiar(descripcion); 
    if (!nueva_accion->descripcion) { // Validación adicional
        free(nueva_accion);
        return false;
    }

    nueva_accion->funcion = accion;
    nueva_accion->contexto = contexto;
    accion_t* accion_anterior = NULL;
    if (!hash_insertar((hash_t*)menu, letra, nueva_accion, (void**)&accion_anterior)) {
        free(nueva_accion->descripcion); // ver si esto esta bien !!
        free(nueva_accion);
        return false;
    }
    if (accion_anterior){ //si habia una antes
        free(accion_anterior->descripcion);
        free(accion_anterior);
    }
    return true;
}

// Función de iteración para imprimir las opciones del menú
bool imprimir_opcion_menu(char* letra, void* valor, void* ctx) {
    accion_t* accion_a_imprimir = (accion_t*)valor;

    // Imprimir la letra con un color y la descripción con otro
    printf(ANSI_COLOR_MAGENTA ANSI_COLOR_BOLD "(%s) " ANSI_COLOR_RESET, letra);
    printf(ANSI_COLOR_WHITE "%s\n" ANSI_COLOR_RESET, accion_a_imprimir->descripcion);

    return true; // sigo iterando
}

void menu_mostrar(menu_t* menu) {
    printf("\n" ANSI_COLOR_CYAN ANSI_COLOR_BOLD  "Opciones disponibles:\n" ANSI_COLOR_RESET);
    hash_iterar((hash_t*)menu, imprimir_opcion_menu, NULL);
}

// Función para manejar la selección de opciones y ejecutar la acción correspondiente
void menu_ejecutar_opcion(menu_t* menu, char* letra) {
    if (!menu || !letra)
        return;

    accion_t* accion = (accion_t*)hash_buscar((hash_t*)menu, letra);
    if (!accion) {
        printf("Opción inválida: %s\n", letra);
        return;
    }
    if (!accion->funcion) 
        return;
    accion->funcion(accion->contexto);
}

void menu_destruir_todo(menu_t* menu) {
    hash_destruir_todo((hash_t*)menu, destruir_accion);
}
