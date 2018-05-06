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
double viajante_rp(int **mat, int N, vector<int> &sol_mejor);
int *calculo_minimos(int **mat, int N, int &costes_length);
double calculo_coste_estimado(int *costes_minimos, int aristas);

typedef struct {
	vector<int> sol;
	int k;
	double coste, coste_estimado; //ordenamos la cola de prioridad en funcion del coste_estimado
	vector<bool> usado;
}nodo;

struct comparison_nodes{
	bool operator() (const nodo &a, const nodo &b) const
		{return a.coste_estimado > b.coste_estimado;}
};


//Grafo representado con una matriz de adyacencia
double viajante_rp(int **mat, int N, vector<int> &sol_mejor){
	double coste_mejor;
	int costes_length;
    nodo Y, X;
	//usara una cola de prioridad para ir abriendo los nodos en funcion de su coste estimado,
	//es decir, los mas prometedores antes.
	priority_queue<nodo, vector<nodo>, comparison_nodes> cp = priority_queue<nodo, vector<nodo>, comparison_nodes>();

    int *costes_minimos = new int[N];
	
	//cota 1:
	//guardamos en costes_minimos los costes de las aristas de menor a mayor
	//para que, cuando sepamos el numero de aristas que nos quedan por recorrer, x,
	//podamos establecer un coste optimista sumando las x primeras
    costes_minimos = calculo_minimos(mat, N, costes_length);

	//generamos la raiz
    //necesitamos preparar un array solucion y usado
	Y.sol = vector<int>(N);
	Y.usado = vector<bool>(N);

    for(int i = 0; i < N; i++){
        if(i == 0){
            Y.sol[i] = 0;
            Y.usado[i] = true;
			
        }else{
            Y.sol[i] = -1;
            Y.usado[i] = false;
        }
    }

	Y.k = 0; Y.coste = 0; 
	Y.coste_estimado = calculo_coste_estimado(costes_minimos, N);
	cp.push(Y);
	coste_mejor = INFINITY;

	while(!cp.empty() && cp.top().coste_estimado < coste_mejor){
		X.sol = vector<int>(N);
		X.usado = vector<bool>(N);

		Y = cp.top();
		cp.pop();
		//generamos hijos de Y
		X.k = Y.k + 1;

		X.sol = Y.sol;
		X.usado = Y.usado;
		
		int anterior = X.sol[X.k - 1]; //ultimo nodo visitado

		for(int vertice = 1; vertice < N; vertice++){
			if(!X.usado[vertice] && mat[anterior][vertice] != 0 /*es decir, existe una arista*/){
				X.sol[X.k] = vertice;
				X.usado[vertice] = true;
				X.coste = Y.coste + mat[anterior][vertice];
				
				if(X.k == N - 1){
					/*fin del arbol*/
					
					if(mat[X.sol[N - 1]][0] > 0 && (X.coste + mat[X.sol[N - 1]][0]) < coste_mejor){
						sol_mejor = X.sol;
						coste_mejor = X.coste + mat[X.sol[N - 1]][0];
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

	//delete[] costes_minimos;
	return coste_mejor;
}

int *calculo_minimos(int **mat, int N, int &costes_length){
    //recorremos la matiz cogiendo los costes y ordenandolos
    //solo nos interesa una mitad de la matriz
    int *costes = new int[N];
	int count = 0;

    for(int i = 0; i < N; i++){
        for(int j = 0; j < i; j++){
			if(mat[i][j] != 0){
				costes[count] = mat[i][j];
				count++;
			}
        }
    }

    int aux;
    //ahora que tenemos los costes, los ordenamos de menor a mayor
    for(int i = 0; i < count; i++){
        for(int j = i + 1; j < count; j++){
            if (costes[i] > costes[j]){
                aux = costes[i];
                costes[i] = costes[j];
                costes[j] = aux;
            }
        }
    }
	costes_length = count;
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
	vector<int> sol_mejor;
	double coste;

	int **matriz_ady = inicializarMatriz(N);

	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			cout << matriz_ady[i][j] << " ";
		}
		cout << endl;
	}
	//llamamos al algoritmo de ramificacion y poda

	coste = viajante_rp(matriz_ady, N, sol_mejor);

	//imprimimos la mejor solucion
	cout << "El coste de la mejor solucion es: " << coste << endl;
	for(int i = 0; i < N; i++){
		cout << sol_mejor[i] << " ";
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
