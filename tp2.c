#include "extra/engine.h"
#include "extra/ansi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "src/menu.h"
#include "src/juego.h"
#include "src/pokedex.h"
#include "src/csv.h"
#include "src/utils.h"
#include "src/texto_ascii.h"

typedef struct contexto_jugar {
	juego_t *juego;
	int (*f_logica)(int entrada, void *datos);
	pokedex_t *pokedex;
} contexto_jugar_t;

void liberar_todo(menu_t *menu, contexto_jugar_t *contexto_jugar,
		  pokedex_t *pokedex, juego_t *juego)
{
	if (menu)
		menu_destruir(menu);
	if (contexto_jugar)
		free(contexto_jugar);
	if (pokedex)
		pokedex_destruir(pokedex);
	if (juego)
		juego_destruir(juego);
}

int logica(int entrada, void *datos)
{
	juego_t *juego = (juego_t *)datos;

	borrar_pantalla();
	esconder_cursor();

	juego->variables.iteraciones++;
	if (juego->variables.iteraciones % 5 == 0)
		juego->variables.segundos_restantes--;

	juego_mover(entrada, juego);
	juego_dibujar_tablero(juego);

	if (entrada == 'q' || entrada == 'Q' ||
	    juego->variables.segundos_restantes <= 0) {
		juego_mostrar_estadisticas(juego);
		return 1;
	}
	return 0;
}

void solicitar_valores_juego(int *ancho, int *alto, size_t *segundos,
			     size_t *cantidad_pokemones, char *icono)
{
	char buffer[20];
	char temp_icono;
	bool entrada_valida = false;

	do {
		printf("Ingrese el ancho del tablero (mínimo 10, máximo 50): ");
		if (fgets(buffer, sizeof(buffer), stdin) &&
		    sscanf(buffer, "%d", ancho) == 1 && *ancho >= 10 &&
		    *ancho <= 50)
			entrada_valida = true;
		else
			printf("Entrada inválida. Asegúrese de ingresar un número entre 10 y 50.\n");
	} while (!entrada_valida);

	entrada_valida = false;
	do {
		printf("Ingrese el alto del tablero (mínimo 5, máximo 26): ");
		if (fgets(buffer, sizeof(buffer), stdin) &&
		    sscanf(buffer, "%d", alto) == 1 && *alto >= 5 &&
		    *alto <= 26)
			entrada_valida = true;
		else
			printf("Entrada inválida. Asegúrese de ingresar un número entre 5 y 30.\n");
	} while (!entrada_valida);

	entrada_valida = false;
	do {
		printf("Ingrese el tiempo del juego en segundos (mínimo 10, máximo 60): ");
		if (fgets(buffer, sizeof(buffer), stdin) &&
		    sscanf(buffer, "%zu", segundos) == 1 && *segundos >= 10 &&
		    *segundos <= 60)
			entrada_valida = true;
		else
			printf("Entrada inválida. Asegúrese de ingresar un número entre 10 y 60.\n");
	} while (!entrada_valida);

	entrada_valida = false;
	do {
		printf("Ingrese la cantidad de pokemones a agregar (mínimo 1, máximo 100): ");
		if (fgets(buffer, sizeof(buffer), stdin) &&
		    sscanf(buffer, "%zu", cantidad_pokemones) == 1 &&
		    *cantidad_pokemones >= 1 && *cantidad_pokemones <= 100)
			entrada_valida = true;
		else
			printf("Entrada inválida. Asegúrese de ingresar un número entre 1 y 100.\n");
	} while (!entrada_valida);

	entrada_valida = false;
	do {
		printf("Ingrese el ícono del jugador (un solo carácter visible): ");
		if (fgets(buffer, sizeof(buffer), stdin) &&
		    sscanf(buffer, " %c", &temp_icono) == 1 &&
		    isprint((unsigned char)temp_icono)) {
			entrada_valida = true;
			*icono = temp_icono;
		} else {
			printf("Entrada inválida. Ingrese un único carácter visible.\n");
		}
	} while (!entrada_valida);
}

bool jugar(void *logica)
{
	int ancho, alto;
	size_t segundos, cantidad_pokemones;
	char icono;
	solicitar_valores_juego(&ancho, &alto, &segundos, &cantidad_pokemones,
				&icono);
	contexto_jugar_t *ctx = (contexto_jugar_t *)logica;
	ctx->juego->ancho = ancho;
	ctx->juego->alto = alto;
	ctx->juego->variables.segundos_restantes = segundos;
	ctx->juego->jugador->icono = icono;
	ctx->juego->jugador->x = 0;
	ctx->juego->jugador->y = 0;
	if (!ctx->juego->semilla) {
		int semilla = (int)time(NULL);
		srand((unsigned int)semilla);
		semilla = rand();
		srand((unsigned int)semilla);
		ctx->juego->semilla = (size_t)semilla;
	}
	juego_subir_pokemones(ctx->juego, ctx->pokedex, cantidad_pokemones);
	game_loop(ctx->f_logica, ctx->juego);
	return false;
}

