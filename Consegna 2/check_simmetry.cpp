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

// In this case, isomorphism is intended as simmetry 
bool Isomorphism(node* root1, node* root2) {
    stack<Pair*> stack1;
    stack<Pair*> stack2;

    Pair* roots = new Pair;
    roots->A = root1;
    roots->B = root2;

    stack1.push(roots);
    stack2.push(roots);

    bool isomorphism1 = true;
    bool isomorphism2 = true;

    bool isomorphism = isomorphism1 || isomorphism2;

    while(isomorphism && (!stack2.empty() || !stack2.empty())) {

        /// First combination (A->L, B->L) - (A->R, B->R)
        if(!stack1.empty()) {

            // retrieve nodes from branches

            Pair *top1 = new Pair;
            top1 = stack1.top();
            stack1.pop();

            if(top1->A == NULL && top1->B == NULL) isomorphism1 = true;
            else if(top1->A == NULL && top1->B != NULL) isomorphism1 = false;
            else if(top1->B == NULL && top1->A != NULL) isomorphism1 = false;
            else if(children(top1->A) == children(top1->B)) {
                isomorphism1 = true;

                // right branch stack 1
                Pair *new_pair = new Pair;
                new_pair->A = top1->A->R;
                new_pair->B = top1->B->R;
                stack1.push(new_pair);

                // left branch stack 1
                new_pair = new Pair;
                new_pair->A = top1->A->L;
                new_pair->B = top1->B->L;
                stack1.push(new_pair);
            } else isomorphism1 = false;
        }

        /// Second combination (A->L, B->R) - (A->R, B->L)
        if(!stack2.empty()) {
            Pair *top2 = new Pair;
            top2 = stack2.top();
            stack2.pop();

            if(top2->A == NULL && top2->B == NULL) isomorphism2 = true;
            else if(top2->A == NULL && top2->B != NULL) isomorphism2 = false;
            else if(top2->B == NULL && top2->A != NULL) isomorphism2 = false;
            else if(children(top2->A) == children(top2->B)) {
                isomorphism2 = true;

                // right branch stack 1
                Pair *new_pair = new Pair;
                new_pair->A = top2->A->R;
                new_pair->B = top2->B->L;
                stack2.push(new_pair);

                // left branch stack 1
                new_pair = new Pair;
                new_pair->A = top2->A->L;
                new_pair->B = top2->B->R;
                stack2.push(new_pair);
            } else isomorphism2 = false;
        }

        isomorphism = isomorphism1 || isomorphism2;
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