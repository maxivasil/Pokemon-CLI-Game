#include "pila.h"
#include "lista.h"

Pila *pila_crear()
{
	return (Pila *)lista_crear();
}

void pila_destruir(Pila *pila)
{
	lista_destruir((Lista *)pila);
}

void pila_destruir_todo(Pila *pila, void (*f)(void *))
{
	lista_destruir_todo((Lista *)pila, f);
}

size_t pila_cantidad(Pila *pila)
{
	return lista_cantidad_elementos((Lista *)pila);
}

void *pila_tope(Pila *pila)
{
	if (!pila || lista_cantidad_elementos((Lista *)pila) == 0)
		return NULL;
	void *tope = NULL;
	lista_obtener_elemento((Lista *)pila, 0, &tope);
	return tope;
}

bool pila_apilar(Pila *pila, void *cosa)
{
	if (!pila)
		return false;
	return lista_agregar_elemento((Lista *)pila, 0, cosa);
}

void *pila_desapilar(Pila *pila)
{
	if (!pila || lista_cantidad_elementos((Lista *)pila) == 0)
		return NULL;
	void *elemento_desapilado = NULL;
	lista_quitar_elemento((Lista *)pila, 0, &elemento_desapilado);
	return elemento_desapilado;
}

bool pila_esta_vacía(Pila *pila)
{
	if (!pila)
		return false;
	return (lista_cantidad_elementos((Lista *)pila) == 0);
}