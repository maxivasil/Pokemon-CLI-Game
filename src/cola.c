#include "cola.h"
#include "lista.h"

Cola *cola_crear()
{
	return (Cola *)(lista_crear());
}

void cola_destruir(Cola *cola)
{
	lista_destruir((Lista *)cola);
}

void cola_destruir_todo(Cola *cola, void (*f)(void *))
{
	lista_destruir_todo((Lista *)cola, f);
}

size_t cola_cantidad(Cola *cola)
{
	return lista_cantidad_elementos((Lista *)cola);
}

void *cola_frente(Cola *cola)
{
	if (!cola)
		return NULL;
	void *frente = NULL;
	lista_obtener_elemento((Lista *)cola, 0, &frente);
	return frente;
}

bool cola_encolar(Cola *cola, void *cosa)
{
	if (!cola)
		return false;
	return lista_agregar_al_final((Lista *)cola, cosa);
}

void *cola_desencolar(Cola *cola)
{
	if (!cola)
		return false;
	void *elemento_desencolado = NULL;
	lista_quitar_elemento((Lista *)cola, 0, &elemento_desencolado);
	return elemento_desencolado;
}

bool cola_esta_vacía(Cola *cola)
{
	if (!cola)
		return false;
	return lista_cantidad_elementos((Lista *)cola) == 0;
}