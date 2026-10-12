/*
 * =======================================================================================
 * Proyecto 1 - HPC 2026: Estimación de Constantes Matemáticas (Pi y Euler)
 * Cátedra: Métodos Numéricos II - Computación Científica
 * Institución: FACET - Universidad Nacional de Tucumán (UNT)
 * 
 * Implementación paralela mediante el estándar MPI (Message Passing Interface)
 * en Lenguaje C para la aproximación estocástica simultánea de Pi (pi) y Euler (e)
 * utilizando el Método de Monte Carlo sobre memoria distribuida.
 * =======================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

// Valor exacto de Euler (base de logaritmos naturales)
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

// Generador de número pseudoaleatorio continuo uniforme en [-1.0, 1.0]
double random_uniform_bilateral() {
    return ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
}

// Generador de número pseudoaleatorio continuo uniforme en [0.0, 1.0]
double random_uniform_unitario() {
    return (double)rand() / (double)RAND_MAX;
}

// Función 1: Estimación geométrica de Pi en el círculo unitario inscrito
long long calcular_puntos_circulo(long long iteraciones) {
    long long puntos_adentro = 0;
    for (long long i = 0; i < iteraciones; i++) {
        double x = random_uniform_bilateral();
        double y = random_uniform_bilateral();
        if (x * x + y * y <= 1.0) {
            puntos_adentro++;
        }
    }
    return puntos_adentro;
}

// Función 2: Estimación probabilística de Euler mediante conteo de sumas uniformes > 1.0
long long calcular_extracciones_euler(long long iteraciones) {
    long long total_extracciones = 0;
    for (long long i = 0; i < iteraciones; i++) {
        double suma = 0.0;
        int conteo = 0;
        while (suma <= 1.0) {
            suma += random_uniform_unitario();
            conteo++;
        }
        total_extracciones += conteo;
    }
    return total_extracciones;
}

int main(int argc, char** argv) {
    // Número total de simulaciones por defecto: 10^7 (10 millones)
    long long total_iteraciones = 10000000LL;
    
    // Si se pasa un argumento por línea de comandos, se adopta como N total
    if (argc > 1) {
        total_iteraciones = atoll(argv[1]);
        if (total_iteraciones <= 0) {
            total_iteraciones = 10000000LL;
        }
    }

    int my_rank, num_procs;
    long long iteraciones_por_proceso;
    
    // Variables locales y globales de reducción para Pi
    long long puntos_pi_local = 0, puntos_pi_total = 0;
    
    // Variables locales y globales de reducción para Euler
    long long extracciones_e_local = 0, extracciones_e_total = 0;
    
    double pi_aproximado = 0.0, e_aproximado = 0.0;
    double tiempo_inicio = 0.0, tiempo_fin = 0.0;

    // 1. Inicialización formal del entorno distribuido MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // 2. Particionamiento estático equitativo de carga de trabajo
    iteraciones_por_proceso = total_iteraciones / num_procs;

    // 3. Desfase crítico de semilla pseudoaleatoria por rango para evitar trayectorias duplicadas
    srand((unsigned int)(time(NULL) + my_rank * 1013));

    // 4. Marca de tiempo de alta resolución en el proceso coordinador (root)
    if (my_rank == 0) {
        printf("===================================================================\n");
        printf(" HPC 2026: Estimacion de Constantes (PI y E) con Monte Carlo y MPI\n");
        printf("===================================================================\n");
        printf(" Procesos concurrentes (P)   : %d\n", num_procs);
        printf(" Muestras totales (N)        : %lld\n", total_iteraciones);
        printf(" Muestras por proceso (N_loc): %lld\n", iteraciones_por_proceso);
        printf("-------------------------------------------------------------------\n");
        tiempo_inicio = MPI_Wtime();
    }

    // 5. Cómputo local independiente y concurrente (sin comunicación inter-procesos)
    puntos_pi_local = calcular_puntos_circulo(iteraciones_por_proceso);
    extracciones_e_local = calcular_extracciones_euler(iteraciones_por_proceso);

    // 6. Reducciones colectivas globales hacia el proceso raíz (rank 0)
    MPI_Reduce(&puntos_pi_local, &puntos_pi_total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&extracciones_e_local, &extracciones_e_total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // 7. Consolidación de resultados, cálculo de errores y reporte en proceso 0
    if (my_rank == 0) {
        tiempo_fin = MPI_Wtime();
        double tiempo_total = tiempo_fin - tiempo_inicio;

        long long n_evaluado = iteraciones_por_proceso * num_procs;
        pi_aproximado = 4.0 * ((double)puntos_pi_total / (double)n_evaluado);
        e_aproximado = (double)extracciones_e_total / (double)n_evaluado;

        double pi_teorico = M_PI;
        double e_teorico = M_E;

        double err_abs_pi = fabs(pi_aproximado - pi_teorico);
        double err_rel_pi = (err_abs_pi / pi_teorico) * 100.0;

        double err_abs_e = fabs(e_aproximado - e_teorico);
        double err_rel_e = (err_abs_e / e_teorico) * 100.0;

        printf(" [RESULTADOS EMPIRICOS]\n");
        printf(" Tiempo de ejecucion paralelo : %.4f segundos\n", tiempo_total);
        printf(" -------------------------------------------------------------------\n");
        printf(" [CONSTANTE PI]\n");
        printf("   Valor teorico exacto      : %.15f\n", pi_teorico);
        printf("   Valor aproximado obtenido : %.15f\n", pi_aproximado);
        printf("   Error absoluto            : %.15f\n", err_abs_pi);
        printf("   Error relativo porcentual : %.6f %%\n", err_rel_pi);
        printf(" -------------------------------------------------------------------\n");
        printf(" [CONSTANTE EULER (e)]\n");
        printf("   Valor teorico exacto      : %.15f\n", e_teorico);
        printf("   Valor aproximado obtenido : %.15f\n", e_aproximado);
        printf("   Error absoluto            : %.15f\n", err_abs_e);
        printf("   Error relativo porcentual : %.6f %%\n", err_rel_e);
        printf("===================================================================\n");
    }

    // 8. Cierre y liberación de recursos de la interfaz MPI
    MPI_Finalize();
    return 0;
}
