#include "abb.h"
#include "abb_estructura_privada.h"

typedef struct {
	void **vector;
	size_t tamaño;
	size_t recorridos;
} vector_ctx_t;

abb_t *abb_crear(int (*comparador)(void *, void *))
{
	if (!comparador)
		return NULL;
	abb_t *abb = calloc(1, sizeof(abb_t));
	if (!abb)
		return NULL;
	abb->comparador = comparador;
	return abb;
}

void destruir_nodos(nodo_t *raiz, void (*destructor)(void *))
{
	if (raiz->izq != NULL)
		destruir_nodos(raiz->izq, destructor);
	if (raiz->der != NULL)
		destruir_nodos(raiz->der, destructor);
	if (destructor)
		destructor(raiz->elemento);
	free(raiz);
}

void abb_destruir(abb_t *abb)
{
	abb_destruir_todo(abb, NULL);
}

void abb_destruir_todo(abb_t *abb, void (*destructor)(void *))
{
	if (!abb)
		return;
	if (abb->nodos != 0)
		destruir_nodos(abb->raiz, destructor);
	free(abb);
}

bool insertar_nodo(nodo_t *raiz, nodo_t *nuevo_nodo,
		   int (*comparador)(void *, void *))
{
	if (comparador(nuevo_nodo->elemento, raiz->elemento) > 0) {
		if (raiz->der)
			return insertar_nodo(raiz->der, nuevo_nodo, comparador);
		else {
			raiz->der = nuevo_nodo;
			return true;
		}
	} else {
		if (raiz->izq)
			return insertar_nodo(raiz->izq, nuevo_nodo, comparador);
		else {
			raiz->izq = nuevo_nodo;
			return true;
		}
	}
}

bool abb_insertar(abb_t *abb, void *elemento)
{
	if (!abb)
		return false;
	nodo_t *nuevo_nodo = calloc(1, sizeof(nodo_t));
	if (!nuevo_nodo)
		return false;
	nuevo_nodo->elemento = elemento;
	if (abb->nodos == 0) {
		abb->raiz = nuevo_nodo;
		abb->nodos++;
		return true;
	}
	bool resultado = insertar_nodo(abb->raiz, nuevo_nodo, abb->comparador);
	if (resultado)
		abb->nodos++;
	return resultado;
}

nodo_t *_quitar_nodo(nodo_t *nodo)
{
	if (!nodo->izq && !nodo->der) {
		free(nodo);
		return NULL;
	}

	if (!nodo->izq || !nodo->der) {
		nodo_t *hijo = (nodo->izq) ? nodo->izq : nodo->der;
		free(nodo);
		return hijo;
	}

	nodo_t *predecesor = nodo->izq;
	nodo_t *padre_predecesor = nodo;

	while (predecesor->der != NULL) {
		padre_predecesor = predecesor;
		predecesor = predecesor->der;
	}

	if (padre_predecesor->der == predecesor)
		padre_predecesor->der = predecesor->izq;
	else
		padre_predecesor->izq = predecesor->izq;

	predecesor->izq = nodo->izq;
	predecesor->der = nodo->der;

	free(nodo);

	return predecesor;
}

bool quitar_nodo(nodo_t *padre, nodo_t *raiz, void *buscado, void **encontrado,
		 int (*comparador)(void *, void *))
{
	if (!raiz)
		return false;
	int resultado = comparador(buscado, raiz->elemento);
	if (resultado == 0) {
		if (encontrado)
			*encontrado = raiz->elemento;
		nodo_t *nuevo_subarbol = _quitar_nodo(raiz);
		if (!padre)
			return true;
		if (padre->izq == raiz)
			padre->izq = nuevo_subarbol;
		else
			padre->der = nuevo_subarbol;
		return true;
	} else if (resultado > 0) {
		return quitar_nodo(raiz, raiz->der, buscado, encontrado,
				   comparador);
	} else {
		return quitar_nodo(raiz, raiz->izq, buscado, encontrado,
				   comparador);
	}
}

bool abb_quitar(abb_t *abb, void *buscado, void **encontrado)
{
	if (!abb || !abb->raiz)
		return false;
	if (abb->comparador(buscado, abb->raiz->elemento) == 0) {
		if (encontrado)
			*encontrado = abb->raiz->elemento;
		nodo_t *nueva_raiz = _quitar_nodo(abb->raiz);
		abb->raiz = nueva_raiz;
		abb->nodos--;
		return true;
	}
	bool eliminado = quitar_nodo(NULL, abb->raiz, buscado, encontrado,
				     abb->comparador);
	if (eliminado) {
		abb->nodos--;
	}

	return eliminado;
}

