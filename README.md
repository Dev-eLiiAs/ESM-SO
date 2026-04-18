# Ejecución del Script `run-all.sh`

## Requisitos

Antes de ejecutar el script, asegúrese de que su sistema cuenta con:

- **Bash** (normalmente instalado en Ubuntu por defecto)
- **Python3** (si el script requiere análisis posterior)
- **Permisos de ejecución** en el planificador
- **Make** (para compilar el planificador con `Makefile`)

## Compilación del planificador

Antes de ejecutar el script, es necesario compilar el planificador utilizando `Makefile`. Desde la ruta `Planificadores/`, ejecute:

```bash
make -f Makefile
```

Esto generará el ejecutable del planificador si no está previamente compilado.

## Instalación y permisos

Si no lo ha hecho aún, asigne permisos de ejecución al script:

```bash
chmod +x Planificadores/scripts/run-all.sh
```

## Ejecución del script

El script debe ejecutarse con `bash`, no con `sh`, para evitar errores como `Illegal option -o pipefail`:

```bash
bash Planificadores/scripts/run-all.sh Planificadores/planificador ./Planificadores/data/ ./output_dir
```

## Argumentos requeridos

El script necesita **tres** argumentos:

1. **Ruta del planificador**: `Planificadores/planificador` (el ejecutable que se encargará de la planificación).
2. **Directorio de entrada**: `./Planificadores/data/` (contiene los archivos `procesos_*.bin`).
3. **Directorio de salida**: `./output_dir` (donde se almacenarán los resultados generados).

Ejemplo de ejecución:

```bash
bash Planificadores/scripts/run-all.sh Planificadores/planificador ./Planificadores/data/ ./output_dir
```

## Verificación del planificador

Si el planificador no tiene permisos de ejecución, habilítelos:

```bash
chmod +x Planificadores/planificador
```

## Depuración de errores

Si el script falla, ejecute con `bash -x` para ver cada paso en detalle:

```bash
bash -x Planificadores/scripts/run-all.sh Planificadores/planificador ./Planificadores/data/ ./output_dir
```

También puede verificar permisos y rutas con:

```bash
ls -l Planificadores/planificador
ls -l ./Planificadores/data/
ls -l ./output_dir
```


