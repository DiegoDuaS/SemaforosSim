#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        // Configuración inicial:
        // Semáforos N y S verdes, E y O rojos
        if (i == 0 || i == 2) { // N y S
            inter->semaforos[i].estado = 1; // verde
        } else {
            inter->semaforos[i].estado = 0; // rojo
        }
        inter->semaforos[i].tiempo_verde = 3;  
        inter->semaforos[i].tiempo_rojo = 3;
         inter->semaforos[i].tiempo_amarillo = 1;
    }
}

void crearVehiculos(Vehiculo vehiculos[], int num) {
    for (int i = 0; i < num; i++) {
        vehiculos[i].id = i+1;
        strcpy(vehiculos[i].tipo, "Auto"); // Por simplicidad
        vehiculos[i].direccion = i % 4;    // Alterna dirección N,E,S,O
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
    for (int dir = 0; dir < 4; dir++) {
        Semaforo *s = &inter->semaforos[dir];
        if (s->estado == 1) { // semáforo verde
            if (inter->frente[dir] < inter->fin[dir]) { // hay vehículos esperando
                Vehiculo *v = inter->cola[dir][inter->frente[dir]];
                printf("Vehiculo %d (dir %d) avanza porque semaforo %d esta VERDE\n",
                    v->id, dir, s->id);
                inter->frente[dir]++;  // vehículo avanza (sale de la cola)
            }
        }
    }
}


int quedanVehiculos(Interseccion *inter) {
    for (int i = 0; i < 4; i++) {
        if (inter->frente[i] < inter->fin[i]) {
            return 1; // Aún quedan vehículos
        }
    }
    return 0; // Ya no quedan vehículos
}

void imprimirEstadoInicial(Vehiculo vehiculos[], int num) {
    printf("=== ESTADO INICIAL DE LOS VEHICULOS ===\n");
    for (int i = 0; i < num; i++) {
        printf("Vehiculo %d -> Direccion: %d\n",
               vehiculos[i].id, vehiculos[i].direccion);
    }
    printf("=======================================\n\n");
}

int main() {
    Interseccion inter;
    Vehiculo vehiculos[MAX_VEHICULOS];

    inicializarSemaforos(&inter);
    inicializarColas(&inter);
    crearVehiculos(vehiculos, MAX_VEHICULOS);

    imprimirEstadoInicial(vehiculos, MAX_VEHICULOS);

    for (int i = 0; i < MAX_VEHICULOS; i++) {
        int dir = vehiculos[i].direccion;
        inter.cola[dir][inter.fin[dir]] = &vehiculos[i];
        inter.fin[dir]++;
    }

    int ciclo = 0;
    while (quedanVehiculos(&inter) && ciclo < 10) { // max 10 ciclos para evitar loops infinitos
        ciclo++;
        printf("\n--- Ciclo %d ---\n", ciclo);

        // Cambiar estado de semáforos secuencialmente
        for (int i = 0; i < 4; i++) {
            cambiarEstado(&inter.semaforos[i]);
        }

        // Mover vehículos que puedan avanzar
        moverVehiculos(&inter);
    }

    printf("\nSimulación terminada. Todos los vehículos han avanzado o ciclo límite alcanzado.\n");

    return 0;
}