#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>
#include <limits.h>

#define TARGET 1081
#define N 6
#define MAX_VALS_PER_MASK 8192 // Límite seguro de combinaciones por máscara
#define HASH_SIZE 8191         // Número primo para evitar colisiones en la tabla Hash

// El array inicial de números
int numbers[N] = {1, 2, 3, 4, 5, 6};

// Estructura para recordar CÓMO se formó un número (para hacer Traceback al final)
typedef struct {
    int val;
    int l_val;
    int r_val;
    int l_mask;
    int r_mask;
    char op; // '+', '-', '*', '/', o ' ' para los números base
} Expression;

// Cada máscara de bits guarda todos los valores únicos que puede generar
typedef struct {
    Expression data[MAX_VALS_PER_MASK];
    int count;
} SubsetMemo;

SubsetMemo dp[64];

// Estado global para la mejor aproximación
int best_diff = 9999999;
int best_val = -1;
int best_mask = -1;

bool parse_positive_int(const char *text, int *value) {
    char *end;
    errno = 0;
    long parsed = strtol(text, &end, 10);

    if (errno == ERANGE || end == text || *end != '\0' || parsed <= 0 || parsed > INT_MAX) {
        return false;
    }

    *value = (int)parsed;
    return true;
}

// Función rápida para contar cuántos bits en '1' tiene una máscara (Popcount)
// Esto nos dice cuántos números se están usando en ese subconjunto
int popcount(int mask) {
    int count = 0;
    for (int m = mask; m; m >>= 1) {
        count += m & 1;
    }
    return count;
}

// Función recursiva que reconstruye el texto solo al final (Traceback)
void print_trace(int mask, int val) {
    // Buscamos el valor dentro de la máscara
    Expression *expr = NULL;
    for (int i = 0; i < dp[mask].count; i++) {
        if (dp[mask].data[i].val == val) {
            expr = &dp[mask].data[i];
            break;
        }
    }
    
    // Si es un número base, cortamos la recursión
    if (!expr || expr->op == ' ') return;
    
    // Imprimimos primero cómo se formaron sus componentes (Bottom-Up)
    print_trace(expr->l_mask, expr->l_val);
    print_trace(expr->r_mask, expr->r_val);
    
    // Imprimimos la operación actual
    printf("  %d %c %d = %d\n", expr->l_val, expr->op, expr->r_val, expr->val);
}

