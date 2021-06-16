#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
#include<fstream>
#include<vector>
#include<sstream>
#include <regex>
#define MAX_K 1000


int SIZE;
std::string path;

struct Node
{

    Node *left;
    Node *right;
    Node *up;
    Node *down;
    Node *head;

    int size;     //used for Column header
    int rowID[3]; //used to identify row in order to map solutions to a sudoku grid
                  //ID Format: (Candidate, Row, Column)
};

struct Node Head;
struct Node *HeadNode = &Head;
struct Node *solution[MAX_K];
struct Node *orig_values[MAX_K];
bool** matrix;
bool isSolved = false;
void MapSolutionToGrid(int** Sudoku);
void PrintGrid(int** Sudoku);
std::string GetUserInput();
int *rowvalues(int size, std::string line);

clock_t timer, timer2;

//===============================================================================================================//
//---------------------------------------------DLX Functions-----------------------------------------------------//
//===============================================================================================================//

void coverColumn(Node *col)
{
    col->left->right = col->right;
    col->right->left = col->left;
    for (Node *node = col->down; node != col; node = node->down)
    {
        for (Node *temp = node->right; temp != node; temp = temp->right)
        {
            temp->down->up = temp->up;
            temp->up->down = temp->down;
            temp->head->size--;
        }
    }
}

void uncoverColumn(Node *col)
{
    for (Node *node = col->up; node != col; node = node->up)
    {
        for (Node *temp = node->left; temp != node; temp = temp->left)
        {
            temp->head->size++;
            temp->down->up = temp;
            temp->up->down = temp;
        }
    }
    col->left->right = col;
    col->right->left = col;
}

void search(int k)
{

    if (HeadNode->right == HeadNode)
    {
        timer2 = clock() - timer;
        int **Grid= new int*[SIZE];
        for(int i = 0; i < SIZE; ++i){
            Grid[i] = new int[SIZE];

        }
        MapSolutionToGrid(Grid);
        PrintGrid(Grid);
        std::cout << "Time Elapsed: " << (float)timer2 / CLOCKS_PER_SEC << " seconds.\n\n";

        timer = clock();
        isSolved = true;
        return;
    }

    //Choose Column Object Deterministically: Choose the column with the smallest Size
    Node *Col = HeadNode->right;
    for (Node *temp = Col->right; temp != HeadNode; temp = temp->right)
        if (temp->size < Col->size)
            Col = temp;

    coverColumn(Col);

    for (Node *temp = Col->down; temp != Col; temp = temp->down)
    {
        solution[k] = temp;
        for (Node *node = temp->right; node != temp; node = node->right)
        {
            coverColumn(node->head);
        }

        search(k + 1);

        temp = solution[k];
        solution[k] = NULL;
        Col = temp->head;
        for (Node *node = temp->left; node != temp; node = node->left)
        {
            uncoverColumn(node->head);
        }
    }

    uncoverColumn(Col);
}

//===============================================================================================================//
//----------------------Functions to turn a Sudoku grid into an Exact Cover problem -----------------------------//
//===============================================================================================================//

//--------------------------BUILD THE INITIAL MATRIX CONTAINING ALL POSSIBILITIES--------------------------------//
void BuildSparseMatrix(bool** matrix)
{

    //Constraint 1: There can only be one value in any given cell
    int j = 0, counter = 0;
    for (int i = 0; i < (SIZE * SIZE * SIZE); i++)
    { //iterate over all rows
        matrix[i][j] = 1;
        counter++;
        if (counter >= SIZE)
        {
            j++;
            counter = 0;
        }
    }

    //Constraint 2: There can only be one instance of a number in any given row
    int x = 0;
    counter = 1;
    for (j = (SIZE * SIZE); j < 2 * (SIZE * SIZE); j++)
    {
        for (int i = x; i < counter * (SIZE * SIZE); i += SIZE)
            matrix[i][j] = 1;

        if ((j + 1) % SIZE == 0)
        {
            x = counter * (SIZE * SIZE);
            counter++;
        }
        else
            x++;
    }

    //Constraint 3: There can only be one instance of a number in any given column
    j = 2 * (SIZE * SIZE);
    for (int i = 0; i < (SIZE * SIZE * SIZE); i++)
    {
        matrix[i][j] = 1;
        j++;
        if (j >= 3 * (SIZE * SIZE))
            j = 2 * (SIZE * SIZE);
    }

    //Constraint 4: There can only be one instance of a number in any given region
    x = 0;
    for (j = 3 * (SIZE * SIZE); j < (4 * SIZE * SIZE); j++)
    {

        for (int l = 0; l < int(sqrt((double)SIZE)); l++)
        {
            for (int k = 0; k < int(sqrt((double)SIZE)); k++)
                matrix[x + l * SIZE + k * (SIZE * SIZE)][j] = 1;
        }

        int temp = j + 1 - 3 * (SIZE * SIZE);

        if (temp % (int)(int(sqrt((double)SIZE)) * SIZE) == 0)
            x += (int(sqrt((double)SIZE)) - 1) * (SIZE * SIZE) + (int(sqrt((double)SIZE)) - 1) * SIZE + 1;
        else if (temp % SIZE == 0)
            x += SIZE * (int(sqrt((double)SIZE)) - 1) + 1;
        else
            x++;
    }
}

