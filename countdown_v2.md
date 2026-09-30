# Cómo funciona `countdown_v2.c`

Este programa busca cómo combinar seis números para acercarse lo más posible a un objetivo. Si se ejecuta sin argumentos, usa los valores predeterminados `1, 2, 3, 4, 5, 6` y el objetivo `1081`.

La idea importante es que el programa no prueba una expresión completa de principio a fin. Primero calcula qué resultados se pueden obtener con cada subconjunto de números y guarda esos resultados para reutilizarlos. Esta técnica se llama **programación dinámica**.

## 1. Configuración y estructuras

Al principio del archivo están las constantes principales:

- `TARGET`: el número que se quiere alcanzar.
- `N`: cuántos números hay disponibles.
- `MAX_VALS_PER_MASK`: el máximo de resultados que se reserva para cada subconjunto.
- `HASH_SIZE`: el tamaño de la tabla hash que evita guardar dos veces el mismo resultado para un subconjunto.
- `numbers`: el arreglo con los números disponibles.

La estructura `Expression` guarda un resultado (`val`) y la información necesaria para reconstruir cómo se obtuvo: los dos operandos, las máscaras de sus subconjuntos y el operador. Si `op` es un espacio, el resultado corresponde a uno de los números iniciales.

`SubsetMemo` almacena una lista de estructuras `Expression`. El arreglo global `dp` tiene una posición para cada máscara posible. Como hay seis números, las máscaras van de `0` a `63`.

## 2. Representar subconjuntos con bits

Cada número tiene una posición en el arreglo y, por tanto, un bit:

| Índice | Número | Bit |
|---:|---:|---:|
| 0 | 1 | `000001` |
| 1 | 2 | `000010` |
| 2 | 3 | `000100` |
| 3 | 4 | `001000` |
| 4 | 5 | `010000` |
| 5 | 6 | `100000` |

Un bit en `1` indica que ese número pertenece al subconjunto. Por ejemplo, el subconjunto `{2, 3, 4}` activa los bits de los índices 1, 2 y 3: `001110` en binario, que vale `14`.

La expresión `1 << i` crea una máscara con el bit `i` activado. Las operaciones `&` y `^` permiten comprobar pertenencia a un subconjunto y separar una máscara en dos partes.

## 3. Construir resultados de menor a mayor

`main` inicializa en cero el contador de cada entrada de `dp` y recorre los subconjuntos según su tamaño, desde uno hasta seis. `popcount(mask)` cuenta cuántos bits tiene activados una máscara; así el programa sabe cuántos números contiene.

### Caso base: un solo número

Cuando el subconjunto contiene un número, ese número es el único resultado posible. Por ejemplo, para la máscara de `{3}`, el programa guarda el valor `3` y marca su operador como espacio. Esos casos base permiten construir los subconjuntos más grandes.

### Caso general: combinar dos subconjuntos

Para un subconjunto mayor, el programa lo divide en dos partes disjuntas, `sub1` y `sub2`. Por ejemplo, `{2, 3, 4}` puede dividirse en `{2, 3}` y `{4}`. Como los subconjuntos pequeños ya se calcularon, el programa puede combinar cada resultado de una parte con cada resultado de la otra.

La condición `sub1 < sub2` evita procesar dos veces la misma división, una vez como A con B y otra como B con A. Para las operaciones que sí dependen del orden, como la resta y la división, el código prueba la orientación válida de los operandos.

Por ejemplo, si `{2, 3}` puede producir `5` y `{4}` produce `4`, al combinarlos se pueden obtener `9` (`5 + 4`), `20` (`5 * 4`) y `1` (`5 - 4`). El registro del resultado `20` guarda que se obtuvo multiplicando `5` por `4`, además de las máscaras que explican de dónde salió cada operando.

## 4. Reglas para las operaciones

El programa aplica estas reglas:

- **Suma:** siempre se permite.
- **Multiplicación:** se permite si ambos operandos son mayores que `1`. Así se evitan multiplicaciones por uno, que no producen un valor nuevo útil.
- **Resta:** solo se permite cuando los operandos son distintos y el resultado es positivo. El código coloca primero el mayor.
- **División:** solo se permite si la división es exacta y el divisor es mayor que `1`.

