#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

int max_dim = 1000;

int main() {
    int *A = new int[max_dim];

    ifstream input_data;

    input_data.open("data.txt");

    for(int i = 0; i < max_dim; i++) {
        char comma;
        input_data >> A[i];
        input_data >> comma;

        cout << "valore effettivo n." << i << " = " << A[i] << endl;
    }

    
    int min = A[477];
    int max = A[max_dim-1];
    for(int i = 477; i < max_dim-1; i++) {
        if(A[i] < min) min = A[i];

        if(A[i] > max) max = A[i];
    }

    cout << "minimo: " << min << endl;
    cout << "massimo: " << max << endl;

    input_data.close();

    for(int i = 0; i < max_dim; i++) {
        A[i] = (int)(100000*pow(2,-i/(0.0+1000)*25));
        cout << "generazione, valore n." << i << " = " << A[i] << endl; 
        if(i == 204) cout << endl;
    }

    for (int i = 0; i < max_dim; i++) {
        int v = 0;
        if(i != max_dim - 1)
        v = A[i] - A[i+1];
        
        cout << "differenza valori in " << i << "-" << i+1 << " = " << v << endl;
    }
}