//-------------------BUILD A TOROIDAL DOUBLY LINKED LIST OUT OF THE SPARSE MATRIX-------------------------//
void BuildLinkedList(bool** matrix)
{

    Node *header = new Node;
    header->left = header;
    header->right = header;
    header->down = header;
    header->up = header;
    header->size = -1;
    header->head = header;
    Node *temp = header;

    //Create all Column Nodes
    for (int i = 0; i < (4 * SIZE * SIZE); i++)
    {
        Node *newNode = new Node;
        newNode->size = 0;
        newNode->up = newNode;
        newNode->down = newNode;
        newNode->head = newNode;
        newNode->right = header;
        newNode->left = temp;
        temp->right = newNode;
        temp = newNode;
    }

    int ID[3] = {0, 1, 1};
    //Add a Node for each 1 present in the sparse matrix and update Column Nodes accordingly
    for (int i = 0; i < (SIZE * SIZE * SIZE); i++)
    {
        Node *top = header->right;
        Node *prev = NULL;

        if (i != 0 && i % (SIZE * SIZE) == 0)
        {
            ID[0] -= SIZE - 1;
            ID[1]++;
            ID[2] -= SIZE - 1;
        }
        else if (i != 0 && i % SIZE == 0)
        {
            ID[0] -= SIZE - 1;
            ID[2]++;
        }
        else
        {
            ID[0]++;
        }

        for (int j = 0; j < (4 * SIZE * SIZE); j++, top = top->right)
        {
            if (matrix[i][j])
            {
                Node *newNode = new Node;
                newNode->rowID[0] = ID[0];
                newNode->rowID[1] = ID[1];
                newNode->rowID[2] = ID[2];
                if (prev == NULL)
                {
                    prev = newNode;
                    prev->right = newNode;
                }
                newNode->left = prev;
                newNode->right = prev->right;
                newNode->right->left = newNode;
                prev->right = newNode;
                newNode->head = top;
                newNode->down = top;
                newNode->up = top->up;
                top->up->down = newNode;
                top->size++;
                top->up = newNode;
                if (top->down == top)
                    top->down = newNode;
                prev = newNode;
            }
        }
    }

    HeadNode = header;
}

