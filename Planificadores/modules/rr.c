#include "proceso.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    FUNCIONES A MODIFICAR PARA LA ACTIVIDAD:
    - void planificarRR(procesos_t* procesos, int quantum, const char* outputPath)
        - Descomentar líneas 70-102 y añadir código donde corresponda dentro de la función.
*/

static int nextProcess(const procesos_t *procesos, const context_t *context)
{
    assert(procesos_is_valid(procesos));
    assert(context != NULL);

    bool processPending = false;

    // Iteramos todos los procesos para encontrar cuál se debe ejecutar.
    int next = context->process + 1;
    const Proceso *p = procesos_get(procesos, next);
    for (; next != context->process; p = procesos_get(procesos, ++next))
    {
        if (p == NULL)
        {
            // Si no se estaba ejecutando ningún proceso, dimos toda la vuelta
            // y no encontramos ningún proceso listo, devolvemos
            // NO_PROCESS_READY
            if (context->process == NO_PROCESS_READY)
            {
                return NO_PROCESS_READY;
            }

            next = 0;
            p = procesos_get(procesos, next);
        }

        if (procesos_is_ready(p, context->time))
        {
            return next;
        }

        if (p->duracionRafaga != 0)
        {
            processPending = true;
        }
    }

    // Llegado este punto estamos en el mismo proceso inicial,
    // si está listo para ejecutar, lo ejecutamos.
    if (procesos_is_ready(procesos_get(procesos, next), context->time))
    {
        return context->process;
    }

    if (processPending)
    {
        // No hemos terminado de ejecutar procesos, pero no tenemos procesos preparados
        return NO_PROCESS_READY;
    }
    // Revisamos todos los procesos y no queda nada por procesar
    return DONE_PROCESSING;
}

void planificarRR(procesos_t *procesos, int quantum, const char *outputPath)
{
    // Inicialización del contexto de ejecución
    context_t context = CONTEXT_INIT;
    FILE *output = prepararFicheroSalida(outputPath);

    // Verifica si el archivo de salida se abrió correctamente
    if (output == NULL)
    {
        return;
    }

    // Bucle principal de planificación
    while (true)
    {
        // Determina el siguiente proceso a ejecutar según la política Round Robin
        context.process = nextProcess(procesos, &context);

        // Si no tenemos procesos listos para ejecutar, dejamos pasar un tick.
        if (context.process == NO_PROCESS_READY)
        {
            context.time++;
            continue;
        }
        // Si todos los procesos han terminado, finaliza la planificación

        if (context.process == DONE_PROCESSING)
        {
            break;
        }

        // Obtiene el proceso actual de la lista de procesos

        Proceso *p = procesos_get(procesos, context.process);
        unsigned int startTime = context.time;

        // Verifica si el proceso puede ejecutarse completamente en el quantum asignado
        if (p->duracionRafaga <= quantum)
        {
            // Si la ráfaga restante es menor que el quantum, el proceso termina su ejecución
            context.time += p->duracionRafaga; // Avanzamos reloj
            p->duracionRafaga = 0;             // El proceso no necesita mas CPU
        }
        else
        {
            // Si no, ejecuta el proceso por un quantum y reduce su ráfaga restante
            context.time += quantum;      // Avanzamos el reloj lo que mide el quatum
            p->duracionRafaga -= quantum; // Desconectamos la rafaga que se ejecuta
        }
        // Guarda los datos de la ejecución en un frame de ejecución
        RunFrame rf = {
            .id = p->id,
            .tiempoLlegada = p->tiempoLlegada,
            .duracion = context.time - startTime,
            .inicio = startTime,
            .fin = context.time,
        };

        // Almacena la información en el archivo de salida
        guardarFrame(output, &rf);
    }

    fclose(output);
}
