/**
 * @author Leonardo Ongari
 * @brief: Sulla base dell'algoritmo di Longest Common Subsequence, implementare un esempio
 *  in grado di confrontare codice sorgente. Caricare due file di testo e dare in output il miglior
 *  risultato con queste proprietà: match di caratteri identici "{};=&|" hanno score 5,
 *  cifre aritmetiche hanno score 3, match di caratteri (case sensitive) score 2, tab e spazi
 *  hanno costo 0 (nel caso in cui ci sia una cancellazione nell'altra stringa).
 *  Provare varianti dei pesi per ottenere un match di struttura di codice tollerante a
 *  rinomina di variabili.
 *
 *  Nel contesto, "tab e spazi hanno costo 0" significa che non viene assegnato alcun punteggio
 *  o costo aggiuntivo quando un carattere di tabulazione (tab) o uno spazio vengono confrontati
 *  tra i due file di testo. Quindi, se uno dei file contiene un tab o uno spazio mentre l'altro
 *  non lo ha, non verrà penalizzato né assegnato un punteggio negativo. Questo permette di
 *  considerare gli spazi e le tabulazioni come equivalenti nella valutazione della similarità
 *  tra i due codici sorgente.
 */

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stack>

using namespace std;

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

int **M;

int n = 0; /// dimensione dell'array

const int MAX_DIM = 50 * 1000;

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

int ct_visit = 0; // contatore durante visita

void print_array(int *A, int dim)
{
    for (int j = 0; j < dim; j++)
    {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int parse_cmd(int argc, char **argv)
{
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][1] == 'v')
        {
            details = 1;
            ok_parse = 1;
        }
        if (argv[i][1] == 'g')
        {
            graph = 1;
            ok_parse = 1;
        }
    }

    if (argc > 1 && !ok_parse)
    {
        printf("Usage: %s [Options]\n", argv[0]);
        printf("Options:\n");
        printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
        printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
        return 1;
    }

    return 0;
}

bool is_special(char symbol)
{
    if (symbol == '{' || symbol == '}' || symbol == ';' || symbol == '=' || symbol == '&' || symbol == '|')
        return true;
    else
        return false;
}

bool is_number(char symbol)
{
    if (symbol >= 48 && symbol <= 57)
        return true;
    else
        return false;
}

bool is_char(char symbol)
{
    if (symbol >= 'a' && symbol <= 'z' || symbol >= 'A' && symbol <= 'Z')
        return true;
    else
        return false;
}

int is_match(char temp1, char temp2)
{
    if (is_special(temp1) && temp1 == temp2)
        return 5;
    if (is_number(temp1) && temp1 == temp2)
        return 3;
    if (is_char(temp1) && temp1 == temp2)
        return 2;

    return 0;
}

int main(int argc, char **argv)
{
    int i, test;

    srand((unsigned)time(NULL));

    if (parse_cmd(argc, argv))
        return 1;

    ifstream file1;
    ifstream file2;

    file1.open("../Consegna 4/bellman-ford.cpp");
    file2.open("../Consegna 4/heap-shortest-path.cpp");

    if (!file1 || !file2)
    {
        cout << "ERRORE IN APERTURA FILE:" << endl;
        cout << strerror(errno) << endl;
        return -1;
    }

    char *str1 = new char[MAX_DIM];
    char *str2 = new char[MAX_DIM];

    /// assegnazione stringhe
    char c;
    int s1 = 0;
    while (!file1.eof())
    {
        file1 >> c;
        str1[s1] = c;
        s1++;
    }

    int s2 = 0;
    while (!file2.eof())
    {
        file2 >> c;
        str2[s2] = c;
        s2++;
    }

    M = new int *[s1 + 1];
    for (i = 0; i < s1 + 1; i++)
        M[i] = new int[s2 + 1];

    for (int i = 0; i < s1 + 1; i++)
        M[i][0] = 0;
    for (int j = 0; j < s2 + 1; j++)
        M[0][j] = 0;

    for (int i = 1; i < s1 + 1; i++)
        for (int j = 1; j < s2 + 1; j++)
        {

            /// sottostringa (con maiuscole/minuscole)
            int match = is_match(str1[i - 1], str2[j - 1]);

            // /// calcolo costo sottostringa (con maiuscole/minuscole)
            // int match = 0;
            // if (str1[i - 1] == str2[j - 1])
            //     match = 1;
            // else {
            //     char temp1 = str1[i - 1];
            //     if (temp1 >= 'a' && temp1 <= 'z')
            //         temp1 -= abs('A' - 'a');
            //     char temp2 = str2[j - 1];
            //     if (temp2 >= 'a' && temp2 <= 'z')
            //         temp2 -= abs('A' - 'a');
            //     if (temp1 == temp2)
            //         match = 1;
            // }

            // };1=b{0==a}}{
            // =b{0==a}}{

            /// sottosequenza
            if (match)
            { // match
              /// M(i, j) ← 1 + M(i − 1, j − 1)
                M[i][j] = match + M[i - 1][j - 1];
            }
            else
            { // mismatch
                // M(i, j) ← max(M(i − 1, j), M(i, j − 1))
                int max = M[i - 1][j];
                if (max < M[i][j - 1])
                    max = M[i][j - 1];
                M[i][j] = max;
            }

            /// sottostringa
            // if (match) { // match
            //              /// M(i, j) ← 1 + M(i − 1, j − 1)
            //     M[i][j] = 1 + M[i - 1][j - 1];
            // } else { // mismatch
            //     M[i][j] = 0;
            // }
        }

    for (int i = 0; i < s1 + 1; i++)
    {
        if (i == 0)
        {
            // printf("    ");
            // for (int j = 0; j < s2 + 1; j++)
                // printf("%5d ", j);
            // printf("\n");
        }

        // printf("%5d: ", i);

        for (int j = 0; j < s2 + 1; j++)
        {
            // printf("%5d ", M[i][j]);
        }
        // printf("\n");
    }

    i = s1;
    int j = s2;

    stack<char> lcs;

    // ricostruzione sottosequenza
    while (i > 0 || j > 0)
    {
        // printf("posizione: %d %d\n", i, j);
        if (i > 0 && j > 0 && is_match(str1[i - 1], str2[j - 1]))
        {
            // printf("%c", str1[i - 1]);
            lcs.push(str1[i-1]);
            i--;
            j--;
        }
        else
        {
            // if M(i,j) = M(i−1,j) then i ← i−1 else j ← j−1
            if (i > 0 && M[i][j] == M[i - 1][j])
                i--;
            else
                j--;
        }
    }
    printf("\n");

    // for(int i = 0; i < s1; i++) {
    //     cout << str1[i];
    // }

    // for(int i = 0; i < s1; i++) {
    //     cout << str2[i];
    // }

    while(!lcs.empty()) {
        cout << lcs.top();
        lcs.pop();
    }

    cout << M[s1][s2] << endl;
    cout << max(s1, s2) << endl;
    cout << "Percentuale corrispondenza: " << ((float)M[s1][s2] / (max(s1, s2) * 5)) * 100 << "%" << endl;

    file1.close();
    file2.close();

    return 0;
}
