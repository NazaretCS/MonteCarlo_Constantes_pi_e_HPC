# Memoria Técnica y Registro Experimental Oficial en Clúster HPC (2026)

**Proyecto:** Estimación de Constantes Matemáticas ($\pi$ y $e$) mediante Monte Carlo y Cómputo Paralelo (MPI)\
**Institución:** FACET - Universidad Nacional de Tucumán (UNT) / FaMAF - UNC\
**Autores:** Campos Nazaret, Cano Juan, Diaz Daniela\
**Año:** 2026

---

## 1. Protocolo Experimental y Guía de Ejecución Paso a Paso

A continuación se detalla el procedimiento real que seguimos en la plataforma de supercómputo para configurar los entornos, compilar el código fuente y obtener cada una de las tablas numéricas del informe final.

### Fase 1: Puesta a punto y validación inicial (Instancia Base: 2 Cores y 6 GB RAM)

1. **Configuración del Servidor en el Portal Web:**
   * **Tipo de servidor:** Solo CPU (Cluster Boogie | AMD EPYC 7B12).
   * **Entorno de trabajo:** Python Científico (con GCC, OpenMPI y librerías numéricas preinstaladas).
   * **Interfaz:** Lab (JupyterLab).
   * **Recursos seleccionados:** **`6G de RAM y 2 cores`** (para realizar las primeras validaciones de código sin saturar recursos del clúster).

2. **Verificación de Compiladores por Terminal:**
   Abrimos la terminal integrada de Linux y comprobamos la disponibilidad nativa de GCC y de las bibliotecas de paso de mensajes MPI:
   ```bash
   gcc --version
   which mpicc
   which mpirun
   ```
   *Respuesta del sistema:* GCC versión 13.3.0 en Ubuntu 24.04 LTS y ejecutables oficiales en `/usr/bin/mpicc` y `/usr/bin/mpirun`.

3. **Subida y Compilación del Código C:**
   Subimos nuestro archivo `MonteCarlo_Aproximacion_PI_y_E.c` al directorio de trabajo y lo compilamos con optimizaciones agresivas de nivel 3 (`-O3`) y enlace matemático (`-lm`):
   ```bash
   mpicc -O3 -o montecarlo_hpc MonteCarlo_Aproximacion_PI_y_E.c -lm
   ```

4. **Prueba de Humo y Verificación de SpeedUp ($P = 2$ vs $P = 1$):**
   Lanzamos una prueba preliminar con 10 millones de muestras distribuidas en 2 procesos concurrentes:
   ```bash
   mpirun -np 2 ./montecarlo_hpc 10000000
   ```
   *Resultado:* Tardó **`0.3276` segundos**, obteniendo $\pi \approx 3.1418616$ (3 decimales) y $e \approx 2.7182401$ (4 decimales).  
   Luego medimos el tiempo base monoproceso ($P = 1$):
   ```bash
   mpirun -np 1 ./montecarlo_hpc 10000000
   ```
   *Resultado:* Tardó **`0.6511` segundos**, arrojando un SpeedUp inicial de $S_2 = 0.6511 / 0.3276 = \mathbf{1.987\times}$ y una eficiencia del **$99.37\%$**.

5. **Obtención de las Tablas 1 y 2 (Convergencia de $10^1$ a $8 \times 10^7$ muestras):**
   Ejecutamos el barrido de potencias de 10 con 2 procesos MPI activos para registrar el decaimiento asintótico del error en ambas constantes:
   ```bash
   # Comandos clásicos ejecutados para cada escala N:
   mpirun -np 2 ./montecarlo_hpc 10
   mpirun -np 2 ./montecarlo_hpc 100
   mpirun -np 2 ./montecarlo_hpc 1000
   mpirun -np 2 ./montecarlo_hpc 10000
   mpirun -np 2 ./montecarlo_hpc 100000
   mpirun -np 2 ./montecarlo_hpc 1000000
   mpirun -np 2 ./montecarlo_hpc 10000000
   mpirun -np 2 ./montecarlo_hpc 80000000
   ```

---

