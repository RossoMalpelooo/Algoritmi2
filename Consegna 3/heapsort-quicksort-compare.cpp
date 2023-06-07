#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

// compilazione: g++ lezione4-6-quicksort.c
//
// Obiettivo:

// prove con array ordinato crescente, decrescente --> numero di confronti?
// ./a.out 1024 -d=16

// prove con array random --> numero di confronti?
// ./a.out 1024 -d=16 -t=100

int ct_swap_qs = 0;
int ct_cmp_qs = 0;
int ct_read_qs = 0;

int ct_swap_hs = 0;
int ct_cmp_hs = 0;
int ct_read_hs = 0;

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0;         /// dimensione dell'array
int heap_size = 0; // dimensione heap

void print_array(int *A, int dim)
{
    for (int j = 0; j < dim; j++)
    {
        cout << A[j] << " ";
    }
    cout << "\n";
}

void swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap_qs++;
}

int partition(int *A, int p, int r)
{

    /// copia valori delle due meta p..q e q+1..r
    ct_read_qs++;
    int x = A[r];
    int i = p - 1;

    for (int j = p; j < r; j++)
    {
        ct_cmp_qs++;
        ct_read_qs++;
        if (A[j] <= x)
        {
            i++;
            ct_read_qs++;
            ct_read_qs++;
            swap(A[i], A[j]);
        }
    }
    ct_read_qs++;
    ct_read_qs++;
    swap(A[i + 1], A[r]);

    return i + 1;
}

void quick_sort(int *A, int p, int r)
{
    /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
    if (p < r)
    {
        int q = partition(A, p, r);
        quick_sort(A, p, q - 1);
        quick_sort(A, q + 1, r);
    }
}

/// uso -1 per indicare un indice non esistente
int parent_idx(int n)
{
    if (n == 0)
        return -1;
    return (n - 1) / 2;
}

int child_L_idx(int n)
{
    if (2 * n + 1 >= heap_size)
        return -1;
    return 2 * n + 1;
}

int child_R_idx(int n)
{
    if (2 * n + 2 >= heap_size)
        return -1;
    return 2 * n + 2;
}

/// restituisce 0 se il nodo in posizione n e' un nodo interno (almeno un figlio)
/// restituisce 1 se il nodo non ha figli
int is_leaf(int n)
{
    return (child_L_idx(n) == -1); // non c'e' bisogno di controllare il figlio R

    /* versione equivalente
    if (child_L_idx(n)==-1)
      return 1;
    return 0;
    */
}

void heap_insert(int *heap, int elem)
{
    /// inserisco il nuovo nodo con contenuto elem
    /// nell'ultima posizione dell'array
    /// ovvero continuo a completare il livello corrente

    if (details)
        printf("Inserisco elemento %d in posizione %d\n", elem, heap_size);

    if (heap_size < max_dim)
    {
        int i = heap_size;
        heap_size++;

        heap[i] = elem;

        while (i != 0)
        {
            ct_read_hs++;
            // non sono sulla radice
            if (heap[parent_idx(i)] >= heap[i])
            { /// proprieta' dell' heap e' rispettata -> esco
                if (details)
                    printf("Il genitore ha valore %d >= del nodo %d, esco\n", heap[parent_idx(i)], heap[i]);
                return;
            }

            if (details)
                printf("Il genitore ha valore %d < del nodo %d, swap\n", heap[parent_idx(i)], heap[i]);
            /// il nodo ha un genitore sicuramente <   --> swap
            ct_read_hs++;
            int t = heap[parent_idx(i)];
            ct_read_hs++;
            heap[parent_idx(i)] = heap[i];
            heap[i] = t;

            // tree_print_graph(0); // radice
            // n_operazione++;

            i = parent_idx(i);
        }
    }
    else
    {
        printf("Heap pieno!\n");
    }
}

int heap_remove_max(int *heap)
{

    if (heap_size <= 0)
    { /// heap vuoto!
        printf("Errore: heap vuoto\n");
        return -1;
    }

    ct_read_hs++;
    int massimo = heap[0];

    if (details)
        printf("Massimo identificato %d\n", massimo);
    /// scambio la radice con l'ultima foglia a destra
    /// il massimo e' stato spostato in fondo --> pronto per l'eliminazione
    ct_read_hs++;
    int t = heap[0];
    ct_read_hs++;
    heap[0] = heap[heap_size - 1];
    heap[heap_size - 1] = t;

    // elimino il massimo (ora in fondo all'array)
    heap_size--;

    //    tree_print_graph(0);  // radice

    /// nella radice c'e' un valore piccolo (minimo?)
    int i = 0; // indice di lavoro (parto dalla root)

    while (!is_leaf(i))
    { /// garantisco di fermarmi alla foglia

        if (details)
            printf("Lavoro con il nodo in posizione i = %d, valore %d\n", i, heap[i]);

        int con_chi_mi_scambio = -1;

        /// controllo il nodo i con il suo figlio L
        ct_read_hs++;
        ct_read_hs++;
        if (heap[i] < heap[child_L_idx(i)])
        { // il nodo i e' piu' piccolo
            /// attivare uno swap (la proprieta' heap non e' rispettata)
            con_chi_mi_scambio = child_L_idx(i);
            if (details)
                printf("Figlio L e' piu' grande (valore %d)\n", heap[child_L_idx(i)]);

            if (child_R_idx(i) >= 0 && // esiste il nodo destro
                heap[child_L_idx(i)] < heap[child_R_idx(i)])
            {
                ct_read_hs++;
                ct_read_hs++;
                con_chi_mi_scambio = child_R_idx(i);
                if (details)
                    printf("Figlio R e' ancora piu' grande (valore %d)\n", heap[child_R_idx(i)]);
            }
        }
        else
        { // il nodo e' piu' grande del figlio L

            if (child_R_idx(i) >= 0)
            { // esiste il figlio R
                if (heap[i] < heap[child_R_idx(i)])
                { /// attivo lo swap
                    con_chi_mi_scambio = child_R_idx(i);
                    if (details)
                        printf("Figlio R e' piu' grande del nodo (valore %d)\n", heap[child_R_idx(i)]);
                }
                else
                    break;
            }
            else
                break;
        }

        /// swap tra i e con_chi_mi_scambio
        ct_read_hs++;
        int t = heap[i];
        ct_read_hs++;
        heap[i] = heap[con_chi_mi_scambio];
        heap[con_chi_mi_scambio] = t;

        i = con_chi_mi_scambio;

        // tree_print_graph(0);  // radice
        // n_operazione++;
    }

    return massimo;
}

