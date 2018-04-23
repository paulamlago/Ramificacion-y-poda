//solucion al problema del viajante usando tecnicas de ramificacion y poda
//PAULA MUÑOZ LAGO

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
using namespace std;

const string file_name = "grafo.txt";
int **inicializarMatriz(int &n);
double viajante_rp(int **mat, int N, vector<int> sol_mejor);
int *calculo_minimos(int **mat, int N);
double calculo_coste_estimado(int *costes_minimos, int aristas);

typedef struct {
	vector<int> sol;
	int k;
	double coste, coste_estimado; //ordenamos la cola de prioridad en funcion del coste_estimado
	vector<bool> usado;
}nodo;

//Grafo representado con una matriz de adyacencia
double viajante_rp(int **mat, int N, vector<int> sol_mejor){
	double coste_mejor;
    nodo X, Y;
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
    for(int i = 0; i < N; i++){
        if(i == 0){
            Y.sol[i] = 0;
            Y.usado[i] = true;
        }else{
            Y.sol[i] = -1;
            Y.usado[i] = false;
        }
    }

	Y.k = 0; Y.coste = 0; Y.coste_estimado = calculo_coste_estimado(costes_minimos, N);
	cp.push(Y);
	coste_mejor = INFINITY;

	while(!cp.empty() && cp.top().coste_estimado < coste_mejor){
		Y = cp.top();
		cp.pop();
		cout << "ahora mismo el tamano es: " << cp.size() << endl;
		//generamos hijos de Y
		X.k = Y.k + 1; X.sol = Y.sol; X.usado = Y.usado;
		int anterior = X.sol[X.k - 1]; //ultimo nodo visitado

		for(int vertice = 1; vertice < N; vertice++){
			if(!X.usado[vertice] && mat[anterior][vertice] != 0 /*es decir, existe una arista*/){
				X.sol[X.k] = vertice;
				X.usado[vertice] = true;
				X.coste = Y.coste + mat[anterior][vertice];
				
				if(X.k == N){
					/*fin del arbol*/
					
					if(mat[X.sol[N - 1]][1] > 0 && (X.coste + mat[X.sol[N - 1]][1]) < coste_mejor){
						sol_mejor = X.sol;
						coste_mejor = X.coste + mat[X.sol[N - 1]][1];
					}
				}else{
					X.coste_estimado = (X.coste + calculo_coste_estimado(costes_minimos, N - X.k));
					if(X.coste_estimado < coste_mejor){
						cp.push(X);
					}
				}
				X.usado[vertice] = false;
			}
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
	vector<int> sol;
	double coste;

	int **matriz_ady = inicializarMatriz(N);


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