### Fase 2: Escalamiento a Nodo de Supercómputo Masivo (Instancia Dedicada: 128 Cores y 503 GB RAM)

Para poder evaluar la escalabilidad fuerte hasta 28 procesos en paralelo sin cuellos de botella de hardware:

1. **Reconfiguración del Servidor en JupyterHub:**
   * Fuimos a **File -> Hub Control Panel** y detuvimos la máquina actual con **Stop My Server**.
   * Iniciamos un nuevo servidor (**Start My Server**) seleccionando la opción de máxima capacidad: **`192G de RAM y 48 cores`**.
   * El sistema operativo detectó físicamente la capacidad total del nodo multi-socket: **128 núcleos físicos AMD EPYC 7B12 y 503.71 GB de memoria RAM**.

2. **Ejecución de la Tabla 3 (SpeedUp en Estación de Trabajo, $N = 10^7$):**
   Evaluamos el escalamiento variando de 1 a 6 procesos mediante el parámetro `-np` de MPI:
   ```bash
   mpirun -np 1 ./montecarlo_hpc 10000000
   mpirun -np 2 ./montecarlo_hpc 10000000
   mpirun -np 3 ./montecarlo_hpc 10000000
   mpirun -np 4 ./montecarlo_hpc 10000000
   mpirun -np 5 ./montecarlo_hpc 10000000
   mpirun -np 6 ./montecarlo_hpc 10000000
   ```

3. **Ejecución de la Tabla 4 (Escalabilidad Fuerte en Clúster, $N = 100\,000\,000$):**
   Sometimos el nodo a una carga fija masiva de 100 millones de puntos, ejecutando todos y cada uno de los **15 números de procesos oficiales** requeridos por la cátedra:
   ```bash
   for P in 1 2 3 4 5 6 7 8 12 14 16 18 24 26 28; do
       mpirun --oversubscribe -np $P ./montecarlo_hpc 100000000
   done
   ```

4. **Ejecución de Curvas de Error para el Gráfico 4 ($P = 4$ y $P = 6$):**
   Para alimentar las curvas comparativas del informe, medimos las aproximaciones de $\pi$ desde $N = 10^3$ hasta $8 \times 10^7$ muestras fijando 4 y 6 procesos respectivamente:
   ```bash
   # Serie P = 4 procesos:
   for N in 1000 10000 100000 1000000 10000000 80000000; do
       mpirun -np 4 ./montecarlo_hpc $N
   done

   # Serie P = 6 procesos:
   for N in 1000 10000 100000 1000000 10000000 80000000; do
       mpirun -np 6 ./montecarlo_hpc $N
   done
   ```

---

## 2. Tablas Numéricas Consolidadas

### TABLA 1: ESTIMACIÓN DE PI ($P = 2$)
*Entorno de medición:* Instancia Base (6 GB RAM, 2 Cores dedicados, $P = 2$)  
*Valor analítico exacto:* $\pi \approx 3.141592653589793...$

| Muestras ($N$) | Pi Aprox ($\hat{\pi}$) | Error Absoluto ($\varepsilon_{\text{abs}}$) | Error Relativo (%) | Tiempo de Cómputo (s) |
| :---: | :---: | :---: | :---: | :---: |
| **$10$** | $3.600000$ | $0.458407$ | $14.5916\%$ | $0.0000$ |
| **$100$** | $3.360000$ | $0.218407$ | $6.9521\%$ | $0.0001$ |
| **$1\,000$** | $3.148000$ | $0.006407$ | $0.2040\%$ | $0.0001$ |
| **$10\,000$** | $3.122800$ | $0.018793$ | $0.5982\%$ | $0.0004$ |
| **$100\,000$** | $3.149080$ | $0.007487$ | $0.2383\%$ | $0.0033$ |
| **$1\,000\,000$** | $3.141640$ | $0.000047$ | $0.0015\%$ | $0.0329$ |
| **$10\,000\,000$** | $3.141820$ | $0.000227$ | $0.0072\%$ | $0.3259$ |
| **$80\,000\,000$** | **$3.141656$** | **$0.000063$** | **$0.0020\%$** | $2.6100$ |

---

