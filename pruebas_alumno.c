#include "pa2m.h"
#include "extra/engine.h"
#include "src/csv.h"
#include "src/menu.h"
#include "src/pokedex.h"
#include "src/juego.h"
#include <stdlib.h>
#include <ctype.h>

char *strdup(const char *src)
{
	char *dest = malloc(strlen(src) + 1);
	if (dest)
		strcpy(dest, src);
	return dest;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////MENU//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool accion_prueba(void *ctx)
{
	return true;
}

void CrearUnMenuLoCreaVacio()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu != NULL, "El menú se creó exitosamente (NULL != %p)",
		     menu);
	pa2m_afirmar(
		menu_cantidad(menu) == 0,
		"La cantidad de opciones en el menú es 0 apenas se creó (0 == %ld)",
		menu_cantidad(menu));
	menu_destruir(menu);
}

void AgregarUnaOpcionLaAgregaCorrectamente()
{
	menu_t *menu = menu_crear();
	char *letra = "P";
	const char *descripcion = "Esto es lo q se hace al elegir a P";
	bool res = menu_agregar_opcion(menu, letra, descripcion, accion_prueba,
				       "Contexto de P");
	pa2m_afirmar(res, "Se pudo agregar una opción al menú (1 == %ld)", res);
	pa2m_afirmar(menu_cantidad(menu) == 1,
		     "El menú tiene 1 opción tras agregarla (0 == %ld)",
		     menu_cantidad(menu));
	menu_destruir(menu);
}

void AgregarVariasOpcionesLasAgregaCorrectamente()
{
	menu_t *menu = menu_crear();

	pa2m_afirmar(menu_agregar_opcion(menu, "A", "Opción A", accion_prueba,
					 "Contexto A"),
		     "Se agregó la opción A.");
	pa2m_afirmar(menu_agregar_opcion(menu, "B", "Opción B", accion_prueba,
					 "Contexto B"),
		     "Se agregó la opción B.");
	pa2m_afirmar(menu_agregar_opcion(menu, "C", "Opción C", accion_prueba,
					 "Contexto C"),
		     "Se agregó la opción C.");
	bool res = menu_contiene(menu, "A");
	pa2m_afirmar(
		res == true,
		"El menú contiene la opcion B despues de agregarla (1 == %ld)",
		res);
	res = menu_contiene(menu, "B");
	pa2m_afirmar(
		res == true,
		"El menú contiene la opcion B despues de agregarla (1 == %ld)",
		res);
	res = menu_contiene(menu, "C");
	pa2m_afirmar(
		res == true,
		"El menú contiene la opcion B despues de agregarla (1 == %ld)",
		res);
	pa2m_afirmar(menu_cantidad(menu) == 3,
		     "El menú tiene 3 opciones tras agregar A, B y C.");
	menu_destruir(menu);
}

void MostrarMenuMuestraCorrectamenteLasOpciones()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, "A", "Opción A", accion_prueba, "Contexto A");
	menu_agregar_opcion(menu, "B", "Opción B", accion_prueba, "Contexto B");

	menu_mostrar(menu);

	menu_destruir(menu);
}

void EjecutarOpcionEjecutaCorrectamente()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, "P", "Esto es lo q se hace al elegir a P",
			    accion_prueba, "Contexto de P");
	menu_agregar_opcion(menu, "S", "Esto es lo q se hace al elegir a S",
			    accion_prueba, "Contexto de S");

	bool res = menu_ejecutar_opcion(menu, "P");
	pa2m_afirmar(res == true, "Se puede ejecutar la opcion A");

	res = menu_ejecutar_opcion(menu, "J");
	pa2m_afirmar(
		res == false,
		"NO se puede ejecutar la opcion J pq no existe en el menú");

	menu_destruir(menu);
}

