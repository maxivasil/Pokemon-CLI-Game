#include <stdio.h>
#include "../extra/ansi.h"
#include "../extra/engine.h"
#include "lista.h"
#include "juego.h"
#include "utils.h"
#include <string.h>

pokemon_t* copiar_pokemon(pokemon_t* pokemon) {
    pokemon_t* pokemon_copia = calloc(1, sizeof(pokemon_t));
    pokemon_copia->nombre = copiar(pokemon->nombre);
    pokemon_copia->color = pokemon->color;
    pokemon_copia->patron_movimiento = copiar(pokemon->patron_movimiento);
    pokemon_copia->iteracion = pokemon->iteracion;
    pokemon_copia->puntos = pokemon->puntos;
    pokemon_copia->x = pokemon->x;
    pokemon_copia->y = pokemon->y;
    return pokemon_copia;
}

bool agregar_pokemon_a_lista(void* pokemon, void* lista) {
    pokemon_t* poke = (pokemon_t*)pokemon;
    Lista* list = (Lista*)lista;

    pokemon_t* copia_poke = copiar_pokemon(poke);
    lista_agregar_al_final(list, copia_poke);
    return true;
}

void agregar_pokemon_al_tablero(juego_t* juego, size_t cant_pokemones_a_agregar) {
    for (size_t i = 0; i < cant_pokemones_a_agregar; i++) {
        size_t indice = (size_t)rand() % lista_cantidad_elementos(juego->fuente_de_pokemones);
        pokemon_t* poke = NULL;
        lista_obtener_elemento(juego->fuente_de_pokemones, indice, (void**)&poke);
        juego_agregar_pokemon(juego, poke);
    }
}

juego_t* juego_crear(int ancho, int alto, size_t segundos, char icono_jugador, size_t cant_pokemones_tablero, pokedex_t* pokedex){
    if(!ancho || !alto || !segundos || !pokedex)
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
    juego->fuente_de_pokemones = lista_crear();
    if(!juego->fuente_de_pokemones) {
        lista_destruir_todo(juego->pokemones_capturados, destruir_pokemon);
        lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
        free(juego->jugador);
        free(juego);
        return NULL;
    }
    
    pokedex_iterar(pokedex, agregar_pokemon_a_lista, juego->fuente_de_pokemones);
    agregar_pokemon_al_tablero(juego, cant_pokemones_tablero);
    
    juego->cant_pokemones_tablero = cant_pokemones_tablero;
    juego->jugador->icono = icono_jugador;
    juego->segundos = segundos; 
    juego->ancho = ancho;
    juego->alto = alto;

    return juego;
}

void juego_agregar_pokemon(juego_t* juego, pokemon_t *pokemon){
    if(!pokemon || !juego)
        return;
    pokemon_t* pokemon_copia = copiar_pokemon(pokemon);
    lista_agregar_al_final(juego->pokemones_tablero, pokemon_copia);
}

