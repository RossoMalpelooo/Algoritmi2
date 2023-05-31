/**
 * @author Leonardo Ongari 324034
 * @brief Estendere il codice di base di shortest path, 
 * sostituendo la gestione per l'estrazione del minimo nodo 
 * con l'heap utilizzato nelle lezioni precedenti. 
 * Il punto di attenzione principale e' di garantire la 
 * complessita' log(n) e di recuperare correttamente l'indice 
 * dell'array dell'heap a partire dal codice del nodo (ad esempio nella decrease_key).
*/

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

#define INFTY 1000000

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

int ct_visit = 0; // contatore durante visita

//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node {
    int val; /// prossimo nodo
    float w; /// peso dell'arco
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list {
    node *head;
} list_t;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati lista
//////////////////////////////////////////////////

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int *V;          // elenco dei nodi del grafo 
int *min_heap_queue; // coda di tipo min heap
int *V_visitato; // nodo visitato?
int *V_prev;     // nodo precedente dalla visita
float *V_dist;   // distanza da sorgente

// list_t* E;  /// array con le liste di adiacenza per ogni nodo
list_t **E; /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;
int heap_size = 0;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati grafo
//////////////////////////////////////////////////

/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

list_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node) {
    return (int)((long)node - (long)global_ptr_ref);
}

/// uso -1 per indicare un indice non esistente
int parent_idx(int n) {
    if (n == 0)
        return -1;
    return (n - 1) / 2;
}

int child_L_idx(int n) {
    if (2 * n + 1 >= heap_size)
        return -1;
    return 2 * n + 1;
}

int child_R_idx(int n) {
    if (2 * n + 2 >= heap_size)
        return -1;
    return 2 * n + 2;
}

/// restituisce 0 se il nodo in posizione n e' un nodo interno (almeno un figlio)
/// restituisce 1 se il nodo non ha figli
int is_leaf(int n) {
    return (child_L_idx(n) == -1); // non c'e' bisogno di controllare il figlio R

    /* versione equivalente
    if (child_L_idx(n)==-1)
      return 1;
    return 0;
    */
}

void heap_insert(int elem) {
    /// inserisco il nuovo nodo con contenuto elem
    /// nell'ultima posizione dell'array
    /// ovvero continuo a completare il livello corrente

    if (details)
        printf("Inserisco elemento %d in posizione %d\n", elem, heap_size);

    cout << endl << endl << "heap_size: " << heap_size << endl << endl;
    if (heap_size < n_nodi) {
        int i = heap_size;
        heap_size++;

        min_heap_queue[i] = elem;

        while (i != 0) {                          
            // non sono sulla radice
            if (min_heap_queue[parent_idx(i)] <= min_heap_queue[i]) { /// proprieta' dell' heap e' rispettata -> esco
                if (details)
                    printf("Il genitore ha valore %d <= del nodo %d, esco\n", min_heap_queue[parent_idx(i)], min_heap_queue[i]);
                return;
            }

            if (details)
                printf("Il genitore ha valore %d > del nodo %d, swap\n", min_heap_queue[parent_idx(i)], min_heap_queue[i]);
            /// il nodo ha un genitore sicuramente >   --> swap
            int t = min_heap_queue[parent_idx(i)];
            min_heap_queue[parent_idx(i)] = min_heap_queue[i];
            min_heap_queue[i] = t;

            // tree_print_graph(0); // radice
            // n_operazione++;

            i = parent_idx(i);
        }

    } else {
        printf("Heap pieno!\n");
    }
}

void decrease_key(int indice_nodo, int key) {
    // key = nuovo valore

    if (indice_nodo < 0 || indice_nodo >= heap_size) {
        printf("Nodo non esistente\n");
        return;
    }

    if (min_heap_queue[indice_nodo] < key) {
        printf("la chiave corrente non e' piu' piccola!\n");
        return;
    }

    min_heap_queue[indice_nodo] = key;

    int i = indice_nodo;
    while (i != 0) {                          // non sono sulla radice
        if (min_heap_queue[parent_idx(i)] <= min_heap_queue[i]) { /// proprieta' dell' heap e' rispettata -> esco
            if (details)
                printf("Il genitore ha valore %d <= del nodo %d, esco\n", min_heap_queue[parent_idx(i)], min_heap_queue[i]);
            return;
        }

        if (details)
            printf("Il genitore ha valore %d > del nodo %d, swap\n", min_heap_queue[parent_idx(i)], min_heap_queue[i]);
        /// il nodo ha un genitore sicuramente >   --> swap
        int t = min_heap_queue[parent_idx(i)];
        min_heap_queue[parent_idx(i)] = min_heap_queue[i];
        min_heap_queue[i] = t;

        i = parent_idx(i);
    }
}

