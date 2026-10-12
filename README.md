# Estimación de Constantes Matemáticas (π y e) con Monte Carlo y MPI

**Proyecto 1 - Evaluación Parcial 1 (HPC 2026)**
Métodos Numéricos II - Computación Científica
Facultad de Ciencias Exactas y Tecnología - Universidad Nacional de Tucumán

**Integrantes:** Campos Nazaret · Cano Juan · Diaz Daniela

---

## Descripción

Implementación paralela en C con MPI del método de Monte Carlo para estimar dos constantes matemáticas:

- **π:** se generan puntos al azar en el cuadrado [−1, 1] × [−1, 1] y se cuenta cuántos caen dentro del círculo unitario. Como el círculo ocupa π/4 del cuadrado, π ≈ 4 · (puntos adentro / puntos totales).
- **e:** se suman números al azar uniformes en [0, 1] hasta que la suma supera 1, y se cuenta cuántos hicieron falta. El promedio de esa cantidad, sobre muchos experimentos, converge a e.

Las N simulaciones se reparten en partes iguales entre los P procesos MPI. Cada proceso cuenta sus resultados de forma independiente y al final `MPI_Reduce` suma los contadores en el proceso 0, que calcula las aproximaciones, los errores y el tiempo de ejecución.

## Estructura del repositorio

```
├── README.md
├── docs/
│   ├── Consigna_Proyecto1.md                   Consigna del proyecto
│   ├── Resultados_Empiricos_Cluster_2026.md    Registro de las ejecuciones en el clúster FaMAF (UNC)
│   └── (informe final en PDF)                  Informe del proyecto
└── src/
    ├── MonteCarlo_Aproximacion_PI_y_E.c        Programa en C con MPI: estimación de π y e
    └── simulaciones_y_graficos.ipynb           Notebook que genera los gráficos del informe
```

## Requisitos

- Compilador de C y OpenMPI (`mpicc` y `mpirun`). En Ubuntu / WSL:

- Para el notebook: Python 3 con `numpy`, `matplotlib` y Jupyter (o VS Code con la extensión Jupyter).

## Compilación

Desde la carpeta principal del repositorio:

```bash
mpicc -O3 -o montecarlo_hpc src/MonteCarlo_Aproximacion_PI_y_E.c -lm
```

## Ejecución

```bash
mpirun -n <P> ./montecarlo_hpc <N>
```

- `<P>`: cantidad de procesos MPI.
- `<N>`: cantidad total de simulaciones. Debe escribirse con todos los dígitos (por ejemplo `10000000`, no `1e7`). Si se omite, se usan 10.000.000.

Ejemplo con 4 procesos y 10 millones de simulaciones:

```bash
mpirun -n 4 ./montecarlo_hpc 10000000
```

El programa imprime la cantidad de procesos y de simulaciones, el tiempo de ejecución, y para cada constante el valor obtenido, el valor teórico y los errores absoluto y relativo.



## Gráficos

Abrir `src/simulaciones_y_graficos` y ejecutar todas las celdas (*Run All*). Cada celda genera y muestra uno de los gráficos del informe.
