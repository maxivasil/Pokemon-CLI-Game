<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP 2

## Repositorio de Máximo Augusto Calderón Vasil - 111810 - mcalderonv@fi.uba.ar

- Para compilar:


```bash
gcc -Wall -Werror -O2 -g -c extra/engine.c -o engine.o
gcc -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O2 -g src/*.c tp2.c engine.o -o tp2
```

- Para ejecutar:

```bash
./tp2 datos/pokedex.csv
```

- Para ejecutar con valgrind:
```bash
valgrind --leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes ./tp2 datos/pokedex.csv
```

---
##  Funcionamiento

Explicación de cómo funcionan las estructuras desarrolladas en el TP y el funcionamiento general del mismo.

Aclarar en esta parte todas las decisiones que se tomaron al realizar el TP, cosas que no se aclaren en el enunciado, fragmentos de código que necesiten explicación extra, etc.

Incluir **EN TODOS LOS TPS** los diagramas relevantes al problema (mayormente diagramas de memoria para explicar las estructuras, pero se pueden utilizar otros diagramas si es necesario).

### Por ejemplo:

El programa tp2.c funciona de la siguiente manera:
Se abre el archivo pasado por parámetro, leyendo linea a linea. Por cada linea leida, se "parsea" el pokemon y se lo agrega a una **pokedex**.Esta pokedex, como mencionaré proximamente, esta implementada como un ABB.

Detrás de este proceso de *inserción*, se implementan funciones que reservan memoria de manera tal que:

* Se crea un pokemon con los atributos correspondientes (se reserva memoria ademas para su char* nombre, char* color y char* patron_movimiento). La memoria para dicho pokemon y para dichos atributos se reserva en ambos casos mediante un malloc. En el caso del color, se utiliza una funcion para obtener, dado un nombre (AZUL, ROJO, etc), el color ANSI asociado. Luego, se libera dicho color.

Una vez cargados los pokemones en la pokedex, se cierra el archivo, liberando toda la memoria reservada. Luego, se crea y se agregan las opciones necesarias al menú, que son las siguientes:

    (S) Iniciar Juego con semilla
    (P) Mostrar pokemones
    (Q) Salir
    (J) Iniciar Juego

Una vez mostradas las opciones, se le solicitará al usuario que ingrese una opcion correcta. Es valido ingresar minusculas.

Si el usuario elige la opcion Q, se ejecutara dicha accion del menú, que libera y cierra el programa. Si, al contrario, el usuario ingresa la opcion P, se mostrarán los pokemones dentro de la pokedex y se volverá a mostrar el menú. Por ultimo, al jugar, con o sin semilla, se entrará al juego y luego se finalizará el programa. Cabe mencionar que si el usuario selecciona la opcion "S", se le solicitará que ingrese un numero positivo, mayor a cero, para usar de semilla.

Volviendo al flujo del programa, una ve 

<div align="center">
<img width="70%" src="img/diagrama1.svg">
</div>

En el archivo `sarasa.c` la función `funcion1` utiliza `realloc` para agrandar la zona de memoria utilizada para conquistar el mundo. El resultado de `realloc` lo guardo en una variable auxiliar para no perder el puntero original en caso de error:

```c
int *vector = realloc(vector_original, (n+1)*sizeof(int));

if(vector == NULL)
    return -1;
vector_original = vector;
```


<div align="center">
<img width="70%" src="img/diagrama2.svg">
</div>

---

## Respuestas a las preguntas teóricas
Incluír acá las respuestas a las preguntas del enunciado (si aplica).
