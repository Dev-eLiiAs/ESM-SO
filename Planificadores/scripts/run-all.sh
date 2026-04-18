#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

if [[ $# -lt 3 ]]; then
    echo "Uso: $0 <planificador> <input_dir> <output_dir>"
    exit 1
fi

PLANIFICADOR=$1
INPUT_DIR=$2
OUTPUT_DIR=$3

ALGORITMOS=(
    "fcfs"
    "sjf"
    "prioridad"
    "prioridad_preemptive"
    "rr"
)

mkdir -p "${OUTPUT_DIR}"
LOG_FILE="${OUTPUT_DIR}/run_all.log"
echo "Ejecución iniciada: $(date)" > "$LOG_FILE"

while IFS="" read -r -d $'\0'; do
    input="$REPLY"
    input_type="$(basename "${input}")"
    input_type="${input_type%.bin}"
    input_type="${input_type#"procesos_"}"

    mkdir -p "${OUTPUT_DIR}/${input_type}"
    output_dir="$(realpath "${OUTPUT_DIR}")"

    for algoritmo in "${ALGORITMOS[@]}"; do
        extra_args=()
        if [[ "$algoritmo" == "prioridad_preemptive" ]]; then
            extra_args+=("-p")
        fi

        # Verificar si el planificador está implementado antes de ejecutarlo
        if [[ "$algoritmo" == "prioridad_preemptive" && ! -x "$PLANIFICADOR" ]]; then
            echo "El planificador no está completamente implementado para ${algoritmo}. Se omite." | tee -a "$LOG_FILE"
            continue
        fi

        echo "ejecutando planificador con algoritmo ${algoritmo}" | tee -a "$LOG_FILE"
        {
            "${PLANIFICADOR}" \
            -o "${output_dir}/${input_type}" \
            "${extra_args[@]}" \
            "${algoritmo%'_preemptive'}" \
            "${input}"
        } || {
            echo "Error ejecutando ${algoritmo} en ${input_type}. Se registra en el log." | tee -a "$LOG_FILE"
            continue
        }
        

        mkdir -p "${output_dir}/${input_type}/${algoritmo}"

        echo "Generando métricas y diagrama de Gantt" | tee -a "$LOG_FILE"
        (
            cd "$SCRIPT_DIR"
            python3 -m "analysis.run" \
                "${output_dir}/${input_type}/${algoritmo}_procesos.csv" \
                "${algoritmo}" \
                "${input_type}" \
                -o "${output_dir}"
        ) || {
            echo "Error generando métricas para ${algoritmo}. Se registra en el log." | tee -a "$LOG_FILE"
            continue
        }
    done
done < <(find "$INPUT_DIR" -name 'procesos_*.bin' -type f -print0)

# Generar gráficos de métricas
python3 Planificadores/scripts/plot-metrics.py "${output_dir}/metrics.jsonl" -o "${output_dir}/graficos" || {
echo "Error generando gráficos de métricas. Se registra en el log." | tee -a "$LOG_FILE"
}
