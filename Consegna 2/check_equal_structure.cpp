#include <iostream>
#include <stack>
#include <fstream>

using namespace std;

struct node {
    int val;
    struct node *L;
    struct node *R;
};

struct Pair{
    node* A;
    node* B;
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

int children(node* node) {
    if(node == NULL) return -1;

    int children = 0;

    if(node->L != NULL) children++;
    if(node->R != NULL) children++;

    return children;
}

// In this case, isomorphism is intended as equal tree structure
bool Isomorphism(node* root1, node* root2) {
    stack<Pair*> stack;

    Pair* roots = new Pair;
    roots->A = root1;
    roots->B = root2;

    stack.push(roots);

    bool isomorphism = true;
    while(isomorphism && !stack.empty()) {
        // retrieve nodes from branch (left should be first cause of preorder and LIFO stack)
        Pair *top = new Pair;
        top = stack.top();
        stack.pop();

        if(top->A == NULL && top->B == NULL) isomorphism = true;
        else if(top->A == NULL && top->B != NULL) isomorphism = false;
        else if(top->B == NULL && top->A != NULL) isomorphism = false;
        else if(children(top->A) == children(top->B)) {
            isomorphism = true;

            // right branch
            Pair *new_pair = new Pair;
            new_pair->A = top->A->R;
            new_pair->B = top->B->R;
            stack.push(new_pair);

            // left branch
            new_pair = new Pair;
            new_pair->A = top->A->L;
            new_pair->B = top->B->L;
            stack.push(new_pair);
        }
    }

    if(isomorphism) return true;
    else return false;
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

int main(int argc, char **argv) {
    node* n1;
    node* n2;

    srand((unsigned)time(NULL));

    cout << "Note: Correct number of nodes and different values in PARENT-CHILD pairs must be granted in order to get" << endl;
    cout << "       a successful building of the binary tree from Euler Tour output. Type '-1' to stop the input." << endl;

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

    string iso = Isomorphism(n1, n2) ? "isomorphism found" : "isomorphism NOT found";
    cout << iso << endl;;
}