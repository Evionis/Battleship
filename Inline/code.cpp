#include <iostream>
#include <windows.h> 
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <regex>

using namespace std;

bool checkTarget(int locations[25], char board[25], string target)
{
    bool hit = false;
    int trueTarget = 0;
    switch (target[1])
    {
        case '1':
            trueTarget = 0;
            break;
        case '2':
            trueTarget = 5;
            break;
        case '3':
            trueTarget = 10;
            break;
        case '4':
            trueTarget = 15;
            break;
        case '5':
            trueTarget = 20;
            break;
    }
    switch (target[0])
    {
    case 'A':
        trueTarget = trueTarget + 0;
        break;
    case 'B':
        trueTarget = trueTarget + 1;
        break;
    case 'C':
        trueTarget = trueTarget + 2;
        break;
    case 'D':
        trueTarget = trueTarget + 3;
        break;
    case 'E':
        trueTarget = trueTarget + 4;
        break;
    }
    __asm {
        mov eax, trueTarget
        mov ebx, 4
        mul ebx
        mov ebx, locations
        cmp [ebx + eax], 0
        JE Miss
        mov eax, trueTarget
        mov edx, board
        mov [edx + eax], 'X'
        JMP End
        Miss:
            mov eax, trueTarget
            mov edx, board
            mov[edx + eax], 'O'
        End:
    }
    if (board[trueTarget] == 'X')
        hit = true;
    return hit;
}

void printBoard(char locations[]) 
{
    system("CLS");
    //https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int row = 1;
    cout << "  A B C D E" << endl;
    for (int i = 0; i < 25; i++)
    {
        if (i % 5 == 0)
        {
            SetConsoleTextAttribute(hConsole, 15);
            cout << row << " ";
            row++;
        }
        if (locations[i] == '?')
            SetConsoleTextAttribute(hConsole, 15);
        if (locations[i] == 'O')
            SetConsoleTextAttribute(hConsole, 9);
        if (locations[i] == 'X')
            SetConsoleTextAttribute(hConsole, 12);
        cout << locations[i] << " ";
        if (i % 5 == 4) 
            cout << endl;
    }
    SetConsoleTextAttribute(hConsole, 15);
    return;
}

void placeShip(int locations[], int shipSize) 
{
    int startLocation = (rand() % 25);
    int direction = (rand() % 4); // 0 = N, 1 = E, 2 = S, 3 = W
    bool placed = false;
    while (!placed)
    {
        switch (direction)
        {
            case 0:
                __asm {
                    movsx eax, startLocation
                    cmp eax, 14
                    JB Done1
                    mov ebx, 4
                    mul ebx
                    mov edx, 1
                    movsx ecx, shipSize
                    L1:
                        mov ebx, locations
                        mov[ebx + eax], edx
                        sub eax, 20
                        loop L1
                    Done1:
                }
                if (locations[startLocation] == 1)
                    placed = true;
                direction++;
                if (direction == 4)
                    direction = 0;
                break;
            case 1:
                __asm {
                    mov eax, startLocation
                    mov ebx, 5
                    xor edx, edx
                    div ebx
                    cmp edx, 4
                    JE Done2
                    cmp edx, 3
                    JE Done2
                    mov eax, startLocation
                    mov ebx, 4
                    mul ebx
                    mov edx, 1
                    mov ecx, shipSize
                    L2:
                        mov ebx, locations
                        mov[ebx + eax], edx
                        add eax, 4
                        loop L2
                    Done2:
                }
                if (locations[startLocation] == 1)
                    placed = true;
                direction++;
                if (direction == 4)
                    direction = 0;
                break;
            case 2:
                __asm {
                    movsx eax, startLocation
                    cmp eax, 10
                    JGE Done3
                    movsx eax, startLocation
                    mov ebx, 4
                    imul ebx
                    mov edx, 1
                    movsx ecx, shipSize
                    L3:
                        mov ebx, locations
                        mov[ebx + eax], edx
                        add eax, 20
                        loop L3
                    Done3:
                }
                if (locations[startLocation] == 1)
                    placed = true;
                direction++;
                if (direction == 4)
                    direction = 0;
                break;
            case 3:
                __asm {
                    mov eax, startLocation
                    mov ebx, 5
                    xor edx, edx
                    div ebx
                    cmp edx, 0
                    JE Done4
                    cmp edx, 1
                    JE Done4
                    mov eax, startLocation
                    mov ebx, 4
                    mul ebx
                    mov edx, 1
                    mov ecx, shipSize
                    L4:
                        mov ebx, locations
                        mov[ebx + eax], edx
                        sub eax, 4
                        loop L4
                    Done4:
                }
                if (locations[startLocation] == 1)
                    placed = true;
                direction++;
                if (direction == 4)
                    direction = 0;
                break;
        }
    }
    return;
}

int main() 
{
    int actualArray[25] = {0};
    char visualArray[25];
    for (int i = 0; i < 25; i++)
        visualArray[i] = '?';
    string target;
    bool validTarget = false;
    bool playing = true;
    bool won = false;
    bool hit = false;
    int attempts = 0;
    int hits = 0;
    regex searchParams("([A-E])""([1-5])");
    srand((int)time(0));
    while (playing)
    {
        placeShip(actualArray, 3);
        while (!won)
        {
            while (!validTarget)
            {
                printBoard(visualArray);
                cout << "Select a target: ";
                cin >> target;
                for (int i = 0; i < 2; i++)
                    target[i] = toupper(target[i]);
                //https://en.cppreference.com/w/cpp/regex/regex_search
                validTarget = regex_search(target, searchParams);
                if (!validTarget)
                {
                    cout << "Invalid target." << endl;
                    Sleep(2000);
                }
            }
            validTarget = false;
            hit = checkTarget(actualArray, visualArray, target);
            attempts++;
            if (hit) 
            {
                hits++;
                hit = false;
            }  
            if (hits == 3)
                won = true;
        }
        printBoard(visualArray);
        cout << "You destroyed all enemy ships in " << attempts << " moves." << endl;
        playing = false;
    }
    return 0;
}