int main(int argc, char *argv[]) {
    int target = TARGET;

    if (argc == 2) {
        if (!parse_positive_int(argv[1], &target)) {
            fprintf(stderr, "Error: el objetivo debe ser un entero positivo.\n");
            return 1;
        }
    } else if (argc == N + 1 || argc == N + 2) {
        for (int i = 0; i < N; i++) {
            if (!parse_positive_int(argv[i + 1], &numbers[i])) {
                fprintf(stderr, "Error: cada numero debe ser un entero positivo.\n");
                return 1;
            }
        }

        if (argc == N + 2 && !parse_positive_int(argv[N + 1], &target)) {
            fprintf(stderr, "Error: el objetivo debe ser un entero positivo.\n");
            return 1;
        }
    } else if (argc != 1) {
        fprintf(stderr, "Uso: %s [objetivo]\n", argv[0]);
        fprintf(stderr, "     %s n1 n2 n3 n4 n5 n6 [objetivo]\n", argv[0]);
        return 1;
    }

    // Inicializar el espacio DP
    for (int i = 0; i < 64; i++) dp[i].count = 0;
    
    // Iteramos por tamaño de subconjunto (de 1 a 6 números)
    // Esto garantiza que el primer resultado encontrado SIEMPRE es el de menor número de pasos
    for (int len = 1; len <= N; len++) {
        for (int mask = 1; mask < 64; mask++) {
            if (popcount(mask) != len) continue;
            
            // CASO BASE: Subconjuntos de 1 solo número
            if (len == 1) {
                for (int i = 0; i < N; i++) {
                    if (mask == (1 << i)) {
                        dp[mask].data[0] = (Expression){numbers[i], 0, 0, 0, 0, ' '};
                        dp[mask].count = 1;
                        
                        int diff = abs(numbers[i] - target);
                        if (diff < best_diff) {
                            best_diff = diff;
                            best_val = numbers[i];
                            best_mask = mask;
                        }
                        break;
                    }
                }
                continue; // Pasamos a la siguiente máscara
            }
            
            // CASO GENERAL: Cruzar dos subconjuntos (Memoización)
            int hash_table[HASH_SIZE] = {0}; // 0 indica celda vacía (las reglas prohíben resultados 0)
            
            // Iteramos sobre todos los posibles subconjuntos (sub1) de la máscara actual
            for (int sub1 = 1; sub1 < mask; sub1++) {
                if ((sub1 & mask) == sub1) {
                    int sub2 = mask ^ sub1;
                    
                    // Para evitar duplicidades lógicas (A cruzado con B y B cruzado con A)
                    if (sub1 < sub2) {
                        
                        // Cruce cartesiano de los valores memoizados en sub1 y sub2
                        for (int i = 0; i < dp[sub1].count; i++) {
                            for (int j = 0; j < dp[sub2].count; j++) {
                                int vA = dp[sub1].data[i].val;
                                int vB = dp[sub2].data[j].val;
                                
                                int r_vals[4];
                                char r_ops[4];
                                int l_vals[4], r_vals_ordered[4];
                                int l_masks[4], r_masks_ordered[4];
                                int r_cnt = 0;
                                
                                // SUMA
                                r_vals[r_cnt] = vA + vB; r_ops[r_cnt] = '+'; 
                                l_vals[r_cnt] = vA; r_vals_ordered[r_cnt] = vB;
                                l_masks[r_cnt] = sub1; r_masks_ordered[r_cnt++] = sub2;
                                
                                // MULTIPLICACIÓN
                                if (vA > 1 && vB > 1) {
                                    r_vals[r_cnt] = vA * vB; r_ops[r_cnt] = '*';
                                    l_vals[r_cnt] = vA; r_vals_ordered[r_cnt] = vB;
                                    l_masks[r_cnt] = sub1; r_masks_ordered[r_cnt++] = sub2;
                                }
                                
                                // RESTA (Filtro estricto)
                                if (vA > vB) {
                                    r_vals[r_cnt] = vA - vB; r_ops[r_cnt] = '-';
                                    l_vals[r_cnt] = vA; r_vals_ordered[r_cnt] = vB;
                                    l_masks[r_cnt] = sub1; r_masks_ordered[r_cnt++] = sub2;
                                } else if (vB > vA) {
                                    r_vals[r_cnt] = vB - vA; r_ops[r_cnt] = '-';
                                    l_vals[r_cnt] = vB; r_vals_ordered[r_cnt] = vA;
                                    l_masks[r_cnt] = sub2; r_masks_ordered[r_cnt++] = sub1;
                                }
                                
                                // DIVISIÓN (Filtro estricto)
                                if (vB > 1 && vA % vB == 0) {
                                    r_vals[r_cnt] = vA / vB; r_ops[r_cnt] = '/';
                                    l_vals[r_cnt] = vA; r_vals_ordered[r_cnt] = vB;
                                    l_masks[r_cnt] = sub1; r_masks_ordered[r_cnt++] = sub2;
                                } else if (vA > 1 && vB % vA == 0) {
                                    r_vals[r_cnt] = vB / vA; r_ops[r_cnt] = '/';
                                    l_vals[r_cnt] = vB; r_vals_ordered[r_cnt] = vA;
                                    l_masks[r_cnt] = sub2; r_masks_ordered[r_cnt++] = sub1;
                                }
                                
                                // Evaluar e insertar los resultados válidos usando Memoización Local (Hash)
                                for (int k = 0; k < r_cnt; k++) {
                                    int val = r_vals[k];
                                    
                                    // Búsqueda en tabla Hash (Linear Probing rápido)
                                    int slot = val % HASH_SIZE;
                                    while (hash_table[slot] != 0 && hash_table[slot] != val) {
                                        slot = (slot + 1) % HASH_SIZE;
                                    }
                                    
                                    // Si la celda está vacía, es un número nuevo para esta máscara
                                    if (hash_table[slot] == 0) {
                                        hash_table[slot] = val; // Lo memoizamos
                                        
                                        int idx = dp[mask].count++;
                                        dp[mask].data[idx] = (Expression){
                                            val, l_vals[k], r_vals_ordered[k], 
                                            l_masks[k], r_masks_ordered[k], r_ops[k]
                                        };
                                        
                                        // Actualizamos el mejor resultado global
                                        int diff = abs(val - target);
                                        if (diff < best_diff) {
                                            best_diff = diff;
                                            best_val = val;
                                            best_mask = mask;
                                            
                                            // OPTATIVO: Cortocircuito si encontramos coincidencia exacta.
                                            // Como el bucle exterior avanza por 'len' (nº de operaciones), 
                                            // el primer diff==0 SIEMPRE es el camino más corto.
                                            if (diff == 0) goto exit_loops;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

exit_loops:
    
    printf("Target: %d\n", target);
    if (best_diff == 0) {
        printf("¡Solucion exacta encontrada!\n\n");
    } else {
        printf("Mejor aproximacion: %d (Diferencia: %d)\n\n", best_val, best_diff);
    }
    
    printf("Pasos tomados:\n");
    print_trace(best_mask, best_val);
    
    return 0;
}