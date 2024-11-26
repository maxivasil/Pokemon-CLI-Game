#include "hash.h"
#include "string.h"
#include "stdlib.h"

typedef struct nodo nodo_t;

struct nodo {
	char *clave;
	void *valor;
	nodo_t *siguiente;
};

struct hash {
	size_t capacidad;
	size_t cantidad;
	nodo_t **tabla;
};

size_t hash_djb2(const char *str)
{
	size_t hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + (size_t)c;

	return hash;
}

hash_t *hash_crear(size_t capacidad_inicial)
{
	if (capacidad_inicial < 3)
		capacidad_inicial = 3;
	hash_t *hash = calloc(1, sizeof(hash_t));
	if (!hash)
		return NULL;
	nodo_t **tabla = calloc(capacidad_inicial, sizeof(nodo_t *));
	if (!tabla) {
		free(hash);
		return NULL;
	}
	hash->tabla = tabla;
	hash->capacidad = capacidad_inicial;
	return hash;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->cantidad;
}

void actualizar_valor(hash_t *hash, char *clave, void *valor, void **encontrado)
{
	size_t posicion = hash_djb2(clave) % hash->capacidad;
	nodo_t *actual = hash->tabla[posicion];
	while (actual) {
		if (strcmp(actual->clave, clave) == 0) {
			if (encontrado)
				*encontrado = actual->valor;
			actual->valor = valor;
			return;
		}
		actual = actual->siguiente;
	}
}

bool rehash(hash_t *hash)
{
	size_t nueva_capacidad = 2 * hash->capacidad;
	nodo_t **nueva_tabla = calloc(nueva_capacidad, sizeof(nodo_t *));
	if (!nueva_tabla)
		return false;
	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_t *actual = hash->tabla[i];
		while (actual) {
			nodo_t *siguiente = actual->siguiente;
			size_t nueva_posicion =
				hash_djb2(actual->clave) % nueva_capacidad;
			actual->siguiente = nueva_tabla[nueva_posicion];
			nueva_tabla[nueva_posicion] = actual;
			actual = siguiente;
		}
	}
	free(hash->tabla);
	hash->tabla = nueva_tabla;
	hash->capacidad = nueva_capacidad;
	return true;
}

bool copiar_clave(char *clave, void *ctx)
{
	char *copia_clave = malloc(strlen(clave) + 1);
	if (!copia_clave)
		return false;
	strcpy(copia_clave, clave);
	*(char **)ctx = copia_clave;
	return true;
}

bool insertar_nuevo_par(hash_t *hash, char *clave, void *valor)
{
	char *copia_clave = NULL;
	if (!copiar_clave(clave, &copia_clave))
		return false;
	size_t posicion = hash_djb2(copia_clave) % hash->capacidad;
	nodo_t *nuevo_nodo = calloc(1, sizeof(nodo_t));
	if (!nuevo_nodo) {
		free(copia_clave);
		return false;
	}
	nuevo_nodo->clave = copia_clave;
	nuevo_nodo->valor = valor;
	nuevo_nodo->siguiente = hash->tabla[posicion];
	hash->tabla[posicion] = nuevo_nodo;
	hash->cantidad++;
	return true;
}

bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado)
{
	if (!hash || !clave)
		return false;
	bool existe_clave = hash_contiene(hash, clave);
	if (existe_clave) {
		actualizar_valor(hash, clave, valor, encontrado);
		return true;
	}
	if (encontrado)
		*encontrado = NULL;
	double factor_carga =
		(double)(hash->cantidad + 1) / (double)hash->capacidad;
	if (factor_carga >= 0.75) {
		if (!rehash(hash))
			return false;
	}
	return insertar_nuevo_par(hash, clave, valor);
}

nodo_t *buscar_par(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return NULL;
	size_t posicion_esperada = hash_djb2(clave) % hash->capacidad;
	nodo_t *actual = hash->tabla[posicion_esperada];
	while (actual) {
		if (strcmp(actual->clave, clave) == 0)
			return actual;
		actual = actual->siguiente;
	}
	return NULL;
}

void *hash_buscar(hash_t *hash, char *clave)
{
	nodo_t *encontrado = buscar_par(hash, clave);
	if (!encontrado)
		return NULL;
	return encontrado->valor;
}

bool hash_contiene(hash_t *hash, char *clave)
{
	return buscar_par(hash, clave) != NULL;
}

void *nodo_quitar(hash_t *hash, nodo_t **nodo_ptr)
{
	nodo_t *nodo_a_eliminar = *nodo_ptr;
	void *valor = nodo_a_eliminar->valor;
	*nodo_ptr = nodo_a_eliminar->siguiente;
	free(nodo_a_eliminar->clave);
	free(nodo_a_eliminar);
	hash->cantidad--;
	return valor;
}

void *hash_quitar(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return NULL;
	size_t posicion_esperada = hash_djb2(clave) % hash->capacidad;
	nodo_t *actual = hash->tabla[posicion_esperada];
	if (!actual)
		return NULL;
	if (strcmp(actual->clave, clave) == 0) {
		return nodo_quitar(hash, &hash->tabla[posicion_esperada]);
	}
	while (actual->siguiente) {
		if (strcmp(actual->siguiente->clave, clave) == 0) {
			return nodo_quitar(hash, &actual->siguiente);
		}
		actual = actual->siguiente;
	}
	return NULL;
}

size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx)
{
	if (!hash || !f)
		return 0;
	size_t iterados = 0;
	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_t *actual = hash->tabla[i];
		while (actual) {
			iterados++;
			if (!f(actual->clave, actual->valor, ctx))
				return iterados;
			actual = actual->siguiente;
		}
	}
	return iterados;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;
	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_t *actual = hash->tabla[i];
		while (actual) {
			nodo_t *siguiente = actual->siguiente;
			if (destructor)
				destructor(actual->valor);
			free(actual->clave);
			free(actual);
			actual = siguiente;
		}
	}
	free(hash->tabla);
	free(hash);
}