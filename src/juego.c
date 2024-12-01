#include <stdio.h>
#include <string.h>
#include "../extra/ansi.h"
#include "../extra/engine.h"
#include "lista.h"
#include "juego.h"
#include "utils.h"

typedef struct elemento {
    int x;
    int y;
    char icono;
    char* color;
} elemento_t;

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

pokemon_juego_t* copiar_pokemon(pokemon_juego_t* pokemon) {
    pokemon_juego_t* pokemon_copia = calloc(1, sizeof(pokemon_juego_t));
    if(!pokemon_copia)
        return NULL;
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
    if(!pokemon || !lista)
        return false;
    Lista* list = (Lista*)lista;
    pokemon_t* poke_parseado = (pokemon_t*)pokemon;
    pokemon_juego_t pokemon_juego = {.nombre=poke_parseado->nombre, 
                                     .puntos=poke_parseado->puntos, 
                                     .color=poke_parseado->color, 
                                     .patron_movimiento=poke_parseado->patron_movimiento,
                                     .x=generar_posicion_random(ANCHO_TABLERO), 
                                     .y=generar_posicion_random(ALTO_TABLERO),
                                     .iteracion=0};
    pokemon_juego_t* copia_poke = copiar_pokemon(&pokemon_juego);
    lista_agregar_al_final(list, copia_poke);
    return true;
}

void juego_agregar_pokemones(juego_t* juego, size_t cant_pokemones_a_agregar) {
    juego->cant_pokemones_tablero = cant_pokemones_a_agregar;
    for (size_t i = 0; i < cant_pokemones_a_agregar; i++) {
        size_t indice = (size_t)rand() % lista_cantidad_elementos(juego->fuente_de_pokemones);
        pokemon_juego_t* poke = NULL;
        lista_obtener_elemento(juego->fuente_de_pokemones, indice, (void**)&poke);
        agregar_pokemon_a_lista((void*)poke, (void*)juego->pokemones_tablero);
    }
}

juego_t* juego_crear(int ancho, int alto, size_t segundos, char icono_jugador, pokedex_t* pokedex){
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
    juego->pokemones_capturados = pila_crear();
    if(!juego->pokemones_capturados) {
        lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
        free(juego->jugador);
        free(juego);
        return NULL;
    }
    juego->fuente_de_pokemones = lista_crear();
    if(!juego->fuente_de_pokemones) {
        pila_destruir_todo(juego->pokemones_capturados, destruir_pokemon);
        lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
        free(juego->jugador);
        free(juego);
        return NULL;
    }
    
    pokedex_iterar(pokedex, agregar_pokemon_a_lista, juego->fuente_de_pokemones);
    juego->variables.multiplicador = 1;
    juego->jugador->icono = icono_jugador;
    juego->variables.segundos_restantes = segundos; 
    juego->ancho = ancho;
    juego->alto = alto;

    return juego;
}

void juego_destruir(juego_t* juego) {
    if (!juego)
        return;
    
    free(juego->jugador);
    lista_destruir_todo(juego->fuente_de_pokemones, destruir_pokemon);
    lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
    pila_destruir_todo(juego->pokemones_capturados, destruir_pokemon);
    free(juego);
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
        pokemon_juego_t* pokemon = NULL;
        lista_obtener_elemento(juego->pokemones_tablero, k, (void**)&pokemon);
        if (!pokemon || !pokemon->patron_movimiento)
            continue;
        size_t patron_len = strlen(pokemon->patron_movimiento);
        if (patron_len == 0) 
            continue;
        size_t iteracion = pokemon->iteracion;
        char movimiento = pokemon->patron_movimiento[iteracion % patron_len];
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
                    case 0:
                        nueva_y--; break;
                    case 1: 
                        nueva_y++;break;
                    case 2: 
                        nueva_x--; break;
        
                    case 3: 
                        nueva_x++; break;
                    }
                }break;
            }
        nueva_x = min(juego->ancho - 1, max(0, nueva_x));
        nueva_y = min(juego->alto - 1, max(0, nueva_y));

        pokemon->x = nueva_x;
        pokemon->y = nueva_y; 
        pokemon->iteracion++;
    }
}

