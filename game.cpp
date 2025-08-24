#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

class GameOfLife {
private:
    vector<vector<bool>> grid;
    vector<vector<bool>> nextGrid;
    int width, height;

public:
    GameOfLife(int w, int h) : width(w), height(h) {
        grid.resize(height, vector<bool>(width, false));
        nextGrid.resize(height, vector<bool>(width, false));
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
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) 
		    continue; // Skip the cell itself
                
                int nx = x + dx;
                int ny = y + dy;
                
                // Check whether the neighbor within bounds or not
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (grid[ny][nx]) count++;
                }
            }
        }
        return count;
    }

    // Apply Game of Life rules to compute next generation
    void nextGeneration() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int neighbors = countNeighbors(x, y);
                bool current_cell = grid[y][x];
                
                // Apply the rules
                if (current_cell) {
                    // Rule 1: Live cell with 2 or 3 neighbors survives
                    // Rule 3: All other live cells die
                    nextGrid[y][x] = (neighbors == 2 || neighbors == 3);
                } else {
                    // Rule 2: Dead cell with exactly 3 neighbors becomes alive
                    nextGrid[y][x] = (neighbors == 3);
                }
            }
        }
        
        // Swap grids
        grid.swap(nextGrid);
    }

    // Display the current state of the universe
    void display() {
        // Clear screen (works on most terminals)
        std::system("clear");
        
        std::cout << "Conway's Game of Life - Generation\n";
        std::cout << std::string(width + 2, '-') << "\n";
        
        for (int y = 0; y < height; y++) {
            std::cout << "|";
            for (int x = 0; x < width; x++) {
                std::cout << (grid[y][x] ? "■" : " ");
                //std::cout << (grid[y][x] ? "█" : " ");
            }
            std::cout << "|\n";
        }
        
        std::cout << std::string(width + 2, '-') << "\n";
        std::cout << "Press Ctrl+C to exit\n";
    }

    // Set up a glider pattern
    void setupGlider(int startX, int startY) {
        // Classic glider pattern:
        //  █
        //   █
        // ███
        setCell(startX + 1, startY, true);     // Top
        setCell(startX + 2, startY + 1, true); // Middle right
        setCell(startX, startY + 2, true);     // Bottom left
        setCell(startX + 1, startY + 2, true); // Bottom middle
        setCell(startX + 2, startY + 2, true); // Bottom right
    }

    // Run the simulation
    void run(int generations = -1) {
        int gen = 0;
        while (generations < 0 || gen < generations) {
            display();
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            nextGeneration();
            gen++;
        }
    }
};

int main() {
    const int WIDTH = 40;
    const int HEIGHT = 20;
    
    GameOfLife game(WIDTH, HEIGHT);
    
    // Set up a glider starting at position (5, 5)
    game.setupGlider(5, 5);
    
    std::cout << "Starting Conway's Game of Life with a glider pattern...\n";
    std::cout << "The glider will move diagonally across the screen.\n";
    std::cout << "Press Enter to start...";
    std::cin.get();
    
    // Run indefinitely (use Ctrl+C to stop)
    game.run();
    
    return 0;
}