int heap_remove_min() {

    if (heap_size <= 0) { /// heap vuoto!
        printf("Errore: heap vuoto\n");
        return -1;
    }

    int minimo = min_heap_queue[0];

    if (details)
        printf("Massimo identificato %d\n", minimo);
    /// scambio la radice con l'ultima foglia a destra
    /// il minimo e' stato spostato in fondo --> pronto per l'eliminazione
    int t = min_heap_queue[0];
    min_heap_queue[0] = min_heap_queue[heap_size - 1];
    min_heap_queue[heap_size - 1] = t;

    // elimino il minimo (ora in fondo all'array)
    heap_size--;

    //    tree_print_graph(0);  // radice

    /// nella radice c'e' un valore piccolo (minimo?)
    int i = 0; // indice di lavoro (parto dalla root)

    while (!is_leaf(i)) { /// garantisco di fermarmi alla foglia

        if (details)
            printf("Lavoro con il nodo in posizione i = %d, valore %d\n", i, min_heap_queue[i]);

        int con_chi_mi_scambio = -1;

        /// controllo il nodo i con il suo figlio L
        if (min_heap_queue[i] > min_heap_queue[child_L_idx(i)]) { // il nodo i e' piu' grande
            /// attivare uno swap (la proprieta' heap non e' rispettata)
            con_chi_mi_scambio = child_L_idx(i);
            if (details)
                printf("Figlio L e' piu' piccolo (valore %d)\n", min_heap_queue[child_L_idx(i)]);

            if (child_R_idx(i) >= 0 && // esiste il nodo destro
                min_heap_queue[child_L_idx(i)] > min_heap_queue[child_R_idx(i)]) {
                con_chi_mi_scambio = child_R_idx(i);
                if (details)
                    printf("Figlio R e' ancora piu' piccolo (valore %d)\n", min_heap_queue[child_R_idx(i)]);
            }
        } else { // il nodo e' piu' piccolo del figlio L

            if (child_R_idx(i) >= 0) {                // esiste il figlio R
                if (min_heap_queue[i] > min_heap_queue[child_R_idx(i)]) { /// attivo lo swap
                    con_chi_mi_scambio = child_R_idx(i);
                    if (details)
                        printf("Figlio R e' piu' piccolo del nodo (valore %d)\n", min_heap_queue[child_R_idx(i)]);
                } else
                    break;
            } else
                break;
        }

        /// swap tra i e con_chi_mi_scambio
        int t = min_heap_queue[i];
        min_heap_queue[i] = min_heap_queue[con_chi_mi_scambio];
        min_heap_queue[con_chi_mi_scambio] = t;

        i = con_chi_mi_scambio;

        // tree_print_graph(0);  // radice
        // n_operazione++;
    }

    return minimo;
}

void node_print(int n) {

    /// calcolo massima distanza (eccetto infinito)
    float max_d = 0;
    for (int i = 0; i < n_nodi; i++)
        if (max_d < V_dist[i] && V_dist[i] < INFTY)
            max_d = V_dist[i];

    output_graph << "node_" << n << "_" << n_operazione << endl;
    output_graph << "[ shape = oval; ";

    if (V_visitato[n] == 1)
        output_graph << "penwidth = 4; ";

    float col = V_dist[n] / max_d; /// distanza in scala 0..1
    output_graph << "fillcolor = \"0.0 0.0 " << col / 2 + 0.5 << "\"; style=filled; ";
    if (V_dist[n] < INFTY)
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: " << V_dist[n] << ", val: " << V[n] << "\" ];\n";
    else
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: INF, val: " << V[n] << "\" ];\n";

    node_t *elem = E[n]->head;
    while (elem != NULL) { /// disegno arco
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << elem->val << "_" << n_operazione << " [ label=\"" << elem->w << "\", len=" << elem->w / 100 * 10 << " ]\n";
        elem = elem->next;
    }

    if (V_prev[n] != -1) { // se c'e' un nodo precedente visitato -> disegno arco

        float len = 0;
        /*
        //cerco arco V_prev[n] --> V[n]
        node_t* elem=E[ V_prev[n] ]->head;
        while (elem!=NULL){
        int v=elem->val; /// arco u --> v
        if (v == V[n])
          len=elem->w;
        elem=elem->next;
          }
        */

        len = 1;
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << V_prev[n] << "_" << n_operazione << " [ color=blue, penwidth=5, len=" << len / 100 * 10 << " ]\n";
    }
}

void graph_print() {
    for (int i = 0; i < n_nodi; i++)
        node_print(i);
    n_operazione++;
}

