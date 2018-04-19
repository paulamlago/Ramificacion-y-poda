#include <iostream>
using namespace std;


class nodo {
	int *sol;
    int k;
    double coste, coste_estimado; //ordenamos la cola de prioridad en funcion del coste_estimado
    bool *usado;
    
public:
    nodo();
    nodo(int etapa, int n, int *soluciones, bool *u, int coste, int coste_est);
	int getCoste_estimado() {return coste_estimado; }
	int getK(){return k;}
	int *getSoluciones(){return sol;}
	int getSolucion(int i) {return sol[i]; }
	bool *getUsados(){ return usado;}
	int getCoste(){return coste;}
	void modifySolution(int pos, int x) {sol[pos] =x; }
	void modifyUsado(int pos, bool x){usado[pos] = x;}
};

nodo::nodo(int etapa, int n, int *soluciones, bool *u, int cost, int coste_est){
    k = etapa;
    sol = new int[n];
    usado = new bool[n];

    sol = soluciones;
    usado = u;
    /*
    for(int i = 0; i < soluciones.size(); i++){
        sol[i] = soluciones[i];
        usado[i] = u[i];
    }*/
    coste = cost;
    coste_estimado = coste_est;
}

class Monticulo_Williams_Minimos {
	nodo *v; //vector en el que guardamos los valores de los nodos, primera posicion: 0
	int *posiciones;
	int ultimo;
	int max_size; //ultima posicion: tamañoMaximo-1
	int size;

public:

	Monticulo_Williams_Minimos(int); //el parametro es el tamaño máximo 
	void insert(nodo);
	void flotar(int);
	void hundir(int j, int k);
	void intercambiar(int, int); //nodoa flotar y hundir: necesario a la hora de insertar, borrar, decrecer clave...
	nodo  elem_minimo(); //como es un montículo de mínimos, es el primer elemento del vector
	void borra_min();
	void decrecer_clave(int); //!!
	void modificar(int valor, int destino);
	int getSize() { return size; }
	nodo getValue(int i) { return v[i]; }
	//las siguientes funciones devuelven la posicion en la que se encuentran los hijos y el padre, no el elemento
	int hijo_iz(int padre) { return padre * 2 + 1; }
	int hijo_dr(int padre) { return padre * 2 + 2; }
	int padre(int nodo) { return (nodo - 1) / 2; }
};

Monticulo_Williams_Minimos::Monticulo_Williams_Minimos(int t) {
	max_size = t;
	size = 0;
	v = new nodo[t];
	posiciones = new int[t];
}

void Monticulo_Williams_Minimos::insert(nodo element) {
	//si se puede insertar, es decir, no hemos llegado a la capacidad máxima: insertamos
	if (size != max_size) {
		v[size] = element;
		posiciones[size] = element.getElem();
		size++;
		//si no se trata del primer elemento que insertamos y el padre del elemento es 
		//mayor que este nuevo elemento habremos violado la condicion de montículo de minimos
		//para solucionarlo usamos la funcion intercambiar
		flotar(size - 1);
	}
	else cout << "Ya no se pueden insertar más elementos!" << endl;
}

void Monticulo_Williams_Minimos::flotar(int i) {
	if (i != 0) {
		for (; v[padre(i)].getElem() > v[i].getElem(); i = padre(i)) {
			intercambiar(padre(size - 1), i);
		}
	}
}

void Monticulo_Williams_Minimos::hundir(int j, int k) {
	bool fin = false;
	int m;

	while (2 * j <= size && !fin) {

		if (2 * j + 1 <= k && v[2 * j + 1].getElem() < v[2 * j].getElem()) m = 2 * j + 1;
		if (2 * j + 1 > k || v[2 * j + 1].getElem() >= v[2 * j].getElem()) m = 2 * j;

		if (v[j].getElem() > v[m].getElem()) { 
			intercambiar(j, m); 
			j = m; 
		}
		else fin = true;
	}
}

void Monticulo_Williams_Minimos::intercambiar(int x, int y) {
	nodo aux = v[x];
	v[x] = v[y];
	v[y] = aux;
}

nodo Monticulo_Williams_Minimos::elem_minimo() {
	if (size > 0) return v[0];
	else cout << "elem_minimo: No hay elementos en el monticulo" << endl;
	return nodo(); 
}

void Monticulo_Williams_Minimos::borra_min() {
	//el tamaño del monticulo es mayor que 0 siempre
	if (size == 1) {
		size = 0;
	}
	else {
		v[0] = v[size - 1]; //convertimos en raiz la hoja más a la derecha del ultimo nivel
		size--;
		//hundimos la raíz (la intercambiamos por el menor de sus hijos)
		//mientras que el elemento no sea una hoja o sea mayor que alguno de sus hijos los intercambiamos
		hundir(1, size);
	}
}

int main(){

    return 0;
}

//mat - matriz de adyacencia que representa el grafo
double viajante_rp(int **mat, int N, int *sol_mejor, double coste_mejor){
    nodo *X, *Y;
    Monticulo_Williams_Minimos *cp = new Monticulo_Williams_Minimos(N);
    int *costes_minimos = new int[N];
	
	//guardamos en costes_minimos los costes de las aristas de menor a mayor
    costes_minimos = calculo_minimos(mat, N);
    
	//generamos la raiz
    //necesitamos preparar un array solucion y usado
    int *sol = new int[N];
    bool *usado = new bool[N];

    for(int i = 0; i < N; i++){
        if(i == 0){
            sol[i] = 0;
            usado[i] = true;
        }else{
            sol[i] = -1;
            usado[i] = false;
        }
    }

    Y = new nodo(1, N, sol, usado, 0, calculo_coste_estimado(costes_minimos, N));

	while(cp->getSize() != 0 && cp->elem_minimo().getCoste_estimado() < coste_mejor){
		Y = &cp->elem_minimo();
		cp->borra_min();
		//generamos hijos de Y
		X = new nodo(Y->getK() + 1, N, Y->getSoluciones(), Y->getUsados(),/*costes*/ 0, /*coste estimado*/1.0);
		int anterior = X->getSolucion(X->getK() - 1);

		for(int vertice = 2; vertice < N; vertice++){
			if(!X->getUsados()[vertice] && mat[anterior][vertice] != 0 /*es decir, existe una arista*/){
				X->modifySolution(X->getK(), vertice);
				X->modifyUsado(vertice, true);
				if(X->getK() == N){
					/*fin del arbol*/


				}else{
					
				}
			}
		}
	}
}

int *calculo_minimos(int **mat, int N){
    //recorremos la matiz cogiendo los costes y ordenandolos
    //solo nos interesa una mitad de la matriz
    int *costes = new int[N];

    for(int i = 0; i < N; i++){
        for(int j = 0; j < i; j++){
            costes[i] = mat[i][j];
        }
    }

    int aux;
    //ahora que tenemos los costes los ordenamos de menor a mayor
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            if (costes[i] > costes[j]){
                aux = costes[i];
                costes[i] = costes[j];
                costes[j] = aux;
            }
        }
    }
    return costes;
}

double calculo_coste_estimado(int *costes_minimos, int aristas){
    double acc = 0;
    for(int i = 0; i < aristas; i++){
        acc += costes_minimos[i];
    }
    return acc;
}