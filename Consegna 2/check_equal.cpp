#include <iostream>
#include <fstream>

using namespace std;

struct node {
    int val;
    struct node *L;
    struct node *R;
};

/// file di output per grafo
ofstream output_graph;
int graph = 0;

int parse_cmd(int argc, char **argv) {
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++) {
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

int rec_i = 0;
void build_euler_rec(int* nodes, node* &building_node) {
    if(nodes[rec_i+1] == -1) return;

    if(building_node->val != nodes[rec_i+1]) {
        building_node->L = new node;
        building_node->L->val = nodes[rec_i+1];
        rec_i++;
        build_euler_rec(nodes, building_node->L);
    }
    
    rec_i++;
    
    if(nodes[rec_i+1] == -1) return;

    if(building_node->val != nodes[rec_i+1]) {
        building_node->R = new node;
        building_node->R->val = nodes[rec_i+1];
        rec_i++;
        build_euler_rec(nodes, building_node->R);
    }

    rec_i++;
}

node *build_euler() {
    int dim = 100;
    int euler_nodes[dim];

    for(int i = 0; i < dim; i++)
        euler_nodes[i] = -1;

    cout << "Insert array: " << endl;

    int n;
    int i = 0;
    while(cin >> n) {
        if(n == -1) break;
        euler_nodes[i] = n;
        i++;
    }

    node* root = new node;

    root->val = euler_nodes[0];

    build_euler_rec(euler_nodes, root);

    return root;
}

bool Equal(node* n1, node* n2) {
    
    if(n1 == nullptr && n2 == nullptr) return true;
    if(n1 == nullptr && n2 != nullptr) return false;
    if(n1 != nullptr && n2 == nullptr) return false;
    if(n1 != nullptr && n2 != nullptr)
        if(n1->val == n2->val)
            if(Equal(n1->L, n2->L) && Equal(n1->R, n2->R))
                return true;
        return false;
}

void inOrder(node *n) {

    if (n->L != NULL) {
        inOrder(n->L);
    }

    printf("%d ", n->val);

    if (n->R != NULL) {
        inOrder(n->R);
    }
}

int null_counter = 0;
void print_node_rec(node* n, string prefix) {
    if(n == NULL) return;

    output_graph << prefix << "_" << n << " [label=<" << n->val << ">]" << endl;

    print_node_rec(n->L, prefix);
    if(n->L != NULL) {
        output_graph << prefix << "_" << n << "->" << prefix << "_" << n->L << endl;
    }
    else {
        output_graph << prefix << "_null_" << null_counter << " [shape=point]" << endl;
        output_graph << prefix << "_" << n << "->" << prefix << "_null_" << null_counter++ << endl;
    }    

    print_node_rec(n->R, prefix);
    if(n->R != NULL) {
        output_graph << prefix << "_" << n << "->" << prefix << "_" << n->R << endl;
    }
    else {
        output_graph << prefix << "_null_" << null_counter << " [shape=point]" << endl;
        output_graph << prefix << "_" << n << "->" << prefix << "_null_" << null_counter++ << endl;
    }
}

void print_tree(node* root, string prefix="") {
    print_node_rec(root, prefix);
}

int main(int argc, char** argv) {
    node* n1;
    node* n2;

    srand((unsigned)time(NULL));

    n1 = build_euler();

    rec_i=0;
    n2 = build_euler();

    if (parse_cmd(argc, argv))
        return 1;

    if (graph) {
        output_graph.open("graph.dot");
        output_graph << "digraph g" << endl;
        output_graph << "{ " << endl;
    }

    // stampa albero
    if (graph) {
        print_tree(n1, "A");
        print_tree(n2, "B");
    }

    if (graph) {
        /// preparo footer e chiudo file
        output_graph << "}" << endl;
        output_graph.close();
        cout << " File graph.dot written" << endl
             << "To generate pdf, type: dot graph.dot -Tpdf -o graph.pdf" << endl;
    }

    cout << "n1: ";
    inOrder(n1);
    cout << endl;
    
    cout << "n2: ";
    inOrder(n2);
    cout << endl;

    string equal = Equal(n1, n2) ? "equal" : "not equal";
    cout << equal << endl;;
}