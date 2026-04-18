import argparse
import os

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


INPUT_TYPES = [
    'sys',
    'rand',
    'graduales',
    'rafagas',
    'gauss',
]

METRICS = [
    ('tiempo_espera_promedio', 'Tiempo Espera Promedio'),
    ('tiempo_retorno_promedio', 'Tiempo Retorno Promedio'),
    ('tiempo_respuesta_promedio', 'Tiempo Respuesta Promedio'),
    ('var_tiempo_espera', 'Varianza Tiempo de Espera'),
    ('max_tiempo_espera', 'Máximo Tiempo de Espera'),
    ('utilizacion_cpu', 'Utilización CPU'),
    ('rendimiento', 'Rendimiento'),
]


def plot_results(df, field, title, output):

    if output is not None:
        os.makedirs(output, exist_ok=True)  # Crea la carpeta si no existe

    #x = np.arange(len(INPUT_TYPES))  # the label locations
    available_input_types = df['input_type'].unique()  # Detectar qué tipos de entrada hay en los datos
    #x = np.arange(len(available_input_types))  # Ajustar a la cantidad real de datos

    width = 0.15  # the width of the bars
    multiplier = 0

    fig, ax = plt.subplots(layout='constrained')

    #for i in INPUT_TYPES:
    for i in available_input_types:
        offset = width * multiplier
        df_filtered = df[df['input_type'] == i]

        if df_filtered.empty:
            continue  # Saltar si no hay datos para este tipo

        x = np.arange(len(df_filtered))  # Ajustar el tamaño del eje X

        print(f"Procesando input_type={i} con {len(df_filtered)} datos")  # Depuración

        rects = ax.bar(
            x + offset,
            df_filtered[field],
            width,
            label=i
        )
        ax.bar_label(
            rects,
            padding=-35,
            labels=[f'{i:.2f}' for i in df_filtered[field]],
            rotation='vertical'
        )
        multiplier += 1

    ax.set_title(title)
    ax.set_xticks(x + width, df[df['input_type'] ==
                  'sys']['algorithm'], rotation=30)
    # Shrink current axis by 20%
    box = ax.get_position()
    ax.set_position([box.x0, box.y0, box.width * 0.8, box.height])

    # Put a legend to the right of the current axis
    ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))

    if output is None:
        plt.show()
    else:
        plt.savefig(os.path.join(output, f'{field}.png'))


def main(input, output):
    df = pd.read_json(input, orient='records', lines=True)
    for field, title in METRICS:
        plot_results(df, field, title, output)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', type=str, help='Archivo jsonl con resultados')
    parser.add_argument(
        '--output', '-o', type=str, default=None,
        help='Archivo de salida'
    )

    args = parser.parse_args()

    main(args.input, args.output)