bool jugar_con_semilla(void *contexto)
{
	int semilla = 0;
	char buffer[20];
	char *endptr;

	do {
		printf("Inserte el número de semilla (entero positivo): ");
		if (!fgets(buffer, sizeof(buffer), stdin)) {
			printf("Error al leer la entrada. Intente nuevamente.\n");
			continue;
		}

		semilla = (int)strtol(buffer, &endptr, 10);

		if (buffer == endptr || *endptr != '\n' || semilla <= 0)
			printf("Entrada no válida. Asegúrese de ingresar un número entero positivo mayor que 0.\n");
	} while (buffer == endptr || *endptr != '\n' || semilla <= 0);

	srand((unsigned int)semilla);

	contexto_jugar_t *ctx = (contexto_jugar_t *)contexto;
	ctx->juego->semilla = (size_t)semilla;
	return jugar((void *)ctx);
}

int solicitar_opcion(menu_t *menu)
{
	int opcion_elegida;
	char opcion[20];
	bool opcion_valida = false;

	printf("Ingrese una opción: ");
	while (fgets(opcion, sizeof(opcion), stdin) != NULL) {
		size_t len = strlen(opcion);
		if (len > 0 && opcion[len - 1] == '\n')
			opcion[len - 1] = '\0';
		opcion_elegida = toupper(opcion[0]);
		char opcion_str[2] = { (char)opcion_elegida, '\0' };
		if (menu_contiene(menu, opcion_str)) {
			opcion_valida = true;
			break;
		}
		printf("Opción inválida, intente nuevamente.\nIngrese una opción: ");
	}
	return opcion_valida ? opcion_elegida : 0;
}

bool ejecutar_opcion(int opcion, menu_t *menu)
{
	char opcion_str[2] = { (char)opcion, '\0' };
	if (!menu_ejecutar_opcion(menu, opcion_str))
		return false;
	return true;
}

pokedex_t *manejar_archivo(int argc, char *argv[])
{
	if (argc < 2) {
		printf("%s <archivo>\n", argv[0]);
		return 0;
	}
	struct archivo_csv *archivo = abrir_archivo_csv(argv[1], ',');
	if (!archivo) {
		printf("No se pudo abrir el archivo CSV.\n");
		return 0;
	}
	pokedex_t *pokedex = pokedex_crear();
	if (!pokedex) {
		cerrar_archivo_csv(archivo);
		return 0;
	}
	pokedex_insertar_desde_archivo(archivo, pokedex);
	cerrar_archivo_csv(archivo);
	return pokedex;
}

int main(int argc, char *argv[])
{
	borrar_pantalla();
	pokedex_t *pokedex = manejar_archivo(argc, argv);
	if (!pokedex)
		return -1;
	imprimir_mensaje_bienvenida();
	printf(ANSI_COLOR_WHITE ANSI_COLOR_BOLD
	       "\n\n\n				Presiona cualquier tecla para ver el menú..." ANSI_COLOR_RESET);
	getchar();
	borrar_pantalla();
	imprimir_gengar();
	menu_t *menu = menu_crear();
	if (!menu) {
		liberar_todo(NULL, NULL, pokedex, NULL);
		return -1;
	}

	struct juego *juego_nuevo = juego_crear(1, 1, 1, '@');
	if (!juego_nuevo) {
		liberar_todo(menu, NULL, pokedex, NULL);
		return -1;
	}
	contexto_jugar_t *contexto_jugar = malloc(sizeof(contexto_jugar_t));
	if (!contexto_jugar) {
		liberar_todo(menu, NULL, pokedex, juego_nuevo);
		return -1;
	}
	contexto_jugar->juego = juego_nuevo;
	contexto_jugar->f_logica = logica;
	contexto_jugar->pokedex = pokedex;

	menu_agregar_opcion(menu, "P", "Mostrar pokemones", pokedex_imprimir,
			    (void *)pokedex);
	menu_agregar_opcion(menu, "J", "Iniciar Juego", jugar, contexto_jugar);
	menu_agregar_opcion(menu, "S", "Iniciar Juego con semilla",
			    jugar_con_semilla, contexto_jugar);
	menu_agregar_opcion(menu, "Q", "Salir", NULL, NULL);

	int opcion;
	do {
		menu_mostrar(menu);
		opcion = solicitar_opcion(menu);
	} while (ejecutar_opcion(opcion, menu));

	liberar_todo(menu, contexto_jugar, pokedex, juego_nuevo);
	mostrar_cursor();

	return 0;
}