void incrementar_multiplicador(juego_t* juego, pokemon_juego_t* poke_atrapado) {
    if(!juego->variables.ultimo_poke_capturado) {
        juego->variables.multiplicador = 1;
        juego->variables.combo_maximo = 1;
        return;
    }
    if(juego->variables.ultimo_poke_capturado->color == poke_atrapado->color || 
        juego->variables.ultimo_poke_capturado->nombre[0] == poke_atrapado->nombre[0]) {
        juego->variables.multiplicador *= 2;
        if(1 + my_log2(juego->variables.multiplicador) > juego->variables.combo_maximo)
            juego->variables.combo_maximo++;
        return;
    }
    juego->variables.multiplicador = 1;

}

void capturar_pokemon(juego_t *juego) {
    jugador_t* jugador = juego->jugador;
    size_t cant_capturados = 0;
    int cantidad_pokemones = (int)lista_cantidad_elementos(juego->pokemones_tablero);
    for(int i = cantidad_pokemones - 1; i >= 0; i--){
        pokemon_juego_t *pokemon_actual = NULL;
        lista_obtener_elemento(juego->pokemones_tablero, (size_t)i, (void**)&pokemon_actual);
        if (pokemon_actual) {
            if (jugador->x == pokemon_actual->x && jugador->y == pokemon_actual->y) {
                lista_quitar_elemento(juego->pokemones_tablero, (size_t)i, NULL);
                pila_apilar(juego->pokemones_capturados, (void*)pokemon_actual);
                incrementar_multiplicador(juego, pokemon_actual);
                juego->variables.cant_atrapados++;
                juego->variables.puntos_obtenidos += (pokemon_actual->puntos * juego->variables.multiplicador);
                juego->variables.ultimo_poke_capturado = pokemon_actual;
                cant_capturados++;
            }        
        }
    }
    juego_agregar_pokemones(juego, cant_capturados);
}

void juego_mover(int entrada, juego_t* juego) {
    if (!juego || !entrada || toupper(entrada) == 'Q')
        return;
    mover_jugador(entrada, juego);
    mover_pokemones(entrada, juego);
    capturar_pokemon(juego);
}

void dibujar_cabecera(juego_t* juego){
    printf(ANSI_COLOR_WHITE "┌───────────────────────────────────────────────┐\n" ANSI_COLOR_RESET);
	printf("  Utilizar " ANSI_COLOR_CYAN ANSI_COLOR_BOLD
	       "⬆⬇⬅➡" ANSI_COLOR_RESET " para moverse\n");

	printf("  Presionar " ANSI_COLOR_RED ANSI_COLOR_BOLD "Q" ANSI_COLOR_RESET
	       " para salir\n");
    printf("  🌱 Semilla N°: %ld \n", juego->semilla);
    printf(ANSI_COLOR_WHITE "└───────────────────────────────────────────────┘\n" ANSI_COLOR_RESET);
}

void dibujar_pie(juego_t* juego) {
    printf(ANSI_COLOR_WHITE "┌───────────────────────────────────────────────┐\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_WHITE "  Tiempo: %zu  Puntaje: %ld  Multiplicador: %ld.0\n" ANSI_COLOR_RESET,
           juego->variables.segundos_restantes, juego->variables.puntos_obtenidos, juego->variables.multiplicador);
    printf(ANSI_COLOR_WHITE "  Capturados: %ld  Último capturado: " ANSI_COLOR_RESET ANSI_COLOR_BOLD "%s %s\n" ANSI_COLOR_RESET, 
           juego->variables.cant_atrapados,
           juego->variables.ultimo_poke_capturado ? juego->variables.ultimo_poke_capturado->color : "",
           juego->variables.ultimo_poke_capturado ? juego->variables.ultimo_poke_capturado->nombre : "Ninguno");
    printf(ANSI_COLOR_WHITE "└───────────────────────────────────────────────┘\n" ANSI_COLOR_RESET);
}

