//solucion al problema del viajante usando tecnicas de ramificacion y poda
//PAULA MUÑOZ LAGO

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <ctime>
using namespace std;

const string file_name = "grafo.txt";
int **inicializarMatriz(int &n);
double viajante_rp(int **mat, int N, vector<int> &sol_mejor);
vector<int> calculo_minimos(int **mat, int N);
double calculo_coste_estimado(vector<int> costes_minimos, vector<int> visitados);

typedef struct {
	vector<int> sol;
	int k;
	double coste, coste_estimado; //ordenamos la cola de prioridad en funcion del coste_estimado
	vector<bool> usado;
}nodo;

struct comparison_nodes {
	bool operator() (const nodo &a, const nodo &b) const
	{
		return a.coste_estimado > b.coste_estimado;
	}
};


//Grafo representado con una matriz de adyacencia
double viajante_rp(int **mat, int N, vector<int> &sol_mejor) {
	double coste_mejor;
	unsigned t0, t1, acc = 0;
	int counter_nodos_explorados = 0;
	nodo Y, X;
	//usara una cola de prioridad para ir abriendo los nodos en funcion de su coste estimado,
	//es decir, los mas prometedores antes.
	priority_queue<nodo, vector<nodo>, comparison_nodes> cp = priority_queue<nodo, vector<nodo>, comparison_nodes>();
	
	//cota 1:
	//guardamos en el vactor los costes minimos de ir de un nodo a otro desde cada uno de ellos
	vector<int> costes_minimos(N);
	costes_minimos = calculo_minimos(mat, N);

	//generamos la raiz
	//necesitamos preparar un array solucion y usado
	Y.sol = vector<int>(N);
	Y.usado = vector<bool>(N);
	for (int i = 0; i < N; i++) {
		if (i == 0) {
			Y.sol[i] = 0;
			Y.usado[i] = true;

		}
		else {
			Y.sol[i] = -1;
			Y.usado[i] = false;
		}
	}
	Y.k = 0; Y.coste = 0;
	Y.coste_estimado = calculo_coste_estimado(costes_minimos, Y.sol);
	cp.push(Y);
	coste_mejor = INFINITY;

	while (!cp.empty() && cp.top().coste_estimado < coste_mejor) {
		X.usado = vector<bool>(N);
		X.sol = vector<int>(N);
		t0 = clock();

		Y = cp.top();
		cp.pop();

		counter_nodos_explorados++;
		cout << counter_nodos_explorados << "- Exploramos nodo: " << Y.sol[Y.k] << endl;

		//generamos hijos de Y
		X.k = Y.k + 1;

		X.sol = Y.sol;
		X.usado = Y.usado;

		int anterior = X.sol[X.k - 1]; //ultimo nodo visitado

		for (int vertice = 1; vertice < N; vertice++) {
			if (!X.usado[vertice] && mat[anterior][vertice] != -1 /*es decir, existe una arista*/) {
				X.sol[X.k] = vertice;
				X.usado[vertice] = true;
				X.coste = Y.coste + mat[anterior][vertice];

				if (X.k == N - 1) {
					/*fin del arbol*/

					if (mat[X.sol[N - 1]][0] > -1 && (X.coste + mat[X.sol[N - 1]][0]) < coste_mejor) {
						sol_mejor = X.sol;
						coste_mejor = X.coste + mat[X.sol[N - 1]][0];
					}
				}
				else {
					X.coste_estimado = (X.coste + calculo_coste_estimado(costes_minimos, X.sol));
					if (X.coste_estimado < coste_mejor) {
						cp.push(X);
					}
				}
				X.usado[vertice] = false;
			}
		}
		t1 = clock();
		acc += (t1 - t0);
	}

	cout << "En total hemos explorado " << counter_nodos_explorados << " nodos" << endl;
	cout << "El tiempo medio de exploracion de cada nodo es " << acc / counter_nodos_explorados << endl;
	return coste_mejor;
}

vector<int> calculo_minimos(int **mat, int N) {
	//recorremos la matiz cogiendo los costes y ordenandolos
	//solo nos interesa una mitad de la matriz
	vector<int> costes(N);
	int count = 0;
	double  min;
	for (int i = 0; i < N; i++) {
		min = INFINITY;
		for (int j = 0; j < N; j++) {
			if (mat[i][j] != -1 && mat[i][j] < min) {
				costes[count] = mat[i][j];
				min = mat[i][j];
			}
		}
		count++;
	}
	return costes;
}

double calculo_coste_estimado(vector<int> costes_minimos, vector<int> visitados) {
	//nos quedamos con el coste minimo de ir de un nodo de los que faltan por visitar a otro
	double acc = 0;
	for (int i = 0; i < visitados.size(); i++) {
		if(visitados[i] == -1) acc += costes_minimos[i];
	}
	return acc;
}


int main() {
	//cargamos el archivo con el grafo
	int N;
	vector<int> sol_mejor;
	double coste;
	unsigned t0, t1;

	t0 = clock();
	int **matriz_ady = inicializarMatriz(N);
	//llamamos al algoritmo de ramificacion y poda

	coste = viajante_rp(matriz_ady, N, sol_mejor);

	//imprimimos la mejor solucion
	cout << "El coste de la mejor solucion tiene coste: " << coste << endl;
	for (int i = 0; i < sol_mejor.size(); i++) {
		cout << sol_mejor[i] << " ";
	}

	t1 = clock();
	cout << endl << "El tiempo total de ejecucion es " << (t1 - t0) << endl;
	for (int i = 0; i < N; i++) {
		delete[] matriz_ady[i];
	}
	return 0;
}

int **inicializarMatriz(int &n) {
	ifstream file;
	file.open(file_name);
	int x;
	file >> x;
	int **matriz = NULL;

	if (file.is_open()) {
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