void *obtener_nodo(nodo_t *raiz, void *elemento,
		   int (*comparador)(void *, void *))
{
	if (!raiz)
		return NULL;
	int resultado = comparador(elemento, raiz->elemento);
	if (resultado == 0)
		return raiz->elemento;
	else if (resultado > 0)
		return obtener_nodo(raiz->der, elemento, comparador);
	else
		return obtener_nodo(raiz->izq, elemento, comparador);
}

void *abb_obtener(abb_t *abb, void *elemento)
{
	if (!abb || !abb->raiz)
		return NULL;
	return obtener_nodo(abb->raiz, elemento, abb->comparador);
}

size_t abb_cantidad(abb_t *abb)
{
	if (!abb)
		return 0;
	return abb->nodos;
}

size_t iterar_nodos_inorden(nodo_t *raiz, bool (*f)(void *, void *), void *ctx,
			    bool *seguir_iterando)
{
	if (!raiz || !f)
		return 0;
	size_t iterados = 0;
	iterados += iterar_nodos_inorden(raiz->izq, f, ctx, seguir_iterando);
	if (!(*seguir_iterando))
		return iterados;
	if (!f(raiz->elemento, ctx)) {
		*seguir_iterando = false;
		return ++iterados;
	}
	iterados++;
	iterados += iterar_nodos_inorden(raiz->der, f, ctx, seguir_iterando);
	return iterados;
}

size_t abb_iterar_inorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (!abb || !abb->raiz)
		return 0;
	bool seguir_iterando = true;
	return iterar_nodos_inorden(abb->raiz, f, ctx, &seguir_iterando);
}

size_t iterar_nodos_preorden(nodo_t *raiz, bool (*f)(void *, void *), void *ctx,
			     bool *seguir_iterando)
{
	if (!raiz || !f)
		return 0;
	size_t iterados = 0;
	if (!f(raiz->elemento, ctx)) {
		*seguir_iterando = false;
		return ++iterados;
	}
	iterados++;
	iterados += iterar_nodos_preorden(raiz->izq, f, ctx, seguir_iterando);
	if (!(*seguir_iterando))
		return iterados;
	iterados += iterar_nodos_preorden(raiz->der, f, ctx, seguir_iterando);
	return iterados;
}

size_t abb_iterar_preorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (!abb || !abb->raiz)
		return 0;
	bool seguir_iterando = true;
	return iterar_nodos_preorden(abb->raiz, f, ctx, &seguir_iterando);
}

size_t iterar_nodos_postorden(nodo_t *raiz, bool (*f)(void *, void *),
			      void *ctx, bool *seguir_iterando)
{
	if (!raiz || !f)
		return 0;
	size_t iterados = 0;
	iterados += iterar_nodos_postorden(raiz->izq, f, ctx, seguir_iterando);
	if (!(*seguir_iterando))
		return iterados;
	iterados += iterar_nodos_postorden(raiz->der, f, ctx, seguir_iterando);
	if (!(*seguir_iterando))
		return iterados;
	if (!f(raiz->elemento, ctx)) {
		*seguir_iterando = false;
		return ++iterados;
	}
	iterados++;
	return iterados;
}

size_t abb_iterar_postorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (!abb || !abb->raiz)
		return 0;
	bool seguir_iterando = true;
	return iterar_nodos_postorden(abb->raiz, f, ctx, &seguir_iterando);
}

bool agregar_al_vector(void *elemento, void *ctx)
{
	vector_ctx_t *contexto = (vector_ctx_t *)ctx;

	if (contexto->recorridos >= contexto->tamaño)
		return false;

	contexto->vector[contexto->recorridos] = elemento;
	contexto->recorridos++;
	return true;
}

size_t abb_vectorizar_inorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (!abb || !abb->raiz || !vector || tamaño == 0)
		return 0;

	vector_ctx_t ctx = { vector, tamaño, 0 };
	abb_iterar_inorden(abb, agregar_al_vector, &ctx);
	return ctx.recorridos;
}

size_t abb_vectorizar_preorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (!abb || !abb->raiz || !vector || tamaño == 0)
		return 0;

	vector_ctx_t ctx = { vector, tamaño, 0 };
	abb_iterar_preorden(abb, agregar_al_vector, &ctx);
	return ctx.recorridos;
}

size_t abb_vectorizar_postorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (!abb || !abb->raiz || !vector || tamaño == 0)
		return 0;

	vector_ctx_t ctx = { vector, tamaño, 0 };
	abb_iterar_postorden(abb, agregar_al_vector, &ctx);
	return ctx.recorridos;
}