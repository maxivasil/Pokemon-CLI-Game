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

	juego->jugador->iteraciones++;
	if (juego->jugador->iteraciones % 5 == 0)
		juego->segundos--;

	dibujar_cabecera(juego);
	juego_mover(entrada, juego);
    dibujar_tablero(juego); // Renderizar el tablero.

    if(entrada == 'q' || entrada == 'Q' || juego->segundos <= 0) {
		//mostrar_estadisticas();
		return 1;
	}
	return 0;
}

bool jugar(void *logica)
{
	contexto_jugar_t* ctx = (contexto_jugar_t*)logica;
    ctx->juego->jugador->x = 0;//rand() % ANCHO_TABLERO; 
	ctx->juego->jugador->y = 0;//rand() % ALTO_TABLERO; 
	ctx->juego->segundos = SEGUNDOS_DE_JUEGO;
	if(!ctx->juego->semilla) {
		int semilla = (int)time(NULL);
		srand((unsigned int)semilla);
		semilla = rand();
		srand((unsigned int)semilla);
		ctx->juego->semilla = (size_t)semilla;
	}
    game_loop(ctx->f_logica, ctx->juego);
	return false;
}

bool jugar_con_semilla(void* contexto) {
    int semilla = 0;
    char buffer[20];
    char* endptr;

    do {
        printf("Inserte el número de semilla (entero positivo): ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Error al leer la entrada. Intente nuevamente.\n");
            continue;
        }

        semilla = (int)strtol(buffer, &endptr, 10);

        if (buffer == endptr || *endptr != '\n' || semilla <= 0) {
            printf("Entrada no válida. Asegúrese de ingresar un número entero positivo mayor que 0.\n");
        }
    } while (buffer == endptr || *endptr != '\n' || semilla <= 0);

    srand((unsigned int)semilla);

    contexto_jugar_t* ctx = (contexto_jugar_t*)contexto;
    ctx->juego->semilla = (size_t)semilla;
    return jugar((void*)ctx);
}

int obtener_opcion()
{
    int opcion_elegida;
    char opcion[20];
    do {
        printf("Ingrese una opción: ");
        
        if (fgets(opcion, sizeof(opcion), stdin) == NULL)
            return 0;  // Fin de entrada

        opcion_elegida = toupper(opcion[0]);

        if (opcion_elegida != 'P' && opcion_elegida != 'S' && opcion_elegida != 'Q' && opcion_elegida != 'J') {
            printf("Opción no válida, intente nuevamente.\n");
        }

    } while (opcion_elegida != 'P' && opcion_elegida != 'S' && opcion_elegida != 'Q' && opcion_elegida != 'J');

    return opcion_elegida;
}

/*
* Ejecuta la opcion previamente ingresada por el usuario.
*/
bool ejecutar_opcion(int opcion, menu_t* menu)
{
	// Convertir la opción de tipo 'char' a 'char*' para usarla como clave en el hash
	char opcion_str[2] = {(char)opcion, '\0' };
	if (!menu_ejecutar_opcion(menu, opcion_str)) {
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	borrar_pantalla();
	if (argc < 2) {
		printf("%s <archivo>\n", argv[0]);
		return -1;
	}
	//srand((unsigned int)time(NULL)); // Inicializa la semilla de aleatoriedad

	struct archivo_csv *archivo = abrir_archivo_csv(argv[1], ',');
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
	pokedex_insertar_desde_archivo(archivo, pokedex);
	printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD "\nBIENVENID@ A POKEMON RUN\n" ANSI_COLOR_RESET);
	menu_t* menu = menu_crear();
	if (!menu) {
        printf("Error al crear el menú.\n");
        return -1;
    }
	
	struct juego* juego_nuevo = juego_crear(ANCHO_TABLERO, ALTO_TABLERO, SEGUNDOS_DE_JUEGO, '@', CANTIDAD_POKEMONES_A_AGREGAR, pokedex); 
	
	contexto_jugar_t* contexto_jugar = malloc(sizeof(contexto_jugar_t));
	if (!contexto_jugar) {
		liberar_todo(archivo, menu, NULL, pokedex, juego_nuevo);
		return -1;
	}
	contexto_jugar->juego = juego_nuevo;
	contexto_jugar->f_logica = logica;

	menu_agregar_opcion(menu,"P", "Mostrar pokemones",pokedex_imprimir, (void*)pokedex); //meter los pokemones en un abb con inorden
	menu_agregar_opcion(menu,"J", "Iniciar Juego", jugar , contexto_jugar);
	menu_agregar_opcion(menu,"S", "Iniciar Juego con semilla", jugar_con_semilla ,contexto_jugar);
	menu_agregar_opcion(menu,"Q", "Salir", NULL ,NULL);

	int opcion;
	do {
		menu_mostrar(menu);
		opcion = obtener_opcion();
	} while (ejecutar_opcion(opcion, menu));

	liberar_todo(archivo, menu, contexto_jugar, pokedex, juego_nuevo);
	mostrar_cursor();

	return 0;
}