#include <iostream>
#include <stack>
#include <optional>
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

struct Frame{
    // value obtained by computation and to "propagate up" 
    int* upper_result;

    node* A;
    node* B;
    
    // values delegations
    int AL_BL = -1;
    int AR_BR = -1;
    int AL_BR = -1;
    int AR_BL = -1;
};



bool Isomorphism(node* root1, node* root2) {
    stack<Frame*> execution_stack;

    Frame* roots = new Frame;
    roots->A = root1;
    roots->B = root2;

    execution_stack.push(roots);

    while(!execution_stack.empty()) {
        Frame* top = new Frame;
        top = execution_stack.top();

        if(top->A == NULL && top->B == NULL) {
            *(top->upper_result) = 1;
            execution_stack.pop();
        }
        else if(top->A != NULL && top->B == NULL) {
            *(top->upper_result) = 0;
            execution_stack.pop();
        }
        else if(top->B != NULL && top->A == NULL) {
            *(top->upper_result) = 0;
            execution_stack.pop();
        }
        else {

            if(top->AL_BL == 1 && top->AR_BR == 1 || top->AL_BR == 1 && top->AR_BL == 1) {
                if(top != roots)
                    *(top->upper_result) = 1;

                execution_stack.pop();
            } else if((top->AL_BL == 0 || top->AR_BR == 0) && (top->AL_BR == 0 || top->AR_BL == 0)) {
                if(top != roots)
                    *(top->upper_result) = 0;
                    
                execution_stack.pop();
            }

            Frame* new_frame = new Frame;
            new_frame->A = top->A->R;
            new_frame->B = top->B->R;
            new_frame->upper_result = &(top->AR_BR);
            execution_stack.push(new_frame);

            new_frame = new Frame;
            new_frame->A = top->A->L;
            new_frame->B = top->B->R;
            new_frame->upper_result = &(top->AL_BR);
            execution_stack.push(new_frame);

            new_frame = new Frame;
            new_frame->A = top->A->R;
            new_frame->B = top->B->L;
            new_frame->upper_result = &(top->AR_BL);
            execution_stack.push(new_frame);

            new_frame = new Frame;
            new_frame->A = top->A->L;
            new_frame->B = top->B->L;
            new_frame->upper_result = &(top->AL_BL);
            execution_stack.push(new_frame);
        } 
    }

    if(roots->AL_BL == 1 && roots->AR_BR == 1 || roots->AL_BR == 1 && roots->AR_BL == 1) {
        return true;
    } else {
        return false;
    }

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
    cout << iso << endl;
}