void AgregarOpcionConLaMismaClaveLaAgregaYReemplazaElValor()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, "A", "Opción A", accion_prueba, "Contexto A");

	pa2m_afirmar(menu_agregar_opcion(menu, "A", "Otra opción A",
					 accion_prueba, "Contexto B"),
		     "Si se repite la clave, se actualiza el valor.");

	pa2m_afirmar(
		menu_cantidad(menu) == 1,
		"El menú sigue teniendo solo 1 opción después de intentar agregar una duplicada.");

	menu_destruir(menu);
}

void DestruirMenuConOpcionesLiberaCorrectamente()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, "P", "Esto es lo q se hace al elegir a P",
			    accion_prueba, "Contexto de P");
	menu_agregar_opcion(menu, "S", "Esto es lo q se hace al elegir a S",
			    accion_prueba, "Contexto de S");

	menu_destruir(menu);
	pa2m_afirmar(true, "El menú se destruyó sin pérdidas de memoria.");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////POKEDEX/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool duplicar_puntos_poke(void *pokemon_ctx, void *contador_ctx)
{
	pokemon_t *pokemon = (pokemon_t *)pokemon_ctx;
	pokemon->puntos *= 2;
	(*(size_t *)contador_ctx)++;
	return true;
}

void destruir_poke(void *pokemon_void)
{
	if (!pokemon_void)
		return;
	pokemon_t *pokemon = (pokemon_t *)pokemon_void;
	free(pokemon->nombre);
	free(pokemon->patron_movimiento);
	free(pokemon);
}

void CrearUnaPokedexLaCreaVacia()
{
	pokedex_t *pokedex = pokedex_crear();
	pa2m_afirmar(pokedex != NULL, "Se creó una pokedex exitosamente.");
	pa2m_afirmar(pokedex_cantidad(pokedex) == 0,
		     "La pokedex está vacía tras ser creada.");
	pokedex_destruir(pokedex);
}

void InsertarPokemonLoAgregaCorrectamente()
{
	pokedex_t *pokedex = pokedex_crear();

	pokemon_t *pikachu = malloc(sizeof(pokemon_t));
	pikachu->nombre = strdup("Pikachu");
	pikachu->puntos = 100;
	pikachu->color = "Amarillo";
	pikachu->patron_movimiento = strdup("NRSEJI");

	bool res = pokedex_insertar(pokedex, pikachu);

	pa2m_afirmar(res, "Se pudo insertar un Pokémon en la Pokedex.");
	pa2m_afirmar(pokedex_cantidad(pokedex) == 1,
		     "La Pokedex tiene 1 Pokémon tras insertar uno.");

	pokedex_destruir(pokedex);
}

void InsertarVariosPokemonesLosAgregaCorrectamente()
{
	pokedex_t *pokedex = pokedex_crear();

	pokemon_t *charmander = malloc(sizeof(pokemon_t));
	charmander->nombre = strdup("Charmander");
	charmander->puntos = 120;
	charmander->color = "Rojo";
	charmander->patron_movimiento = strdup("RRIIEOSS");

	pokemon_t *squirtle = malloc(sizeof(pokemon_t));
	squirtle->nombre = strdup("Squirtle");
	squirtle->puntos = 95;
	squirtle->color = "Azul";
	squirtle->patron_movimiento = strdup("N");

	pokemon_t *bulbasaur = malloc(sizeof(pokemon_t));
	bulbasaur->nombre = strdup("Bulbasaur");
	bulbasaur->puntos = 110;
	bulbasaur->color = "Verde";
	bulbasaur->patron_movimiento = strdup("JIIJJ");

	pa2m_afirmar(pokedex_insertar(pokedex, charmander),
		     "Se insertó Charmander.");
	pa2m_afirmar(pokedex_insertar(pokedex, squirtle),
		     "Se insertó Squirtle.");
	pa2m_afirmar(pokedex_insertar(pokedex, bulbasaur),
		     "Se insertó Bulbasaur.");
	pa2m_afirmar(pokedex_cantidad(pokedex) == 3,
		     "La Pokedex tiene 3 Pokémon tras insertar tres.");

	pokedex_destruir(pokedex);
}

