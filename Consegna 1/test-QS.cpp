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

ofstream output_graph;

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

void print_array_graph(int* A, int p, int r, string s,int pivot){
  /// uso codice HTML per creare una tabella con il contenuto dell'array
  
  output_graph << s << p << "_" << r << " [label=<"<< endl
               << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" ";
  if (pivot) output_graph << "bgcolor=\"#ff8080\"";
  output_graph << "> <TR>"<<endl;
  for (int j=p; j<=r; j++) {
    output_graph << "<TD>" << A[j] << "</TD>"<<endl;
  }
  output_graph << "</TR> </TABLE>>];"<<endl;  
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

        if (graph){
            output_graph << "<TR>";
            for (int k=p; k<=r; k++) {
                output_graph << "<TD";
                if (k<=i) output_graph << " bgcolor=\"#e0e0e0\"";           // light gray
                if (k>i && k<=j) output_graph << " bgcolor=\"#a0a0a0\"";    // dark gray
                if (k==r) output_graph << " bgcolor=\"#ff8080\"";           // red
                output_graph << ">" << A[k] << "</TD>"<<endl;
            }
            output_graph << "</TR>"<< endl;
        }
    }
    
    ct_read++;
    ct_read++;
    swap(A[i + 1], A[r]);

    if (graph){
        output_graph << "<TR>";
        for (int k=p; k<=r; k++) {
            output_graph << "<TD";
            if (k<=i) output_graph << " bgcolor=\"#e0e0e0\"";
            if (k>i+1) output_graph << " bgcolor=\"#a0a0a0\"";
            if (k==i+1) output_graph << " bgcolor=\"#ff8080\"";
            output_graph << ">" << A[k] << "</TD>"<<endl;
        }
        output_graph << "</TR>"<< endl;
    }

    return i + 1;
}

void quick_sort(int *A, int p, int r) {
    /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
    if (p < r) {
        if (graph){
            output_graph << "node_before" << p << "_" << r << " [label=<"<< endl << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" ";
            output_graph << "> "<<endl;
        }
        
        int q = partition(A, p, r);

        if (graph){
      output_graph << " </TABLE>>];"<<endl;
      
      /// disegna arco p_r -> p_q (se array non vuoto)
      if (p<=q-1){
      //print_array_graph(A,p,q-1,"node_before",0);
      output_graph << "node_before" << p << "_" << r << " -> ";
      output_graph << "node_before" << p << "_" << q-1 << endl;
      }
      
      /// disegna arco p_r -> q_q 1=pivot
      print_array_graph(A,q,q,"node_before",1);
      output_graph << "node_before" << p << "_" << r << " -> ";
      output_graph << "node_before" << q << "_" << q << endl; 

      /// disegna arco q_q -> q_q 
      output_graph << "node_before" << q << "_" << q << " -> ";
      output_graph << "node_after" << q << "_" << q << endl; 

      /// disegna arco p_r -> q+1_r
      if (q+1<=r){
      //print_array_graph(A,q+1,r,"node_before",0);
      output_graph << "node_before" << p << "_" << r << " -> ";
      output_graph << "node_before" << q+1 << "_" << r << endl;
      }

      /// per visualizzazione ben organizzata a livelli
      output_graph << "{rank = same; ";	
      if (p<=q-1) output_graph << "node_before" << p << "_" << q-1 <<"; ";
      output_graph << "node_before" << q << "_" << q <<"; ";
      if (q+1<=r) output_graph << "node_before" << q+1 << "_" << r <<";";
      output_graph << "}"<<endl;
    }

    quick_sort(A, p, q - 1);
    quick_sort(A, q + 1, r);

    if (graph){
      /// disegna arco p_q-1 -> p_r
      if (p<=q-1){
        print_array_graph(A,p,q-1,"node_after",0);
        output_graph << "node_after" << p << "_" << q-1 << " -> "; 
        output_graph << "node_after" << p << "_" << r << "\n";
      }
      print_array_graph(A,q,q,"node_after",1);
      /// disegna arco q_q -> p_r
      output_graph << "node_after" << q << "_" << q << " -> "; 
      output_graph << "node_after" << p << "_" << r << "\n";

      /// disegna arco q+1_r -> p_r
      if (q+1<=r){
        print_array_graph(A,q+1,r,"node_after",0);
        output_graph << "node_after" << q+1 << "_" << r << " -> "; 
        output_graph << "node_after" << p << "_" << r << "\n";
      }
      /// per visualizzazione ben organizzata a livelli
      output_graph << "{rank = same; ";	
      if (p<=q-1) output_graph << "node_after" << p << "_" << q-1 <<"; ";
      output_graph << "node_after" << q << "_" << q <<"; ";
      if (q+1<=r) output_graph << "node_after" << q+1 << "_" << r <<";";
      output_graph << "}"<<endl;
    }

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
        }
    }

    return 0;
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

    if (graph){
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g"<<endl; 
        output_graph << "{ rankdir = TB;"<<endl;
        output_graph << "node [shape=plaintext]"<<endl;
    }

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

        if(graph) print_array_graph(A, 0, n-1, "node_before", 0);

        /// algoritmo di sorting
        quick_sort(A, 0, n-1);

        if(graph) print_array_graph(A, 0, n-1, "node_after", 0);

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
