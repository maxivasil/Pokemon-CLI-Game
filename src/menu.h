#ifndef MENU_H
#define MENU_H
#include "hash.h"

typedef struct menu menu_t;

typedef struct accion {
    char* descripcion;          // que hace la accion
    void (*funcion)(void*);     // la funcion q se va a ejecutar
    void* contexto;             // un contexto
}accion_t;

// Crea un menú.
menu_t* menu_crear();

size_t menu_cantidad(menu_t *menu);

// Agrega una opción al menú. Devuelve true si tuvo éxito, false si hubo error.
bool menu_agregar_opcion(menu_t* menu, char* letra, const char* descripcion, void (*accion)(void*), void* contexto);

// Muestra el menú y procesa la opción seleccionada.
void menu_mostrar(menu_t* menu);

void menu_ejecutar_opcion(menu_t* menu, char* letra);

// Destruye el menú y libera toda la memoria asociada.
void menu_destruir_todo(menu_t* menu);

#endif  // MENU_H