#include <stdio.h>
#include <string.h>
#include "../extra/ansi.h"
#include "../extra/engine.h"
#include "lista.h"
#include "juego.h"
#include "utils.h"

struct pokemon_juego {
	char *nombre;
	size_t puntos;
	char *color;
	char *patron_movimiento;
	int x;
	int y;
	size_t iteracion;
};

typedef struct elemento {
	int x;
	int y;
	char icono;
	char *color;
} elemento_t;

typedef struct contexto_agregar_pokemones {
	int ancho;
	int alto;
	Lista *lista;
} ctx_t;

int generar_posicion_random(int limite)
{
	return (int)rand() % limite;
}

void limitar_rango(int *valor, int minimo, int maximo)
{
	*valor = (*valor < minimo) ? minimo :
		 (*valor > maximo) ? maximo :
				     *valor;
}

size_t my_pow2(size_t x)
{
	return 1ULL << (x);
}

size_t my_log2(size_t n)
{
	size_t log = 0;
	while (n >>= 1)
		log++;
	return log;
}

void procesar_entrada(int entrada, struct juego *juego)
{
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

	limitar_rango(&(jugador->x), 0, juego->ancho - 1);
	limitar_rango(&(jugador->y), 0, juego->alto - 1);
}

pokemon_juego_t *copiar_pokemon(pokemon_juego_t *pokemon)
{
	pokemon_juego_t *pokemon_copia = calloc(1, sizeof(pokemon_juego_t));
	if (!pokemon_copia)
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

bool agregar_pokemon_a_lista(void *pokemon, void *ctx)
{
	if (!pokemon || !ctx)
		return false;
	ctx_t *contexto = (ctx_t *)ctx;
	pokemon_t *poke_parseado = (pokemon_t *)pokemon;
	pokemon_juego_t pokemon_juego = {
		.nombre = poke_parseado->nombre,
		.puntos = poke_parseado->puntos,
		.color = poke_parseado->color,
		.patron_movimiento = poke_parseado->patron_movimiento,
		.x = generar_posicion_random(contexto->ancho),
		.y = generar_posicion_random(contexto->alto),
		.iteracion = 0
	};
	pokemon_juego_t *copia_poke = copiar_pokemon(&pokemon_juego);
	lista_agregar_al_final(contexto->lista, copia_poke);
	return true;
}

bool agregar_pokemones(juego_t *juego, size_t cant_pokemones_a_agregar)
{
	if (!juego || cant_pokemones_a_agregar < 1 ||
	    lista_cantidad_elementos(juego->fuente_de_pokemones) == 0)
		return false;
	//juego->cant_pokemones_tablero = cant_pokemones_a_agregar;
	for (size_t i = 0; i < cant_pokemones_a_agregar; i++) {
		size_t indice = (size_t)generar_posicion_random(
			(int)lista_cantidad_elementos(
				juego->fuente_de_pokemones));
		pokemon_juego_t *poke = NULL;
		if (!lista_obtener_elemento(juego->fuente_de_pokemones, indice,
					    (void **)&poke))
			return false;
		ctx_t ctx = { .alto = juego->alto,
			      .ancho = juego->ancho,
			      .lista = juego->pokemones_tablero };
		if (!agregar_pokemon_a_lista((void *)poke, (void *)&ctx))
			return false;
	}
	return true;
}

bool juego_subir_pokemones(juego_t *juego, pokedex_t *pokedex,
			   size_t cantidad_pokemones_tablero)
{
	if (!juego)
		return false;
	ctx_t ctx = { .alto = juego->alto,
		      .ancho = juego->ancho,
		      .lista = juego->fuente_de_pokemones };
	if (pokedex_iterar(pokedex, agregar_pokemon_a_lista, (void *)&ctx) <
	    pokedex_cantidad(pokedex))
		return false;
	juego->cant_pokemones_tablero = cantidad_pokemones_tablero;
	return agregar_pokemones(juego, juego->cant_pokemones_tablero);
}

juego_t *juego_crear(int ancho, int alto, size_t segundos, char icono_jugador)
{
	if (ancho < 1 || alto < 1 || segundos < 1)
		return NULL;
	juego_t *juego = calloc(1, sizeof(juego_t));
	if (!juego)
		return NULL;
	juego->jugador = calloc(1, sizeof(jugador_t));
	if (!juego->jugador) {
		juego_destruir(juego);
		return NULL;
	}
	juego->pokemones_tablero = lista_crear();
	if (!juego->pokemones_tablero) {
		juego_destruir(juego);
		return NULL;
	}
	juego->pokemones_capturados = pila_crear();
	if (!juego->pokemones_capturados) {
		juego_destruir(juego);
		return NULL;
	}
	juego->fuente_de_pokemones = lista_crear();
	if (!juego->fuente_de_pokemones) {
		juego_destruir(juego);
		return NULL;
	}
	juego->variables.multiplicador = 1;
	juego->jugador->icono = icono_jugador;
	juego->variables.segundos_restantes = segundos;
	juego->ancho = ancho;
	juego->alto = alto;

	return juego;
}

void juego_destruir(juego_t *juego)
{
	if (!juego)
		return;
	if (juego->jugador)
		free(juego->jugador);
	if (juego->fuente_de_pokemones)
		lista_destruir_todo(juego->fuente_de_pokemones,
				    destruir_pokemon);
	if (juego->pokemones_tablero)
		lista_destruir_todo(juego->pokemones_tablero, destruir_pokemon);
	if (juego->pokemones_capturados)
		pila_destruir_todo(juego->pokemones_capturados,
				   destruir_pokemon);
	free(juego);
}

void mover_jugador(int entrada, juego_t *juego)
{
	if (!juego || !juego->jugador)
		return;
	procesar_entrada(entrada, juego);
}

void obtener_nueva_posicion_por_movimiento(char movimiento, int *nueva_x,
					   int *nueva_y)
{
	switch (movimiento) {
	case 'N':
		(*nueva_y)--;
		break;
	case 'S':
		(*nueva_y)++;
		break;
	case 'E':
		(*nueva_x)++;
		break;
	case 'O':
		(*nueva_x)--;
		break;
	}
}

void obtener_nueva_posicion_por_jugador(int movimiento_jugador, int *nueva_x,
					int *nueva_y, bool invertido)
{
	switch (movimiento_jugador) {
	case TECLA_DERECHA:
		*nueva_x += (invertido ? -1 : 1);
		break;
	case TECLA_IZQUIERDA:
		*nueva_x += (invertido ? 1 : -1);
		break;
	case TECLA_ABAJO:
		*nueva_y += (invertido ? -1 : 1);
		break;
	case TECLA_ARRIBA:
		*nueva_y += (invertido ? 1 : -1);
		break;
	}
}

void obtener_nueva_posicion_aleatoria(int *nueva_x, int *nueva_y)
{
	int direccion = generar_posicion_random(4);
	switch (direccion) {
	case 0:
		(*nueva_y)--;
		break;
	case 1:
		(*nueva_y)++;
		break;
	case 2:
		(*nueva_x)--;
		break;
	case 3:
		(*nueva_x)++;
		break;
	}
}

void actualizar_posicion_pokemon(pokemon_juego_t *pokemon,
				 int movimiento_jugador, int ancho, int alto)
{
	if (!pokemon || !pokemon->patron_movimiento)
		return;

	size_t patron_len = strlen(pokemon->patron_movimiento);
	if (patron_len == 0)
		return;

	size_t iteracion = pokemon->iteracion;
	char movimiento = pokemon->patron_movimiento[iteracion % patron_len];
	int nueva_x = pokemon->x;
	int nueva_y = pokemon->y;

	switch (movimiento) {
	case 'N':
	case 'S':
	case 'E':
	case 'O':
		obtener_nueva_posicion_por_movimiento(movimiento, &nueva_x,
						      &nueva_y);
		break;
	case 'J':
		obtener_nueva_posicion_por_jugador(movimiento_jugador, &nueva_x,
						   &nueva_y, false);
		break;
	case 'I':
		obtener_nueva_posicion_por_jugador(movimiento_jugador, &nueva_x,
						   &nueva_y, true);
		break;
	case 'R':
		obtener_nueva_posicion_aleatoria(&nueva_x, &nueva_y);
		break;
	}

	limitar_rango(&(nueva_x), 0, ancho - 1);
	limitar_rango(&(nueva_y), 0, alto - 1);

	pokemon->x = nueva_x;
	pokemon->y = nueva_y;
	pokemon->iteracion++;
}

void mover_pokemones(int movimiento_jugador, juego_t *juego)
{
	if (!juego)
		return;

	size_t cantidad_pokemones =
		lista_cantidad_elementos(juego->pokemones_tablero);
	for (size_t k = 0; k < cantidad_pokemones; k++) {
		pokemon_juego_t *pokemon = NULL;
		lista_obtener_elemento(juego->pokemones_tablero, k,
				       (void **)&pokemon);
		actualizar_posicion_pokemon(pokemon, movimiento_jugador,
					    juego->ancho, juego->alto);
	}
}

void incrementar_multiplicador(juego_t *juego, pokemon_juego_t *poke_atrapado)
{
	if (!juego->variables.ultimo_poke_capturado) {
		juego->variables.combo_maximo = 1;
		return;
	}
	if (juego->variables.ultimo_poke_capturado->color ==
		    poke_atrapado->color ||
	    juego->variables.ultimo_poke_capturado->nombre[0] ==
		    poke_atrapado->nombre[0]) {
		juego->variables.multiplicador *= 2;
		if (1 + my_log2(juego->variables.multiplicador) >
		    juego->variables.combo_maximo)
			juego->variables.combo_maximo++;
		return;
	}
	juego->variables.multiplicador = 1;
}

void procesar_captura_pokemon(juego_t *juego, pokemon_juego_t *pokemon,
			      size_t indice)
{
	if (!pokemon)
		return;

	lista_quitar_elemento(juego->pokemones_tablero, indice, NULL);
	pila_apilar(juego->pokemones_capturados, pokemon);
	incrementar_multiplicador(juego, pokemon);
	juego->variables.cant_atrapados++;
	juego->variables.puntos_obtenidos +=
		(pokemon->puntos * juego->variables.multiplicador);
	juego->variables.ultimo_poke_capturado = pokemon;
}

void capturar_pokemon(juego_t *juego)
{
	if (!juego)
		return;
	jugador_t *jugador = juego->jugador;
	size_t cant_capturados = 0;

	int cantidad_pokemones =
		(int)lista_cantidad_elementos(juego->pokemones_tablero);
	for (int i = cantidad_pokemones - 1; i >= 0; i--) {
		pokemon_juego_t *pokemon_actual = NULL;
		lista_obtener_elemento(juego->pokemones_tablero, (size_t)i,
				       (void **)&pokemon_actual);

		if (pokemon_actual && jugador->x == pokemon_actual->x &&
		    jugador->y == pokemon_actual->y) {
			procesar_captura_pokemon(juego, pokemon_actual,
						 (size_t)i);
			cant_capturados++;
		}
	}

	agregar_pokemones(juego, cant_capturados);
}

void juego_mover(int entrada, juego_t *juego)
{
	if (!juego || !entrada || toupper(entrada) == 'Q')
		return;
	mover_jugador(entrada, juego);
	mover_pokemones(entrada, juego);
	capturar_pokemon(juego);
}

void dibujar_cabecera(juego_t *juego)
{
	printf(ANSI_COLOR_WHITE
	       "┌─────────────────────────────────────────────┐\n" ANSI_COLOR_RESET);
	printf("  Utilizar " ANSI_COLOR_CYAN ANSI_COLOR_BOLD
	       "⬆⬇⬅➡" ANSI_COLOR_RESET " para moverse\n");

	printf("  Presionar " ANSI_COLOR_RED ANSI_COLOR_BOLD
	       "Q" ANSI_COLOR_RESET " para salir\n");
	printf("  🌱 %ld \n", juego->semilla);
	printf(ANSI_COLOR_WHITE
	       "└─────────────────────────────────────────────┘\n" ANSI_COLOR_RESET);
}

void dibujar_pie(juego_t *juego)
{
	printf(ANSI_COLOR_WHITE
	       "┌─────────────────────────────────────────────┐\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_WHITE
	       "  ⌚ %zu  💯 %ld  ✖️​  %ld.0\n" ANSI_COLOR_RESET,
	       juego->variables.segundos_restantes,
	       juego->variables.puntos_obtenidos,
	       juego->variables.multiplicador);
	printf(ANSI_COLOR_WHITE
	       "  Capturados: %ld"
	       "Último capturado: " ANSI_COLOR_RESET ANSI_COLOR_BOLD
	       "%s %s\n" ANSI_COLOR_RESET,
	       juego->variables.cant_atrapados,
	       juego->variables.ultimo_poke_capturado ?
		       juego->variables.ultimo_poke_capturado->color :
		       "",
	       juego->variables.ultimo_poke_capturado ?
		       juego->variables.ultimo_poke_capturado->nombre :
		       "Ninguno");
	printf(ANSI_COLOR_WHITE
	       "└─────────────────────────────────────────────┘\n" ANSI_COLOR_RESET);
}

void dibujar_bordes_tablero(size_t ancho)
{
	printf("╔");
	for (size_t j = 0; j < ancho; j++)
		printf("═");
	printf("╗\n");
}

void dibujar_filas_tablero(elemento_t **tablero, size_t ancho, size_t alto)
{
	for (size_t i = 0; i < alto; i++) {
		printf("║");
		for (size_t j = 0; j < ancho; j++) {
			elemento_t elemento = tablero[i][j];
			if (elemento.icono != '\0')
				printf("%s" ANSI_COLOR_BOLD
				       "%c" ANSI_COLOR_RESET,
				       elemento.color, elemento.icono);
			else
				printf(" ");
		}
		printf("║\n");
	}
}

void inicializar_tablero(elemento_t ***tablero, size_t ancho, size_t alto)
{
	*tablero = calloc(alto, sizeof(elemento_t *));
	for (size_t i = 0; i < alto; i++)
		(*tablero)[i] = calloc(ancho, sizeof(elemento_t));
}

void liberar_tablero(elemento_t **tablero, size_t alto)
{
	for (size_t i = 0; i < alto; i++)
		free(tablero[i]);
	free(tablero);
}

void juego_dibujar_tablero(juego_t *juego)
{
	if (!juego)
		return;
	dibujar_cabecera(juego);

	elemento_t **tablero = NULL;
	inicializar_tablero(&tablero, (size_t)juego->ancho,
			    (size_t)juego->alto);

	tablero[juego->jugador->y][juego->jugador->x].x = juego->jugador->x;
	tablero[juego->jugador->y][juego->jugador->x].y = juego->jugador->y;
	tablero[juego->jugador->y][juego->jugador->x].icono =
		juego->jugador->icono;
	tablero[juego->jugador->y][juego->jugador->x].color = ANSI_COLOR_WHITE;

	size_t cantidad_pokemones =
		lista_cantidad_elementos(juego->pokemones_tablero);
	for (size_t k = 0; k < cantidad_pokemones; k++) {
		pokemon_juego_t *pokemon_actual = NULL;
		lista_obtener_elemento(juego->pokemones_tablero, k,
				       (void **)&pokemon_actual);
		if (pokemon_actual && pokemon_actual->x < juego->ancho &&
		    pokemon_actual->y < juego->alto) {
			tablero[pokemon_actual->y][pokemon_actual->x].x =
				pokemon_actual->x;
			tablero[pokemon_actual->y][pokemon_actual->x].y =
				pokemon_actual->y;
			tablero[pokemon_actual->y][pokemon_actual->x].icono =
				pokemon_actual->nombre[0];
			tablero[pokemon_actual->y][pokemon_actual->x].color =
				pokemon_actual->color;
		}
	}

	dibujar_bordes_tablero((size_t)juego->ancho);
	dibujar_filas_tablero(tablero, (size_t)juego->ancho,
			      (size_t)juego->alto);
	printf("╚");
	for (size_t j = 0; j < juego->ancho; j++)
		printf("═");
	printf("╝\n");

	liberar_tablero(tablero, (size_t)juego->alto);
	dibujar_pie(juego);
}

void juego_mostrar_estadisticas(juego_t *juego)
{
	if (!juego)
		return;
	if (juego->variables.cant_atrapados == 0)
		printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD
		       "\n NO ATRAPASTE NINGÚN POKEMÓN\n Más suerte la próxima!\n" ANSI_COLOR_RESET);
	printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD
	       "\n TU PUNTAJE FUE: %ld\n Combo máximo: %ld  Multiplicador máximo: %ld.0  Total atrapados: %ld\n" ANSI_COLOR_RESET,
	       juego->variables.puntos_obtenidos, juego->variables.combo_maximo,
	       juego->variables.cant_atrapados ?
		       my_pow2(juego->variables.combo_maximo - 1) :
		       1,
	       juego->variables.cant_atrapados);
	if (juego->variables.cant_atrapados != 0) {
		printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD
		       " De último a primero:\n" ANSI_COLOR_RESET);
		while (!pila_esta_vacía(juego->pokemones_capturados)) {
			pokemon_juego_t *atrapado =
				pila_desapilar(juego->pokemones_capturados);
			if (atrapado) {
				printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD
				       " - " ANSI_COLOR_RESET ANSI_COLOR_BOLD
				       " %s%s\n" ANSI_COLOR_RESET,
				       atrapado->color, atrapado->nombre);
				destruir_pokemon(atrapado);
			}
		}
	}
}