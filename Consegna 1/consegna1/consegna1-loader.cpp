/**
 * @author Leonardo Ongari - 324034
 * @date 30/03/2023
*/

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// compilazione: g++ consegna1-loader.c
//

// Il programma carica il file data.txt contenente 100 righe con dati da ordinare in modo crescente
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

int ct_swap = 0;
int ct_cmp = 0;
int ct_read = 0;

int max_dim = 0;
int ntests = 100;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int partition(int *A, int p, int r) {

    /// copia valori delle due meta p..q e q+1..r
    ct_read++;
    int x = A[r];
    int i = p - 1;

    for (int j = p; j < r; j++) {
        ct_cmp++;
        ct_read++;
        if (A[j] <= x) {
            i++;
            ct_read++;
            ct_read++;
            swap(A[i], A[j]);
        }
    }
    ct_read++;
    ct_read++;
    swap(A[i + 1], A[r]);

    return i + 1;
}

void quick_sort(int *A, int p, int r) {
    /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
    if (p < r) {
        int q = partition(A, p, r);
        quick_sort(A, p, q - 1);
        quick_sort(A, q + 1, r);
    }
}

int parse_cmd(int argc, char **argv) {

    /// parsing argomento
    max_dim = 1000;

    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g') {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

void shaker_sort(int* A, int l, int r) {
    bool swapped = true;
    while (swapped) {
        swapped = false;
        for (int i = l; i < r; ++i) {
            ct_read++;
            ct_read++;
            if (A[i] < A[i + 1]) {
                ct_read++;
                ct_read++;
                swap(A[i], A[i + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
        swapped = false;
        for (int i = r - 1; i >= l; --i) {
            ct_read++;
            ct_read++;
            if (A[i] < A[i + 1]) {
                ct_read++;
                ct_read++;
                swap(A[i], A[i + 1]);
                swapped = true;
            }
        }
    }
}

void shell_sort(int* A, int l, int r)
{
    int n = r - l + 1;

    int gap = 1;
    while (gap < n / 3) {
        gap = 3 * gap + 1;
    }

    while (gap > 0) {
        for (int i = l + gap; i <= r; i++) {
            int j = i;
            ct_read++;
            ct_read++;
            int temp = A[j];
            int temp2 = A[j-gap];
            while(j >= l+gap && temp2 < temp) {
                ct_read++;
                ct_read++;
                swap(A[j], A[j-gap]);
                j-=gap;
                ct_read++;
                ct_read++;
                temp = A[j];
                temp2 = A[j-gap];
            }
        }
        gap /= 3;
    }
}

int bucketIndex(int elem, int n_buckets, int min, int max) {
    int tot = max-min+1;
    int bucket_size = tot/n_buckets;
    
    int tmp = min;
    int index = 0;
    while(tmp <= elem) {
        tmp += bucket_size;
        index++;
    }

    return index-1;
}

void bucket_sort(int* A, int l, int r) {
    // valori fissi per caso specifico
    int min = -25;
    int max = 24;
    int k = 5;

    int** bucket = new int*[r-l+1];
    int* indexes = new int[r-l+1];
 
    if(details) cout << endl << "initial index value" << endl;
    for (int i = 0; i < k; i++) {
        bucket[i] = new int[100];
        if(details) cout << "indexes[" << i << "] <- 0" << endl;
        indexes[i] = 0;
    }

    if(details) cout << endl << "finding bucket indexes and adding values" << endl;
    for(int i = l; i <= r; i++) {
        ct_read++;
        int bucket_index = bucketIndex(A[i], k, min, max);
        if(details) cout << "bucket index for " << A[i] << " -> " << bucket_index << endl;
        ct_read+=3;
        bucket[bucket_index][indexes[bucket_index]]=A[i];
        indexes[bucket_index]++;
        if(details) cout << "incremented bucket index: " << indexes[bucket_index]-1 << " -> " << indexes[bucket_index] << endl;
    }

    
    if(details) cout << endl << "sorting buckets" << endl;
    if(details) {
        cout << "bucket n.0 effective size: " << indexes[0] << endl;
        cout << "bucket n.1 effective size: " << indexes[1] << endl;
        cout << "bucket n.2 effective size: " << indexes[2] << endl;
        cout << "bucket n.3 effective size: " << indexes[3] << endl;
        cout << "bucket n.4 effective size: " << indexes[4] << endl;
    }   
    for(int i = 0; i < k; i++) {
        ct_read+=2;
        if(details) cout << endl;
        if(details) print_array(bucket[i], indexes[i]);
        shell_sort(bucket[i], 0, indexes[i]-1);
        if(details) print_array(bucket[i], indexes[i]);
    }

    if(details) cout << endl << "copying values back into the array" << endl;
    int arr_index = l;
    for(int i = k-1; i >= 0; i--) {
        int bucket_index = 0;
        ct_read++;
        while(bucket_index < indexes[i]) {
            if(arr_index != l) ct_read++; 
            if(details) {
                cout << "A[" << arr_index << "] <- bucket[" << i << "][" << bucket_index << "] = " << bucket[i][bucket_index] << endl;
            }
            ct_read+=2;
            A[arr_index] = bucket[i][bucket_index];
            bucket_index++;
            arr_index++;
        }
    }
}

void fix(int* &A, int left, int start, int right) {
    int* tmp = new int[right-left+1];
    
    int L = left;
    int R = start;

    for(int i = left; i < right; i++) {
        ct_read++;
        ct_read++;
        int num1 = A[L];
        int num2 = A[R];

        if(details) {
            cout << "first: A[" << L << "]" << ": " << A[L] << endl;
            cout << "second: A[" << R << "]" << ": " << A[R] << endl;
        }

        if(num1 > num2 && L < start) {
            if(details) {
                cout << "choose first" << endl;
            }
            tmp[i-left] = num1;
            L++;
        } else {
            if(details) {
                cout << "choose second" << endl;
            }
            tmp[i-left] = num2;
            R++;
        }
    }

    for(int i = left; i < right; i++) {
        if(details) {
            cout << "A[" << i << "] <- tmp[" << i << "-" << L << "] = tmp[" << i-L << "] = " << tmp[i-L] << endl; 
        }
        ct_read++;
        A[i] = tmp[i-left];
    }
}

void reverse_array(int* A) {
    for(int i=0; i<max_dim/2; i++) {
        ct_read++;
        ct_read++;
        swap(A[i], A[max_dim-i-1]);
    }
}

int main(int argc, char **argv) {
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];

    n = max_dim;

    // creazione file input: NON USARE PIU' --> il file data.txt ufficiale è stato allegato, per permettere confronti equivalenti
    //  FILE *f = fopen("data.txt", "w+");
    //  for (int j = 0; j < 100; j++) {
    //      for (int i = 0; i < n; i++) {
    //          int v = 0;
    //          v=(int)(100000*pow(2,-i/(0.0+n)*25));
    //          v+=rand()%50-25;
    //          fprintf(f, "%d,", v);
    //      }
    //      fprintf(f, "\n");
    //  }
    //  fclose(f);

    ifstream input_data;
    input_data.open("data.txt");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++) {

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++) {
            char comma;
            input_data >> A[i];
            input_data >> comma;
        }

        if (details) {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        /// algoritmo di sorting
                // intervallo quasi-ordinato in ordine decrescente
        shaker_sort(A, 205, 300);

        // intervallo abbastanza-ordinato in ordine decrescente
        shell_sort(A, 301, 664);

        // intervallo con distribuzione uniforme (valori tra -25 e 24);
        bucket_sort(A, 665, n-1);

        // PROBLEMA
        // L'utilizzo di 2 algoritmi su porzioni di array diverse non funziona perché vi è rischio di disallineamento agli estremi:
        // Per esempio, si può ottenere un array di questo tipo: [10 8 7 6 5 4 3 2 1 | 4 3 2 1 0]
        // Il simbolo | denota l'indice su cui si sono utilizzati algoritmi diversi

        fix(A, 438, 665, n-1);
        fix(A, 296, 301, 306);

        reverse_array(A);

        if (details) {
            printf("Output:\n");
            print_array(A, n);
        }

        /// statistiche
        read_avg += ct_read;
        if (read_min < 0 || read_min > ct_read)
            read_min = ct_read;
        if (read_max < 0 || read_max < ct_read)
            read_max = ct_read;
        printf("Test %d %d\n", test, ct_read);
    }

    printf("%d,%d,%.1f,%d\n",
           ntests,
           read_min, (0.0 + read_avg) / ntests, read_max);

    delete[] A;

    return 0;
}
