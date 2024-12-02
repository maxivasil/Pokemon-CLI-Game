#ifndef JUEGO_H_
#define JUEGO_H_
#include <stdio.h>
#include "lista.h"
#include "pila.h"
#include "pokedex.h"

typedef struct pokemon_juego pokemon_juego_t;

typedef struct variables {
	size_t segundos_restantes;
	size_t cant_atrapados;
	size_t puntos_obtenidos;
	size_t iteraciones;
	size_t multiplicador;
	size_t combo_maximo;
	pokemon_juego_t *ultimo_poke_capturado;
} variables_t;

typedef struct jugador {
	int x;
	int y;
	char icono;
} jugador_t;

typedef struct juego {
	jugador_t *jugador;
	variables_t variables;
	Lista *fuente_de_pokemones;
	Lista *pokemones_tablero;
	Pila *pokemones_capturados;
	size_t cant_pokemones_tablero;
	size_t semilla;
	int ancho;
	int alto;
} juego_t;

/**
 * Crea un juego y devuelve un puntero al mismo. Devuelve NULL en caso de que
 * alguno de los parámetros sea inválido.
 */
juego_t *juego_crear(int ancho, int alto, size_t tiempo, char icono);

/**
 * Agrega la cantidad de pokemones solicitada a la lista de pokemones del tablero
 * del juego.
 * No devuelve nada.
 */
void juego_agregar_pokemones(juego_t *juego, size_t cant_pokemones_a_agregar);

/**
 * Recibe una pokedex y un puntero a un juego. Carga todos los pokemones de la 
 * pokedex en la lista fuente de pokemones.
 * No devuelve nada.
 */
void juego_subir_pokemones(juego_t *juego, pokedex_t *pokedex);

/**
 * Realiza un movimiento completo de todos los elementos del tablero, el jugador
 * y los pokemones, a partir de la entrada recibida por parámetro, que es válida
 * solamente si es alguno de los siguientes códigos ascii: 256,257,258,259, correspondientes
 * a las diferentes flechas del teclado. 
 * No devuelve nada.
 */
void juego_mover(int entrada, juego_t *juego);

/**
 * Dibuja un tablero por pantalla en el cual coloca todos los elementos participantes en sus
 * respectivas posiciones.
 * No devuelve nada.
 */
void juego_dibujar_tablero(juego_t *juego);

/**
 * Destruye un juego y toda su memoria asociada. No devuelve nada.
 */
void juego_destruir(juego_t *juego);

/**
 * Muestra por pantalla las estadísticas finales del juego: El combo máximo alcanzado,
 * el multiplicador máximo, y el puntaje obtenidos.
 * No devuelve nada.
 */
void juego_mostrar_estadisticas(juego_t *juego);

#endif // JUEGO_H_