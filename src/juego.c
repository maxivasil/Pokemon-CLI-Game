#include <stdio.h>
#include "../extra/ansi.h"
#include "../extra/engine.h"
#include "lista.h"
#include "juego.h"
#include "utils.h"
#include <string.h>


juego_t* juego_crear(size_t ancho, size_t alto, size_t tiempo, jugador_t* jugador){
    if(!ancho || !alto || !jugador)
        return NULL;

    juego_t* juego = malloc(sizeof(juego_t));
    if (!juego)
        return NULL;

    juego->tablero = malloc(alto * sizeof(casillero_t*));
    if (!juego->tablero) {
        free(juego);
        return NULL;
    }
    int error = 0;

    for(int i = 0;i < alto; i++){
        juego->tablero[i] = malloc(ancho * sizeof(casillero_t));
        error += juego->tablero[i] == NULL;

        for (size_t j = 0; j < ancho; j++) {
            juego->tablero[i][j].elementos = lista_crear(); //me creo una lista x cada casillero
            error += juego->tablero[i][j].elementos == NULL;
        }
    }

    if(error > 0){ // ver si esto esta bien pensado !!!
        for(int i = 0;i < alto; i++)
            free(juego->tablero[i]);
        free(juego->tablero);
        free(juego);
        return NULL;
    }

    juego->jugador = jugador; // mmmmmmmmmmmmmmm
    juego->segundos = tiempo;  // del parametro
    juego->ancho = ancho;
    juego->alto = alto;

    return juego;
}

void juego_agregar_pokemon(juego_t* juego, pokemon_t *pokemon){
    if(!pokemon || !juego)
        return;
    
    casillero_t* casillero_actual = &juego->tablero[pokemon->y][pokemon->x];
    lista_agregar_al_final(casillero_actual->elementos, pokemon);
}

void mover_jugador(int entrada, juego_t* juego) {
    if(!juego || !juego->tablero || !juego->jugador)
        return;
    jugador_t* jugador = juego->jugador;

    if (jugador->x >= juego->ancho || jugador->y >= juego->alto) {
        fprintf(stderr, "Error: Jugador fuera de los límites del tablero\n");
        return;
    }

    casillero_t* casillero_actual = &juego->tablero[jugador->y][jugador->x];
    procesar_entrada(entrada, juego);

    void *elemento_quitado = NULL;
    lista_quitar_elemento(casillero_actual->elementos, 0, &elemento_quitado);

    casillero_t* casillero_nuevo = &juego->tablero[jugador->y][jugador->x];

    //lo meto en el nuevo casillero al inicio, cosa de que si el tamaño
    //de la lista es mayor a 1, se que se atrapa un pokemon
    lista_agregar_elemento(casillero_nuevo->elementos, 0, jugador);
    jugador->iteraciones++;
}

void mover_pokemones(juego_t *juego) {
    if (!juego || !juego->tablero)
        return;

    for (size_t i = 0; i < juego->alto; i++) {
        for (size_t j = 0; j < juego->ancho; j++) {
            casillero_t *casillero = &juego->tablero[i][j];
            if (!casillero->elementos)
                continue;

            size_t cantidad_pokemones = lista_cantidad_elementos(casillero->elementos);
            for (size_t k = 0; k < cantidad_pokemones; k++) {
                void *poke;
                lista_obtener_elemento(casillero->elementos, k, &poke);
                pokemon_t* pokemon = (pokemon_t*)poke;
                if (!pokemon || !pokemon->patron_movimiento)
                    continue;

                size_t patron_len = strlen(pokemon->patron_movimiento);
                if (patron_len == 0)
                    continue;

                size_t iteracion = pokemon->iteracion;
                char movimiento = pokemon->patron_movimiento[iteracion % patron_len];

                // Calcular la nueva posición del Pokémon
                size_t nueva_x = pokemon->x;
                size_t nueva_y = pokemon->y;

                switch (movimiento) {
                    case 'N': nueva_y--; break;
                    case 'S': nueva_y++; break;
                    case 'E': nueva_x++; break;
                    case 'O': nueva_x--; break;
                    case 'J':
                        nueva_x = juego->jugador->x;
                        nueva_y = juego->jugador->y;
                        break;
                    case 'I':
                        nueva_x = juego->jugador->x - (pokemon->x - juego->jugador->x);
                        nueva_y = juego->jugador->y - (pokemon->y - juego->jugador->y);
                        break;
                    case 'R': {
                        int direccion = rand() % 4;
                        if (direccion == 0) nueva_y--;
                        else if (direccion == 1) nueva_y++;
                        else if (direccion == 2) nueva_x--;
                        else nueva_x++;
                        break;
                    }
                }

                // Asegurarse de que la posición esté dentro de los límites
                nueva_x = min(juego->ancho - 1, max(0, nueva_x));
                nueva_y = min(juego->alto - 1, max(0, nueva_y));

                // Si cambió de posición, mover el Pokémon
                if (nueva_x != pokemon->x || nueva_y != pokemon->y) {
                    casillero_t *nuevo_casillero = &juego->tablero[nueva_y][nueva_x];

                    lista_quitar_elemento(casillero->elementos, k, NULL);
                    lista_agregar_al_final(nuevo_casillero->elementos, pokemon);

                    pokemon->x = nueva_x;
                    pokemon->y = nueva_y;
                }

                // Incrementar la iteración
                pokemon->iteracion++;
            }
        }
    }
}




