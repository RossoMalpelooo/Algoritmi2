// C++ program to find two
// Binary Tree are Isomorphic or not
 
#include <bits/stdc++.h>
using namespace std;
 
/* A binary tree node has data,
pointer to left and right children */
struct node {
    int data;
    struct node* left;
    struct node* right;
};
 
/* function to return if
   tree are isomorphic or not*/
bool isIsomorphic(node* root1, node* root2)
{
    // if Both roots are null
    // then tree is isomorphic
    if (root1 == NULL and root2 == NULL)
        return true;
 
    // check if one node is false
    else if (root1 == NULL or root2 == NULL)
        return false;
 
    queue<node *> q1, q2;
 
    // enqueue roots
    q1.push(root1);
    q2.push(root2);
 
    int level = 0;
    int size;
 
    vector<int> v2;
 
    unordered_map<int, int> mp;
 
    while (!q1.empty() and !q2.empty()) {
 
        // check if no. of nodes are
        // not same at a given level
        if (q1.size() != q2.size())
            return false;
 
        size = q1.size();
 
        level++;
 
        v2.clear();
        mp.clear();
 
        while (size--) {
 
            node* temp1 = q1.front();
            node* temp2 = q2.front();
 
            // dequeue the nodes
            q1.pop();
            q2.pop();
 
            // check if value
            // exists in the map
            if (mp.find(temp1->data) == mp.end())
                mp[temp1->data] = 1;
 
            else
                mp[temp1->data]++;
 
            v2.push_back(temp2->data);
 
            // enqueue the child nodes
            if (temp1->left)
                q1.push(temp1->left);
 
            if (temp1->right)
                q1.push(temp1->right);
 
            if (temp2->left)
                q2.push(temp2->left);
 
            if (temp2->right)
                q2.push(temp2->right);
        }
 
        // Iterate through each node at a level
        // to check whether it exists or not.
        for (auto i : v2) {
 
            if (mp.find(i) == mp.end())
                return false;
 
            else {
                mp[i]--;
 
                if (mp[i] < 0)
                    return false;
 
                else if (mp[i] == 0)
                    mp.erase(i);
            }
        }
 
        // check if the key remain
        if (mp.size() != 0)
            return false;
    }
    return true;
}
 
/* function that allocates a new node with the
given data and NULL left and right pointers. */
node* newnode(int data)
{
    node* temp = new node;
    temp->data = data;
    temp->left = NULL;
    temp->right = NULL;
 
    return (temp);
}
 
/* Driver program*/
 
int main()
{
    // create tree
    struct node* n1 = newnode(1);
    n1->left = newnode(2);
    n1->right = newnode(3);
    n1->left->left = newnode(4);
    n1->right->left = newnode(5);
 
    struct node* n2 = newnode(1);
    n2->left = newnode(2);
    n2->right = newnode(3);
    n2->left->right = newnode(4);
    n2->left->left = newnode(5);
 
    if (isIsomorphic(n1, n2) == true)
        cout << "Yes";
    else
        cout << "No";
 
    return 0;
}