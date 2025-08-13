#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // sleep
#include <omp.h>      // OpenMP

#define MAX_VEHICULOS 20
#define MAX_COLA 100

typedef struct {
    int id;
    char tipo[16];
    int direccion;  // 0=N,1=E,2=S,3=O
} Vehiculo;

typedef struct {
    int id;
    int estado;       // 0=rojo, 1=verde, 2=amarillo
    int tiempo_verde;
    int tiempo_amarillo;
    int tiempo_rojo;
} Semaforo;

typedef struct {
    Semaforo semaforos[4];
    Vehiculo *cola[4][MAX_COLA];
    int frente[4];
    int fin[4];
} Interseccion;

void inicializarSemaforos(Interseccion *inter) {
    for (int i = 0; i < 4; i++) {
        inter->semaforos[i].id = i;
        if (i == 0 || i == 2) { // N y S verdes
            inter->semaforos[i].estado = 1;
        } else { // E y O rojos
            inter->semaforos[i].estado = 0;
        }
        inter->semaforos[i].tiempo_verde = 10;
        inter->semaforos[i].tiempo_amarillo = 3;
        inter->semaforos[i].tiempo_rojo = 10;
    }
}

void inicializarColas(Interseccion *inter) {
    for (int i = 0; i < 4; i++) {
        inter->frente[i] = 0;
        inter->fin[i] = 0;
        for (int j = 0; j < MAX_COLA; j++) {
            inter->cola[i][j] = NULL;
        }
    }
}

void crearVehiculos(Vehiculo vehiculos[], int num) {
    for (int i = 0; i < num; i++) {
        vehiculos[i].id = i + 1;
        strcpy(vehiculos[i].tipo, "Auto");
        vehiculos[i].direccion = i % 4;
    }
}

void cambiarEstado(Semaforo *s) {
    switch(s->estado) {
        case 0: // Rojo -> Verde
            s->estado = 1;
            printf("Semaforo %d cambia a VERDE\n", s->id);
            sleep(s->tiempo_verde);
            break;
        case 1: // Verde -> Amarillo
            s->estado = 2;
            printf("Semaforo %d cambia a AMARILLO\n", s->id);
            sleep(s->tiempo_amarillo);
            break;
        case 2: // Amarillo -> Rojo
            s->estado = 0;
            printf("Semaforo %d cambia a ROJO\n", s->id);
            sleep(s->tiempo_rojo);
            break;
        default:
            break;
    }
}

void moverVehiculos(Interseccion *inter) {
    #pragma omp parallel for
    for (int dir = 0; dir < 4; dir++) {
        Semaforo *s = &inter->semaforos[dir];
        if (s->estado == 1) { // verde
            if (inter->frente[dir] < inter->fin[dir]) {
                Vehiculo *v = inter->cola[dir][inter->frente[dir]];
                printf("Vehiculo %d (dir %d) avanza porque semaforo %d esta VERDE\n",
                       v->id, dir, s->id);
                // Aquí incrementamos frente; acceso concurrente en hilos distintos no afecta
                inter->frente[dir]++;
            }
        }
    }
}

int quedanVehiculos(Interseccion *inter) {
    for (int i = 0; i < 4; i++) {
        if (inter->frente[i] < inter->fin[i]) {
            return 1;
        }
    }
    return 0;
}

int main() {
    Interseccion inter;
    Vehiculo vehiculos[MAX_VEHICULOS];

    inicializarSemaforos(&inter);
    inicializarColas(&inter);
    crearVehiculos(vehiculos, MAX_VEHICULOS);

    for (int i = 0; i < MAX_VEHICULOS; i++) {
        int dir = vehiculos[i].direccion;
        inter.cola[dir][inter.fin[dir]] = &vehiculos[i];
        inter.fin[dir]++;
    }

    int ciclo = 0;
    while (quedanVehiculos(&inter) && ciclo < 10) {
        ciclo++;
        printf("\n--- Ciclo %d ---\n", ciclo);

        #pragma omp parallel for
        for (int i = 0; i < 4; i++) {
            cambiarEstado(&inter.semaforos[i]);
        }

        moverVehiculos(&inter);
    }

    printf("\nSimulación terminada. Todos los vehículos han avanzado o ciclo límite alcanzado.\n");
    return 0;
}