void list_print(list_t *l) {
    printf("Stampa lista\n");

    if (l->head == NULL) {
        printf("Lista vuota\n");
    } else {
        node_t *current = l->head;

        while (current != NULL) {
            if (!details)
                printf("%d w:%f, ", current->val, current->w);
            else { /// stampa completa
                if (current->next == NULL)
                    printf("allocato in %d [Val: %d, W: %f, Next: NULL]\n",
                           get_address(current),
                           current->val,
                           current->w);
                else
                    printf("allocato in %d [Val: %d, W: %f, Next: %d]\n",
                           get_address(current),
                           current->val,
                           current->w,
                           get_address(current->next));
            }
            current = current->next;
        }
        printf("\n");
    }
}

list_t *list_new(void) {
    list_t *l = new list;
    if (details) {
        printf("Lista creata\n");
    }

    l->head = NULL; //// perche' non e' l.head ?
    if (details) {
        printf("Imposto a NULL head\n");
    }

    return l;
}

void list_insert_front(list_t *l, int elem, float w) {
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->next = NULL;

    new_node->val = elem;
    new_node->w = w;

    new_node->next = l->head;

    l->head = new_node;
}

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void print_array_graph(int *A, int n, string c, int a, int l, int m, int r) {
    /// prepara il disegno dell'array A ed il suo contenuto (n celle)
    /// a e' il codice del nodo e c la stringa
    /// l,m,r i tre indici della bisezione

    // return ;

    output_graph << c << a << " [label=<" << endl;

    /// tabella con contenuto array
    output_graph << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" > " << endl;
    /// indici
    output_graph << "<TR  >";
    for (int j = 0; j < n; j++) {
        output_graph << "<TD ";
        output_graph << ">" << j << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "<TR>";
    // contenuto
    for (int j = 0; j < n; j++) {
        output_graph << "<TD BORDER=\"1\"";
        if (j == m)
            output_graph << " bgcolor=\"#00a000\""; /// valore testato
        else if (j >= l && j <= r)
            output_graph << " bgcolor=\"#80ff80\""; /// range di competenza
        output_graph << ">" << A[j] << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "</TABLE> " << endl;

    output_graph << ">];" << endl;
}

int print_heap_number = 0;
void print_heap_graph() {
    if(graph != 1) return;

    for(int i = 0; i < heap_size; i++) {
        output_graph << "heap_node" << i << "_" << print_heap_number << endl;
        output_graph << "[ shape = oval; ";
        output_graph << "label = \"Idx: " << i << ", val: " << min_heap_queue[i] << "\"];\n";

        if(is_leaf(i) == 0) {
            if(child_L_idx(i) != -1) {
                output_graph << "heap_node" << i << "_" << print_heap_number 
                            << "->heap_node" << child_L_idx(i) << "_" << print_heap_number << "; \n" << endl;

                if(child_R_idx(i) != -1) {
                    output_graph << "heap_node" << i << "_" << print_heap_number << 
                            "->heap_node" << child_R_idx(i) << "_" << print_heap_number << "; \n" << endl;
                }
            }
        }
    }

    print_heap_number++;
}

/// @brief trova e memorizza, per ogni nodo del grafo, i percorsi migliori partendo da source:n
/// @param n nodo da cui partire (indice) 
void shortest_path(int n) {

    /*      V_visitato[i]=0;  // flag = non visitato
      V_prev[i]=-1;  // non c'e' precedente
      V_dist[i]=INFTY;  // infinito
    */

    V_dist[n] = 0;  // la distanza da se stesso è 0

    int q_size = n_nodi; /// contatore degli elementi in coda (V_visitato)

    while (q_size != 0) {

        graph_print();

        /// TASK: eseguire tramite heap
        /// trova il minimo in coda
        float best_dist = INFTY;    
        int best_idx = -1;
        for (int i = 0; i < n_nodi; i++) {
            if (V_visitato[i] == 0 && V_dist[i] < best_dist) { /// nodo e' in coda e e' migliore del nodo corrente
                best_dist = V_dist[i];
                best_idx = i;
                /// TASK: decrease key sul nodo i (i = indice dell'array V)
                /// TASK: trovare corrispondenza tra indici di V e indici di min_heap_queue
            }
        }

        if (best_idx >= 0) {
            /// estrai dalla coda
            int u = best_idx;
            V_visitato[u] = 1;
            q_size--;

            /// esploro la lista di adiacenza
            node_t *elem = E[u]->head;
            while (elem != NULL) {
                int v = elem->val; /// arco u --> v

                /// alt ← dist[u] + Graph.Edges(u, v)
                float alt = V_dist[u] + elem->w; // costo per arrivare al nuovo nodo passando per u
                if (alt < V_dist[v]) {           // il percorso sorgente ---> u --> v migliora il percorso attuale sorgente --> v
                    V_dist[v] = alt;
                    V_prev[v] = u;
                }
                elem = elem->next;
            }

        } else { /// coda non vuota E nodi non raggiungibili ---> FINITO
            q_size = 0;
        }

        print_heap_graph();
    }

    // graph_print();
}

int DFS(int n) {

    graph_print();

    if (details)
        printf("DFS: lavoro sul nodo %d (visitato %d)\n", n, V_visitato[n]);

    if (V_visitato[n])
        return 0;

    V_visitato[n] = 1; // prima volta che incontro questo nodo

    if (details)
        printf("Visito il nodo %d (val %d)\n", n, V[n]);

    /// esploro la lista di adiacenza
    node_t *elem = E[n]->head;
    while (elem != NULL) { /// elenco tutti i nodi nella lista

        /// espando arco  n --> elem->val
        /// quindi DFS(elem->val)
        output_graph << "dfs_" << n << " -> dfs_" << elem->val;
        if (V_visitato[elem->val])
            output_graph << "[color=gray, label = \"" << ct_visit++ << "\"]";
        else
            output_graph << "[color=red, label = \"" << ct_visit++ << "\"]";
        output_graph << endl;

        if (V_visitato[elem->val] == 1)
            return 1;

        int ret = DFS(elem->val);

        if (ret == 1)
            return 1;

        elem = elem->next;
    }

    V_visitato[n] = 2; // abbandono il nodo per sempre
    return 0;
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int parse_cmd(int argc, char **argv) {
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'v') {
            details = 1;
            ok_parse = 1;
        }
        if (argv[i][1] == 'g') {
            graph = 1;
            ok_parse = 1;
        }
    }

    if (argc > 1 && !ok_parse) {
        printf("Usage: %s [Options]\n", argv[0]);
        printf("Options:\n");
        printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
        printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int i, test;

    // init random
    srand((unsigned)time(NULL));

    if (parse_cmd(argc, argv))
        return 1;

    if (graph) {
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g" << endl;
        output_graph << "{ " << endl;
        output_graph << "node [shape=none]" << endl;
        output_graph << "rankdir=\"TB\"" << endl;
        ;
        //    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;
    }

    int N = 10;
    n_nodi = N * N;

    n_nodi = 10;

    //// init nodi
    V = new int[n_nodi];
    V_visitato = new int[n_nodi];
    V_prev = new int[n_nodi];
    V_dist = new float[n_nodi];
    min_heap_queue = new int[n_nodi];

    //// init archi
    E = new list_t *[n_nodi]; //(list_t**)malloc(n_nodi*sizeof(list_t*));

    // costruzione nodi
    for (int i = 0; i < n_nodi; i++) {
        V[i] = 2 * i;
        V_visitato[i] = 0; // flag = non visitato
        V_prev[i] = -1;    // non c'e' precedente
        V_dist[i] = INFTY; // infinito
        heap_insert(V_dist[i]);

        // print_heap_graph();

        E[i] = list_new();

        if (i == 0)
            global_ptr_ref = E[i];

        // int x = i % N;
        // int y = i / N;
        // for (int dx = -2; dx <= 2; dx += 1)
        //     for (int dy = -2; dy <= 2; dy += 1)
        //         if (abs(dx) + abs(dy) >= 1 &&
        //             abs(dx) + abs(dy) <= 1
        //             ) { // limito gli archi ai vicini con 1 variazione assoluta sulle coordinate

        //             int nx = x + dx;
        //             int ny = y + dy;

        //             if (nx >= 0 && nx < N &&
        //                 ny >= 0 && ny < N) { /// coordinate del nuovo nodo sono nel grafo

        //                 int j = nx + N * ny; /// indice del nuovo nodo
        //                 list_insert_front(E[i], j, 15 * sqrt(dx*dx + dy*dy));
        //             }
        //         }
    }

    int partenza = 0;
    int arrivo = n_nodi - 1;
    int w_max = 100;

    /// costruzione archi
    for (int i = 0; i < n_nodi - 1; i++) {
        /// arco costoso
        list_insert_front(E[i], arrivo, w_max - 2 * i);
        /// arco 1
        if (i > 0)
            list_insert_front(E[i-1], i , 1);
    }
    /// TASK: esercitarsi sulla generazione dei dati su cui lavorare tramite algoritmi

    graph_print();

    for (int i = 0; i < n_nodi; i++) {
        printf("Sono il nodo di indice %d nell'array\n", i);
        printf("Il valore del nodo e' %d\n", V[i]);
        printf("La lista di adiacenza e'\n");
        list_print(E[i]);
    }

    shortest_path(0);
    // shortest_path(44);

    if (graph) {
        /// preparo footer e chiudo file
        output_graph << "}" << endl;
        output_graph.close();
        cout << " File graph.dot scritto" << endl
             << "****** Creare il grafo con: neato graph.dot -Tpdf -o graph.pdf" << endl;
    }

    return 0;
}
