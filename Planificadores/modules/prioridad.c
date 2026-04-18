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
int compararPorPrioridad(const void* a, const void* b) { // NOLINT
    // Implementar la comparación de procesos por prioridad.
    // Devuelve un valor negativo si `proc_a` tiene mayor prioridad,
    // un valor positivo si `proc_b` tiene mayor prioridad, y 0 si son iguales.
}

void planificarPrioridades(procesos_t* procesos) {
    // Ordena inicialmente los procesos por prioridad original y tiempo de llegada
    
}


// Ajusta las prioridades de los procesos basándose en el envejecimiento
void ajustarPrioridadesPorEnvejecimiento(procesos_t* procesos, context_t* context) {
    assert(procesos_is_valid(procesos));
    assert(context != NULL);

    static const int PRIORITY_TICK = 20;

    int next   = 0;
    Proceso* p = procesos_get(procesos, next);
    for (; p != NULL; p = procesos_get(procesos, ++next)) {
        int diff = (int)context->time - p->tiempoLlegada;
        if (diff <= 0) {
            continue;
        }

        // Cada PRIORITY_TICK ticks del sistema que espera un proceso,
        // aumentamos su prioridad en 1.
        int modulo = diff % PRIORITY_TICK;
        if (p->prioridad != 0 && modulo == 0) {
            p->prioridad--;
        }
    }

    if (context->prevProcess == NO_PROCESS_READY) {
        return;
    }

    // Una vez se reordenen los procesos, tenemos que ajustar el contexto en
    // caso que el proceso que se estaba ejecutando previamente se haya movido,
    // utilizamos su id para esto
    int id = procesos_get(procesos, context->prevProcess)->id;

    // Ajustamos los procesos según las nuevas prioridades
    planificarPrioridades(procesos);

    next = 0;
    p    = procesos_get(procesos, next);
    for (; p != NULL; p = procesos_get(procesos, ++next)) {
        if (p->id == id) {
            context->prevProcess = next;
            break;
        }
    }
}


// OPCIONALMENTE SE PUEDE PROGRAMAR EL ALGORITMO POR PRIORIDADES PREEMTIVO HACIENDO USO DEL AJUSTE DE PRIORIDAD POR ENVEJECIMIENTO


static int nextProcess(procesos_t* procesos, context_t* context) {
    assert(procesos_is_valid(procesos));
    assert(context != NULL);

    ajustarPrioridadesPorEnvejecimiento(procesos, context);

    bool processPending = false;

    int next   = 0;
    Proceso* p = procesos_get(procesos, next);
    for (; p != NULL; p = procesos_get(procesos, ++next)) {
        if (procesos_is_ready(p, context->time)) {
            return next;
        }

        if (p->duracionRafaga != 0) {
            processPending = true;
        }
    }

    if (processPending) {
        return NO_PROCESS_READY;
    }
    return DONE_PROCESSING;
}



void planificarPrioridadesPreemptive(procesos_t* procesos, const char* outputPath) {
    context_t context = CONTEXT_INIT;
    FILE* output      = prepararFicheroSalida(outputPath);

    if (output == NULL) {
        fprintf(stderr, "Fallo al abrir '%s'\n", outputPath);
        return;
    }

    // Orden inicial dado por la función sin prevaciado
    planificarPrioridades(procesos);

    do {
        context.process = nextProcess(procesos, &context);
        // Si no tenemos procesos listos para ejecutar, dejamos pasar un tick.

        // Pasamos de no ejecutar nada a ejecutar algo


        // En este tick continuamos ejecutando el mismo proceso


        // Cambiamos de proceso, guardamos la ejecución del anterior
        Proceso* p  = procesos_get(procesos, context.prevProcess);
        RunFrame rf = {
            .id            = p->id,
            .tiempoLlegada = p->tiempoLlegada,
            .duracion      = context.time - context.prevTime,
            .inicio        = context.prevTime,
            .fin           = context.time,
        };
        guardarFrame(output, &rf);

        context.prevTime    = context.time;
        context.prevProcess = context.process;

    tick:
        context.time++;

    } while (context.process != DONE_PROCESSING);
}
