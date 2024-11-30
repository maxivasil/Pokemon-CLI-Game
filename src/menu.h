#ifndef MENU_H
#define MENU_H
#include "hash.h"

typedef struct menu menu_t;

typedef struct opcion {
    char* descripcion;          // que hace la accion
    bool (*funcion)(void*);     // la funcion q se va a ejecutar
    void* contexto;             // un contexto
}opcion_t;

// Crea un menú.
menu_t* menu_crear();

size_t menu_cantidad(menu_t *menu);

// Agrega una opción al menú. Devuelve true si tuvo éxito, false si hubo error.
bool menu_agregar_opcion(menu_t* menu, char* identificador, const char* descripcion, bool (*accion)(void*), void* contexto);

// Devuelve si una opción está en el menú a partir de su clave
bool menu_contiene(menu_t* menu, char* identificador);

// Muestra el menú y procesa la opción seleccionada.
void menu_mostrar(menu_t* menu);

bool menu_ejecutar_opcion(menu_t* menu, char* letra);

// Destruye el menú y libera toda la memoria asociada.
void menu_destruir_todo(menu_t* menu);

#endif  // MENU_H