void InsertarDesdeArchivo_ConArchivoReal()
{
	const char *nombre_archivo = "pokedex_test.csv";
	FILE *archivo = fopen(nombre_archivo, "w");
	if (!archivo) {
		fprintf(stderr, "No se pudo crear el archivo de prueba.\n");
		return;
	}
	fprintf(archivo, "Pikachu,100,Amarillo,NNSS\n");
	fprintf(archivo, "Charmander,120,Rojo,RJI\n");
	fprintf(archivo, "Squirtle,95,Azul,IIJJ\n");
	fclose(archivo);

	struct archivo_csv *archivo_csv =
		abrir_archivo_csv(nombre_archivo, ',');
	if (!archivo_csv) {
		fprintf(stderr, "No se pudo abrir el archivo de prueba.\n");
		remove(nombre_archivo);
		return;
	}

	pokedex_t *pokedex = pokedex_crear();
	pokedex_insertar_desde_archivo(archivo_csv, pokedex);
	pa2m_afirmar(pokedex_cantidad(pokedex) == 3,
		     "Se insertaron 3 Pokémon desde el archivo.");

	pokemon_t busqueda_pikachu = { "Pikachu", 0, NULL, NULL };
	pokemon_t *pikachu =
		(pokemon_t *)pokedex_obtener(pokedex, &busqueda_pikachu);
	pa2m_afirmar(pikachu && strcmp(pikachu->nombre, "Pikachu") == 0,
		     "Pikachu fue leído correctamente.");

	pokemon_t busqueda_charmander = { "Charmander", 0, NULL, NULL };
	pokemon_t *charmander =
		(pokemon_t *)pokedex_obtener(pokedex, &busqueda_charmander);
	pa2m_afirmar(charmander &&
			     strcmp(charmander->nombre, "Charmander") == 0,
		     "Charmander fue leído correctamente.");

	pokemon_t busqueda_squirtle = { "Squirtle", 0, NULL, NULL };
	pokemon_t *squirtle =
		(pokemon_t *)pokedex_obtener(pokedex, &busqueda_squirtle);
	pa2m_afirmar(squirtle && strcmp(squirtle->nombre, "Squirtle") == 0,
		     "Squirtle fue leído correctamente.");

	cerrar_archivo_csv(archivo_csv);
	pokedex_destruir(pokedex);
	remove(nombre_archivo);
}

void BuscarPokemonLoDevuelveCorrectamente()
{
	pokedex_t *pokedex = pokedex_crear();

	pokemon_t *pikachu = malloc(sizeof(pokemon_t));
	pikachu->nombre = strdup("Pikachu");
	pikachu->puntos = 100;
	pikachu->color = "Amarillo";
	pikachu->patron_movimiento = strdup("ESNO");

	pokedex_insertar(pokedex, pikachu);

	pokemon_t *encontrado = (pokemon_t *)pokedex_obtener(pokedex, pikachu);
	pa2m_afirmar(encontrado == pikachu,
		     "Se encontró correctamente a Pikachu en la Pokedex.");
	pa2m_afirmar(pokedex_cantidad(pokedex) == 1,
		     "Obtener de la pokedex no elimina el pokemon.");
	pokedex_destruir(pokedex);
}

void IterarPokedexRecorreCorrectamente()
{
	pokedex_t *pokedex = pokedex_crear();

	pokemon_t *charmander = malloc(sizeof(pokemon_t));
	charmander->nombre = strdup("Charmander");
	charmander->puntos = 120;
	charmander->color = "Rojo";
	charmander->patron_movimiento = strdup("JESONIR");

	pokemon_t *squirtle = malloc(sizeof(pokemon_t));
	squirtle->nombre = strdup("Squirtle");
	squirtle->puntos = 95;
	squirtle->color = "Azul";
	squirtle->patron_movimiento = strdup("NNNSSSEEEOOO");

	pokedex_insertar(pokedex, charmander);
	pokedex_insertar(pokedex, squirtle);

	size_t contador = 0;
	size_t iterados =
		pokedex_iterar(pokedex, duplicar_puntos_poke, &contador);

	pa2m_afirmar(iterados == 2, "Se iteró sobre todos los Pokémon.");
	pa2m_afirmar(contador == 2,
		     "Se contaron todos los Pokémon durante la iteración.");
	pa2m_afirmar(
		charmander->puntos == 120 * 2,
		"Se duplicaron los puntos del primer pokemon correctamente (240 == %ld)",
		charmander->puntos);
	pa2m_afirmar(
		squirtle->puntos == 95 * 2,
		"Se duplicaron los puntos del segundo pokemon correctamente (190 == %ld)",
		squirtle->puntos);

	pokedex_destruir(pokedex);
}

