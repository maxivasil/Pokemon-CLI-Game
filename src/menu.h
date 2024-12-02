#ifndef MENU_H
#define MENU_H
#include "hash.h"

typedef struct menu menu_t;

typedef struct opcion {
    char* descripcion;          
    bool (*funcion)(void*);     
    void* contexto;            
}opcion_t;

/**
 * Crea un menú y lo devuelve.
 * */
menu_t* menu_crear();

/**
 * Devuelve la cantidad de opciones en el menú.
 */
size_t menu_cantidad(menu_t *menu);

/**
 * Agrega una opción al menú. Devuelve true si tuvo éxito, false si hubo error.
 * El identificador de cada opción debe ser único. Si ya existía, se reemplaza la opción anterior.
 */
bool menu_agregar_opcion(menu_t* menu, char* identificador, const char* descripcion, bool (*accion)(void*), void* contexto);

/**
 * Devuelve true si una opción está en el menú a partir de su identificador,
 * false en caso contrario.
 */
bool menu_contiene(menu_t* menu, char* identificador);

/**
 * Muestra por pantalla las opciones del menú (si hay por lo menos una).
 * Devuelve la cantidad de opciones mostradas.
 */
// 
size_t menu_mostrar(menu_t* menu);

/**
 * Busca en el menú la opción asociada al identificador dado. Si la encuentra
 * y la función no es nula (cuyo caso devuelve false), ejecuta la misma con 
 * el contexto de la opción y retorna true o false en base a lo que retorne la
 * función de la opción.
 */
bool menu_ejecutar_opcion(menu_t* menu, char* identificador);

/**
 * Destruye el menú y libera toda la memoria asociada al mismo y sus elementos.
 */
void menu_destruir(menu_t* menu);

#endif  // MENU_H