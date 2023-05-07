#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <map>
#include <string>
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

/// file di output per grafo
ofstream output_graph;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        cout << j << "->" << A[j] << " ";
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
        if (argv[i][1] == 's') 
            max_dim = atoi(argv[i] + 3);
    }

    return 0;
}

void print_array_graph(int* A, int p, int r, string s, int swapped_index1, int swapped_index2, int n_iteration){
  /// uso codice HTML per creare una tabella con il contenuto dell'array
  
  output_graph << s << n_iteration << " [label=<"<< endl
               << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" " << "> <TR>"<<endl;

  for (int j=p; j<=r; j++) {
    output_graph << "<TD";
    if(j == swapped_index1 || j == swapped_index2) output_graph << " bgcolor=\"#ff8080\"";
    output_graph << ">" << A[j] << "</TD>"<<endl;
  }
  output_graph << "</TR> </TABLE>>];"<<endl;  
}

/// @brief implementa l'algoritmo di ordinamento "Cocktail Shaker Sort"
/// @param A array
/// @param l indice di partenza
/// @param r indice di arrivo
void shaker_sort(int A[], int l, int r) {
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

/// @brief implementa l'algoritmo di ordinamento "Shell Sort"
/// @param A array
/// @param l indice di partenza
/// @param r indice di arrivo
void shell_sort(int* A, int l, int r)
{
    int n = r - l + 1;

    // Determine the gap sequence
    int gap = 1;
    while (gap < n / 3) {
        gap = 3 * gap + 1;
    }

    // Perform shell sort with decreasing gap sequence
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

/// @brief calcola l'indice del bucket in cui inserire l'elemento
/// @param elem elemento di cui va trovato l'indice
/// @param n_buckets numero di buckets
/// @param min valore minimo
/// @param max valore massimo
/// @return 
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

/// @brief supponendo valori compresi tra -25 e 49, distribuiti uniformemente.
/// @param A array
/// @param l indice di partenza
/// @param r indice di arrivo
void bucket_sort(int* A, int l, int r) {
    int min = -25;
    int max = 24;
    int k = 5;

    int** bucket = new int*[r-l+1];
    int* indexes = new int[r-l+1];
 
    if(details) cout << endl << "initial index value" << endl;
    for (int i = 0; i < k; i++) {
        // size of each bucket
        bucket[i] = new int[200];
        if(details) cout << "indexes[" << i << "] <- 0" << endl;
        indexes[i] = 0;
    }

    if(details) cout << endl << "finding bucket indexes and adding values" << endl;
    for(int i = l; i <= r; i++) {
        ct_read++;
        int bucket_index = bucketIndex(A[i], k, min, max);
        if(details) cout << "bucket index for " << A[i] << " -> " << bucket_index << endl;
        ct_read+=2;
        bucket[bucket_index][indexes[bucket_index]]=A[i];
        indexes[bucket_index]++;
        if(details) cout << "incremented bucket dimension: " << indexes[bucket_index]-1 << " -> " << indexes[bucket_index] << endl;
    }

    
    if(details) cout << endl << "sorting buckets" << endl;
    if(details) {
        cout << "bucket n.0 size: " << indexes[0] << endl;
        cout << "bucket n.1 size: " << indexes[1] << endl;
        cout << "bucket n.2 size: " << indexes[2] << endl;
        cout << "bucket n.3 size: " << indexes[3] << endl;
        cout << "bucket n.4 size: " << indexes[4] << endl;
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
            if(arr_index != l) ct_read++; // controllo per evitare incrementi doppi
            ct_read+=2;
            if(details) {
                cout << "A[" << arr_index << "] <- bucket[" << i << "][" << bucket_index << "] = " << bucket[i][bucket_index] << endl;
            }
            A[arr_index] = bucket[i][bucket_index];
            bucket_index++;
            arr_index++;
        }
    }
}

/// @brief inverte l'array
/// @param A array
void reverse_array(int* A) {
    for(int i=0; i<max_dim/2; i++) {
        ct_read++;
        ct_read++;
        swap(A[i], A[max_dim-i-1]);
    }
}

/// @brief verifica l'ordinamento dell'array
/// @param A array  
/// @param l indice di partenza
/// @param r indice di arrivo
/// @return valore bool che indica il corretto ordinamento dell'array
bool check_sorted(int* A, int l, int r) {
    if(details) {
        for(int i = l; i < r; i++)
        cout << "A[i]=" << A[i] << endl;
    }

    for(int i = l; i < r; i++) 
        if(i != r-1 && A[i] > A[i+1]) {
            cout << i << " " << i+1 << endl;
            cout << A[i] << " > " << A[i+1] << endl;
            return false;
        }

    return true;
}

/// @brief soluzione allo scompenso causato dall'utilizzo di 2 algoritmi 
///         su porzioni di array diverse, implementa una sorta di merge
/// @param A 
/// @param threshold 
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
        A[i] = tmp[i-left];
    }
}

template <typename Map>
bool map_compare (Map const &lhs, Map const &rhs) {
    // No predicate needed because there is operator== for pairs already.
    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(), lhs.end(),
                      rhs.begin());
}

/// @brief legge da file 100 righe contenenti ognuna 1000 numeri, poi li ordina 
///         utilizzando un algoritmo di ordinamento.
/// @param argc numero di argomenti da riga di comando
/// @param argv array di argomenti da riga di comando
/// @return 
int main(int argc, char **argv) {
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];
    B = new int[max_dim];
    
    map<string, int> unordered_map; 
    map<string, int> ordered_map; 

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

    if (graph){
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g"<<endl; 
        output_graph << "{ rankdir = TB;"<<endl;
        output_graph << "node [shape=plaintext]"<<endl;
    }

    //// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++) {
        unordered_map.clear();
        ordered_map.clear();

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++) {
            char comma;
            input_data >> A[i];
            input_data >> comma;

            string key = to_string(A[i]);
            if(unordered_map.find(key) == unordered_map.end()) {
                unordered_map.insert(make_pair(key, 1));
            } else {
                unordered_map[key]++;
            }
        }

        // int sum_values = 0;
        // cout << "Unordered map: " << endl;
        // for(auto elem : unordered_map) {
        //     cout << elem.first << ": " << elem.second << endl;
        //     sum_values += elem.second;
        // }

        // cout << "sum of elements: " << sum_values << endl;

        if (details) {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        // algoritmo di sorting

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

        // if(check_sorted(A, 0, max_dim)) cout << "array is sorted" << endl;
        // else cout << "array is not sorted" << endl;

        for(int i = 0; i < n; i++) {
            string key = to_string(A[i]);
            if(ordered_map.find(key) == ordered_map.end()) {
                ordered_map.insert(make_pair(key, 1));
            } else {
                ordered_map[key]++;
            }
        }

        // if(map_compare(unordered_map, ordered_map) == 1) cout << "values are correct" << endl;
        // else cout << "values are not correct" << endl;

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

    if (graph){
        /// preparo footer e chiudo file
        output_graph << "}"<<endl; 
        output_graph.close();
        cout << " File graph.dot scritto" << endl<< "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf"<<endl;
    }

    delete[] A;

    return 0;
}
