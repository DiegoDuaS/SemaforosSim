#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <omp.h>      

#define MAX_VEHICULOS 15000
#define MAX_COLA 100

// Estados del semáforo
#define ROJO 0
#define VERDE 1
#define AMARILLO 2

typedef struct {
    int id;
    int direccion;   
} Vehiculo;

typedef struct {
    int id;
    int estado;       
    Vehiculo *cola[MAX_COLA];  
    int num_vehiculos;
} Semaforo;

typedef struct {
    Semaforo *semaforos; 
    int num_semaforos;  
} Interseccion;

void inicializarSemaforos(Interseccion *inter) {
    for (int i = 0; i < inter->num_semaforos; i++) {
        inter->semaforos[i].id = i;

        // Ejemplo: pares en verde, impares en rojo al inicio
        if (i % 2 == 0) {
            inter->semaforos[i].estado = 1; // verde
        } else {
            inter->semaforos[i].estado = 0; // rojo
        }
    }
}

void crearVehiculos(Vehiculo vehiculos[], int num) {
    for (int i = 0; i < num; i++) {
        vehiculos[i].id = i + 1;       
        vehiculos[i].direccion = i % 4;
    }
}

void cambiarEstados(Interseccion *inter) {
    for (int i = 0; i < inter->num_semaforos; i++) {
        if (inter->semaforos[i].estado == 1) {
            inter->semaforos[i].estado = 2; // de verde a amarillo
        } else if (inter->semaforos[i].estado == 2) {
            inter->semaforos[i].estado = 0; // de amarillo a rojo
        } else {
            inter->semaforos[i].estado = 1; // de rojo a verde
        }
    }
}


void moverCarros(Semaforo *semaforo, char *buffer) {
    if (semaforo->estado == VERDE && semaforo->num_vehiculos > 0) {
        sprintf(buffer, "Vehiculo %d paso el semaforo %d\n",
                semaforo->cola[0]->id,
                semaforo->id);

        // Mover la cola
        for (int i = 1; i < semaforo->num_vehiculos; i++)
            semaforo->cola[i-1] = semaforo->cola[i];
        semaforo->num_vehiculos--;
    } else {
        buffer[0] = '\0'; // No hay mensaje
    }
}

// Función para imprimir buffer de todos los semáforos
void imprimirBuffer(char **buffers, int num_semaforos) {
    for (int i = 0; i < num_semaforos; i++) {
        if (buffers[i][0] != '\0')
            printf("%s", buffers[i]);
    }
}


int main() {
    int num_semaforos = 250;
    int num_vehiculos = 1500;
    omp_set_dynamic(1);
    omp_set_num_threads(12);

    Interseccion inter;
    inter.num_semaforos = num_semaforos;
    inter.semaforos = malloc(sizeof(Semaforo) * num_semaforos);
    for (int i = 0; i < num_semaforos; i++)
        inter.semaforos[i].num_vehiculos = 0;

    inicializarSemaforos(&inter);

    Vehiculo vehiculos[MAX_VEHICULOS];
    crearVehiculos(vehiculos, num_vehiculos);

    for (int i = 0; i < num_vehiculos; i++) {
        int sem_id = i % num_semaforos;
        int idx = inter.semaforos[sem_id].num_vehiculos;
        inter.semaforos[sem_id].cola[idx] = &vehiculos[i];
        inter.semaforos[sem_id].num_vehiculos++;
    }

    char **buffers = malloc(sizeof(char*) * num_semaforos);
    for (int i = 0; i < num_semaforos; i++)
        buffers[i] = malloc(128);

    double start_time = omp_get_wtime();

    for (int iter = 1; iter <= 10; iter++) {
        printf("\n--- Iteracion %d ---\n", iter);

        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < num_semaforos; i++) {
            moverCarros(&inter.semaforos[i], buffers[i]);
        }

        cambiarEstados(&inter);

        imprimirBuffer(buffers, num_semaforos);
    }

    double end_time = omp_get_wtime();
    printf("\nTiempo total de simulacion: %.6f segundos\n", end_time - start_time);

    free(inter.semaforos);
    return 0;
}

