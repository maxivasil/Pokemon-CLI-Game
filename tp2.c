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

#define ANCHO_TABLERO 32
#define ALTO_TABLERO 15
#define SEGUNDOS_DE_JUEGO 40
#define CANTIDAD_POKEMONES_A_AGREGAR 7

typedef struct archivo_csv Archivo;

typedef struct contexto {
    pokedex_t* pokedex;                     // la pokedex a iterar
    bool (*f_impresion)(void*, void*); // función para imprimir pokémon
} contexto_iterar_t;

bool imprimir_pokemon(void *elemento, void *ctx)
{
	if (!elemento)
		return false;

	pokemon_t *pokemon = (pokemon_t *)elemento;

	char *nombre = pokemon->nombre;
    size_t puntos = pokemon->puntos;
    char *color = pokemon->color;
    char *patron = pokemon->patron_movimiento;

    printf(ANSI_COLOR_BOLD "%s %s " ANSI_COLOR_RESET "(%s) " ANSI_COLOR_BOLD "→ %ld puntos\n" ANSI_COLOR_RESET, color, nombre, patron,puntos);  // Imprimir con color
    return true;
}

int comparar_nombre_pokemon(void *poke1, void *poke2)
{
	pokemon_t *pokemon1 = (pokemon_t *)poke1;
	pokemon_t *pokemon2 = (pokemon_t *)poke2;

	return strcmp(pokemon1->nombre, pokemon2->nombre);
}

void destruir_pokemon(void *pokemon_void) {
    if (!pokemon_void) return;

    pokemon_t *pokemon = (pokemon_t *)pokemon_void;
    free(pokemon->nombre);
    free(pokemon->patron_movimiento);
    free(pokemon);
}

bool leer_nombre(const char *str, void *ctx)
{
	char *nuevo = malloc(strlen(str) + 1);
	if (nuevo == NULL)
		return false;
	strcpy(nuevo, str);
	*(char **)ctx = nuevo;
	return true;
}

bool leer_int(const char *str, void *ctx)
{
	return sscanf(str, "%d", (int *)ctx) == 1;
}

