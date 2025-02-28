#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

void clearScreen() { //clr function for cleanliness of output
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayBanner(const string& title) { //function for styles, this includes the title 
    int width = 60;
    string border(width, '=');

    cout << "\n\n" << border << endl;
    cout << setw((width + title.length()) / 2) << title << endl;
    cout << border << endl << endl;
}

void displayMenuItem(int number, const string& text) { //function for styles, ( [1] Generate DAG )
    cout << "  [" << number << "] " << text << endl;
}

void displayMessage(const string& message, char symbol = '*') { 
    string decoration(4, symbol);
    cout << "\n" << decoration << " " << message << " " << decoration << "\n" << endl;
}

void displayProcessingMessage(const string& message) { //function for message processing
    cout << message << "... Done!" << endl;
}

bool deleteSimulationLogs() { //delets everything inside graph_log
    displayBanner("Delete Simulation Logs");

    ifstream checkFile("graph_log.txt"); //name of txt
    if (!checkFile) {
        displayMessage("No log file found to delete", '!');
        return false;
    }
    checkFile.close();

    cout << "Are you sure you want to delete all simulation logs? (1 = Yes, 0 = No): ";
    int confirmChoice = 0;
    cin >> confirmChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //handler using limits

    if (confirmChoice == 1) {
        ofstream newLogFile("graph_log.txt", ios::trunc);
        if (newLogFile) {
            newLogFile.close();
            displayMessage("Simulation logs successfully deleted", '*');
            return true;
        } else {
            displayMessage("Failed to delete simulation logs", '!');
            return false;
        }
    } else {
        displayMessage("Log deletion cancelled", '!');
        return false;
    }
}

//stack for DFS search
class Stack {
private:
    struct Node {
        int data;
        Node* next;
        Node(int val) : data(val), next(nullptr) {}
    };

    Node* top;
    int size;

public:
    Stack() : top(nullptr), size(0) {}

    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(int val) {
        Node* newNode = new Node(val);
        newNode->next = top;
        top = newNode;
        size++;
    }

    int pop() {
        if (isEmpty()) {
            cerr << "Stack underflow error!" << endl;
            return -1;
        }

        Node* temp = top;
        int val = temp->data;
        top = top->next;
        delete temp;
        size--;
        return val;
    }

    int peek() const {
        if (isEmpty()) {
            cerr << "Stack is empty!" << endl;
            return -1;
        }
        return top->data;
    }

    bool isEmpty() const {
        return top == nullptr;
    }

    int getSize() const {
        return size;
    }
};

//the main function for the whole program
class Graph {
private:
    int numVertices;
    vector<vector<int>> adjList;
    ofstream logFile;

public:
    Graph(int n) : numVertices(n) {
        adjList.resize(n);
        logFile.open("graph_log.txt", ios::app);
        if (!logFile) {
            cerr << "Error opening log file!" << endl;
        }
    }

    ~Graph() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void addEdge(int src, int dest) { 
        adjList[src].push_back(dest);
    }


    bool generateRandomDAG(int n, int m) {
        logFile << "\n--- Generate Graph (Guaranteed DAG) ---\n";
        logFile << "Vertices: " << n << ", Edges: " << m << "\n";


        for (int i = 0; i < numVertices; i++) {
            adjList[i].clear();
        }

        int maxEdges = n * (n - 1) / 2; //this makes sure the edge isn't more than the vertices cause thatd be counter intuitive
        if (m > maxEdges) {
            displayMessage("Warning: Maximum possible edges in a DAG with " + to_string(n) +
                           " vertices is " + to_string(maxEdges), '!');
            m = maxEdges;
        }

        //RANDOMIZER FOR EDGES LES GOOOO
        int edgeCount = 0; 
        while (edgeCount < m) {
            int src = rand() % n; //vertices that will contain a directed edge
            int dest = rand() % n; //vertices that will have a destination to another vertex
            if (src == dest) continue;
            if (src > dest) {  
                swap(src, dest); //just ensures that the if the case is (4,2) just swaps it to (2,4) for formality
            }

            bool edgeExists = false; 
            for (int neighbor : adjList[src]) {
                if (neighbor == dest) {
                    edgeExists = true;
                    break;
                }
            } //just checks whether if the link between two vertices exists/existed

            if (!edgeExists) { //if theres no edge between the two vertices, add (generally for heuristics)
                addEdge(src, dest);
                logFile << "Added edge: Task " << (src + 1) << " -> Task " << (dest + 1) << "\n";
                edgeCount++;
            }
        }

        return true;
    }

    //generally the same fundamentals as the genDAG one, however, this can contain cycles, this will be utilized later
    bool generateRandomGraphWithPossibleCycles(int n, int m) {
        logFile << "\n--- Generate Graph (Gacha) ---\n";
        logFile << "Vertices: " << n << ", Edges: " << m << "\n";

        for (int i = 0; i < numVertices; i++) {
            adjList[i].clear();
        }

        int maxEdges = n * (n - 1);
        if (m > maxEdges) {
            displayMessage("Warning: Maximum possible edges in a directed graph with " +
                           to_string(n) + " vertices is " + to_string(maxEdges), '!');
            m = maxEdges;
        }

        int edgeCount = 0;
        while (edgeCount < m) {
            int src = rand() % n;
            int dest = rand() % n;

            if (src == dest) continue;

            bool edgeExists = false;
            for (int neighbor : adjList[src]) {
                if (neighbor == dest) {
                    edgeExists = true;
                    break;
                }
            }

            if (!edgeExists) {
                addEdge(src, dest);
                logFile << "Added edge: Task " << (src + 1) << " -> Task " << (dest + 1) << "\n";
                edgeCount++;
            }
        }

        logFile << "Note: This method can randomly generate an acyclic graph by chance\n";

        return true;
    }

    //checks cycle in the graph, this utilizes the stack and reads from the stack
    //this one is the initializer to detect cycles and traverses the whole stack
    bool detectCycle() {
        vector<bool> visited(numVertices, false);
        vector<bool> recStack(numVertices, false);

        logFile << "\n--- Cycle Detection ---\n";

        for (int i = 0; i < numVertices; i++) {
            if (detectCycleUtil(i, visited, recStack)) {
                logFile << "Cycle detected starting from Task " << (i + 1) << "\n";
                return true;
            }
        }

        logFile << "No cycles detected, graph is a DAG.\n";
        return false;
    }

    //recursion stack to detect cycle
    //this ones the recursive function to detect cycle
    bool detectCycleUtil(int vertex, vector<bool>& visited, vector<bool>& recStack) {
        if (!visited[vertex]) {
            visited[vertex] = true;
            recStack[vertex] = true;
            logFile << "Visiting Task " << (vertex + 1) << " in cycle detection\n";

            for (int neighbor : adjList[vertex]) {
                if (!visited[neighbor] && detectCycleUtil(neighbor, visited, recStack)) {
                    return true;
                } else if (recStack[neighbor]) {
                    logFile << "Cycle found: Task " << (neighbor + 1) << " is already in recursion stack\n";
                    return true;
                }
            }
        }

        recStack[vertex] = false;
        return false;
    }

    //sort function, uses stack for sorting, this function just prints the result of the topological sort from the other function
    bool topologicalSort(vector<int>& result) {
        displayProcessingMessage("Checking for cycles");

        if (detectCycle()) {
            displayMessage("Graph contains cycle, topological sort is not possible", '!');
            logFile << "Topological sort aborted due to cycle detection.\n";
            return false;
        }

        logFile << "\n--- Topological Sort Process ---\n";

        displayProcessingMessage("Performing topological sort");

        Stack sortStack;
        vector<bool> visited(numVertices, false);

        for (int i = 0; i < numVertices; i++) {
            if (!visited[i]) {
                topologicalSortUtil(i, visited, sortStack);
            }
        }

        result.clear();
        while (!sortStack.isEmpty()) {
            int vertex = sortStack.pop();
            result.push_back(vertex);
            logFile << "Popped from stack: Task " << (vertex + 1) << "\n";
        }

        return true;
    }

    //sorts the graph, utilizes recursion
    void topologicalSortUtil(int vertex, vector<bool>& visited, Stack& sortStack) {
        visited[vertex] = true;
        logFile << "Visiting Task " << (vertex + 1) << " in topological sort\n";

        for (int neighbor : adjList[vertex]) {
            if (!visited[neighbor]) {
                topologicalSortUtil(neighbor, visited, sortStack);
            }
        }

        logFile << "Pushing to stack: Task " << (vertex + 1) << "\n";
        sortStack.push(vertex);
    }

    //prints result
    void printGraph() const {
        displayMessage("Graph Adjacency List");

        cout << setw(10) << left << "Task" << setw(5) << "|->" << "Dependencies" << endl; //style
        cout << string(60, '-') << endl; //formatting and style

        for (int i = 0; i < numVertices; i++) {
            cout << setw(10) << left << ("Task " + to_string(i + 1)) << setw(5) << "|->"; // Task n |-> node

            if (adjList[i].empty()) { 
                cout << "None";
            } else { // checks if the node has any adjacent nodes, and lists all of them
                for (size_t j = 0; j < adjList[i].size(); j++) {
                    cout << "Task " << (adjList[i][j] + 1);
                    if (j < adjList[i].size() - 1) {
                        cout << ", ";
                    }
                }
            }
            cout << endl;
        }
        cout << endl;
    }

    //puts graph to file
    void printGraphToFile() {
        logFile << "\n--- Graph Adjacency List ---\n";
        for (int i = 0; i < numVertices; i++) {
            logFile << "Task " << (i + 1) << " -> ";
            for (int j : adjList[i]) {
                logFile << "Task " << (j + 1) << " ";
            }
            logFile << "\n";
        }
    }

    //file handlign to save graph in a file, this can be accessed using function, it can also be directly accessed
    bool saveGraph(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file for saving graph!" << endl;
            return false;
        }

        file << numVertices << endl;

        //it only saves the adjacency list of the graph
        for (int i = 0; i < numVertices; i++) {
            file << adjList[i].size();
            for (int neighbor : adjList[i]) {
                file << " " << neighbor;
            }
            file << endl;
        }

        file.close();
        return true;
    }

    //file handling to load saved graph
    bool loadGraph(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file for loading graph!" << endl;
            return false;
        }

        int n;
        file >> n;

        if (n != numVertices) {
            cerr << "Graph size mismatch!" << endl;
            file.close();
            return false;
        }

        for (int i = 0; i < numVertices; i++) {
            adjList[i].clear();
        }

        for (int i = 0; i < numVertices; i++) {
            int numNeighbors;
            file >> numNeighbors;

            for (int j = 0; j < numNeighbors; j++) {
                int neighbor;
                file >> neighbor;
                adjList[i].push_back(neighbor);
            }
        }

        file.close();
        return true;
    }
};

