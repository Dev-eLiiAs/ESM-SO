#include "fcfs.h"
#include <stdio.h>  // Para fprintf en caso de error
#include <stdlib.h> // Para qsort

/*
    FUNCIONES A MODIFICAR PARA LA ACTIVIDAD:
    - int compararPorTiempoLlegada(const void* a, const void* b)
    - void planificarFCFS(procesos_t* procesos)

*/

// Función de comparación para ordenar procesos por tiempo de llegada
int compararPorTiempoLlegada(const void *a, const void *b)
{ // NOLINT
    // Implementar la comparación de procesos por su tiempo de llegada
    // Debe devolver un valor negativo si `proc_a` llega antes que `proc_b`,
    // un valor positivo si `proc_b` llega antes, y 0 si son iguales.
    // NOTA: si hay empate, por id ascendente (para que el orden sea determinista)
    const Proceso *p1 = (const Proceso *)a;
    const Proceso *p2 = (const Proceso *)b;
 
    

    int diffTiempo = (p1->tiempoLlegada > p2->tiempoLlegada) - (p1->tiempoLlegada < p2->tiempoLlegada);

    //En caso de que se encuentre una diferencia lo devolvemos directamente

    if (diffTiempo != 0 ){
        return diffTiempo;
    }

    //Si hay empate, es decir, que diffTiempo sea 0, hacemos lo mismo para el ID

    return(p1->id > p2->id) - (p1->id < p2-> id);

}

// Función para planificar procesos usando el algoritmo First-Come, First-Served (FCFS)
void planificarFCFS(procesos_t *procesos)
{
    // Validación de los argumentos y ordenar después por su tiempo de llegada (se puede utilizar qsort)
    if (!procesos_is_valid(procesos))
    {
        return;
    }

    // Ordenar los procesos por tiempo de llegada usando qsort

    qsort(procesos->ptr, procesos->size, sizeof(Proceso), compararPorTiempoLlegada);

    /*
    Después de esta llamada a qsort, los procesos están ordenados según el algoritmo FCFS, listos para ser procesados en
    este ordenS.
    */
}
