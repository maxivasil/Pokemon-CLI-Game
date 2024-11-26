#include "lista.h"

typedef struct nodo {
	void *valor;
	struct nodo *siguiente;
} Nodo;

struct lista {
	struct nodo *primero;
	struct nodo *ultimo;
	size_t cantidad_nodos;
};

struct lista_iterador {
	struct lista *lista;
	struct nodo *actual;
};

Lista *lista_crear()
{
	Lista *lista = malloc(sizeof(Lista));
	if (lista == NULL)
		return NULL;
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->cantidad_nodos = 0;
	return lista;
}

void lista_destruir(Lista *lista)
{
	if (!lista)
		return;
	Nodo *actual = lista->primero;
	while (actual) {
		Nodo *siguiente = actual->siguiente;
		free(actual);
		actual = siguiente;
	}
	free(lista);
}

void lista_destruir_todo(Lista *lista, void (*destructor)(void *))
{
	if (!lista)
		return;
	if (destructor) {
		Nodo *actual = lista->primero;
		while (actual) {
			destructor(actual->valor);
			actual = actual->siguiente;
		}
	}
	lista_destruir(lista);
}

size_t lista_cantidad_elementos(Lista *lista)
{
	if (!lista)
		return 0;
	return lista->cantidad_nodos;
}

bool lista_agregar_elemento(Lista *lista, size_t posicion, void *cosa)
{
	if (!lista || posicion > lista->cantidad_nodos)
		return false;
	Nodo *nuevo = malloc(sizeof(Nodo));
	if (!nuevo)
		return false;
	nuevo->valor = cosa;
	if (posicion == 0) {
		nuevo->siguiente = lista->primero;
		lista->primero = nuevo;
		if (lista->cantidad_nodos == 0)
			lista->ultimo = nuevo;
	} else {
		Nodo *actual = lista->primero;
		for (size_t i = 0; i < posicion - 1; i++) {
			actual = actual->siguiente;
		}
		nuevo->siguiente = actual->siguiente;
		actual->siguiente = nuevo;
		if (nuevo->siguiente == NULL)
			lista->ultimo = nuevo;
	}
	lista->cantidad_nodos++;
	return true;
}

bool lista_agregar_al_final(Lista *lista, void *cosa)
{
	if (!lista)
		return false;
	Nodo *nuevo = malloc(sizeof(Nodo));
	if (!nuevo)
		return false;
	nuevo->valor = cosa;
	nuevo->siguiente = NULL;
	if (!lista->primero) {
		lista->primero = nuevo;
		lista->ultimo = nuevo;
	} else {
		lista->ultimo->siguiente = nuevo;
		lista->ultimo = nuevo;
	}
	lista->cantidad_nodos++;
	return true;
}

bool lista_quitar_elemento(Lista *lista, size_t posicion,
			   void **elemento_quitado)
{
	if (!lista || posicion >= lista->cantidad_nodos)
		return false;
	Nodo *actual = lista->primero;
	if (posicion == 0) {
		lista->primero = actual->siguiente;
		if (elemento_quitado)
			*elemento_quitado = actual->valor;
		free(actual);
		if (lista->cantidad_nodos == 1)
			lista->ultimo = NULL;
	} else {
		for (size_t i = 0; i < posicion - 1; i++) {
			actual = actual->siguiente;
		}
		Nodo *aux = actual->siguiente;
		actual->siguiente = aux->siguiente;
		if (elemento_quitado)
			*elemento_quitado = aux->valor;
		if (aux == lista->ultimo)
			lista->ultimo = actual;
		free(aux);
	}
	lista->cantidad_nodos--;
	return true;
}

void *lista_buscar_elemento(Lista *lista, void *buscado,
			    int (*comparador)(void *, void *))
{
	if (!lista || !comparador)
		return NULL;
	Nodo *actual = lista->primero;
	while (actual) {
		if (comparador(actual->valor, buscado) == 0)
			return actual->valor;
		actual = actual->siguiente;
	}
	return NULL;
}

bool lista_obtener_elemento(Lista *lista, size_t posicion,
			    void **elemento_encontrado)
{
	if (!lista || posicion >= lista->cantidad_nodos)
		return false;
	Nodo *actual = lista->primero;
	for (size_t i = 0; i < posicion; i++)
		actual = actual->siguiente;
	if (actual) {
		if (elemento_encontrado)
			*elemento_encontrado = actual->valor;
		return true;
	}
	return false;
}

size_t lista_iterar_elementos(Lista *lista, bool (*f)(void *, void *),
			      void *ctx)
{
	if (!lista || !f)
		return 0;
	size_t iterados = 0;
	Nodo *actual = lista->primero;
	while (actual) {
		iterados++;
		if (!f(actual->valor, ctx))
			break;
		actual = actual->siguiente;
	}
	return iterados;
}

Lista_iterador *lista_iterador_crear(Lista *lista)
{
	if (!lista)
		return NULL;
	Lista_iterador *iterador = malloc(sizeof(Lista_iterador));
	if (!iterador)
		return NULL;
	iterador->lista = lista;
	iterador->actual = lista->primero;
	return iterador;
}

bool lista_iterador_hay_siguiente(Lista_iterador *iterador)
{
	if (!iterador || !iterador->actual)
		return false;
	return true;
}

void lista_iterador_avanzar(Lista_iterador *iterador)
{
	if (!iterador || !iterador->actual)
		return;
	iterador->actual = iterador->actual->siguiente;
}

void *lista_iterador_obtener_elemento_actual(Lista_iterador *iterador)
{
	if (!iterador || !iterador->actual)
		return NULL;
	return iterador->actual->valor;
}

void lista_iterador_destruir(Lista_iterador *iterador)
{
	if (!iterador)
		return;
	free(iterador);
}