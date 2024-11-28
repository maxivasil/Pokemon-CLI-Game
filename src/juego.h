#ifndef JUEGO_H_
#define JUEGO_H_
#include <stdio.h>
#include "lista.h"
#include "pokedex.h"

struct jugador {
	size_t x;
    size_t y;
	size_t iteraciones;
    size_t puntos_obtenidos;
    char* ultimo_poke_capturado;
};

typedef struct casillero {
    Lista* elementos; // Lista para manejar varios ocupantes en el casillero
} casillero_t;

typedef struct juego{
    jugador_t *jugador;
    casillero_t** tablero;  // Matriz de casilleros
    size_t segundos;
    size_t ancho;
    size_t alto;
}juego_t;

juego_t* juego_crear(size_t ancho, size_t alto, size_t tiempo, jugador_t* jugador);

void juego_agregar_pokemon(juego_t* juego, pokemon_t *pokemon);

void mover_jugador(int entrada, juego_t* juego);

void mover_pokemones(juego_t* juego);

void capturar_pokemon(juego_t* juego);

void dibujar_tablero(juego_t* juego);

void mostrar_estadisticas(juego_t* juego);

void juego_destruir(juego_t* juego);

void procesar_entrada(int entrada, struct juego* juego);

void dibujar_cabecera(struct juego* juego);

#endif // JUEGO_H_