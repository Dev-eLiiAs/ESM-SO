#include "sjf.h"
#include <stdio.h> // Para fprintf
#include <stdlib.h>

/* 
    FUNCIONES A MODIFICAR PARA LA ACTIVIDAD:
    - int compararPorDuracionRafaga(const void* a, const void* b)
    - void planificarSJF(procesos_t* procesos)
*/


int compararPorDuracionRafaga(const void* a, const void* b) { // NOLINT
    //Implementar la comparación de procesos por duración de ráfaga
    // Debe devolver un valor negativo si `proc_a` tiene menor duración,
    // un valor positivo si `proc_b` tiene menor duración, y 0 si son iguales.
    // NOTA: si hay empate, por id ascendente (para que el orden sea determinista)
}

void planificarSJF(procesos_t* procesos) {
    //Validar que la estructura de procesos es válida
    
    //Ordenar los procesos por duración de ráfaga usando qsort
    
    
}