void juego_mover(int entrada, juego_t* juego) {
    if (!juego || !entrada || toupper(entrada) == 'Q')
        return;
    mover_jugador(entrada, juego);
    mover_pokemones(entrada, juego);
    capturar_pokemon(juego);
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
        pokemon_t* pokemon = NULL;
        lista_obtener_elemento(juego->pokemones_tablero, k, (void**)&pokemon);
        if (!pokemon || !pokemon->patron_movimiento)
            continue;
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

void dibujar_tablero(juego_t* juego) {
    // Imprimir la línea superior del tablero
    printf("╔");
    for (size_t j = 0; j < juego->ancho; j++) {
        printf("═");
    }
    printf("╗\n");

    for (size_t i = 0; i < juego->alto; i++) {
        printf("║"); // Borde izquierdo

        for (size_t j = 0; j < juego->ancho; j++) {
            bool impreso = false;

            // Priorizar la impresión del jugador
            if (juego->jugador->x == j && juego->jugador->y == i) {
                printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "%c" ANSI_COLOR_RESET, juego->jugador->icono);
                impreso = true;
            }

            // Imprimir Pokémon solo si no se imprimió el jugador
            size_t cantidad_pokemones = lista_cantidad_elementos(juego->pokemones_tablero);
            for (size_t k = 0; k < cantidad_pokemones; k++) {
                if (impreso)
                    break;
                pokemon_t* pokemon_actual = NULL;
                lista_obtener_elemento(juego->pokemones_tablero, k, (void**)&pokemon_actual);
                if (pokemon_actual && pokemon_actual->x == j && pokemon_actual->y == i) {
                    printf("%s" ANSI_COLOR_BOLD "%c" ANSI_COLOR_RESET, pokemon_actual->color, pokemon_actual->nombre[0]);
                    impreso = true;
                    break;
                }
            }

            // Espacio vacío si no se imprimió nada
            if (!impreso)
                printf(" ");
        }
        printf("║\n");
    }

    // Imprimir la línea inferior del tablero
    printf("╚");
    for (size_t j = 0; j < juego->ancho; j++) {
        printf("═");
    }
    printf("╝\n");

    // Imprimir estadísticas al pie del tablero
    printf(ANSI_COLOR_WHITE "┌───────────────────────────────────────────────┐\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_WHITE "  Tiempo: %zu  Puntaje: %ld  Capturados: %ld\n" ANSI_COLOR_RESET,
           juego->segundos, juego->jugador->puntos_obtenidos, juego->cant_atrapados);
    printf(ANSI_COLOR_WHITE "  Último capturado: " ANSI_COLOR_RESET ANSI_COLOR_BOLD "%s %s\n" ANSI_COLOR_RESET, 
           juego->jugador->ultimo_poke_capturado ? juego->jugador->ultimo_poke_capturado->color : "",
           juego->jugador->ultimo_poke_capturado ? juego->jugador->ultimo_poke_capturado->nombre : "Ninguno");
    printf(ANSI_COLOR_WHITE "└───────────────────────────────────────────────┘\n" ANSI_COLOR_RESET);
}


void capturar_pokemon(juego_t *juego) { // devuelve la cantidad de capturados
    jugador_t* jugador = juego->jugador;
    size_t cant_capturados = 0;
    int cantidad_pokemones = (int)lista_cantidad_elementos(juego->pokemones_tablero);
    for(int i = cantidad_pokemones - 1; i >= 0; i--){
        pokemon_t *pokemon_actual = NULL;
        lista_obtener_elemento(juego->pokemones_tablero, (size_t)i, (void**)&pokemon_actual);
        if (pokemon_actual) {
            if (jugador->x == pokemon_actual->x && jugador->y == pokemon_actual->y) {
                lista_quitar_elemento(juego->pokemones_tablero, (size_t)i, NULL);
                lista_agregar_al_final(juego->pokemones_capturados, (void*)pokemon_actual);
                juego->cant_atrapados++;
                jugador->puntos_obtenidos += pokemon_actual->puntos;
                jugador->ultimo_poke_capturado = pokemon_actual;
                cant_capturados++;
            }        
        }
    }
    agregar_pokemon_al_tablero(juego, cant_capturados);
}

void juego_destruir(juego_t* juego) {
    if (!juego)
        return;
    
    free(juego->jugador);
    lista_destruir_todo(juego->fuente_de_pokemones, destruir_pokemon);
    lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
    lista_destruir_todo(juego->pokemones_capturados, destruir_pokemon);
    free(juego);
}


void procesar_entrada(int entrada, struct juego* juego){
	if (!juego || !juego->jugador)
        return;
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
    printf(ANSI_COLOR_WHITE "┌───────────────────────────────────────────────┐\n" ANSI_COLOR_RESET);
	printf("  Utilizar " ANSI_COLOR_CYAN ANSI_COLOR_BOLD
	       "⬆⬇⬅➡" ANSI_COLOR_RESET " para moverse\n");

	printf("  Presionar " ANSI_COLOR_RED ANSI_COLOR_BOLD "Q" ANSI_COLOR_RESET
	       " para salir\n");
    printf("  🌱 Semilla N°: %ld \n", juego->semilla);
    printf(ANSI_COLOR_WHITE "└───────────────────────────────────────────────┘\n" ANSI_COLOR_RESET);
}