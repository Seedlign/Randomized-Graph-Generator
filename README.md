# Graph Generator

## Description
This C++ program generates Directed Acyclic Graphs (DAGs) and general directed graphs (which may contain cycles). It provides options for topological sorting, cycle detection, and file handling for saving/loading graphs. The program also logs operations in `graph_log.txt`.

## Features
- Generates a **random Directed Acyclic Graph (DAG)**.
- Generates a **random directed graph that may contain cycles**.
- **Cycle detection** to check if the graph is a DAG.
- **Topological sorting** for DAGs.
- **File handling**: Save and load graphs.
- **Simulation log**: Records graph operations.
- **Interactive menu** for user-friendly operations.

## Functions
### `void clearScreen()`
- Clears the console screen.

### `void displayBanner(const string& title)`
- Displays a formatted banner with a title.

### `void displayMessage(const string& message, char symbol = '*')`
- Displays a message with a symbol decoration.

### `bool deleteSimulationLogs()`
- Deletes the contents of `graph_log.txt`.

### `class Stack`
- Implements a stack for Depth-First Search (DFS) operations.

### `class Graph`
- **Graph initialization:** Stores adjacency list representation.
- **`void addEdge(int src, int dest)`**: Adds an edge from `src` to `dest`.
- **`bool generateRandomDAG(int n, int m)`**: Generates a random DAG with `n` vertices and `m` edges.
- **`bool generateRandomGraphWithPossibleCycles(int n, int m)`**: Generates a random directed graph with possible cycles.
- **`bool detectCycle()`**: Checks for cycles using DFS.
- **`bool topologicalSort(vector<int>& result)`**: Performs topological sorting.
- **`void printGraph()`**: Displays the graph as an adjacency list.
- **`bool saveGraph(const string& filename)`**: Saves the graph to a file.
- **`bool loadGraph(const string& filename)`**: Loads a graph from a file.

### `int displayMainMenu()`
- Displays the main menu and handles user choices.

## Usage
1. Compile the program using a C++ compiler:
   ```sh
   g++ -o graph_generator GraphGenerator.cpp
   ```
2. Run the executable:
   ```sh
   ./graph_generator
   ```
3. Follow the prompts to generate, analyze, or save/load graphs.

## Output
- The program prints graph details and results to the console.
- Logs are stored in `graph_log.txt`.

## Example Output
```
Enter number of tasks (vertices): 5
Enter number of dependencies (edges): 6
Generating graph... Done!
Successfully generated a random DAG
Graph Adjacency List
Task 1 |-> Task 3, Task 5
Task 2 |-> Task 4
Task 3 |-> Task 5
Task 4 |-> None
Task 5 |-> None

Topological Order of Tasks
Execution sequence: Task 1 -> Task 3 -> Task 5 -> Task 2 -> Task 4
```

## License
This project is open-source and free to use.