//-------------------COVERS VALUES THAT ARE ALREADY PRESENT IN THE GRID-------------------------//
void TransformListToCurrentGrid(int** Puzzle)
{
    int index = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (Puzzle[i][j] > 0)
            {
                Node *Col = NULL;
                Node *temp = NULL;
                for (Col = HeadNode->right; Col != HeadNode; Col = Col->right)
                {
                    for (temp = Col->down; temp != Col; temp = temp->down)
                        if (temp->rowID[0] == Puzzle[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j)
                            goto ExitLoops;
                }
            ExitLoops:
                coverColumn(Col);
                orig_values[index] = temp;
                index++;
                for (Node *node = temp->right; node != temp; node = node->right)
                {
                    coverColumn(node->head);
                }
            }
}

//===============================================================================================================//
//----------------------------------------------- Print Functions -----------------------------------------------//
//===============================================================================================================//

void MapSolutionToGrid(int** Sudoku)
{

    for (int i = 0; solution[i] != NULL; i++)
    {
        Sudoku[solution[i]->rowID[1] - 1][solution[i]->rowID[2] - 1] = solution[i]->rowID[0];
    }
    for (int i = 0; orig_values[i] != NULL; i++)
    {
        Sudoku[orig_values[i]->rowID[1] - 1][orig_values[i]->rowID[2] - 1] = orig_values[i]->rowID[0];
    }
}

//---------------------------------PRINTS A SUDOKU GRID OF ANY SIZE---------------------------------------------//
void PrintGrid(int** Sudoku)
{
    std::string ext_border = "+", int_border = "|";
    int counter = 1;
    int additional = 0;
    if (SIZE > 9)
        additional = SIZE;
    for (int i = 0; i < ((SIZE + int(sqrt((double)SIZE)) - 1) * 2 + additional + 1); i++)
    {
        ext_border += '-';

        if (i > 0 && i % ((int(sqrt((double)SIZE)) * 2 + int(sqrt((double)SIZE)) * (SIZE > 9) + 1) * counter + counter - 1) == 0)
        {
            int_border += '+';
            counter++;
        }
        else
            int_border += '-';
    }
    ext_border += '+';
    int_border += "|";

    std::cout << ext_border << std::endl;
    for (int i = 0; i < SIZE; i++)
    {
        std::cout << "| ";
        for (int j = 0; j < SIZE; j++)
        {
            if (Sudoku[i][j] == 0)
                std::cout << ". ";
            else
                std::cout << Sudoku[i][j] << " ";
            if (additional > 0 && Sudoku[i][j] < 10)
                std::cout << " ";
            if ((j + 1) % int(sqrt((double)SIZE)) == 0)
                std::cout << "| ";
        }
        std::cout << std::endl;
        if ((i + 1) % int(sqrt((double)SIZE)) == 0 && (i + 1) < SIZE)
            std::cout << int_border << std::endl;
    }
    std::cout << ext_border << std::endl
              << std::endl;
    std::regex regexp(".txt");
    std:: string outputpath=std::regex_replace(path,regexp,"_output.txt");
    
    std::ofstream MyFile(outputpath);
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            MyFile<<Sudoku[i][j]<<" ";
        }
        MyFile<<"\n";
    }
    MyFile.close();
}

//--------------------------------------------------------------------------------//

void SolveSudoku(int** Sudoku)
{
    matrix=new bool*[(SIZE * SIZE * SIZE)];
    for(int i=0;i<(SIZE * SIZE * SIZE);i++){
        matrix[i]=new bool[(4 * SIZE * SIZE)];
    }
    for(int i=0;i<(SIZE * SIZE * SIZE);i++){
        for(int j=0;j<(4 * SIZE * SIZE);j++){
            matrix[i][j]=0;
        }
    }
    timer = clock();
    BuildSparseMatrix(matrix);
    BuildLinkedList(matrix);
    TransformListToCurrentGrid(Sudoku);
    search(0);
    if (!isSolved)
        std::cout << "No Solution!" << std::endl;
    isSolved = false;
}

int main(int argc, char *argv[])
{
    //Sudoku Hard to Brute Force
    std::ifstream inputFile;
    
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
    std::string line;
    std::getline(inputFile, line);
    std::string strNumber;
    std::vector<std::string> tokens;
    std::stringstream check1(line);
    while (std::getline(check1, strNumber, ' '))
    {
        tokens.push_back(strNumber);
    }
    int dim = tokens.size();
    SIZE=dim;
    std::cout << "The size of the matrix is : " << dim << std::endl;
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

    

    

    SolveSudoku(matrix);
    

    return 0;
}
std::string GetUserInput()
{
    std::string inputString;
    std::cout << "Enter the relevant path of the input file: ";
    std::cin >> inputString;
    std::ifstream temInputFile(inputString);

    while (!temInputFile)
    {
        std::cout << "Entered relevant path of the input file does not exist please try again: ";
        std::cin >> inputString;
        temInputFile.open(inputString);
    }
    temInputFile.close();
    return inputString;
}
int *rowvalues(int size, std::string line)
{
    int *arr = new int[size];
    std::string strNumber;
    std::stringstream check1(line);
    int i = 0;
    while (std::getline(check1, strNumber, ' '))
    {
        arr[i] = std::stoi(strNumber);
        i++;
    }
    return arr;
}