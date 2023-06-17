/**
 * @author Leonardo Ongari
 * @brief Il file 21-graph.txt elenca triple ID1,ID2, conteggio. Ogni ID rappresenta un esame
 * del corso di laurea (21-graph1.txt contiene anche i nomi degli esami).
 * La tripla indica quanti studenti dopo aver superato l'esame ID1, hanno dato ID2 come esame
 * successivo. Inventare un algoritmo che identifichi dei "percorsi" ID1,ID2, ... IDk formati da
 * superamenti ordinati sostenuti da un numero importante di studenti. In alternativa, trovare
 * gruppi di esami che non mostrano preferenze marcate di ordini di superamento.
 *
 * @TODO:
 * - input dei dati + parsing
 * - shortest path "modificato" su ogni nodo
 * - conversione in albero scambiando ordine degli archi previous
 * - DFS per rilevazione di superamenti ordinati
 */

#include <iostream>
#include <fstream>
#include <string>

#define INFTY 1000000

using namespace std;

typedef struct graph_node
{
    int val; /// prossimo nodo
    float w; /// peso dell'arco
    struct graph_node *next;
} graph_node_t;

typedef struct tree_node
{
    int val;
    int cost;
    struct tree_node *children_head;
    struct tree_node *next;
} tree_node_t;

typedef struct list
{
    graph_node_t *head;
} list_t;

typedef struct pair
{
    int cost;
    int graph_index;
} pair_t;

int *V;                 // elenco dei nodi del grafo
int *V_prev;            // nodo precedente dalla visita
float *V_dist;          // distanza da sorgente
pair_t *min_heap_queue; // coda min heap
int *heap_indexes;      // per ogni nodo nel grafo memorizza l'indice nella coda
tree_node_t *tree_root; // albero

list_t **E; /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;
int heap_size = 0;
int max_cost = 0;

ifstream exam_file;

ofstream output_graph; // file di output per grafo
int n_operazione = 0;  // contatore di operazioni per visualizzare i vari step

/// @brief stampa il nodo nel file per dot
/// @param n
void node_print(int n)
{
    /// calcolo massima distanza (eccetto infinito)
    float max_d = 0;
    for (int i = 0; i < n_nodi; i++)
        if (max_d < V_dist[i] && V_dist[i] < INFTY)
            max_d = V_dist[i];

    output_graph << "node_" << n << "_" << n_operazione << endl;
    output_graph << "[ shape = box; ";

    float col = V_dist[n] / max_d; /// distanza in scala 0..1
    output_graph << "fillcolor = \"0.0 0.0 " << col / 2 + 0.5 << "\"; style=filled; width = 2; ";

    if (V_dist[n] < INFTY)
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: " << V_dist[n] << "\" ];\n";
    else
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: INF\" ];\n";

    graph_node_t *elem = E[n]->head;
    while (elem != NULL)
    { /// disegno arco
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << elem->val << "_" << n_operazione << " [ label=\"" << elem->w << "\", penwidth=" << elem->w / 100 * 10 << " ]\n";
        elem = elem->next;
    }
}

/// @brief genera il file dot per output visivo del grafo
void graph_print()
{
    for (int i = 0; i < n_nodi; i++)
        node_print(i);
    n_operazione++;
}

/// @brief crea una lista di adiacenza vuota
/// @return puntatore alla lista
list_t *list_new()
{
    list_t *l = new list;

    l->head = NULL;

    return l;
}

/// @brief inscerisce un nuovo arco nella lista di adiacenza
/// @param l lista in cui inserire l'arco (nodo di partenza)
/// @param elem nodo di arrivo
/// @param w peso dell'arco
void list_insert_front(list_t *l, int elem, float w)
{
    // inserisce un elemento all'inizio della lista
    graph_node_t *new_node = new graph_node_t;
    new_node->next = NULL;

    new_node->val = elem;
    new_node->w = w;

    new_node->next = l->head;

    l->head = new_node;
}