// --- Función para obtener colores ANSI ---
char* obtener_color_ansi(char* color) {
    switch (color[0]) { // Usamos el primer carácter del nombre para el switch
		case 'A':  // AZUL O AMARILLO
            if (strcmp(color, "AZUL") == 0) return ANSI_COLOR_BLUE;
			if (strcmp(color, "AMARILLO") == 0) return ANSI_COLOR_YELLOW;
            break;
		case 'B':  // AZUL O AMARILLO
            if (strcmp(color, "BLANCO") == 0) return ANSI_COLOR_BLACK;
            break;
		case 'C':  // AZUL O AMARILLO
            if (strcmp(color, "CYAN") == 0) return ANSI_COLOR_CYAN;
            break;
        case 'M':  // MAGENTA
            if (strcmp(color, "MAGENTA") == 0) return ANSI_COLOR_MAGENTA;
            break;
		case 'N':  // AZUL O AMARILLO
            if (strcmp(color, "NEGRO") == 0) return ANSI_COLOR_BLACK;
            break;
		case 'R':  // ROJO
            if (strcmp(color, "ROJO") == 0) return ANSI_COLOR_RED;
            break;
		case 'V':  // VERDE
            if (strcmp(color, "VERDE") == 0) return ANSI_COLOR_GREEN;
            break;
    }
    return ANSI_COLOR_RESET; //si no se encuentra el color
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


int logica(int entrada, void *datos) {
    juego_t *juego = (juego_t *)datos;

    borrar_pantalla();

	procesar_entrada(entrada,juego);
	juego->jugador->iteraciones++;
	dibujar_cabecera(juego);

    mover_jugador(juego);
    mover_pokemones(juego);

    capturar_pokemon(juego);

    dibujar_tablero(juego); // Renderizar el tablero.
    mostrar_estadisticas(juego); // Puntaje, tiempo, etc.

    return entrada == 'q' || entrada == 'Q' || juego->segundos <= 0;
}

void jugar_con_semilla(void* nada){
	return;
}

size_t generar_posicion_random(size_t limite)
{
    return (size_t)rand() % limite;
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

void liberar_todo(struct archivo_csv *archivo, menu_t *menu, void(*fm)(void*), contexto_iterar_t *contexto_iterar, pokedex_t *pokedex, void(*fp)(void*), juego_t *juego) {
	cerrar_archivo_csv(archivo);
	menu_destruir(menu, fm); 
	free(contexto_iterar);
	pokedex_destruir_todo(pokedex, fp);
	juego_destruir(juego);
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

void mezclar_vector(pokemon_t** vector, size_t n) {
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t) rand() % (i + 1); // genera un índice aleatorio entre 0 e i
        pokemon_t* temp = vector[i];
        vector[i] = vector[j];
        vector[j] = temp;
    }
}

void pokedex_imprimir(void* contexto) {
    //paso el contexto como si fuera lo q queria
    contexto_iterar_t* ctx = (contexto_iterar_t*)contexto;
    //y aca hago lo de iterar
    pokedex_iterar(ctx->pokedex, ctx->f_impresion, NULL);
}

void destruir_accion(void *accion_void){
    accion_t *accion = (accion_t *)accion_void;
    if(!accion)
        return;
    free(accion->descripcion); //libero la descripcion q reserve memoria
    free(accion);
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
	pokedex_t* pokedex = pokedex_crear(comparar_nombre_pokemon);
	if (!pokedex) {
		printf("No se pudo crear la Pokedex.\n");
		cerrar_archivo_csv(archivo);
		return 0;
	}
	cargar_pokedex_pokemon(archivo, pokedex);
	printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "BIENVENID@ A POKEMON RUN\n" ANSI_COLOR_RESET);
	menu_t* menu = menu_crear();
	if (!menu) {
        printf("Error al crear el menú.\n");
        return -1;
    }

	contexto_iterar_t* contexto_iterar = malloc(sizeof(contexto_iterar_t));
	if (!contexto_iterar) {
		//ver que poner aca
    	return -1;
	}
	contexto_iterar->pokedex = pokedex;
	contexto_iterar->f_impresion = imprimir_pokemon; 

	menu_agregar_opcion(menu,"P", "Mostrar pokemones",pokedex_imprimir, contexto_iterar); //meter los pokemones en un abb con inorden
	
	
	menu_agregar_opcion(menu,"J", "Iniciar Juego",NULL ,NULL);
	menu_agregar_opcion(menu,"S", "Iniciar Juego con semilla", jugar_con_semilla ,NULL);
	menu_agregar_opcion(menu,"Q", "Salir", NULL ,NULL);
	menu_mostrar(menu);
	
	
	struct jugador jugador = { 0 };
	struct juego* juego_nuevo = juego_crear(ANCHO_TABLERO, ALTO_TABLERO, SEGUNDOS_DE_JUEGO, &jugador);
	
	pokemon_t** vector = malloc(pokedex_cantidad(pokedex) * sizeof(pokemon_t*));
	if (!vector) 
    	return -1;
	size_t cantidad_pokemones = pokedex_vectorizar(pokedex, vector, pokedex_cantidad(pokedex));
	if (cantidad_pokemones < CANTIDAD_POKEMONES_A_AGREGAR) {
    	printf("No hay suficientes pokemones en la pokedex para jugar\n");
    	free(vector);
    	return -1;
	}
	
	mezclar_vector(vector, cantidad_pokemones);

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
		liberar_todo(archivo, menu, destruir_accion, contexto_iterar, pokedex, destruir_pokemon, juego_nuevo);
		return 0;
		}
	} while (opcion != 'Q');  // ver esto pq no esta bien. despuesde jugar una vez ya 
	
	game_loop(logica, &juego_nuevo); //ir en jugar
	mostrar_cursor();

	return 0;
}