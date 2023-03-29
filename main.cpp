#include <iostream>
#include <cstring>
#include "main.hpp"
using namespace std;

void cargarPedido(char numMesa[])
{
    int codPlato, cant, idCom;
    char nombreArchivo[20] = "MESA";

    strcat(nombreArchivo, numMesa);
    strcat(nombreArchivo, ".dat");
    FILE *f = fopen(nombreArchivo, "ab+");
    if (f == NULL)
    {
        cout << "Error al abrir el archivo." << endl;
    }
    else
    {
        cout << "Ingrese el codigo del plato: " << endl;
        cin >> codPlato;
        cout << "Ingrese la cantidad del mismo: " << endl;
        cin >> cant;
        cout << "Ingrese el ID del comensal: " << endl;
        cin >> idCom;

        Pedido p = inicializarPedido(numMesa, codPlato, cant, idCom);
        escribir<Pedido>(f, p);
        fclose(f);
    }
}

void imprimirCuenta(Nodo<Cliente> *lista, char mesa[])
{
    int totalCom = 0, totMesa = 0;
    cout << "--------------------CUENTA MESA " << mesa << "--------------------" << endl;
    while (lista != NULL)
    {
        Nodo<RPlato> *lPlatos = lista->info.listaRP;
        int idCom = lista->info.idComensal;

        cout << "COMENSAL: " << idCom << endl;
        cout << "--------------------" << endl;
        lPlatos = lPlatos->sgte;
        while (lPlatos != NULL)
        {
            cout << lPlatos->info.pla.nombPlato << "\t" << lPlatos->info.pla.valor << "\t" << lPlatos->info.cant << "\t"
                 << "$ " << lPlatos->info.cant * lPlatos->info.pla.valor << endl;
            cout << "--------------------" << endl;
            totalCom += lPlatos->info.cant * lPlatos->info.pla.valor;
            lPlatos = lPlatos->sgte;
        }
        totMesa += totalCom;
        cout << "TOTAL DEL COMENSAL " << idCom << " : $ " << totalCom << endl;
        cout << "------------------------------------------------------------------" << endl;
        totalCom = 0;
        lista = lista->sgte;
    }
    cout << "TOTAL DE MESA: $ " << totMesa << endl;
    cout << "--------------------" << endl;
}

void estadisticasDelDia(Nodo<Mesa> *&lMesa, int idPlato, int cant, int acum, char mesa[])
{
    Nodo<Estadistica> *lEstadistica = NULL;
    Nodo<Mesa> *nodo = NULL;
    nodo = buscar<Mesa, char[]>(lMesa, mesa, cmpMesa);
    Estadistica es;
    // SI LA MESA NO ESTA EN LA LISTA DE MESAS
    if (nodo == NULL)
    {
        Mesa m = inicializarMesa(mesa);
        m.acumMesa += acum;
        Nodo<Mesa> *nodoMesa = insertarSinRepetir<Mesa>(lMesa, m, cmpEstad);
        // AGREGAMOS ID Y CANT A UNA LISTA DE ESTADISTICAS DE TIPO ESTADISTICA
        lEstadistica = nodoMesa->info.lPedidos;
        es = inicializarEstadistica(cant, idPlato);
        // busca el id Plato en la lista, si no lo encuentra entonces lo agrega ordenado por cantidad descendente
        lEstadistica->info.idPlato = idPlato;
        Nodo<Estadistica> *nodoEstadistica = insertarSinRepetir(lEstadistica, es, cmpIdPlato);
        nodoEstadistica->info.cant += cant;
        nodoEstadistica->info.idPlato = idPlato;
    }
    else // SI LA MESA SE ENCUENTRA EN LA LISTA DE MESAS
    {
        nodo->info.acumMesa += acum;
        // AGREGAMOS ID Y CANT A UNA LISTA DE ESTADISTICAS DE TIPO ESTADISTICA
        lEstadistica = nodo->info.lPedidos;
        es = inicializarEstadistica(cant, idPlato);
        // busca el id Plato en la lista, si no lo encuentra entonces lo agrega ordenado por cantidad descendente
        Nodo<Estadistica> *nodoEstadistica = insertarSinRepetir(lEstadistica, es, cmpIdPlato);
        nodoEstadistica->info.cant += cant;
    }
}

void informesYCierreCaja(Nodo<Mesa> *lMesa, FILE *f)
{
    int acumTOT = 0, cont3 = 0;
    Nodo<Mesa> *aux = lMesa;
    while (aux != NULL)
    {
        acumTOT += aux->info.acumMesa;
        aux = aux->sgte;
    }
    cout << "------------------------------------------------------------------" << endl;
    cout << "TOTAL DEL DIA: $" << acumTOT << endl;
    cout << "------------------------------------------------------------------" << endl;

    while (lMesa != NULL)
    {
        Nodo<Estadistica> *lNueva = NULL;
        Nodo<Estadistica> *aux2 = lMesa->info.lPedidos;
        // ordenamos la nueva lista por cantidad de mayor a menor
        while (aux2 != NULL)
        {
            Estadistica a = pop(aux2);
            insertarOrdenado(lNueva, a, cmpCantidad);
        }
        cout << "Los platos mas vendidos de la mesa: " << lMesa->info.mesa << " son: " << endl;
        cout << "------------------------------------------------------------------" << endl;
        while (lNueva != NULL && cont3 != 3)
        {
            Plato p = buscarPlato(f, lNueva->info.idPlato);
            cout << "-Plato: " << p.nombPlato << " -Cantidad: " << lNueva->info.cant << endl;
            cout << "------------------------------------------------------------------" << endl;
            cont3++;
            lNueva = lNueva->sgte;
        }
        cont3 = 0;
        lMesa = lMesa->sgte;
    }
}

