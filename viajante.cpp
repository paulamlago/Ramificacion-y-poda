//solucion al problema del viajante usando tecnicas de ramificacion y poda
//PAULA MUÑOZ LAGO

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

const string file_name = "grafo.txt";
int **inicializarMatriz(int &n);
double viajante_rp(int **mat, int N, int *sol_mejor);
int *calculo_minimos(int **mat, int N);
double calculo_coste_estimado(int *costes_minimos, int aristas);

class nodo {
	int *sol;
    int k;
    double coste, coste_estimado; //ordenamos la cola de prioridad en funcion del coste_estimado
    bool *usado;
    
public:
    nodo(int etapa, int n, int *soluciones, bool *u);
    nodo(int etapa, int n, int *soluciones, bool *u, double coste, double coste_est);
	double getCoste_estimado() {return coste_estimado; }
	int getK(){return k;}
	int *getSoluciones(){return sol;}
	int getSolucion(int i) {return sol[i]; }
	bool *getUsados(){ return usado;}
	double getCoste(){return coste;}
	void modifySolution(int pos, int x) {sol[pos] =x; }
	void modifyUsado(int pos, bool x){usado[pos] = x;}
	void setCoste(double c) {coste = c;}
	void setCoste_estimado(double c){coste = c;}
	bool operator () (nodo l, nodo r);
};

bool nodo::operator () (nodo l, nodo r){
	return l.getCoste_estimado() < r.getCoste_estimado();
}

nodo::nodo(int etapa, int n, int *soluciones, bool *u, double cost, double coste_est){
    k = etapa;
    sol = new int[n];
    usado = new bool[n];
    sol = soluciones;
    usado = u;
    coste = cost;
    coste_estimado = coste_est;
}

nodo::nodo(int etapa, int n, int *soluciones, bool *u){
 	k = etapa;
    sol = new int[n];
    usado = new bool[n];

    sol = soluciones;
    usado = u;
}

//Grafo representado con una matriz de adyacencia
double viajante_rp(int **mat, int N, int *sol_mejor){
	double coste_mejor;
    nodo *X, *Y;
	//usara una cola de prioridad para ir abriendo los nodos en funcion de su coste estimado,
	//es decir, los mas prometedores antes.
	priority_queue<nodo> cp;
	
    int *costes_minimos = new int[N];
	
	//cota 1:
	//guardamos en costes_minimos los costes de las aristas de menor a mayor
	//para que, cuando sepamos el numero de aristas que nos quedan por recorrer, x,
	//podamos establecer un coste optimista sumando las x primeras
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

    Y = new nodo(0, N, sol, usado, 0, calculo_coste_estimado(costes_minimos, N));
	cp.push(*Y);
	coste_mejor = INFINITY;

	nodo aux = cp.top();

	while(!cp.empty() && aux.getCoste_estimado() < coste_mejor){
		*Y = cp.top();
		cp.pop();
		cout << "ahora mismo el tamano es: " << cp.size() << endl;
		//generamos hijos de Y
		X = new nodo(Y->getK() + 1, N, Y->getSoluciones(), Y->getUsados());
		int anterior = X->getSolucion(X->getK() - 1); //ultimo nodo visitado

		for(int vertice = 1; vertice < N; vertice++){
			if(!X->getUsados()[vertice] && mat[anterior][vertice] != 0 /*es decir, existe una arista*/){
				X->modifySolution(X->getK(), vertice);
				X->modifyUsado(vertice, true);
				X->setCoste(Y->getCoste() + mat[anterior][vertice]);
				
				if(X->getK() == N){
					/*fin del arbol*/
					
					if(mat[X->getSolucion(N - 1)][1] > 0 && (X->getCoste() + mat[X->getSolucion(N - 1)][1]) < coste_mejor){
						sol_mejor = X->getSoluciones();
						coste_mejor = X->getCoste() + mat[X->getSolucion(N - 1)][1];
					}
				}else{
					X->setCoste_estimado(X->getCoste() + calculo_coste_estimado(costes_minimos, N - X->getK()));
					if(X->getCoste_estimado() < coste_mejor){
						cp.push(*X);
					}
				}
				X->modifyUsado(vertice, false);
			}

			aux = cp.top();
		}
	}

	cout << "El coste de la mejor solucion es: " << coste_mejor << endl;
	for(int i = 0; i < N; i++){
		cout << sol_mejor[i] << " ";
	}
	cout << endl << endl;

	return coste_mejor;
}

int *calculo_minimos(int **mat, int N){
    //recorremos la matiz cogiendo los costes y ordenandolos
    //solo nos interesa una mitad de la matriz
    int *costes = new int[N];
	int count = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < i; j++){
            costes[count] = mat[i][j];
			count++;
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


int main(){
	//cargamos el archivo con el grafo
	int N;
	int *sol;
	double coste;

	int **matriz_ady = inicializarMatriz(N);
	sol = new int[N];

	//llamamos al algoritmo de ramificacion y poda
	coste = viajante_rp(matriz_ady, N, sol);

	//imprimimos la mejor solucion
	cout << "El coste de la mejor solucion es: " << coste << endl;
	for(int i = 0; i < N; i++){
		cout << sol[i] << " ";
	}
	return 0;
}

int **inicializarMatriz(int &n) {
	ifstream file;
	file.open(file_name);
	int x;
	file >> x;
	int **matriz = NULL;

	if (file.is_open()){
			matriz = new int*[x];
			for (int j = 0; j < x; j++) {
				matriz[j] = new int[x];
				for (int i = 0; i < x; i++) {
					file >> matriz[j][i];
				}
			}

		n = x;
	}
	else {
		cout << "Ha habido un problema con el archivo: " << file_name << endl;
	}
	file.close();

	return matriz;
}