Estas restricciones hacen que todos los resultados sean enteros positivos, lo cual coincide con las reglas habituales de este tipo de reto.

## 5. Evitar resultados repetidos

Distintas operaciones pueden producir el mismo valor usando el mismo subconjunto. Para no guardar todas esas expresiones, cada máscara tiene una tabla hash local.

El índice inicial se calcula con `val % HASH_SIZE`. Si esa posición ya contiene otro valor, el código avanza a la siguiente posición: esto se llama **sondeo lineal**. Si el valor ya está en la tabla, no se vuelve a agregar a `dp[mask]`. Si es nuevo, se guarda el valor y su `Expression`.

Guardar solo una forma de obtener cada valor ahorra memoria y trabajo. La expresión guardada basta para reconstruir una solución.

## 6. Elegir el mejor resultado

Cada vez que se genera un valor, el programa calcula su distancia al objetivo con `abs(val - TARGET)`. Si la distancia es menor que la mejor conocida, actualiza `best_diff`, `best_val` y `best_mask`.

El programa también considera subconjuntos pequeños, no solo el conjunto de los seis números. Por eso puede informar una solución que use únicamente algunos de ellos. Si dos resultados tienen la misma distancia, el código conserva el primero que encontró.

Si encuentra el objetivo exacto, salta a `exit_loops`. Esto no sacrifica una solución con menos operaciones: los subconjuntos se procesan por tamaño, y una expresión binaria construida con `k` números necesita `k - 1` operaciones. Por lo tanto, la primera solución exacta encontrada usa el menor número de operaciones posible.

## 7. Reconstruir e imprimir los pasos

Durante la búsqueda se guardan datos compactos, no textos completos para cada expresión. Cuando termina, `print_trace(mask, val)` busca el registro del resultado elegido y sigue sus máscaras hacia atrás.

La función primero reconstruye el operando izquierdo y después el derecho; al final imprime la operación actual. Esto produce los pasos en orden de ejecución, desde los números iniciales hasta el resultado final. Cuando llega a un número inicial —identificado porque su operador es un espacio— detiene esa rama de la recursión.

## 8. Flujo completo

1. Inicializa el almacenamiento de los subconjuntos.
2. Guarda cada número individual como caso base.
3. Para tamaños crecientes, divide cada subconjunto y combina sus resultados.
4. Descarta resultados duplicados para cada máscara.
5. Actualiza el valor más cercano al objetivo.
6. Al terminar, imprime el mejor resultado y reconstruye sus operaciones.

## 9. Límites de esta versión

El código está preparado para seis números: el arreglo `dp` tiene 64 entradas y los bucles recorren las máscaras de `1` a `63`. Si se cambia `N`, también hay que revisar esos tamaños y los límites asociados.

Además, cada subconjunto tiene una capacidad fija de `MAX_VALS_PER_MASK`. El código actual no comprueba explícitamente si esa capacidad se agota; con otros números o más operaciones, habría que añadir una comprobación o usar almacenamiento dinámico. Los valores también se guardan como `int`, así que una configuración con productos mucho mayores podría desbordar ese tipo.

## 10. Entrada por línea de comandos

El programa acepta tres formas de entrada:

| Argumentos después del nombre del programa | Comportamiento |
|---|---|
| Ninguno | Usa los seis números y el objetivo predeterminados. |
| Un número | Usa ese número como objetivo y conserva los números predeterminados. |
| Seis números | Usa esos números y conserva el objetivo predeterminado. |
| Seis números y un séptimo número | Usa los primeros seis como números y el último como objetivo. |

Ejemplos:

```text
countdown_v2.exe 500
countdown_v2.exe 25 50 3 8 7 9
countdown_v2.exe 25 50 3 8 7 9 721
```

Todos los argumentos deben ser enteros positivos. Si se proporciona una cantidad distinta de argumentos, o un valor no válido, el programa muestra un error de uso.

Para compilar desde una terminal de desarrollador de Visual Studio y ejecutar con el objetivo `500`:

```text
cl /nologo countdown_v2.c
countdown_v2.exe 500
```

La búsqueda puede tardar más si se aumentan los números o la cantidad de resultados posibles por subconjunto.