/// @brief genera il grafo dai dati prelevati dal file
void init_graph()
{
    string str;

    int ID1, ID2, cost;
    while (exam_file >> ID1 >> ID2 >> cost)
    {
        list_insert_front(E[ID1], ID2, cost);

        if (cost > max_cost)
            max_cost = cost;
    }
}

/// @brief genitore
/// @param n
/// @return indice del genitore, -1 per indicare un indice non esistente
int parent_idx(int n)
{
    if (n == 0)
        return -1;
    return (n - 1) / 2;
}

/// @brief figlio sinistro
/// @param n indice corrente
/// @return indice del figlio sinistro
int child_L_idx(int n)
{
    if (2 * n + 1 >= heap_size)
        return -1;
    return 2 * n + 1;
}

/// @brief figlio destro
/// @param n indice corrente
/// @return indice del figlio destro
int child_R_idx(int n)
{
    if (2 * n + 2 >= heap_size)
        return -1;
    return 2 * n + 2;
}

/// @brief restituisce 0 se il nodo in posizione n e' un nodo interno (almeno un figlio)
///         restituisce 1 se il nodo non ha figli
int is_leaf(int n)
{
    return (child_L_idx(n) == -1); // non c'e' bisogno di controllare il figlio R
}

/// @brief inserimento in heap + aggiornamento indici
/// @param cost
/// @param graph_index indice di corrispondenza nel grafo
void heap_insert(int cost, int graph_index)
{
    /// inserisco il nuovo nodo con contenuto elem
    /// nell'ultima posizione dell'array

    pair_t new_pair;
    new_pair.cost = cost;
    new_pair.graph_index = graph_index;

    if (heap_size < n_nodi)
    {
        int i = heap_size;
        heap_size++;

        min_heap_queue[i] = new_pair;

        while (i != 0)
        {
            // non sono sulla radice
            if (min_heap_queue[parent_idx(i)].cost <= min_heap_queue[i].cost)
            { /// proprieta' dell' heap e' rispettata -> esco
                return;
            }

            /// il nodo ha un genitore sicuramente >   --> swap + aggiornamento indici
            pair_t t = min_heap_queue[parent_idx(i)];

            min_heap_queue[parent_idx(i)] = min_heap_queue[i];
            int index = min_heap_queue[parent_idx(i)].graph_index;
            heap_indexes[index] = parent_idx(i);

            min_heap_queue[i] = t;
            index = min_heap_queue[i].graph_index;
            heap_indexes[index] = i;

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

/// @brief rimuove l'elemento più piccolo, mantenendo le proprietà min heap e aggiornando gli indici
void heap_remove_min()
{

    if (heap_size <= 0)
    { /// heap vuoto!
        printf("Errore: heap vuoto\n");
        return;
    }

    pair_t minimo = min_heap_queue[0];

    /// scambio la radice con l'ultima foglia a destra + aggiornamento indici
    /// il minimo e' stato spostato in fondo --> pronto per l'eliminazione
    pair_t t = min_heap_queue[0];

    int index = min_heap_queue[0].graph_index;
    min_heap_queue[0] = min_heap_queue[heap_size - 1];
    heap_indexes[index] = -1; // l'elemento verrà rimosso

    index = min_heap_queue[heap_size - 1].graph_index;
    min_heap_queue[heap_size - 1] = t;
    heap_indexes[index] = 0;

    // elimino il minimo (ora in fondo all'array)
    heap_size--;

    //    tree_print_graph(0);  // radice

    /// nella radice c'e' un valore piccolo (minimo?)
    int i = 0; // indice di lavoro (parto dalla root)

    while (!is_leaf(i))
    { /// garantisco di fermarmi alla foglia

        int con_chi_mi_scambio = -1;

        /// controllo il nodo i con il suo figlio L
        if (min_heap_queue[i].cost > min_heap_queue[child_L_idx(i)].cost)
        { // il nodo i e' piu' grande
            /// attivare uno swap (la proprieta' heap non e' rispettata)
            con_chi_mi_scambio = child_L_idx(i);

            if (child_R_idx(i) >= 0 && // esiste il nodo destro
                min_heap_queue[child_L_idx(i)].cost > min_heap_queue[child_R_idx(i)].cost)
            {
                con_chi_mi_scambio = child_R_idx(i);
            }
        }
        else
        { // il nodo e' piu' piccolo del figlio L

            if (child_R_idx(i) >= 0)
            { // esiste il figlio R
                if (min_heap_queue[i].cost > min_heap_queue[child_R_idx(i)].cost)
                { /// attivo lo swap
                    con_chi_mi_scambio = child_R_idx(i);
                }
                else
                    break;
            }
            else
                break;
        }

        /// swap tra i e con_chi_mi_scambio + aggiornamento indici
        pair_t t = min_heap_queue[i];

        min_heap_queue[i] = min_heap_queue[con_chi_mi_scambio];
        int index = min_heap_queue[i].graph_index;
        heap_indexes[index] = i;

        min_heap_queue[con_chi_mi_scambio] = t;
        index = min_heap_queue[con_chi_mi_scambio].graph_index;
        heap_indexes[index] = con_chi_mi_scambio;

        i = con_chi_mi_scambio; // "incremento" per ciclo while

        // tree_print_graph(0);  // radice
        // n_operazione++;
    }
}

/// @brief modifica il costo del nodo in coda, mantenendo la proprietà min heap e aggiornando gli indici
/// @param indice_nodo indice di heap
/// @param key nuovo costo
void decrease_key(int indice_nodo, int key)
{
    // key = nuovo valore

    if (indice_nodo < 0 || indice_nodo >= heap_size)
    {
        cout << endl;
        printf("Nodo non esistente ");
        cout << "(" << indice_nodo << ")" << endl;
        cout << "graph_index: " << min_heap_queue[indice_nodo].graph_index << endl;
        cout << "heap_size: " << heap_size << endl;
        return;
    }

    if (min_heap_queue[indice_nodo].cost < key)
    {
        cout << endl;
        printf("la chiave corrente non e' piu' piccola! ");
        cout << min_heap_queue[indice_nodo].cost << " - " << key << endl;
        cout << "(" << indice_nodo << ")" << endl;
        cout << "graph_index: " << min_heap_queue[indice_nodo].graph_index << endl;
        return;
    }

    min_heap_queue[indice_nodo].cost = key;

    int i = indice_nodo;
    while (i != 0)
    { // non sono sulla radice
        if (min_heap_queue[parent_idx(i)].cost <= min_heap_queue[i].cost)
        { /// proprieta' dell' heap e' rispettata -> esco
            return;
        }

        /// il nodo ha un genitore sicuramente >   --> swap
        pair_t t = min_heap_queue[parent_idx(i)];

        min_heap_queue[parent_idx(i)] = min_heap_queue[i];
        int index = min_heap_queue[parent_idx(i)].graph_index;
        heap_indexes[index] = parent_idx(i);

        min_heap_queue[i] = t;
        index = min_heap_queue[i].graph_index;
        heap_indexes[index] = i;

        i = parent_idx(i); /// "incremento" per ciclo while
    }
}

/// @brief trova e memorizza, per ogni nodo del grafo, i percorsi migliori partendo da source:n
/// @param n nodo da cui partire (indice)
void shortest_path(int n)
{
    V_dist[n] = 0; // la distanza da se stesso è 0

    /// inserimento iniziale corretto solo se l'inizializzazione delle distanze è INFTY
    for (int i = 0; i < n_nodi; i++)
    {
        heap_insert(V_dist[i], i);
    }

    while (heap_size != 0)
    {
        graph_print();

        /// trova il minimo in coda - array
        // float best_dist = INFTY;
        // int best_idx = -1;
        // for (int i = 0; i < n_nodi; i++) {
        //     if (V_visitato[i] == 0 && V_dist[i] < best_dist) { /// nodo e' in coda e e' migliore del nodo corrente
        //         best_dist = V_dist[i];
        //         best_idx = i;
        //     }
        // }

        int best_idx = min_heap_queue[0].graph_index;
        if (best_idx >= 0)
        {
            /// estrai dalla coda
            int u = best_idx;
            heap_remove_min();
            // q_size--;

            /// esploro la lista di adiacenza
            graph_node_t *elem = E[u]->head;
            while (elem != NULL)
            {
                int v = elem->val; /// arco u --> v

                /// costo per arrivare al nuovo nodo passando per u
                float alt = V_dist[u] + elem->w;
                if (alt < V_dist[v])
                { // il percorso SRC-->u-->v migliora il percorso attuale SRC-->v
                    /// TASK: ottenere l'indice del nodo in tempo ragionevole
                    /// SOLUTION: mantenere aggiornate le corrispondenze heap<->grafo nei metodi

                    int indice_nodo = heap_indexes[v];
                    int key = alt;
                    if (indice_nodo >= 0)
                    {
                        decrease_key(indice_nodo, key);

                        V_dist[v] = alt;
                        V_prev[v] = u;
                    }
                }
                elem = elem->next;
            }
        }
        else
        { /// coda non vuota E nodi non raggiungibili ---> FINITO
            break;
        }
    }
}

void delete_tree(tree_node_t *elem)
{
    if (elem != NULL)
    {
        if (elem->children_head != NULL)
        {
            tree_node_t *child = elem->children_head;

            while (child != NULL)
            {
                elem->children_head = child->next;
                delete_tree(child);
                child = child->next;
            }

            delete elem->children_head;
            delete elem;
        }
        else
        {
            delete elem;
        }
    }
}

/// @brief riporta le componenti del grafo ai valori iniziali per calcolare shortest path su un nodo differente
void reset_graph()
{
    delete[] V_prev;
    delete[] V_dist;
    delete[] min_heap_queue;

    delete_tree(tree_root);

    V_prev = new int[n_nodi];
    V_dist = new float[n_nodi];
    min_heap_queue = new pair_t[n_nodi];

    for (int i = 0; i < n_nodi; i++)
    {
        heap_indexes[i] = i;
        V_prev[i] = -1;    // non c'e' precedente
        V_dist[i] = INFTY; // infinito
    }
}

/// @brief inverte i pesi degli archi in modo da adattare i dati per lanciare shortest path
void reverse_weight()
{
    for (int i = 0; i < n_nodi; i++)
    {
        /// esploro la lista di adiacenza
        graph_node_t *elem = E[i]->head;
        while (elem != NULL)
        {
            elem->w = max_cost - elem->w;
            elem = elem->next;
        }
    }
}

void print_node(tree_node_t *elem)
{
    cout << "STAMPA DEL NODO:" << endl;
    cout << "parent { cost: " << elem->cost << " val: " << elem->val << " }" << endl;

    if (elem->children_head != NULL)
    {
        tree_node_t *child = elem->children_head;
        while (child != NULL)
        {
            cout << "child { cost: " << child->cost << " val: " << child->val << " }" << endl;
            child = child->next;
        }
    }
}

/// @brief adds child at the end of the list
/// @param parent
/// @param child_idx
void add_children(tree_node_t *parent, int child_idx)
{
    tree_node_t *new_child = new tree_node_t;
    new_child->cost = V_dist[child_idx];
    new_child->val = child_idx;
    new_child->next = new tree_node_t;
    new_child->next = NULL;

    // cout << "new_child { cost: " << new_child->cost << " val: " << new_child->val << " }" << endl;

    if (parent->children_head == NULL) // primo inserimento
    {
        parent->children_head = new tree_node_t;
        parent->children_head = new_child;
    }
    else // aggiunta in coda
    {
        tree_node_t *elem = parent->children_head;
        while (elem->next != NULL)
        {
            elem = elem->next;
        }

        elem->next = new_child;
    }
}

void print_tree(tree_node_t *node)
{
    if (node == NULL)
        return;

    cout << "pre: nodo " << node->val << " di costo " << node->cost << endl;

    if (node->children_head != NULL)
    {
        tree_node_t *elem = node->children_head;
        while (elem != NULL)
        {
            print_tree(elem->next);
        }
    }
    else
    {
        cout << "post: nodo " << node->val << " di costo " << node->cost << endl;
    }
}

/// @brief crea l'albero partendo dai dati generati dall'algoritmo shortest path lanciato sul nodo n
/// @param n indice del nodo su cui è stato eseguito shortest path
void create_tree(tree_node_t *elem)
{
    cout << "creo albero su indice " << elem->val << endl;
    for (int i = 0; i < n_nodi; i++)
    {
        if (V_prev[i] == elem->val)
        {
            // manipolare V_prev e V_dist per costruire l'albero
            add_children(elem, i);
        }
    }

    // print_node(elem);

    tree_node_t *child = elem->children_head;
    while (child != NULL)
    {
        // create_tree(child);
        child = child->next;
    }
}

int main(int argc, char **argv)
{
    exam_file.open("21-graph.txt", ios::app);
    output_graph.open("graph.dot");
    output_graph << "digraph g" << endl;
    output_graph << "{ " << endl;
    output_graph << "node [shape=none]" << endl;
    output_graph << "rankdir=\"LR\"" << endl;

    n_nodi = 49; // in alternativa si può consultare il file (numero massimo tra ID1 e ID2 per ogni riga)

    V_prev = new int[n_nodi];
    V_dist = new float[n_nodi];
    min_heap_queue = new pair_t[n_nodi];
    heap_indexes = new int[n_nodi];

    E = new list_t *[n_nodi];

    for (int i = 0; i < n_nodi; i++)
    {
        V_prev[i] = -1;    // non c'e' precedente
        V_dist[i] = INFTY; // infinito
        E[i] = list_new();
        heap_indexes[i] = i;
    }

    // cout << "initialization OK" << endl;

    init_graph();

    // cout << "init_graph() OK" << endl;

    reverse_weight();

    // cout << "reverse_weight() OK" << endl;

    for (int i = 0; i < n_nodi; i++)
    {
        shortest_path(i); /// lanciare shortest path modificato

        // cout << i << " shortest_path() OK" << endl;

        // cout << endl
        //      << "Arrays in " << i << ": " << endl;
        // for (int j = 0; j < n_nodi; j++)
        // {
        //     cout << "V_dist[" << j << "]=" << V_dist[j] << endl;
        //     cout << "V_prev[" << j << "]=" << V_prev[j] << endl;
        // }

        tree_root = new tree_node_t;
        tree_root->val = i;
        tree_root->cost = V_dist[i];
        tree_root->next = NULL;
        create_tree(tree_root); /// generare un albero invertendo i dati contenuti in V_prev

        // cout << i << " create_tree() OK" << endl;

        /// lanciare DFS sull'albero per valutare i percorsi migliori dati dalla generazione del grafo

        /// memorizzare i percorsi migliori per confronto

        reset_graph();

        // cout << i << " reset_graph() OK" << endl;

        // cout << endl
        //      << "Array resettati " << i << ": " << endl;
        // for (int j = 0; j < n_nodi; j++)
        // {
        //     cout << "V_dist[" << j << "]=" << V_dist[j] << endl;
        //     cout << "V_prev[" << j << "]=" << V_prev[j] << endl;
        // }
    }

    graph_print();

    output_graph << "}" << endl;
    exam_file.close();
    output_graph.close();
    return 0;
}