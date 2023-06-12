#include <iostream>

using namespace std;

bool is_number(char symbol)
{
    if (symbol >= 48 && symbol <= 57)
        return true;
    else
        return false;
}

int main() {
    char c = 48;

    for(int i = 0; i < 100; i++) {
        c = i;
        cout << is_number(c) << endl;
    }

    return 0;
}