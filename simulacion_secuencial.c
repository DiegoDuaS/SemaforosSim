#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <omp.h>   
#include <time.h>   


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


void moverCarros(Semaforo *semaforo) {
    // Estado 0 = verde, 1 = amarillo, 2 = rojo
    if (semaforo->estado == 1) { 
        if (semaforo->num_vehiculos > 0) {
            printf("Vehiculo %d paso el semaforo %d\n", 
                   semaforo->cola[0]->id, semaforo->id);

            // Mover la cola
            for (int i = 1; i < semaforo->num_vehiculos; i++) {
                semaforo->cola[i - 1] = semaforo->cola[i];
            }
            semaforo->num_vehiculos--;
        }
    }
}


int main() {
    int num_semaforos = 250;
    int num_vehiculos = 1500;
    clock_t start, end;
    double cpu_time;

    // Crear semáforos
    Interseccion inter;
    inter.num_semaforos = num_semaforos;
    inter.semaforos = malloc(sizeof(Semaforo) * num_semaforos);
    for (int i = 0; i < num_semaforos; i++) {
        inter.semaforos[i].num_vehiculos = 0;
    }

    // Inicializar semáforos
    inicializarSemaforos(&inter);

    // Crear vehículos
    Vehiculo vehiculos[MAX_VEHICULOS];
    crearVehiculos(vehiculos, num_vehiculos);

    // Asignar vehículos a los semáforos (cola simple)
    for (int i = 0; i < num_vehiculos; i++) {
        int sem_id = i % num_semaforos; // repartir vehículos
        int idx = inter.semaforos[sem_id].num_vehiculos;
        inter.semaforos[sem_id].cola[idx] = &vehiculos[i];
        inter.semaforos[sem_id].num_vehiculos++;
    }

    start = clock();

    // Simulación de 10 iteraciones
    for (int iter = 1; iter <= 10; iter++) {
        printf("\n--- Iteracion %d ---\n", iter);

        for (int i = 0; i < num_semaforos; i++) {
            moverCarros(&inter.semaforos[i]);
        }

        cambiarEstados(&inter);

        for (int i = 0; i < num_semaforos; i++) {
            printf("Semaforo %d estado: %d, vehiculos en cola: %d\n",
                   inter.semaforos[i].id,
                   inter.semaforos[i].estado,
                   inter.semaforos[i].num_vehiculos);
        }
 
    }

    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tiempo total de simulacion: %.6f segundos\n", cpu_time);

    free(inter.semaforos);
    return 0;
}

