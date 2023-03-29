#include <iostream>
#include <cstring>
using namespace std;

// STRUCTS

template <typename T>
struct Nodo
{
    T info;
    Nodo<T> *sgte;
};

struct Plato // ESTRUCTURA PARA EL ARCHIVO DE PEDIDOS
{
    char nombPlato[100 + 1];
    int valor;
    int idPlato;
};

struct Pedido // STRUCT DEL PEDIDO
{
    char mesa[10];
    int cant;
    int idPlato;
    int idComensal;
};

struct RPlato // ESTRUCTURA PARA LA LISTA DE PLATOS
{
    Plato pla;
    int cant;
};

struct Cliente // ESTRUCTURA PARA LA LISTA DE CLIENTES
{
    int idComensal;
    Nodo<RPlato> *listaRP;
};

struct Estadistica
{
    int idPlato;
    int cant;
};

struct Mesa
{
    char mesa[10];
    int acumMesa;
    Nodo<Estadistica> *lPedidos;
};

// FUNCIONES DE ARCHIVOS

template <typename T>
T leer(FILE *f)
{
    T r;
    fseek(f, 0, SEEK_CUR);
    fread(&r, sizeof(T), 1, f);
    return r;
}

template <typename T>
void escribir(FILE *f, T r)
{
    fseek(f, 0, SEEK_CUR);
    fwrite(&r, sizeof(T), 1, f);
}

template <typename T>
int fileSize(FILE *f)
{
    fseek(f, 0, SEEK_SET);
    return ftell(f) / sizeof(T);
}

// FUNCIONES DE CMP

int cmpComensal(Cliente a, int idCom)
{
    return a.idComensal - idCom;
}

int cmpCliente(Cliente c, Cliente a)
{
    return c.idComensal - a.idComensal;
}

int cmpPlato(RPlato a, RPlato b)
{
    return a.pla.idPlato - b.pla.idPlato;
}

int cmpID(Plato a, Plato b)
{
    return a.idPlato - b.idPlato;
}

int cmpMesa(Mesa a, char b[])
{
    return strcmp(a.mesa, b);
}

int cmpEstad(Mesa a, Mesa b)
{
    return strcmp(a.mesa, b.mesa);
}

int cmpCantidad(Estadistica a, Estadistica b)
{
    return b.cant - a.cant;
}

int cmpIdPlato(Estadistica b, Estadistica a)
{
    return b.idPlato - a.idPlato;
}

// FUNCIONES DE LISTAS ENLAZADAS

template <typename T>
T pop(Nodo<T> *&p)
{
    Nodo<T> *aux = p;
    T v = aux->info;
    p = aux->sgte;
    delete aux;
    return v;
}

template <typename T, typename K>
Nodo<T> *buscar(Nodo<T> *lista, K k, int cmpTK(T, K))
{
    Nodo<T> *aux = lista;
    while (aux != NULL && cmpTK(aux->info, k) != 0)
    {
        aux = aux->sgte;
    }
    return aux;
}

template <typename T>
Nodo<T> *insertarAlFinal(Nodo<T> *&l, T t)
{
    Nodo<T> *nuevo = new Nodo<T>();
    nuevo->info = t;
    nuevo->sgte = NULL;
    if (l == NULL)
    {
        l = nuevo;
    }
    else
    {
        Nodo<T> *aux = l;
        while (aux->sgte != NULL)
        {
            aux = aux->sgte;
        }
        aux->sgte = nuevo;
    }
    return nuevo;
}

template <typename T, typename K>
/* Nodo<T> *insertarOrdenado(Nodo<T> *&l, K t, int cmp(T, K))
{
    Nodo<T> *nuevo = new Nodo<T>();
    nuevo->info = t;
    nuevo->sgte = NULL;

    if (l == NULL || cmp(l->info, t) > 0)
    {
        l = nuevo;
        return nuevo;
    }
    Nodo<T> *aux = l;
    while (aux->sgte != NULL && cmp(aux->sgte->info, t) < 0)
    {
        aux = aux->sgte;
    }
    nuevo->sgte = aux->sgte;
    aux->sgte = nuevo;
    return nuevo;
} */

Nodo<T> *insertarOrdenado(Nodo<T> *&l, K t, int cmp(T, K))
{
    Nodo<T> *nuevo = new Nodo<T>();
    nuevo->info = t;
    nuevo->sgte = NULL;
    Nodo<T> *actual = l;
    Nodo<T> *ant = NULL;
    while (actual != NULL && cmp(actual->info, t) <= 0)
    {
        ant = actual;
        actual = actual->sgte;
    }
    if (ant == NULL)
    {
        l = nuevo;
    }
    else
    {
        ant->sgte = nuevo;
    }
    nuevo->sgte = actual;
    return nuevo;
}

template <typename T>
Nodo<T> *insertarSinRepetir(Nodo<T> *&l, T t, int cmp(T, T))
{
    Nodo<T> *x = buscar<T, T>(l, t, cmp);

    if (x == NULL)
    {
        x = insertarOrdenado<T>(l, t, cmp);
    }
    return x;
}

// INICIALIZACIONES

Pedido inicializarPedido(char numMesa[], int codPlato, int cant, int idCom)
{
    Pedido ret;
    ret.cant = 0;
    ret.idComensal = 0;
    strcpy(ret.mesa, numMesa);
    ret.idPlato = codPlato;
    ret.cant = cant;
    ret.idComensal = idCom;
    return ret;
}

Cliente inicializarCliente(int idComensal)
{
    Cliente ret;
    ret.idComensal = 0;
    ret.listaRP = new Nodo<RPlato>();
    ret.listaRP->info.cant = 0;
    ret.idComensal = idComensal;
    return ret;
};

Mesa inicializarMesa(char mesa[])
{
    Mesa ret;
    ret.acumMesa = 0;
    strcpy(ret.mesa, mesa);
    ret.lPedidos = new Nodo<Estadistica>();
    ret.lPedidos->info.cant = 0;
    ret.lPedidos->info.idPlato = 0;
    return ret;
}

Estadistica inicializarEstadistica(int cant, int idPlato)
{
    Estadistica ret;
    ret.cant = 0;
    ret.idPlato = idPlato;
    return ret;
}

// BUSQUEDAS Y CONVERSIONES

Plato buscarPlato(FILE *f, int idPlato)
{
    fseek(f, 0, SEEK_SET);
    Plato p = leer<Plato>(f);
    while (!feof(f) && p.idPlato != idPlato)
    {
        p = leer<Plato>(f);
    }
    return p;
}

RPlato convertiraRPlato(Plato p)
{
    RPlato ret;
    ret.cant = 0;
    ret.pla.idPlato = p.idPlato;
    strcpy(ret.pla.nombPlato, p.nombPlato);
    ret.pla.valor = p.valor;
    return ret;
}