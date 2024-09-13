#!/bin/bash

# Define el número de repeticiones
REPS=100

# Ejecuta el comando 100 veces
for i in $(seq 1 $REPS); do
    echo "Ejecución $i de $REPS"

    # Ejecuta el comando y captura su salida
    if ! ./md5 * | ./view; then
        echo "Error encontrado en la ejecución $i. Saliendo..."
        exit 1
    fi
done

echo "Todas las ejecuciones fueron exitosas."
exit