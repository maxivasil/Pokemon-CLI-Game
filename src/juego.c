#include <stdio.h>
#include "../extra/ansi.h"
#include "../extra/engine.h"
#include "lista.h"
#include "juego.h"
#include "utils.h"
#include <string.h>

juego_t* juego_crear(int ancho, int alto, size_t segundos, char icono_jugador){
    if(!ancho || !alto || !segundos)
        return NULL;

    juego_t* juego = calloc(1, sizeof(juego_t));
    if (!juego)
        return NULL;
    juego->jugador = calloc(1, sizeof(jugador_t));
    if(!juego->jugador) {
        free(juego);
        return NULL;
    }
    juego->pokemones_tablero = lista_crear();
    if(!juego->pokemones_tablero) {
        free(juego->jugador);
        free(juego);
        return NULL;
    }
    juego->pokemones_capturados = lista_crear();
    if(!juego->pokemones_capturados) {
        lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
        free(juego->jugador);
        free(juego);
        return NULL;
    }
    juego->jugador->icono = icono_jugador;
    juego->segundos = segundos;  // del parametro
    juego->ancho = ancho;
    juego->alto = alto;

    return juego;
}

void juego_agregar_pokemon(juego_t* juego, pokemon_t *pokemon){
    if(!pokemon || !juego)
        return;
    pokemon_t* pokemon_copia = calloc(1, sizeof(pokemon_t));
    *pokemon_copia = *pokemon;
    lista_agregar_al_final(juego->pokemones_tablero, pokemon_copia);
}

void mover_jugador(int entrada, juego_t* juego) {
    if(!juego || !juego->jugador)
        return;
    procesar_entrada(entrada, juego);
}

void mover_pokemones(int movimiento_jugador, juego_t *juego) {
    if (!juego)
        return;
    size_t cantidad_pokemones = lista_cantidad_elementos(juego->pokemones_tablero);
    for (size_t k = 0; k < cantidad_pokemones; k++) {
        pokemon_t* pokemon;
        lista_obtener_elemento(juego->pokemones_tablero, k, (void**)&pokemon);
        size_t patron_len = strlen(pokemon->patron_movimiento);
        if (patron_len == 0) // DUDOSO ¿Por qué sería cero?
            continue;
        size_t iteracion = pokemon->iteracion;
        char movimiento = pokemon->patron_movimiento[iteracion % patron_len];
        // Calcular la nueva posición del Pokémon
        int nueva_x = pokemon->x;
        int nueva_y = pokemon->y;
        switch (movimiento) {
            case 'N': nueva_y--; break;
            case 'S': nueva_y++; break;
            case 'E': nueva_x++; break;
            case 'O': nueva_x--; break;
            case 'J':
                switch (movimiento_jugador) {
                    case TECLA_DERECHA: nueva_x++; break;
                    case TECLA_IZQUIERDA: nueva_x--; break;
                    case TECLA_ABAJO: nueva_y++; break;
                    case TECLA_ARRIBA: nueva_y--; break;
                } break;
            case 'I': 
                switch (movimiento_jugador) {
                    case TECLA_DERECHA: nueva_x--; break;
                    case TECLA_IZQUIERDA: nueva_x++; break;
                    case TECLA_ABAJO: nueva_y--; break;
                    case TECLA_ARRIBA: nueva_y++; break;
                } break;
            case 'R': {
                int direccion = rand() % 4;
                switch (direccion) {
                    case 0:  // arriba
                        nueva_y--; break;
                    case 1:  // abajo
                        nueva_y++;break;
                    case 2:  // izq
                        nueva_x--; break;
        
                    case 3:  // der
                        nueva_x++; break;
                    }
                }break;
            }
        // Asegurarse de que la posición esté dentro de los límites
        nueva_x = min(juego->ancho - 1, max(0, nueva_x));
        nueva_y = min(juego->alto - 1, max(0, nueva_y));

        pokemon->x = nueva_x;
        pokemon->y = nueva_y; 
        pokemon->iteracion++;
    }
}

void dibujar_tablero(juego_t *juego) {
    for (size_t i = 0; i < juego->alto; i++) {
        for (size_t j = 0; j < juego->ancho; j++) {
            bool casillero_ocupado = false;
            size_t iterados = 0;
            // Determinar qué imprimir en el casillero
            while (iterados < lista_cantidad_elementos(juego->pokemones_tablero) && !casillero_ocupado) { //podria ir CANT_POKES_A_AGREGAR
                pokemon_t* pokemon_actual = NULL;
                lista_obtener_elemento(juego->pokemones_tablero, iterados, (void**)&pokemon_actual);
                if (juego->jugador->x == j && juego->jugador->y == i) {
                    printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "%c" ANSI_COLOR_RESET, juego->jugador->icono); // Jugador
                    casillero_ocupado = true;
                } else if (pokemon_actual->x == j && pokemon_actual->y == i && !casillero_ocupado) {
                    printf("%s" ANSI_COLOR_BOLD "%c" ANSI_COLOR_RESET, pokemon_actual->color,pokemon_actual->nombre[0]); // Pokémon
                    casillero_ocupado = true;
                } else {
                    printf(" "); // Casillero vacío
                }
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

void capturar_pokemon(juego_t *juego) {
    jugador_t* jugador = juego->jugador;

    for(int i = CANTIDAD_POKEMONES_A_AGREGAR - 1; i >= 0; i--){
        pokemon_t *pokemon_actual = NULL;
        pokemon_t *pokemon_quitado = NULL;
        lista_obtener_elemento(juego->pokemones_tablero, (size_t)i, (void**)&pokemon_actual);
        if (jugador->x == pokemon_actual->x && jugador->y == pokemon_actual->y) {
            lista_quitar_elemento(juego->pokemones_tablero, (size_t)i, (void**)pokemon_quitado);
            lista_agregar_al_final(juego->pokemones_capturados, (void*)pokemon_quitado);
            jugador->puntos_obtenidos += pokemon_quitado->puntos;
        }        
    }
    // ACA HABRIA QUE AGREGAR DE ALGUN MODO LA CANTIDAD DE POKEMONES QUITADOS 
    // PORQUE NUNCA TERMINA EL JUEGO
}

void juego_destruir(juego_t* juego) {
    if (!juego)
        return;
    
    free(juego->jugador);
    lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
    lista_destruir_todo(juego->pokemones_capturados, destruir_pokemon);
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

	jugador->x = min(juego->ancho - 1, max(0, jugador->x));
	jugador->y = min(juego->alto - 1, max(0, jugador->y));
}

void dibujar_cabecera(struct juego* juego){
	printf("Utilizar " ANSI_COLOR_CYAN ANSI_COLOR_BOLD
	       "⬆⬇⬅➡" ANSI_COLOR_RESET " para moverse\n");

	printf("Presionar " ANSI_COLOR_RED ANSI_COLOR_BOLD "Q" ANSI_COLOR_RESET
	       " para salir\n");
}