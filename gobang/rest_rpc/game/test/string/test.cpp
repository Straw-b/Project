#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    char board[5][5];
    memset(board, 'a', sizeof(board));

    string s;
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            s.push_back(board[i][j]);
        }
    }

    cout << s << endl;
    return 0;
}
