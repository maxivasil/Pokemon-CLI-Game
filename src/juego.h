#ifndef JUEGO_H_
#define JUEGO_H_
#include <stdio.h>
#include "lista.h"
#include "pokedex.h"

struct jugador {
	int x;
    int y;
	size_t iteraciones;
    size_t puntos_obtenidos;
    char icono;
    char* ultimo_poke_capturado;
};

typedef struct juego{
    Lista* fuente_de_pokemones;
    Lista* pokemones_tablero;
    Lista* pokemones_capturados;
    jugador_t* jugador;
    size_t cant_pokemones_tablero;
    size_t segundos;
    int ancho;
    int alto;
}juego_t;

juego_t* juego_crear(int ancho, int alto, size_t tiempo, char icono, size_t pokemones_participantes, pokedex_t* pokedex);

void juego_agregar_pokemon(juego_t* juego, pokemon_t *pokemon);

void juego_mover(int entrada, juego_t* juego);

void mover_jugador(int entrada, juego_t* juego);

void mover_pokemones(int movimiento_jugador, juego_t* juego);

void capturar_pokemon(juego_t* juego);

void dibujar_tablero(juego_t* juego);

void juego_destruir(juego_t* juego);

void procesar_entrada(int entrada, struct juego* juego);

void dibujar_cabecera(struct juego* juego);

#endif // JUEGO_H_