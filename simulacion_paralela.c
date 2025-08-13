#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <omp.h>      

#define MAX_VEHICULOS 20
#define MAX_COLA 100

typedef struct {
    int id;
    char tipo[16];
    int direccion;  
} Vehiculo;

typedef struct {
    int id;
    int estado;       
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
            break;
        case 1: // Verde -> Amarillo
            s->estado = 2;
            printf("Semaforo %d cambia a AMARILLO\n", s->id);
            break;
        case 2: // Amarillo -> Rojo
            s->estado = 0;
            printf("Semaforo %d cambia a ROJO\n", s->id);
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
                inter->frente[dir]++;
            }
        }
    }
}

int main() {

}
