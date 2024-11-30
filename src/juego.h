#ifndef JUEGO_H_
#define JUEGO_H_
#include <stdio.h>
#include "lista.h"
#include "pila.h"
#include "pokedex.h"

typedef struct variables {
    size_t segundos_restantes;
    size_t cant_atrapados;
    size_t puntos_obtenidos;
    size_t iteraciones;
    size_t multiplicador;
    pokemon_t* ultimo_poke_capturado;
    
} variables_t;

struct jugador {
	int x;
    int y;
    char icono;
};

typedef struct juego{
    jugador_t* jugador;
    variables_t variables;
    Lista* fuente_de_pokemones;
    Lista* pokemones_tablero;
    Pila* pokemones_capturados;
    size_t cant_pokemones_tablero;
    size_t semilla;
    int ancho;
    int alto;
}juego_t;

juego_t* juego_crear(int ancho, int alto, size_t tiempo, char icono, pokedex_t* pokedex);

void juego_agregar_pokemones(juego_t* juego, size_t cant_pokemones_a_agregar);

void juego_mover(int entrada, juego_t* juego);

void juego_dibujar_tablero(juego_t* juego);

void juego_destruir(juego_t* juego);

void juego_mostrar_estadisticas(juego_t* juego);

void juego_dibujar_cabecera(struct juego* juego);

#endif // JUEGO_H_