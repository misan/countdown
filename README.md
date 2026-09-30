# Cifras

Proyecto de práctica con distintas soluciones al problema de **Cifras**: combinar números mediante operaciones aritméticas para alcanzar un objetivo. Incluye implementaciones en C, C++ y Python, además de un script para comparar el rendimiento de tres versiones Python.

## `countdown_v2.c`

Es la versión C con entrada por línea de comandos. Usa programación dinámica sobre subconjuntos de seis números. Permite suma, multiplicación, resta positiva y división entera exacta; informa si encuentra el objetivo y muestra los pasos reconstruidos.

### Compilar

En una terminal de desarrollador de Visual Studio:

```bat
cl /nologo /O2 /Fe:countdown_v2.exe countdown_v2.c
```

También se puede compilar con GCC:

```sh
gcc -std=c99 -O2 countdown_v2.c -o countdown_v2
```

### Ejecutar

Sin argumentos, usa los números `1 2 3 4 5 6` y el objetivo predeterminado `1081`.

```text
countdown_v2.exe
```

Un argumento cambia solo el objetivo:

```text
countdown_v2.exe 500
```

Seis argumentos cambian los números y conservan el objetivo predeterminado:

```text
countdown_v2.exe 25 50 3 8 7 9
```

Con siete argumentos, los primeros seis son los números y el último es el objetivo:

```text
countdown_v2.exe 25 50 3 8 7 9 721
```

En Linux, macOS o Git Bash con la compilación de GCC, se puede invocar como `./countdown_v2` con los mismos argumentos. Los valores deben ser enteros positivos. Una cantidad de argumentos no admitida o un valor inválido produce un mensaje de error.

## Otros programas

- `count_down.c`, `count_down.cpp` y `count_down.py`: otras implementaciones del solucionador con sus propios números y objetivos configurados en el código.
- `cifras_naive.py`, `cifras_improved.py` y `cifras_best.py`: versiones Python con estrategias de búsqueda distintas y configuraciones internas.
- `countdown_v2.md`: explicación paso a paso de la búsqueda por subconjuntos, la memoización y la reconstrucción de la solución.

## Benchmark Python

`benchmark_cifras.py` compara `cifras_naive.py`, `cifras_improved.py` y `cifras_best.py` sobre 25 casos aleatorios, repitiendo cada caso 30 veces. Imprime los tiempos y genera o reemplaza `benchmark_plot.png` en la carpeta del proyecto.

Necesita Python 3.9 o posterior y `matplotlib`:

```sh
python -m pip install matplotlib
python benchmark_cifras.py
```

El benchmark inicia muchos procesos Python y puede tardar. Para reducir su duración, ajusta `RANDOM_CASES` o `REPETITIONS` al principio de `benchmark_cifras.py`.