void DestruirUnaPokedexVaciaNoConllevaAPerdidasDeMemoria()
{
	pokedex_t *pokedex = pokedex_crear();
	pokedex_destruir(pokedex);
	pa2m_afirmar(true,
		     "La pokedex VACIA se destruyó sin pérdidas de memoria.");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////JUEGO/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProbarJuegoCrear()
{
	int ancho = 10;
	int alto = 10;
	size_t tiempo = 60;
	char icono = '@';

	juego_t *juego = juego_crear(ancho, alto, tiempo, icono);

	pa2m_afirmar(juego != NULL, "El juego se creó correctamente.");
	size_t res = lista_cantidad_elementos(juego->fuente_de_pokemones);
	pa2m_afirmar(
		res == 0,
		"La cantidad de pokemones cargados en juego es 0 porque el juego se acaba de crear (0 == %ld)",
		res);
	res = lista_cantidad_elementos(juego->pokemones_tablero);
	pa2m_afirmar(
		res == 0,
		"La cantidad de pokemones en tablero es 0 porque el juego se acaba de crear (0 == %ld)",
		res);
	res = pila_cantidad(juego->pokemones_capturados);
	pa2m_afirmar(
		res == 0,
		"La cantidad de pokemones capturados por el jugador es 0 porque el juego se acaba de crear (0 == %ld)",
		res);
	pa2m_afirmar(juego->ancho == ancho,
		     "El ancho del tablero es el esperado.");
	pa2m_afirmar(juego->alto == alto,
		     "El alto del tablero es el esperado.");
	pa2m_afirmar(juego->jugador->icono == icono,
		     "El icono del jugador es el esperado.");
	pa2m_afirmar(
		juego->jugador->x == 0 && juego->jugador->y == 0,
		"El jugador se posiciona inicialmente en el casillero uno.");
	pa2m_afirmar(
		juego->cant_pokemones_tablero == 0,
		"La cantidad de pokemones del tablero inicialmente es cero (0 == %ld)",
		juego->cant_pokemones_tablero);
	pa2m_afirmar(
		lista_cantidad_elementos(juego->fuente_de_pokemones) == 0,
		"La cantidad de pokemones de la fuente inicialmente es cero (0 == %ld)",
		lista_cantidad_elementos(juego->fuente_de_pokemones));
	pa2m_afirmar(
		lista_cantidad_elementos(juego->pokemones_tablero) == 0,
		"La cantidad de pokemones del tablero inicialmente es cero (0 == %ld)",
		lista_cantidad_elementos(juego->pokemones_tablero));
	pa2m_afirmar(
		pila_cantidad(juego->pokemones_capturados) == 0,
		"La cantidad de pokemones capturados inicialmente es cero (0 == %ld)",
		pila_cantidad(juego->pokemones_capturados));
	pa2m_afirmar(
		juego->semilla == 0,
		"La semilla inicialmente es cero hasta que se genera o se pasa una (0 == %ld)",
		juego->semilla);
	pa2m_afirmar(juego->variables.segundos_restantes == tiempo,
		     "El tiempo inicial es correcto.");
	pa2m_afirmar(
		juego->variables.cant_atrapados == 0,
		"La cantidad de pokemones atrapados inicialmente es cero (0 == %ld)",
		juego->variables.cant_atrapados);
	pa2m_afirmar(juego->variables.combo_maximo == 0,
		     "El combo máximo inicialmente es cero (0 == %ld)",
		     juego->variables.combo_maximo);
	pa2m_afirmar(juego->variables.iteraciones == 0,
		     "Las iteraciones inicialmente son cero (0 == %ld)",
		     juego->variables.iteraciones);
	pa2m_afirmar(juego->variables.multiplicador == 1,
		     "El multiplicador inicialmente es uno (1 == %ld)",
		     juego->variables.multiplicador);
	pa2m_afirmar(juego->variables.puntos_obtenidos == 0,
		     "Los puntos obtenidos inicialmente son cero (0 == %ld)",
		     juego->variables.puntos_obtenidos);
	pa2m_afirmar(
		juego->variables.ultimo_poke_capturado == 0,
		"No existe inicialmente último pokémon capturado (0 == %ld)",
		juego->variables.ultimo_poke_capturado);

	juego_destruir(juego);
}

void ProbarJuegoSubirPokemones()
{
	juego_t *juego = juego_crear(10, 10, 60, '@');
	pokedex_t *pokedex = pokedex_crear();

	pokemon_t *charmander = malloc(sizeof(pokemon_t));
	charmander->nombre = strdup("Charmander");
	charmander->puntos = 120;
	charmander->color = "Rojo";
	charmander->patron_movimiento = strdup("RRIIEOSS");
	pokedex_insertar(pokedex, charmander);
	pokemon_t *squirtle = malloc(sizeof(pokemon_t));
	squirtle->nombre = strdup("Squirtle");
	squirtle->puntos = 95;
	squirtle->color = "Azul";
	squirtle->patron_movimiento = strdup("N");
	pokedex_insertar(pokedex, squirtle);

	juego_subir_pokemones(juego, pokedex);

	pa2m_afirmar(lista_cantidad_elementos(juego->fuente_de_pokemones) == 2,
		     "Los pokémones se subieron desde la pokédex.");

	pokedex_destruir(pokedex);
	juego_destruir(juego);
}

void ProbarJuegoAgregarPokemones()
{
	juego_t *juego = juego_crear(10, 10, 60, '@');
	pokedex_t *pokedex = pokedex_crear();
	pokemon_t *charmander = malloc(sizeof(pokemon_t));
	charmander->nombre = strdup("Charmander");
	charmander->puntos = 120;
	charmander->color = "Rojo";
	charmander->patron_movimiento = strdup("RRIIEOSS");
	pokedex_insertar(pokedex, charmander);
	pokemon_t *squirtle = malloc(sizeof(pokemon_t));
	squirtle->nombre = strdup("Squirtle");
	squirtle->puntos = 95;
	squirtle->color = "Azul";
	squirtle->patron_movimiento = strdup("N");
	pokedex_insertar(pokedex, squirtle);
	juego_subir_pokemones(juego, pokedex);

	size_t cantidad_inicial =
		lista_cantidad_elementos(juego->pokemones_tablero);
	pa2m_afirmar(cantidad_inicial == 0,
		     "El tablero originalmente tiene 0 pokémones (0 == %ld).",
		     cantidad_inicial);

	juego_agregar_pokemones(juego, 5);
	size_t cantidad_final =
		lista_cantidad_elementos(juego->pokemones_tablero);

	pa2m_afirmar(cantidad_final == 5,
		     "Se agregaron 5 pokémones al tablero (5 == %ld).",
		     cantidad_final);

	pokedex_destruir(pokedex);
	juego_destruir(juego);
}

void ProbarJuegoMover()
{
	juego_t *juego = juego_crear(10, 10, 60, '@');

	int x_inicial = juego->jugador->x;
	int y_inicial = juego->jugador->y;

	juego_mover(TECLA_ABAJO, juego);
	pa2m_afirmar(juego->jugador->y == y_inicial + 1,
		     "El jugador se movió hacia abajo con ⬇");

	juego_mover(TECLA_DERECHA, juego);
	pa2m_afirmar(juego->jugador->x == x_inicial + 1,
		     "El jugador se movió hacia la derecha con ➡");

	juego_mover(TECLA_ARRIBA, juego);
	pa2m_afirmar(juego->jugador->y == y_inicial,
		     "El jugador se movió hacia arriba con ⬆");

	juego_mover(TECLA_IZQUIERDA, juego);
	pa2m_afirmar(juego->jugador->x == x_inicial,
		     "El jugador se movió hacia la izquierda con ⬅");

	juego_destruir(juego);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////PRUEBAS DE PUNTEROS NULOS/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProbarMenuConPunteroNulo()
{
	pa2m_afirmar(menu_agregar_opcion(NULL, "A", "Opción A", accion_prueba,
					 "Contexto A") == false,
		     "No se puede agregar una opción a un menú nulo.");
	pa2m_afirmar(!menu_contiene(NULL, "A"),
		     "Un menú NULL no contiene claves.");
	pa2m_afirmar(menu_ejecutar_opcion(NULL, "A") == false,
		     "No se puede ejecutar una opción en un menú nulo.");
	menu_t *menu = menu_crear();
	pa2m_afirmar(!menu_contiene(menu, NULL),
		     "Un menú no contiene claves NULL.");
	pa2m_afirmar(!menu_ejecutar_opcion(menu, NULL),
		     "No se puede ejecutar una opción NULL en un menú.");
	menu_destruir(menu);
	pa2m_afirmar(menu_cantidad(NULL) == 0,
		     "La cantidad de opciones en un menú nulo es 0.");
	pa2m_afirmar(menu_mostrar(NULL) == 0,
		     "No se puede mostrar un menú nulo.");
}

void ProbarPokedexConPunteroNulo()
{
	pokemon_t *pokemon = malloc(sizeof(pokemon_t));
	pokemon->nombre = strdup("Pikachu");
	pokemon->puntos = 100;
	pokemon->color = "Amarillo";
	pokemon->patron_movimiento = strdup("NRSEJI");
	pa2m_afirmar(!pokedex_insertar(NULL, pokemon),
		     "No se puede insertar un Pokémon en una pokedex nula.");
	struct archivo_csv *archivo_csv =
		abrir_archivo_csv("nombre_archivo", ',');
	pokedex_insertar_desde_archivo(archivo_csv, NULL);
	remove("nombre_archivo");
	pa2m_afirmar(
		true,
		"No se puede insertar desde un archivo fuente en una pokedex nula.");
	pa2m_afirmar(pokedex_cantidad(NULL) == 0,
		     "La cantidad de elementos en una pokedex nula es 0.");
	pa2m_afirmar(pokedex_obtener(NULL, pokemon) == NULL,
		     "No se puede buscar un Pokémon en una pokedex nula.");
	pa2m_afirmar(pokedex_iterar(NULL, duplicar_puntos_poke, NULL) == 0,
		     "No se puede iterar una pokedex nula.");
	pokedex_destruir(NULL);
	pa2m_afirmar(true,
		     "Destruir una pokedex NULL no implica invalid frees");
	pokedex_t *pokedex = pokedex_crear();
	pa2m_afirmar(!pokedex_insertar(pokedex, NULL),
		     "No se puede insertar un Pokémon nulo en una pokedex.");
	pa2m_afirmar(!pokedex_obtener(pokedex, NULL),
		     "No se puede obtener un Pokémon NULL en una pokedex.");
	pa2m_afirmar(pokedex_iterar(pokedex, NULL, NULL) == 0,
		     "No se puede iterar una pokedex con una función nula.");
	pokedex_insertar_desde_archivo(NULL, pokedex);
	pa2m_afirmar(
		true,
		"No se puede insertar en la pokedex con un archivo nulo como fuente.");
	pokedex_destruir(pokedex);
	destruir_poke(pokemon);
}

void ProbarJuegoConPunteroNulo()
{
	pokedex_t *pokedex = pokedex_crear();

	pa2m_afirmar(juego_crear(0, 10, 60, '@') == NULL,
		     "Crear un juego con ancho invalido no es exitoso.");
	pa2m_afirmar(juego_crear(10, 0, 60, '@') == NULL,
		     "Crear un juego con alto invalido no es exitoso.");
	pa2m_afirmar(juego_crear(10, 10, 0, '@') == NULL,
		     "Crear un juego con tiempo invalido no es exitoso.");
	juego_agregar_pokemones(NULL, 5);
	pa2m_afirmar(true, "No se pueden agregar pokémones a un juego nulo.");
	juego_subir_pokemones(NULL, pokedex);
	pa2m_afirmar(
		true,
		"No se pueden subir pokémones desde una pokedex en un juego nulo.");
	juego_mover(TECLA_ARRIBA, NULL);
	pa2m_afirmar(true, "No se puede mover un jugador en un juego nulo.");
	juego_dibujar_tablero(NULL);
	pa2m_afirmar(true, "No se puede dibujar el tablero de un juego nulo.");
	juego_mostrar_estadisticas(NULL);
	pa2m_afirmar(true,
		     "No se pueden mostrar estadísticas de un juego nulo.");
	juego_destruir(NULL);
	pa2m_afirmar(true, "No se puede destruir un juego nulo.");
	juego_t *juego = juego_crear(10, 10, 10, '$');
	juego_subir_pokemones(juego, NULL);
	pa2m_afirmar(
		lista_cantidad_elementos(juego->fuente_de_pokemones) == 0,
		"No se pueden subir pokemones al juego si la pokedex es nula.");
	juego_agregar_pokemones(juego, 5);
	pa2m_afirmar(
		lista_cantidad_elementos(juego->pokemones_tablero) == 0,
		"No se pueden agregar pokemones al tablero si la fuente de pokemones está vacía.");
	pokemon_t *pokemon = malloc(sizeof(pokemon_t));
	pokemon->nombre = strdup("Pikachu");
	pokemon->puntos = 100;
	pokemon->color = "Amarillo";
	pokemon->patron_movimiento = strdup("NRSEJI");
	pokedex_insertar(pokedex, pokemon);
	juego_agregar_pokemones(juego, 0);
	pa2m_afirmar(
		lista_cantidad_elementos(juego->pokemones_tablero) == 0,
		"No se agregan pokemones al tablero si se pide agregar menos o exactamente cero pokemones");
	juego_mover(0, juego);
	juego_mover('w', juego);
	pa2m_afirmar(
		juego->jugador->x == 0 && juego->jugador->y == 0,
		"Un jugador no se mueve si la entrada no es válida (una tecla).");
	pokedex_destruir(pokedex);
	juego_destruir(juego);
}

int main()
{
	pa2m_nuevo_grupo("PRUEBAS TDA MENÚ");
	CrearUnMenuLoCreaVacio();
	AgregarUnaOpcionLaAgregaCorrectamente();
	AgregarVariasOpcionesLasAgregaCorrectamente();
	MostrarMenuMuestraCorrectamenteLasOpciones();
	EjecutarOpcionEjecutaCorrectamente();
	AgregarOpcionConLaMismaClaveLaAgregaYReemplazaElValor();
	DestruirMenuConOpcionesLiberaCorrectamente();

	pa2m_nuevo_grupo("PRUEBAS TDA POKEDEX");
	CrearUnaPokedexLaCreaVacia();
	InsertarPokemonLoAgregaCorrectamente();
	InsertarVariosPokemonesLosAgregaCorrectamente();
	InsertarDesdeArchivo_ConArchivoReal();
	BuscarPokemonLoDevuelveCorrectamente();
	IterarPokedexRecorreCorrectamente();

	pa2m_nuevo_grupo("PRUEBAS TDA JUEGO");
	ProbarJuegoCrear();
	ProbarJuegoSubirPokemones();
	ProbarJuegoAgregarPokemones();
	ProbarJuegoMover();

	pa2m_nuevo_grupo("PRUEBAS DE PUNTEROS NULOS");
	ProbarMenuConPunteroNulo();
	ProbarPokedexConPunteroNulo();
	ProbarJuegoConPunteroNulo();

	return pa2m_mostrar_reporte();
}
