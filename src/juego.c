#include <stdio.h>
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

void mover_jugador(juego_t* juego) {
    if(!juego || !juego->tablero || !juego->jugador)
        return;
    jugador_t* jugador = juego->jugador;

    if (jugador->x >= juego->ancho || jugador->y >= juego->alto) {
        fprintf(stderr, "Error: Jugador fuera de los límites del tablero\n");
        return;
    }

    casillero_t* casillero_actual = &juego->tablero[jugador->y][jugador->x];
    void *elemento_quitado = NULL;
    lista_quitar_elemento(casillero_actual->elementos, 0, &elemento_quitado);

    //la posición del jugador fye actualizada por procesar_entrad
    casillero_t* casillero_nuevo = &juego->tablero[jugador->y][jugador->x];

    //lo meto en el nuevo casillero
    lista_agregar_al_final(casillero_nuevo->elementos, jugador);
}

void mover_pokemones(juego_t *juego) {
    for (size_t i = 0; i < juego->alto; i++) {
        for (size_t j = 0; j < juego->ancho; j++) {
            casillero_t *casillero = &juego->tablero[i][j];
            Lista_iterador *iter = lista_iterador_crear(casillero->elementos);
            if (!iter) continue;

            while (lista_iterador_hay_siguiente(iter)) {
                pokemon_t *pokemon = (pokemon_t *)lista_iterador_obtener_elemento_actual(iter);
                size_t patron_len = strlen(pokemon->patron_movimiento);  // longitud del patrón
                size_t iteracion = pokemon->iteracion;  // ves q muevo al pokemon para saber q patro toda

                //muevo al pokemon en base al patron de mov
                char movimiento = pokemon->patron_movimiento[iteracion % patron_len]; // hago esto para q nunca se termine

                switch (movimiento) {
                    case 'N':  // hacia arriba
                        pokemon->y--;
                        break;
                    case 'S':  // hacia abajo
                        pokemon->y++;
                        break;
                    case 'E':  // hacia la der
                        pokemon->x++;
                        break;
                    case 'O':  // hacia la izq
                        pokemon->x--;
                        break;
                    case 'J':  // Movimiento del jugador
                        // el movimiento del pokemon sigue el del jugador
                        pokemon->x = juego->jugador->x;
                        pokemon->y = juego->jugador->y;
                        break;
                    case 'I':  // Movimiento del jugador invertido
                        // el movimiento del pokemon va en sentido opuesto al del jugador
                        pokemon->x = juego->jugador->x - (pokemon->x - juego->jugador->x);
                        pokemon->y = juego->jugador->y - (pokemon->y - juego->jugador->y);
                        break;
                    case 'R':  //al azar:
                        {
                            //arriba, abajo, izquierda, derecha -> 4 opciones 0,1,2,3
                            int direccion = rand() % 4;
                            switch (direccion) {
                                case 0:  // arriba
                                    pokemon->y--;
                                    break;
                                case 1:  // abajo
                                    pokemon->y++;
                                    break;
                                case 2:  // izq
                                    pokemon->x--;
                                    break;
                                case 3:  // der
                                    pokemon->x++;
                                    break;
                            }
                        }
                        break;
                }

                // limitar los movimientos al tablero
                pokemon->x = min(juego->ancho - 1, max(0, pokemon->x));
                pokemon->y = min(juego->alto - 1, max(0, pokemon->y));

                // incrementar la iteración del pokemon
                pokemon->iteracion++;
            }

            lista_iterador_destruir(iter);
        }
    }
}


void dibujar_tablero(juego_t *juego) {
    for (size_t i = 0; i < juego->alto; i++) {
        for (size_t j = 0; j < juego->ancho; j++) {
            casillero_t *casillero = &juego->tablero[i][j];
            printf("Casillero (%zu, %zu): ", i, j);
            // Imprimir elementos en el casillero (jugador y pokémons)
            Lista_iterador *iter = lista_iterador_crear(casillero->elementos);
            while (lista_iterador_hay_siguiente(iter)) {
                void *elemento = lista_iterador_obtener_elemento_actual(iter);
                if (elemento == (void *)juego->jugador) {
                    printf("Jugador ");
                } else {
                    pokemon_t *pokemon = (pokemon_t *)elemento;
                    printf("Pokemon (%ld) ", pokemon->puntos);
                }
                lista_iterador_avanzar(iter);
            }
            lista_iterador_destruir(iter);
            printf("\n");
        }
    }
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
            
            // // Liberar todos los elementos de la lista
            // while (lista_cantidad_elementos(casillero->elementos) > 0) {
            //     void *elemento = NULL;
            //     lista_quitar_elemento(casillero->elementos, 0, &elemento);

            //     // Liberar el elemento si corresponde
            //     if (elemento && elemento != (void *)juego->jugador) {
            //         free(elemento); // Por ejemplo, pokémones asignados dinámicamente
            //     }
            // }

            // Liberar la lista del casillero
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
