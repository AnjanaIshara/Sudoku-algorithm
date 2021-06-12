#include <iostream>
#include <fstream> //file handling library
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include<ctime>
#include <iomanip> 
using namespace std;

#define UNASSIGNED 0

string GetUserInput();
int *rowvalues(int size, string line);
void printMatrix(int **arr, int dim);
bool SolveSudoku(int **grid, int dim);
bool FindUnassignedLocation(int **grid, int dim, int &row, int &col);
bool isSafe(int **grid, int dim, int row, int col, int num);
bool UsedInRow(int **grid, int dim, int row, int num);
bool UsedInCol(int **grid, int dim, int col, int num);
bool UsedInBox(int **grid, int dim, int boxStartRow, int boxStartCol, int num);
int main(int argc, char *argv[])
{
    ifstream inputFile;
    string path;
    if (argc == 2)
    {
        path = argv[1];
        inputFile.open(path);
        if (!inputFile)
        {
            path = GetUserInput();
            inputFile.open(path);
        }
    }
    else
    {
        path = GetUserInput();
        inputFile.open(path);
    }

    //Getting the dimensions determining whether the matrix is 9x9 or 16x16
    time_t start, end;
    time(&start);
    string line;
    getline(inputFile, line);
    string strNumber;
    vector<string> tokens;
    stringstream check1(line);
    while (getline(check1, strNumber, ' '))
    {
        tokens.push_back(strNumber);
    }
    int dim = tokens.size();
    cout << "The size of the matrix is : " << dim << endl;
    //Creating an array based on the dimensions
    int **matrix = new int *[dim];
    int row = 0;

    inputFile.close(); //closing and opening the same file to get the cursor inthe starting position
    inputFile.open(path);
    while (getline(inputFile, line))
    {
        matrix[row] = rowvalues(dim, line);
        row++;
    }
    inputFile.close();
    // printing the array in console
    //printMatrix(matrix, dim);
    if (SolveSudoku(matrix, dim) == true)
        printMatrix(matrix, dim);
    else
        cout << "No solution exists" << endl;
time(&end);
 double time_taken = double(end - start);
    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;
    return 0;
}
string GetUserInput()
{
    string inputString;
    cout << "Enter the relevant path of the input file: ";
    cin >> inputString;
    ifstream temInputFile(inputString);

    while (!temInputFile)
    {
        cout << "Entered relevant path of the input file does not exist please try again: ";
        cin >> inputString;
        temInputFile.open(inputString);
    }
    temInputFile.close();
    return inputString;
}
int *rowvalues(int size, string line)
{
    int *arr = new int[size];
    string strNumber;
    stringstream check1(line);
    int i = 0;
    while (getline(check1, strNumber, ' '))
    {
        arr[i] = stoi(strNumber);
        i++;
    }
    return arr;
}
void printMatrix(int **arr, int dim)
{
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            cout << arr[i][j] << "  ";
        }
        cout << endl;
    }
}

bool SolveSudoku(int **grid, int dim)
{
    int row, col;
    if (!FindUnassignedLocation(grid, dim, row, col))
        return true;
    for (int num = 1; num <= dim; num++)
    {
        if (isSafe(grid, dim, row, col, num))
        {
            grid[row][col] = num;
            if (SolveSudoku(grid, dim))
                return true;
            grid[row][col] = UNASSIGNED;
        }
    }
    return false;
}

bool FindUnassignedLocation(int **grid, int dim, int &row, int &col)
{
    for (row = 0; row < dim; row++)
        for (col = 0; col < dim; col++)
            if (grid[row][col] == UNASSIGNED)
                return true;
    return false;
}
bool isSafe(int **grid, int dim, int row, int col, int num)
{
    return !UsedInRow(grid, dim, row, num) && !UsedInCol(grid, dim, col, num) &&
           !UsedInBox(grid, dim, row - row % int(sqrt(dim)), col - col % int(sqrt(dim)), num);
}
bool UsedInRow(int **grid, int dim, int row, int num)
{
    for (int col = 0; col < dim; col++)
        if (grid[row][col] == num)
            return true;
    return false;
}
bool UsedInCol(int **grid, int dim, int col, int num)
{
    for (int row = 0; row < dim; row++)
        if (grid[row][col] == num)
            return true;
    return false;
}
bool UsedInBox(int **grid, int dim, int boxStartRow, int boxStartCol, int num)
{
    for (int row = 0; row < int(sqrt(dim)); row++)
        for (int col = 0; col < int(sqrt(dim)); col++)
            if (grid[row + boxStartRow][col + boxStartCol] == num)
                return true;
    return false;
}
