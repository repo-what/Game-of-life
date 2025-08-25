#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <chrono>
#include <cstdlib>
#include <thread>


using namespace std;

class GameOfLife {
private:
    vector<vector<bool>> grid;
    vector<vector<bool>> next_grid;
    int width, height;
    string save_file_name;
    int gen_to_save;

public:
    GameOfLife(int w, int h, int save_gen = -1) : width(w), height(h) {
        grid.resize(height, vector<bool>(width, false));
        next_grid.resize(height, vector<bool>(width, false));
	save_file_name = "";
	gen_to_save = -1;
    }

    void setFileNameToSave(string file_prefix, int generation) {
	gen_to_save = generation;
	save_file_name = file_prefix + "_" + to_string(generation);
    }

    // Set a cell to alive
    void setCell(int x, int y, bool alive) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = alive;
        }
    }

    // Count live neighbors for a given cell
    int countNeighbors(int x, int y) {
        int count = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) 
		    continue; // Skip the cell itself
                
                int nx = x + dx;
                int ny = y + dy;
                
                // Check whether the neighbor within bounds or not
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (grid[ny][nx]) ++count;
                }
            }
        }
        return count;
    }

    // Apply Game of Life rules to compute next generation
    void nextGeneration() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int neighbors = countNeighbors(x, y);
                bool current_cell = grid[y][x];
                
                // Apply the rules
                if (current_cell) {
                    // Rule 1: Live cell with 2 or 3 neighbors survives
                    // Rule 3: All other live cells die
                    next_grid[y][x] = (neighbors == 2 || neighbors == 3);
                } else {
                    // Rule 2: Dead cell with exactly 3 neighbors becomes alive
                    next_grid[y][x] = (neighbors == 3);
                }
            }
        }
        
        // Swap grids
        grid.swap(next_grid);
    }


    // Display the current state of the universe
    void display() {
        // Clear screen
        std::system("clear");
        
        cout << "Animation of Conway's Game of Life \n";
        cout << std::string(width + 2, '-') << "\n";
        
        for (int y = 0; y < height; ++y) {
            std::cout << "|";
            for (int x = 0; x < width; ++x) {
                std::cout << (grid[y][x] ? "■" : " ");
            }
            std::cout << "|\n";
        }
        
        std::cout << std::string(width + 2, '-') << "\n";
        std::cout << "Press Ctrl+C to exit\n";
    }

//*
//  Sparse algorithm:
    unordered_set<long long> live; // encode (x,y) as a single number

    long long encode(int x, int y) {
        return ((long long)x << 32) | (unsigned int)y;
    }

    void nextGenerationSparse() {
        unordered_map<long long, int> neighborCount;

        // Count neighbors for each live cell
        for (auto cell : live) {
            int x = cell >> 32;
            int y = (int)cell;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    neighborCount[encode(x+dx, y+dy)]++;
                }
            }
        }

        unordered_set<long long> next;
        for (auto& [pos, count] : neighborCount) {
            if (count == 3 || (count == 2 && live.count(pos))) {
                next.insert(pos);
            }
        }
        live = std::move(next);
    }

    void prepareSparse() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (grid[y][x]) {
                   live.insert(encode(x, y));
                }
            }
        }
    }

    // Display the current state of the universe
    // For large system the heigt and width in this
    // function should some finite small value which
    // show the annimation within display
    void displaySparse() {
        // Clear screen
        std::system("clear");
        int hfixed = 20;
        int wfixed = 20;
        cout << "Animation of Conway's Game of Life (Sparse algorithm) \n";
        cout << std::string(wfixed + 2, '-') << "\n";
        
        for (int y = 0; y < hfixed; ++y) {
            std::cout << "|";
            for (int x = 0; x < wfixed; ++x) {
                long long point = encode(x,y);
                if (live.find(point) != live.end() )
                   std::cout << "■";
                else
                   std::cout << " ";
            }
            std::cout << "|\n";
        }
        
        std::cout << std::string(wfixed + 2, '-') << "\n";
        std::cout << "Press Ctrl+C to exit\n";
    }


