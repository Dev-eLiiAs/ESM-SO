#include "prioridad.h"
#include "proceso.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // Para qsort
#include <string.h>

/*
    FUNCIONES A MODIFICAR PARA LA ACTIVIDAD:
    - int compararPorPrioridad(const void* a, const void* b)
    - void planificarPrioridades(procesos_t* procesos)
    - [OPCIONAL]
        -void planificarPrioridadesPreemptive(procesos_t* procesos, const char* outputPath)
*/

// Función de comparación inicial por prioridad y tiempo de llegada
int compararPorPrioridad(const void *a, const void *b)
{ // NOLINT
    // Implementar la comparación de procesos por prioridad.
    // Devuelve un valor negativo si `proc_a` tiene mayor prioridad,
    // un valor positivo si `proc_b` tiene mayor prioridad, y 0 si son iguales.

    const Proceso *p1 = (const Proceso *)a;
    const Proceso *p2 = (const Proceso *)b;

    // Comparamos prioridad, si el numero es mas bajo tiene mas prioridad
    int diffPrioridad = (p1->prioridad > p2->prioridad) - (p1->prioridad < p2->prioridad);

    if (diffPrioridad != 0)
    {
        return diffPrioridad;
    }

    // En caso de que tengan misma prioridad gana el que llega primero
    int diffLlegada = (p1->tiempoLlegada > p2->tiempoLlegada) - (p1->tiempoLlegada < p2->tiempoLlegada);

    if (diffLlegada != 0)
    {
        return diffLlegada;
    }

    // Y si llegan a la vez gana el que tenga el ID menor
    return ((p1->id > p2->id) - (p1->id < p2->id));
}

void planificarPrioridades(procesos_t *procesos)
{
    // Primero valido si hay datos
    if (!procesos_is_valid(procesos))
    {
        return;
    }
    // Ordena inicialmente los procesos por prioridad original y tiempo de llegada
    qsort(procesos->ptr, procesos->size, sizeof(Proceso), compararPorPrioridad);
}

// Ajusta las prioridades de los procesos basándose en el envejecimiento
void ajustarPrioridadesPorEnvejecimiento(procesos_t *procesos, context_t *context)
{
    assert(procesos_is_valid(procesos));
    assert(context != NULL);

    static const int PRIORITY_TICK = 20;

    int next = 0;
    Proceso *p = procesos_get(procesos, next);
    for (; p != NULL; p = procesos_get(procesos, ++next))
    {
        int diff = (int)context->time - p->tiempoLlegada;
        if (diff <= 0)
        {
            continue;
        }

        // Cada PRIORITY_TICK ticks del sistema que espera un proceso,
        // aumentamos su prioridad en 1.
        int modulo = diff % PRIORITY_TICK;
        if (p->prioridad != 0 && modulo == 0)
        {
            p->prioridad--;
        }
    }

    if (context->prevProcess == NO_PROCESS_READY)
    {
        return;
    }

    // Una vez se reordenen los procesos, tenemos que ajustar el contexto en
    // caso que el proceso que se estaba ejecutando previamente se haya movido,
    // utilizamos su id para esto
    int id = procesos_get(procesos, context->prevProcess)->id;

    // Ajustamos los procesos según las nuevas prioridades
    planificarPrioridades(procesos);

    next = 0;
    p = procesos_get(procesos, next);
    for (; p != NULL; p = procesos_get(procesos, ++next))
    {
        if (p->id == id)
        {
            context->prevProcess = next;
            break;
        }
    }
}

// OPCIONALMENTE SE PUEDE PROGRAMAR EL ALGORITMO POR PRIORIDADES PREEMTIVO HACIENDO USO DEL AJUSTE DE PRIORIDAD POR ENVEJECIMIENTO

static int nextProcess(procesos_t *procesos, context_t *context)
{
    assert(procesos_is_valid(procesos));
    assert(context != NULL);

    ajustarPrioridadesPorEnvejecimiento(procesos, context);

    bool processPending = false;

    int next = 0;
    Proceso *p = procesos_get(procesos, next);
    for (; p != NULL; p = procesos_get(procesos, ++next))
    {
        if (procesos_is_ready(p, context->time))
        {
            return next;
        }

        if (p->duracionRafaga != 0)
        {
            processPending = true;
        }
    }

    if (processPending)
    {
        return NO_PROCESS_READY;
    }
    return DONE_PROCESSING;
}

void planificarPrioridadesPreemptive(procesos_t *procesos, const char *outputPath)
{
    context_t context = CONTEXT_INIT;
    FILE *output = prepararFicheroSalida(outputPath);

    if (output == NULL)
    {
        fprintf(stderr, "Fallo al abrir '%s'\n", outputPath);
        return;
    }

    // Orden inicial dado por la función sin prevaciado
    planificarPrioridades(procesos);

    do
    {
        context.process = nextProcess(procesos, &context);
        // Si no tenemos procesos listos para ejecutar, dejamos pasar un tick.
        if (context.process == NO_PROCESS_READY && context.prevProcess == NO_PROCESS_READY)
        {
            goto tick;
        }
        if (context.prevProcess == DONE_PROCESSING && context.prevProcess == NO_PROCESS_READY)
        {
            break;
        }
        //Pasamos de no ejecutar nada a ejecutar algo
        if (context.prevProcess == NO_PROCESS_READY && context.process >= 0)
        {
            context.prevProcess = context.process;
            context.prevTime = context.time;
        }
         
        //En este tick continuamos ejecutando el mismo proceso

        if (context.process == context.prevProcess && context.process != DONE_PROCESSING)
        {
            Proceso* pActual = procesos_get(procesos,context.process);
            pActual->duracionRafaga--;
            goto tick;
        }


        // Cambiamos de proceso, guardamos la ejecución del anterior
        Proceso *p = procesos_get(procesos, context.prevProcess);
        RunFrame rf = {
            .id = p->id,
            .tiempoLlegada = p->tiempoLlegada,
            .duracion = context.time - context.prevTime,
            .inicio = context.prevTime,
            .fin = context.time,
        };
        guardarFrame(output, &rf);
        // Procedemos a actualizar el contexto del nuevo proceso
        context.prevTime = context.time;
        context.prevProcess = context.process;

        // El nuevo proceso se ejecuta en este tick, le quitamos su rafaga
        if (context.process != DONE_PROCESSING)
        {
            Proceso *pNuevo = procesos_get(procesos, context.process);
            pNuevo->duracionRafaga--;
        }
    tick:
        context.time++;

    } while (context.process != DONE_PROCESSING);
    fclose(output);
}
