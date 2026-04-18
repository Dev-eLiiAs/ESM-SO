#include "sjf.h"
#include <stdio.h> // Para fprintf
#include <stdlib.h>

/*
    FUNCIONES A MODIFICAR PARA LA ACTIVIDAD:
    - int compararPorDuracionRafaga(const void* a, const void* b)
    - void planificarSJF(procesos_t* procesos)
*/

int compararPorDuracionRafaga(const void *a, const void *b)
{ // NOLINT
    // Implementar la comparación de procesos por duración de ráfaga
    //  Debe devolver un valor negativo si `proc_a` tiene menor duración,
    //  un valor positivo si `proc_b` tiene menor duración, y 0 si son iguales.
    //  NOTA: si hay empate, por id ascendente (para que el orden sea determinista)

    const Proceso *p1 = (const Proceso *)a;
    const Proceso *p2 = (const Proceso *)b;
    // La rafaga mas corta debe de ir primero
    int diffRafaga = (p1->duracionRafaga > p2->duracionRafaga) - (p1->duracionRafaga < p2->duracionRafaga);

    if (diffRafaga != 0)
    {
        return diffRafaga;
    }

    return ((p1->id > p2->id) - (p1->id < p2->id));
}

void planificarSJF(procesos_t *procesos)
{
    // Validar que la estructura de procesos es válida
    if (!procesos_is_valid(procesos))
    {
        return;
    }
    // Ordenar los procesos por duración de ráfaga usando qsort
    qsort(procesos->ptr, procesos->size, sizeof(Proceso), compararPorDuracionRafaga);
}