### TABLA 2: ESTIMACIÓN DE EULER ($e$) ($P = 2$)
*Entorno de medición:* Instancia Base (6 GB RAM, 2 Cores dedicados, $P = 2$)  
*Valor analítico exacto:* $e \approx 2.718281828459045...$

| Muestras ($N$) | Euler Aprox ($\hat{e}$) | Error Absoluto ($\varepsilon_{\text{abs}}$) | Error Relativo (%) | Tiempo de Cómputo (s) |
| :---: | :---: | :---: | :---: | :---: |
| **$10$** | $2.600000$ | $0.118282$ | $4.3513\%$ | $0.0000$ |
| **$100$** | $2.770000$ | $0.051718$ | $1.9026\%$ | $0.0001$ |
| **$1\,000$** | $2.750000$ | $0.031718$ | $1.1668\%$ | $0.0001$ |
| **$10\,000$** | $2.709500$ | $0.008782$ | $0.3231\%$ | $0.0004$ |
| **$100\,000$** | $2.713650$ | $0.004632$ | $0.1704\%$ | $0.0033$ |
| **$1\,000\,000$** | $2.719123$ | $0.000841$ | $0.0309\%$ | $0.0329$ |
| **$10\,000\,000$** | $2.718568$ | $0.000286$ | $0.0105\%$ | $0.3259$ |
| **$80\,000\,000$** | **$2.718314$** | **$0.000032$** | **$0.0012\%$** | $2.6100$ |

---

### TABLA 3: SPEEDUP Y EFICIENCIA EN ESTACIÓN DE TRABAJO LOCAL ($N = 10\,000\,000$)
*Entorno de medición:* Nodo AMD EPYC 7B12 (evaluando escala local de 1 a 6 procesos)

| Procesos ($P$) | Tiempo Real ($T_P$) | Aproximación $\hat{\pi}$ | SpeedUp ($S_P$) | Eficiencia ($E_P$) | Eficiencia Porcentual |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **1** | $0.6513$ s | $3.142830$ | $1.000\times$ | $1.0000$ | $100.00\%$ |
| **2** | $0.3258$ s | $3.141842$ | $1.999\times$ | $0.9995$ | $99.95\%$ |
| **3** | $0.2184$ s | $3.141956$ | $2.982\times$ | $0.9940$ | $99.40\%$ |
| **4** | $0.1633$ s | $3.142052$ | $3.988\times$ | $0.9971$ | $99.71\%$ |
| **5** | $0.1317$ s | $3.141310$ | $4.945\times$ | $0.9891$ | $98.91\%$ |
| **6** | $0.1092$ s | $3.141803$ | $5.964\times$ | $0.9940$ | $99.40\%$ |

---

### TABLA 4: BENCHMARK COMPLETO EN CLÚSTER FAMAF (15 PROCESOS, $N = 100\,000\,000$)
*Entorno de medición:* Nodo Dedicado de Supercómputo (AMD EPYC 7B12, 128 Cores, 503.71 GB RAM)

| Procesos ($P$) | Tiempo Real ($T_P$) | Aproximación $\hat{\pi}$ | SpeedUp Empírico ($S_P$) | Eficiencia ($E_P$) | Eficiencia Porcentual |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **1** | $6.5279$ s | $3.141500$ | $1.000\times$ | $1.0000$ | $100.00\%$ |
| **2** | $3.2573$ s | $3.141659$ | $2.004\times$ | $1.0020$ | $100.20\%$ |
| **3** | $2.1959$ s | $3.141825$ | $2.973\times$ | $0.9909$ | $99.09\%$ |
| **4** | $1.6592$ s | $3.141598$ | $3.934\times$ | $0.9836$ | $98.36\%$ |
| **5** | $1.3081$ s | $3.141750$ | $4.990\times$ | $0.9981$ | $99.81\%$ |
| **6** | $1.0909$ s | $3.141412$ | $5.984\times$ | $0.9973$ | $99.73\%$ |
| **7** | $0.9340$ s | $3.141439$ | $6.989\times$ | $0.9985$ | $99.85\%$ |
| **8** | $0.8302$ s | $3.141637$ | $7.863\times$ | $0.9829$ | $98.29\%$ |
| **12** | $0.5457$ s | $3.141634$ | $11.962\times$ | $0.9969$ | $99.69\%$ |
| **14** | $0.4679$ s | $3.141911$ | $13.951\times$ | $0.9965$ | $99.65\%$ |
| **16** | $0.4361$ s | $3.141362$ | $14.969\times$ | $0.9356$ | $93.56\%$ |
| **18** | $0.3653$ s | $3.141802$ | $17.870\times$ | $0.9928$ | $99.28\%$ |
| **24** | $0.2728$ s | $3.141745$ | $23.929\times$ | $0.9971$ | $99.71\%$ |
| **26** | $0.2521$ s | $3.141638$ | $25.894\times$ | $0.9959$ | $99.59\%$ |
| **28** | **$0.2351$ s** | **$3.141431$** | **$27.766\times$** | **$0.9917$** | **$99.17\%$** |