void juego_dibujar_tablero(juego_t* juego) {
    dibujar_cabecera(juego);
    elemento_t** tablero = calloc((size_t)juego->alto, sizeof(elemento_t*));
    if (!tablero) {
        fprintf(stderr, "Error al asignar memoria para el tablero\n");
        return;
    }

    for (size_t i = 0; i < juego->alto; i++) {
        tablero[i] = calloc((size_t)juego->ancho, sizeof(elemento_t));
        if (!tablero[i]) {
            fprintf(stderr, "Error al asignar memoria para una fila del tablero\n");
            for (size_t k = 0; k < i; k++) free(tablero[k]);
            free(tablero);
            return;
        }
    }
    tablero[juego->jugador->y][juego->jugador->x].x = juego->jugador->x;
    tablero[juego->jugador->y][juego->jugador->x].y = juego->jugador->y;
    tablero[juego->jugador->y][juego->jugador->x].icono = juego->jugador->icono;
    tablero[juego->jugador->y][juego->jugador->x].color = ANSI_COLOR_WHITE;

    size_t cantidad_pokemones = lista_cantidad_elementos(juego->pokemones_tablero);
    for (size_t k = 0; k < cantidad_pokemones; k++) {
        pokemon_juego_t* pokemon_actual = NULL;
        lista_obtener_elemento(juego->pokemones_tablero, k, (void**)&pokemon_actual);
        if (pokemon_actual &&
            pokemon_actual->x < juego->ancho &&
            pokemon_actual->y < juego->alto) {
            tablero[pokemon_actual->y][pokemon_actual->x].x = pokemon_actual->x;
            tablero[pokemon_actual->y][pokemon_actual->x].y = pokemon_actual->y;
            tablero[pokemon_actual->y][pokemon_actual->x].icono = pokemon_actual->nombre[0];
            tablero[pokemon_actual->y][pokemon_actual->x].color = pokemon_actual->color;
        }
    }
    printf("╔");
    for (size_t j = 0; j < juego->ancho; j++)
        printf("═");
    printf("╗\n");

    for (size_t i = 0; i < juego->alto; i++) {
        printf("║");
        for (size_t j = 0; j < juego->ancho; j++) {
            elemento_t elemento = tablero[i][j];
            if (elemento.icono != '\0')
                printf("%s" ANSI_COLOR_BOLD "%c" ANSI_COLOR_RESET, elemento.color, elemento.icono);
            else 
                printf(" ");
        }
        printf("║\n");
    }

    printf("╚");
    for (size_t j = 0; j < juego->ancho; j++)
        printf("═");
    printf("╝\n");

    for (size_t i = 0; i < juego->alto; i++) {
        free(tablero[i]);
    }
    free(tablero);
    dibujar_pie(juego);
}

void juego_mostrar_estadisticas(juego_t* juego) {
    if (juego->variables.cant_atrapados == 0) {
        printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "\n NO ATRAPASTE NINGÚN POKEMÓN\n Más suerte la próxima!\n" ANSI_COLOR_RESET);
        return;
    }
    printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "\n Combo máximo: %ld  Multiplicador máximo: %ld.0  Total atrapados: %ld\n" ANSI_COLOR_RESET, 
    juego->variables.combo_maximo, my_pow2(juego->variables.combo_maximo - 1), juego->variables.cant_atrapados);
    printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD " De último a primero:\n" ANSI_COLOR_RESET);
    while (!pila_esta_vacía(juego->pokemones_capturados)) {
        pokemon_juego_t* atrapado = pila_desapilar(juego->pokemones_capturados);
        if (atrapado) {
            printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD" - " ANSI_COLOR_RESET ANSI_COLOR_BOLD " %s%s\n" ANSI_COLOR_RESET, atrapado->color, atrapado->nombre);
            destruir_pokemon(atrapado);
        }
    }
}