#include <iostream>
#include <fstream> //file handling library
#include <string>
#include <vector>
#include <sstream>
using namespace std;
string GetUserInput();
int *rowvalues(int size, string line);
void printMatrix(int **arr, int dim);

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
    
    printMatrix(matrix, dim);

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
