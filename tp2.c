#include "extra/engine.h"
#include "extra/ansi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "src/menu.h"
#include "src/juego.h"
#include "src/pokedex.h"
#include <time.h>
#include "src/csv.h"
#include "src/utils.h"


typedef struct archivo_csv Archivo;

typedef struct contexto_jugar {
	juego_t* juego;
    int (*f_logica)(int entrada, void* datos);
} contexto_jugar_t;

void liberar_todo(struct archivo_csv *archivo, menu_t *menu, contexto_jugar_t* contexto_jugar, pokedex_t *pokedex, juego_t *juego) {
	cerrar_archivo_csv(archivo);
	menu_destruir_todo(menu);
	free(contexto_jugar);
	pokedex_destruir_todo(pokedex);
	juego_destruir(juego);
}

int logica(int entrada, void *datos) {
    juego_t *juego = (juego_t *)datos;

    borrar_pantalla();

	//procesar_entrada(entrada,juego);  IRIA DENTRO DE mover_jugador

	dibujar_cabecera(juego);

    mover_jugador(entrada, juego);
    mover_pokemones(juego);

    capturar_pokemon(juego);

    dibujar_tablero(juego); // Renderizar el tablero.
    mostrar_estadisticas(juego); // Puntaje, tiempo, etc.

    return entrada == 'q' || entrada == 'Q' || juego->segundos <= 0;
}

void jugar(void *logica)
{
	contexto_jugar_t* ctx = (contexto_jugar_t*)logica;
    ctx->juego->jugador->x = (size_t)rand() % ANCHO_TABLERO; 
	ctx->juego->jugador->y = (size_t)rand() % ALTO_TABLERO; 
	ctx->juego->segundos = 0;
    game_loop(ctx->f_logica, ctx->juego);
}

void jugar_con_semilla(void* nada){
	return;
}


void cargar_pokedex_pokemon(Archivo *archivo, pokedex_t* pokedex)
{
	bool (*funciones[])(const char *, void *) = { leer_nombre,
						      leer_int, leer_nombre, leer_nombre};
	char *nombre = NULL;
	size_t puntos = 0;
	char *color= NULL;
	char *patron_mov = NULL;

	void *punteros[] = { &nombre, &puntos, &color, &patron_mov};

	while (leer_linea_csv(archivo, 5, funciones, punteros) > 0) {
		pokemon_t *pokemon_leido = malloc(sizeof(struct pokemon));
		if (!pokemon_leido) {
			free(nombre); 
            free(color);  //VER SI ESTO HACE FALTA 
            free(patron_mov);
			break;
		}

		pokemon_leido->nombre = nombre;
		pokemon_leido->puntos = (size_t)puntos;

		if (patron_mov) {
            size_t len = strlen(patron_mov);
            if (len > 0 && patron_mov[len - 1] == '\n') {
                patron_mov[len - 1] = '\0'; // saco el '\n'
            }
        }
        pokemon_leido->color = obtener_color_ansi(color);
		free(color);

		pokemon_leido->patron_movimiento = patron_mov;
		pokemon_leido->x = generar_posicion_random(ANCHO_TABLERO);
		pokemon_leido->y = generar_posicion_random(ALTO_TABLERO);
		pokedex_insertar(pokedex, pokemon_leido);
	}
}

int obtener_opcion()
{
	int opcion_elegida;

	while (true) {
		char opcion[20];
		printf("Ingrese una opción: ");

		if (fgets(opcion, sizeof(opcion), stdin) == NULL)
			return 0;

		opcion_elegida = toupper(opcion[0]);

		if (opcion_elegida == 'P' || opcion_elegida == 'S' || opcion_elegida== 'Q' || opcion_elegida == 'J') {
            break;  // Opción válida, salir del bucle
        } else {
            printf("Opción no válida, intente nuevamente.\n");
        }
    }
    return opcion_elegida;
}

/*
* Ejecuta la opcion previamente ingresada por el usuario.
*/
void ejecutar_opcion(int opcion_a_ejecutar, menu_t* menu)
{
	// Convertir la opción de tipo 'char' a 'char*' para usarla como clave en el hash
	char opcion_str[2] = {(char)opcion_a_ejecutar, '\0' };
	menu_ejecutar_opcion(menu, opcion_str);
	menu_mostrar(menu); //vuelvo a mostrar el menu
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Argumentos no validos, ingrese un archivo a 'analizar'\n");
		return -1;
	}
	srand((unsigned int)time(NULL)); // Inicializa la semilla de aleatoriedad

	Archivo *archivo = abrir_archivo_csv(argv[1], ',');
	if (!archivo){
		printf("No se pudo abrir el archivo CSV.\n");
		return 0;
	}
	pokedex_t* pokedex = pokedex_crear();
	if (!pokedex) {
		printf("No se pudo crear la Pokedex.\n");
		cerrar_archivo_csv(archivo);
		return 0;
	}
	cargar_pokedex_pokemon(archivo, pokedex);
	printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "\nBIENVENID@ A POKEMON RUN\n" ANSI_COLOR_RESET);
	menu_t* menu = menu_crear();
	if (!menu) {
        printf("Error al crear el menú.\n");
        return -1;
    }
	
	struct jugador jugador = { 0 };
	struct juego* juego_nuevo = juego_crear(ANCHO_TABLERO, ALTO_TABLERO, SEGUNDOS_DE_JUEGO, &jugador); 
	
	contexto_jugar_t* contexto_jugar = malloc(sizeof(contexto_jugar_t));
	if (!contexto_jugar)
		return -1;
	contexto_jugar->juego = juego_nuevo;
	contexto_jugar->f_logica = logica;

	menu_agregar_opcion(menu,"P", "Mostrar pokemones",pokedex_imprimir, (void*)pokedex); //meter los pokemones en un abb con inorden
	menu_agregar_opcion(menu,"J", "Iniciar Juego", jugar , contexto_jugar);
	menu_agregar_opcion(menu,"S", "Iniciar Juego con semilla", jugar_con_semilla ,NULL);
	menu_agregar_opcion(menu,"Q", "Salir", NULL ,NULL);
	menu_mostrar(menu);

	pokemon_t** vector = malloc(pokedex_cantidad(pokedex) * sizeof(pokemon_t*));
	if (!vector) 
    	return -1;
	size_t cantidad_pokemones = pokedex_vectorizar(pokedex, vector, pokedex_cantidad(pokedex));
	if (cantidad_pokemones < CANTIDAD_POKEMONES_A_AGREGAR) {
    	printf("No hay suficientes pokemones en la pokedex para jugar\n");
    	free(vector);
    	return -1;
	}

	for (size_t i = 0; i < CANTIDAD_POKEMONES_A_AGREGAR; i++) {
    	juego_agregar_pokemon(juego_nuevo, vector[i]);
	}
	free(vector); //libero el vector q recien use. ver si hace falta q posta sea dinamico!!!!!!
	
	int opcion;
	// Bucle que sigue ejecutándose hasta que se elige salir ('Q')
	do {
		opcion = obtener_opcion();  // esperar una opción válida
		if (opcion != 'Q') {        // ejecutar solo si no se elige 'Q'
			ejecutar_opcion(opcion, menu);  // ejecutar la opción seleccionada
		} else {
		liberar_todo(archivo, menu, contexto_jugar, pokedex, juego_nuevo);
		return 0;
		}
	} while (opcion != 'Q');  // ver esto pq no esta bien. despuesde jugar una vez ya 
	
	game_loop(logica, &juego_nuevo); //ir en jugar
	mostrar_cursor();

	return 0;
}