void dibujar_tablero(juego_t *juego) {
    borrar_pantalla(); // Limpia la terminal antes de dibujar
    for (size_t i = 0; i < juego->alto; i++) {
        for (size_t j = 0; j < juego->ancho; j++) {
            casillero_t *casillero = &juego->tablero[i][j];

            // Verificar qué elementos hay en el casillero
            bool hay_jugador = false;
            size_t cantidad_pokemones = 0;
            char letra_pokemon = ' ';

            Lista_iterador *iter = lista_iterador_crear(casillero->elementos);
            while (lista_iterador_hay_siguiente(iter)) {
                void *elemento = lista_iterador_obtener_elemento_actual(iter);
                if (elemento == (void *)juego->jugador) {
                    hay_jugador = true;
                } else {
                    pokemon_t *pokemon = (pokemon_t *)elemento;
                    letra_pokemon = pokemon->nombre[0]; // Primera letra del Pokémon
                    cantidad_pokemones++;
                }
                lista_iterador_avanzar(iter);
            }
            lista_iterador_destruir(iter);

            // Determinar qué imprimir en el casillero
            if (hay_jugador) {
                printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "@" ANSI_COLOR_RESET); // Jugador
            } else if (cantidad_pokemones > 0) {
                printf(ANSI_COLOR_BOLD "%s" ANSI_COLOR_RESET, letra_pokemon); // Pokémon
            } else {
                printf(" "); // Casillero vacío
            }
        }
        printf("\n");
    }

    // Imprimir estadísticas al pie del tablero
    printf(ANSI_COLOR_YELLOW "Tiempo: %zu  Puntaje: %ld\n" ANSI_COLOR_RESET,
           juego->segundos, juego->jugador->puntos_obtenidos);
    printf(ANSI_COLOR_CYAN "Último capturado: %s\n" ANSI_COLOR_RESET, 
           juego->jugador->ultimo_poke_capturado ? juego->jugador->ultimo_poke_capturado : "Ninguno");
}


void mostrar_estadisticas(juego_t *juego) {
    printf("Puntaje: %ld\n", juego->jugador->puntos_obtenidos);
    printf("Tiempo restante: %zu segundos\n", juego->segundos);
}

void capturar_pokemon(juego_t *juego) {
    casillero_t *casillero = &juego->tablero[juego->jugador->y][juego->jugador->x];
    Lista_iterador *iter = lista_iterador_crear(casillero->elementos);
    if (!iter) return;

    while (lista_iterador_hay_siguiente(iter)) {
        pokemon_t *pokemon = (pokemon_t *)lista_iterador_obtener_elemento_actual(iter);
        // Si el Pokémon está en el mismo casillero que el jugador, lo captura
        if (pokemon->x == juego->jugador->x && pokemon->y == juego->jugador->y) {
            // Aumentar el puntaje
            juego->jugador->puntos_obtenidos += pokemon->puntos;
            // Eliminar el Pokémon de la lista
            lista_quitar_elemento(casillero->elementos,0, NULL); //saco el q este primero?? tengo q ver, pq en esa lista esta tambien el jugador!
        }
        lista_iterador_avanzar(iter);
    }
    lista_iterador_destruir(iter);
}

void juego_destruir(juego_t* juego) {
    if (!juego)
        return;

    // Iterar sobre cada casillero del tablero
    for (size_t i = 0; i < juego->alto; i++) {
        for (size_t j = 0; j < juego->ancho; j++) {
            casillero_t *casillero = &juego->tablero[i][j];
            lista_destruir(casillero->elementos);
        }

        // Liberar la fila actual del tablero
        free(juego->tablero[i]);
    }

    // Liberar el tablero completo
    free(juego->tablero);

    // Liberar la estructura del juego
    free(juego);
}


void procesar_entrada(int entrada, struct juego* juego){
	struct jugador *jugador = juego->jugador;
	if (entrada == TECLA_DERECHA)
		jugador->x++;
	else if (entrada == TECLA_IZQUIERDA)
		jugador->x--;
	else if (entrada == TECLA_ARRIBA)
		jugador->y--;
	else if (entrada == TECLA_ABAJO)
		jugador->y++;

	jugador->x = min(ANCHO_TABLERO, max(0, jugador->x));
	jugador->y = min(ALTO_TABLERO, max(0, jugador->y));
}

void dibujar_cabecera(struct juego* juego){
	printf("Utilizar " ANSI_COLOR_CYAN ANSI_COLOR_BOLD
	       "⬆⬇⬅➡" ANSI_COLOR_RESET " para moverse\n");

	printf("Presionar " ANSI_COLOR_RED ANSI_COLOR_BOLD "Q" ANSI_COLOR_RESET
	       " para salir\n");

	printf("Iteraciones: %ld Tiempo: %ld\n", juego->jugador->iteraciones,
	       juego->jugador->iteraciones / 5);
}