void cerrarMesa(char mesa[], Nodo<Mesa> *&lMesa, Nodo<Cliente> *&lClientes)
{
    Nodo<RPlato> *lPlatos = NULL;
    Nodo<Cliente> *r = NULL;
    char nombreArchivo[20] = "MESA";
    strcat(nombreArchivo, mesa);
    strcat(nombreArchivo, ".dat");

    FILE *fPrecios = fopen("PRECIOS.dat", "rb"); // LEEMOS EL ARCHIVO DE PRECIOS
    FILE *fMesa = fopen(nombreArchivo, "rb");    // ABRIMOS LA MESA QUE QUIERA CERRAR, LEEMOS EL ARCHIVO DE LA MESA

    Pedido pe = leer<Pedido>(fMesa);
    Plato p;
    RPlato rP;

    int idPlatoEstad, cantEstad, acumEstad = 0; // DATOS PARA STATS

    while (!feof(fMesa)) // mientras no haya llegado al final de archivo de la mesa
    {
        p = buscarPlato(fPrecios, pe.idPlato);
        idPlatoEstad = pe.idPlato;
        cantEstad = pe.cant;
        acumEstad += (cantEstad * p.valor);

        int idCom = pe.idComensal;
        r = buscar<Cliente, int>(lClientes, idCom, cmpComensal); // busca al cliente

        if (r == NULL)
        {
            // SI NO ESTA EL CLIENTE LO AGREGO A LA LISTA
            Cliente c = inicializarCliente(idCom);
            Nodo<Cliente> *nodoCliente = insertarOrdenado<Cliente>(lClientes, c, cmpCliente);
            // INSERTA EL PRIMER PLATO AL CLIENTE
            lPlatos = nodoCliente->info.listaRP;
            p = buscarPlato(fPrecios, pe.idPlato);
            rP = convertiraRPlato(p);
            Nodo<RPlato> *nodoPlato = insertarSinRepetir<RPlato>(lPlatos, rP, cmpPlato);
            nodoPlato->info.cant += pe.cant;
        }
        else // si esta el cliente
        {
            // INSERTAMOS LOS PLATOS SIGUIENTES AL PRIMERO AL CLIENTE
            p = buscarPlato(fPrecios, pe.idPlato);
            rP = convertiraRPlato(p);
            //  BUSCA EL PLATO EN LA LISTA PARA SUMAR
            Nodo<RPlato> *nodoPlato = insertarSinRepetir(r->info.listaRP, rP, cmpPlato);
            nodoPlato->info.cant += pe.cant;
        }
        pe = leer<Pedido>(fMesa);
        estadisticasDelDia(lMesa, idPlatoEstad,cantEstad , acumEstad, mesa);
        acumEstad = 0;
    }
    imprimirCuenta(lClientes, mesa); // CUANDO FINALIZO DE PROCESAR LOS DATOS
    lClientes = NULL;
    fclose(fMesa);
    remove(nombreArchivo); // DESTRUYO EL ARCHIVO
}

char menu()
{
    cout << "Ingrese una opcion:" << endl;
    cout << "============================" << endl;
    cout << "1- Cargar nueva mesa" << endl;
    cout << "============================" << endl;
    cout << "2- Cerrar mesa" << endl;
    cout << "============================" << endl;
    cout << "3- Cierre de caja" << endl;
    cout << "============================" << endl;
    cout << "0 - Salir" << endl;
    char opcion;
    do
    {
        opcion = getchar();
    } while ((opcion != '1') && (opcion != '2') && (opcion != '3') && (opcion != '0'));

    return opcion;
}

int main()
{
    Nodo<Mesa> *lMesa = NULL;
    Nodo<Cliente> *lCliente = NULL;
    char numMesa[10];
    char opcion;
    bool repetir = true;
    do
    {
        opcion = menu();
        switch (opcion)
        {
        case '0':
            repetir = false;
            break;
        case '1':
            cout << "===============================" << endl;
            cout << "Ingrese el numero de mesa a cargar: " << endl;
            cin >> numMesa;
            cargarPedido(numMesa);
            cout << "===============================" << endl;
            cout << "Cargando pedido..." << endl;
            cout << "===============================" << endl;
            break;
        case '2':
            cout << "Ingrese la mesa que desea cerrar: " << endl;
            cin >> numMesa;
            cerrarMesa(numMesa, lMesa, lCliente);
            cout << "===============================" << endl;
            cout << "Se cerro la mesa" << endl;
            break;
        case '3':
            cout << "Cierre de caja" << endl;
            FILE *fP = fopen("PRECIOS.dat", "rb");
            informesYCierreCaja(lMesa, fP);
            fclose(fP);
            cout << "===============================" << endl;
            cout << "Se cerro la caja" << endl;
            break;
        }
        getchar();
    } while (repetir);
    return 0;
}