void heap_sort(int *A)
{
    while (heap_size > 0)
    {
        heap_remove_max(A);
    }
}

int parse_cmd(int argc, char **argv)
{
    /// controllo argomenti
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << "%s max-dim [Options]\n";
        cout << "   max-dim: specifica la massima dimensione n del problema\n";
        cout << "Options:\n";
        cout << "  -d=<int>: Specifica quali dimensioni n del problema vengono lanciate in sequenza [default: 1] \n";
        cout << "            n = k * max-dim / d, k=1 .. d\n";
        cout << "  -t=<int>: Specifica quanti volte viene lanciato l'algoritmo per una specifica dimensione n [default: 1]\n";
        cout << "            Utile nel caso in cui l'input viene inizializzato in modo random\n";
        cout << "  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n";
        cout << "  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n";
        return 1;
    }

    /// parsing argomento
    max_dim = atoi(argv[1]);

    for (int i = 2; i < argc; i++)
    {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g')
        {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    int i, test;
    int *A;
    int *heap;

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];
    heap = new int[max_dim];
    // A_HS = new int[max_dim];

    // init random
    srand((unsigned)time(NULL));

    //// inizio ciclo per calcolare ndiv dimensioni di array crescenti
    for (n = max_dim / ndiv; n <= max_dim; n += max_dim / ndiv)
    {
        int read_min_qs = -1;
        int read_max_qs = -1;
        long read_avg_qs = 0;

        int read_min_hs = -1;
        int read_max_hs = -1;
        long read_avg_hs = 0;

        //// lancio ntests volte per coprire diversi casi di input random
        for (test = 0; test < ntests; test++)
        {

            ct_swap_qs = 0;
            ct_cmp_qs = 0;
            ct_read_qs = 0;

            ct_swap_hs = 0;
            ct_cmp_hs = 0;
            ct_read_hs = 0;

            /// inizializzazione array: numeri random con range dimensione array
            for (i = 0; i < n; i++)
            {
                // A[i] = i;
                // A[i] = n - i;
                A[i] = rand() % (n * 50);
                ct_read_hs++;
                heap_insert(heap, A[i]);
            }

            if (details)
            {
                cout << "creato array di dimensione " << n << "\n";
                print_array(A, n);
            }

            /// algoritmi di sorting
            quick_sort(A, 0, n - 1);

            heap_sort(heap);

            if (details)
            {
                cout << "Output:\n";
                cout << "Array ordinato con QS:" << endl;
                print_array(A, n);
                cout << endl
                     << "Array ordinato con HS:" << endl;
                print_array(heap, n);
                cout << endl
                     << endl;
            }

            /// statistiche
            read_avg_qs += ct_read_qs;
            if (read_min_qs < 0 || read_min_qs > ct_read_qs)
                read_min_qs = ct_read_qs;
            if (read_max_qs < 0 || read_max_qs < ct_read_qs)
                read_max_qs = ct_read_qs;

            read_avg_hs += ct_read_hs;
            if (read_min_hs < 0 || read_min_hs > ct_read_hs)
                read_min_hs = ct_read_hs;
            if (read_max_hs < 0 || read_max_hs < ct_read_hs)
                read_max_hs = ct_read_hs;
        }

        if (ndiv > 0)
        {
            cout << "Statistiche Quick Sort: " << endl;
            cout << "Dim_array,N_test,min_read,avg_read,max_read,n*n,n*log(n)\n";
            cout << n << "," << ntests << "," << read_min_qs << "," << (0.0 + read_avg_qs) / ntests << "," << read_max_qs << "," << 0.0 + n * n << "," << 0.0 + n * log(n) / log(2) << "\n";

            cout << endl;

            cout << "Statistiche Heap Sort: " << endl;
            cout << "Dim_array,N_test,min_read,avg_read,max_read,n*n,n*log(n)\n";
            cout << n << "," << ntests << "," << read_min_hs << "," << (0.0 + read_avg_hs) / ntests << "," << read_max_hs << "," << 0.0 + n * n << "," << 0.0 + n * log(n) / log(2) << "\n";

            cout << endl
                 << endl;
        }
    }

    delete[] A;
    delete[] heap;

    return 0;
}