//*/

    // Set up a glider pattern
    void setupGlider(int startX, int startY) {
        // Classic glider pattern:
        // ■
        //  ■
	//■■■
        //setCell(startX + 3, startY, true);     // Middle Top
        setCell(startX + 1, startY, true);     // Left   Top
        setCell(startX + 2, startY + 1, true); // Middle right
        setCell(startX, startY + 2, true);     // Bottom left
        setCell(startX + 1, startY + 2, true); // Bottom middle
        setCell(startX + 2, startY + 2, true); // Bottom right
    }

    // Run the simulation
    void run(int generations = -1) {
        int generation = 0;
        while (generations < 0 || generation < generations) {
            display();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
	    if (generation == gen_to_save && save_file_name != "") {
	       saveToFile(save_file_name);
	    }
            nextGeneration();
	      
            generation++;
        }
    }

    // Run the simulation using sparse algorithm
    void runSparseAlgo(int generations = -1) {
        int generation = 0;
        while (generations < 0 || generation < generations) {
            displaySparse();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            cout<< "generation = "<<generation<<endl;
            nextGenerationSparse();
	      
            generation++;
        }
    }

    // Load universe state from a file
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "' for reading.\n";
            return false;
        }

        int fileWidth, fileHeight;
        file >> fileWidth >> fileHeight;

        if (fileWidth != width || fileHeight != height) {
            std::cerr << "Error: File dimensions (" << fileWidth << "x" << fileHeight 
                     << ") don't match current universe (" << width << "x" << height << ").\n";
            std::cerr << "Create a new GameOfLife instance with matching dimensions.\n";
            file.close();
            return false;
        }

        // Read the grid state
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cellState;
                file >> cellState;
                grid[y][x] = (cellState == 1);
            }
        }

        file.close();
        std::cout << "Universe loaded from '" << filename << "' successfully.\n";
        return true;
    }

    // Save the current universe state to a file
    bool saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "' for writing.\n";
            return false;
        }

        // Write dimensions as header
        file << width << " " << height << "\n";

        // Write the grid state (1 for alive, 0 for dead)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                file << (grid[y][x] ? "1" : "0");
                if (x < width - 1) file << " ";
            }
            file << "\n";
        }

        file.close();
        //std::cout << "Universe saved to '" << filename << "' successfully.\n";
        return true;
    }


    // Clear the entire universe (set all cells to dead)
    void clear() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                grid[y][x] = false;
            }
        }
    }

};


int main() {
    const int WIDTH = 40;
    const int HEIGHT = 20;
    
    GameOfLife game(WIDTH, HEIGHT);
    
    std::cout << "Conway's Game of Life with File I/O\n";
    std::cout << "====================================\n\n";
    
    std::cout << "Choose an option:\n";
    std::cout << "1. Start with glider pattern\n";
    std::cout << "2. Load universe from file\n";
    std::cout << "3. Start with empty universe\n";
    std::cout << "4. Using sparse algorithm\n";
    std::cout << "Enter choice (1-4): ";
    int sparse_algo = 0;
    int choice;
    std::cin >> choice;
    
    switch (choice) {
        case 1: {
            // Set up a glider starting at position (5, 5)
            game.setupGlider(5, 5);
            std::cout << "Glider pattern loaded.\n";
            break;
        }
        case 2: {
            std::cout << "Enter filename to load: ";
            std::string filename;
            std::cin >> filename;
            if (!game.loadFromFile(filename)) {
                std::cout << "Failed to load file. Starting with empty universe.\n";
            }
            break;
        }
        case 3: {
            game.setupGlider(5, 5);
            std::cout << "Enter prefix of the filename to save: ";
            std::string file_prefix;
            int generation;
            std::cin >> file_prefix;
            std::cout << "Enter the generation number to save: ";
            std::cin >> generation;
            game.setFileNameToSave(file_prefix, generation);
            break;
        }
        case 4: {
            // Set up a glider starting at position (5, 5)
            game.setupGlider(1, 1);
            std::cout << "Glider pattern loaded for sparse algorithm.\n";
            // Preparing input for only the live cells
	    game.prepareSparse();
            sparse_algo = 1;
            break;
        }

        default: {
            std::cout << "Invalid choice. Starting with glider pattern.\n";
            game.setupGlider(5, 5);
            break;
        }
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
    
    //game.runInteractive();

    // Run indefinitely (use Ctrl+C to stop)
    if (sparse_algo)
        game.runSparseAlgo();
    else
        game.run();
    
    return 0;
}