---

### DATOS PARA GRÁFICO 4: ERROR RELATIVO DE PI CON DISTINTOS PROCESOS

#### Serie $P = 2$ Procesos:
| Muestras ($N$) | $\hat{\pi}$ | Error Relativo (%) | Tiempo (s) |
| :---: | :---: | :---: | :---: |
| **$10^3$** | $3.148000$ | $0.2040\%$ | $0.0001$ |
| **$10^4$** | $3.122800$ | $0.5982\%$ | $0.0004$ |
| **$10^5$** | $3.149080$ | $0.2383\%$ | $0.0033$ |
| **$10^6$** | $3.141640$ | $0.0015\%$ | $0.0329$ |
| **$10^7$** | $3.141820$ | $0.0072\%$ | $0.3259$ |
| **$8 \times 10^7$** | $3.141656$ | $0.0020\%$ | $2.6100$ |

#### Serie $P = 4$ Procesos:
| Muestras ($N$) | $\hat{\pi}$ | Error Relativo (%) | Tiempo (s) |
| :---: | :---: | :---: | :---: |
| **$10^3$** | $3.216000$ | $2.3685\%$ | $0.0001$ |
| **$10^4$** | $3.156400$ | $0.4713\%$ | $0.0002$ |
| **$10^5$** | $3.144640$ | $0.0970\%$ | $0.0017$ |
| **$10^6$** | $3.142284$ | $0.0220\%$ | $0.0165$ |
| **$10^7$** | $3.142126$ | $0.0170\%$ | $0.1739$ |
| **$8 \times 10^7$** | $3.141757$ | $0.0052\%$ | $1.3060$ |

#### Serie $P = 6$ Procesos:
| Muestras ($N$) | $\hat{\pi}$ | Error Relativo (%) | Tiempo (s) |
| :---: | :---: | :---: | :---: |
| **$10^3$** | $3.216867$ | $2.3961\%$ | $0.0000$ |
| **$10^4$** | $3.137255$ | $0.1381\%$ | $0.0002$ |
| **$10^5$** | $3.136765$ | $0.1537\%$ | $0.0012$ |
| **$10^6$** | $3.141093$ | $0.0159\%$ | $0.0112$ |
| **$10^7$** | $3.141735$ | $0.0045\%$ | $0.1092$ |
| **$8 \times 10^7$** | $3.141507$ | $0.0027\%$ | $0.8715$ |

---

## 3. Conclusiones y Dictamen de la Validación

1. **Precisión Numérica Alcanzada:** Con $80$ millones de muestras, el método garantiza de forma estable y reproducible **3 decimales exactos para $\pi$** (`3.141...`, error $\approx 0.002\%$) y **4 decimales exactos para Euler $e$** (`2.7182...`, error $\approx 0.0012\%$).
2. **Escalabilidad Lineal Casi Perfecta:** En el clúster dedicado AMD EPYC de 128 núcleos se redujo el tiempo de cómputo de $6.5279$ segundos a apenas $0.2351$ segundos, alcanzando una aceleración empírica récord de **$27.766\times$ con 28 procesos** y preservando una eficiencia del **$99.17\%$**.
