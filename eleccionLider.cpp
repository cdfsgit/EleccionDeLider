/*
Fernando Núñez, Edgar Yépez
*/

#include <mpi.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

using namespace std;

struct nodo{
    int id;
    int nombre;
};

int calculoId(){
    return rand() % 1000;
}

void enviar(int destinatario, int *datos, int tamano){
    MPI::COMM_WORLD.Send(datos, tamano, MPI::INT, destinatario, 1);
}

int * recibir(int origen){
    MPI::Status status;
    MPI::COMM_WORLD.Probe(origen, 1, status);
    int l = status.Get_count(MPI::INT);
    int *buf = new int[l];
    MPI::COMM_WORLD.Recv(buf, l, MPI::INT, origen, 1, status);
    return buf;
}

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    srand(time(NULL));
    int p, r;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &r);

    struct nodo nod;

    nod.nombre = r;
    int i;
    for(int i=-1; i<r; i++ ){
        nod.id = calculoId();
    }

    printf("Nodo %d: %d\n", nod.nombre, nod.id);

    int d[2];
    if (r == 0){
        d[0] = nod.nombre;
        d[1] = nod.id;
        enviar(1, d, 2);
    } else if (r >= 1 && r <= p - 2) {
        int *datos = recibir(nod.nombre - 1);
        if (nod.id > datos[1]){
            d[0] = nod.nombre;
            d[1] = nod.id;
            enviar(nod.nombre + 1, d, 2);
        }
        else enviar(nod.nombre + 1, datos, 2);
    } else {
        int *datos = recibir(nod.nombre - 1);
        if (nod.id > datos[1]){
            d[0] = nod.nombre;
            d[1] = nod.id;
            enviar(0, d, 2);
        }
        else enviar(0, datos, 2);
    }

    int *datosLider;
    if (nod.nombre == 0) datosLider = recibir(p - 1);
    else datosLider = recibir(nod.nombre - 1);
    if (nod.nombre == datosLider[0]) printf("Nodo %d: Yo soy el lider\n", nod.nombre);
    else printf("Nodo %d: El lider es el nodo %d\n", nod.nombre, datosLider[0]);
    if (r != p - 1) enviar(nod.nombre + 1, datosLider, 2);

    MPI_Finalize();
    return 0;
}