vector<string> listSavedGraphs() {
    vector<string> files;
    ifstream indexFile("graph_index.txt");

    if (!indexFile) {
        cout << "No saved graphs found." << endl;
        return files;
    }

    string filename;
    while (getline(indexFile, filename)) {
        files.push_back(filename);
    }

    indexFile.close();
    return files;
}

void addGraphToIndex(const string& filename) {
    ofstream indexFile("graph_index.txt", ios::app);
    if (indexFile) {
        indexFile << filename << endl;
        indexFile.close();
    }
}

void displayLogFile() {
    displayBanner("Simulation Log");

    ifstream logFile("graph_log.txt");
    if (!logFile) {
        displayMessage("Log file not found", '!');
        return;
    }

    string line;
    bool isHeader = false;

    while (getline(logFile, line)) {
        if (line.find("---") != string::npos) {
            isHeader = true;
            cout << "\n" << line << "\n";
        } else if (!line.empty()) {
            cout << line << endl;
        } else {
            cout << endl;
        }
    }

    logFile.close();

    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getIntInput() {
    int input;
    while (!(cin >> input)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return input;
}


void displayTopologicalOrder(const vector<int>& order) {
    displayMessage("Topological Order of Tasks");

    cout << "Execution sequence: ";

    for (size_t i = 0; i < order.size(); i++) {
        cout << "Task " << (order[i] + 1);

        if (i < order.size() - 1) {
            cout << " -> ";
        }
    }
    cout << endl << endl;

    cout << string(50, '-') << endl;
    cout << setw(15) << left << "Order" << setw(15) << "Task ID" << endl;
    cout << string(50, '-') << endl;

    for (size_t i = 0; i < order.size(); i++) {
        cout << setw(15) << left << (i + 1) << "Task " << (order[i] + 1) << endl;
    }
    cout << string(50, '-') << endl;
}

int displayMainMenu() {
    clearScreen();
    displayBanner("Sort on Directed Acyclic Graph");
    displayMenuItem(1, "Graph Generator (Guaranteed DAG)");
    displayMenuItem(2, "Graph Generator (Gacha)");
    displayMenuItem(3, "Load a saved graph");
    displayMenuItem(4, "Display simulation log");
    displayMenuItem(5, "Delete simulation logs");
    displayMenuItem(0, "Exit");

    cout << "\nEnter your choice: ";
    return getIntInput();
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    int choice, n = 0, m = 0;
    string filename;

    do {
        choice = displayMainMenu();

        switch (choice) {
            case 1:
            case 2: {
                clearScreen();
                if (choice == 1) {
                    displayBanner("Generate Random DAG");
                } else {
                    displayBanner("Generate Random Graph (Possibly Cyclic)");
                }

                cout << "Enter number of tasks (vertices): ";
                n = getIntInput();
                if (n <= 0) {
                    displayMessage("Invalid number of vertices. Must be positive.", '!');
                    cout << "Press Enter to continue...";
                    cin.get();
                    break;
                }

                cout << "Enter number of dependencies (edges): ";
                m = getIntInput();
                if (m < 0) {
                    displayMessage("Invalid number of edges. Must be positive.", '!');
                    cout << "Press Enter to continue...";
                    cin.get();
                    break;
                }

                Graph graph(n);

                displayProcessingMessage("Generating graph");

                if (choice == 1) {
                    graph.generateRandomDAG(n, m);
                    displayMessage("Successfully generated a random DAG", '*');
                } else {
                    graph.generateRandomGraphWithPossibleCycles(n, m);
                    displayMessage("Successfully generated a random graph (may contain cycles)", '*');
                }

                graph.printGraph();
                graph.printGraphToFile();

                vector<int> topologicalOrder;
                bool success = graph.topologicalSort(topologicalOrder);

                if (success) {
                    displayTopologicalOrder(topologicalOrder);
                }

                cout << "Do you want to save this graph? (1 = Yes, 0 = No): ";
                int saveChoice = getIntInput();

                if (saveChoice == 1) {
                    cout << "Enter filename to save (e.g., graph1.txt): ";
                    getline(cin, filename);

                    displayProcessingMessage("Saving graph");

                    if (graph.saveGraph(filename)) {
                        displayMessage("Graph saved successfully", '*');
                        addGraphToIndex(filename);
                    } else {
                        displayMessage("Failed to save graph", '!');
                    }
                }

                cout << "Press Enter to continue...";
                cin.get();
                break;
            }
            case 3: {
                clearScreen();
                displayBanner("Load Saved Graph");
                vector<string> savedGraphs = listSavedGraphs();
                if (savedGraphs.empty()) {
                    cout << "Press Enter to continue...";
                    cin.get();
                    break;
                }
                displayMessage("Available Graphs");
                cout << "+" << string(40, '-') << "+" << endl;
                cout << "| " << setw(9) << left << "ID" << setw(30) << left << "Filename" << " |" << endl;
                cout << "+" << string(40, '-') << "+" << endl;
                for (size_t i = 0; i < savedGraphs.size(); i++) {
                    cout << "| " << setw(8) << left << (i + 1) << setw(30) << left << savedGraphs[i] << " |" << endl;
                }
                cout << "+" << string(40, '-') << "+" << endl;
                cout << "Enter number to load (0 to cancel): ";
                int fileChoice = getIntInput();
                if (fileChoice <= 0 || fileChoice > static_cast<int>(savedGraphs.size())) {
                    break;
                }
                filename = savedGraphs[fileChoice - 1];
                ifstream fileCheck(filename);
                if (fileCheck) {
                    fileCheck >> n;
                    fileCheck.close();
                    Graph graph(n);
                    displayProcessingMessage("Loading graph");
                    if (graph.loadGraph(filename)) {
                        displayMessage("Graph loaded successfully", '*');
                        graph.printGraph();
                        vector<int> topologicalOrder;
                        bool success = graph.topologicalSort(topologicalOrder);
                        if (success) {
                            displayTopologicalOrder(topologicalOrder);
                        }
                    } else {
                        displayMessage("Failed to load graph", '!');
                    }
                } else {
                    displayMessage("Failed to open file", '!');
                }
                cout << "Press Enter to continue...";
                cin.get();
                break;
            }
            case 4:
                clearScreen();
                displayLogFile();
                break;
            case 5:
                clearScreen();
                deleteSimulationLogs();
                cout << "Press Enter to continue...";
                cin.get();
                break;
            case 0:
                clearScreen();
                displayMessage("Thank you!", '*');
                cout << "Exiting program..." << endl;
                break;
            default:
                displayMessage("Invalid choice. Please try again.", '!');
                cout << "Press Enter to continue...";
                cin.get();
        }
    } while (choice != 0);

    return 0;
}
