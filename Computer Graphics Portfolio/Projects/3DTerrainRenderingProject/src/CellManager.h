#pragma once
#include "ofMain.h"
#include "buildTerrainMesh.h"

// A struct for maintaining the state of a single cell.
struct Cell
{
public:
    // The mesh containing the terrain geometry for the cell.
    ofVboMesh terrainMesh {};

    // The corner defining the mesh's location in world space.
    glm::vec2 startPos {};

    // Set to true while the mesh is loading so that it's not rendered mid-load.
    bool loading { false };

    // Set to false while the cell is inactive so that it's not rendered.
    bool live { false };
};

// A template class for managing partial terrain meshes, 
// automatically loading and unloading cells as they go in and out of draw range.
template<unsigned int CELL_PAIRS_PER_DIMENSION>
class CellManager
{
public:
    CellManager(const ofShortImage& heightmap, float heightmapScale, unsigned int cellSize)
        : heightmap{ heightmap }, heightmapScale{ heightmapScale }, cellSize{ cellSize }
    {
    }

    // Don't support copy constructor or copy assignment operator.
    CellManager(const CellManager& c) = delete;
    CellManager& operator= (const CellManager& c) = delete;

    // This function should be called in your ofApp::setup() function.  
    // Pass in whatever position you want the loaded terrain to be centered around.
    void initializeForPosition(glm::vec3 position)
    {
        // The range of loaded cells should be centered on the player.
        glm::vec2 cellGridMidIndices { glm::round(glm::vec2(position.x, position.z) / cellSize) };

        // Calculate the actual start position of the first cell in the loaded grid.
        cellGridStartPos = (cellGridMidIndices - glm::vec2(CELL_PAIRS_PER_DIMENSION)) * cellSize;

        // Load each cell.
        unsigned int bufferIndex { 0 };
        for (unsigned int i { 0 }; i < 2 * CELL_PAIRS_PER_DIMENSION; i++)
        {
            for (unsigned int j { 0 }; j < 2 * CELL_PAIRS_PER_DIMENSION; j++)
            {
                initCell(cellBuffer[bufferIndex], (cellGridStartPos + glm::vec2(i, j) * cellSize));
                bufferIndex++;
            }
        }

        // Start cell loading thread
        cellLoadThread = std::thread 
        {
            [&] () 
            { 
                while (!stopping) 
                { 
                    processLoadQueue(); 

                    if (cellLoadQueue.empty())
                    {
                        // Sleep for 0.1 seconds if the load queue is empty.
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                } 
            }
        };
    }

    // This function should be called in your ofApp::update() function to unload cells that have gotten to be far away
    // and request new cells that have gotten closer.  No meshes are actually loaded in this function;
    // call processLoadQueue to actually load the meshes.
    void optimizeForPosition(glm::vec3 position)
    {
        // Calculate a lower bound (in each dimension) on where the lower grid can start.
        glm::vec2 minGridStartIndices { glm::ceil(glm::vec2(position.x, position.z) / cellSize) - glm::vec2(CELL_PAIRS_PER_DIMENSION) };

        // Calculate the actual start position of the first cell in the loaded grid at the lower bound in each dimension.
        glm::vec2 minGridStartPos { minGridStartIndices * cellSize };

        // THe actual start position of the first cell in the loaded grid at the upper bound in each dimension.
        glm::vec2 maxGridStartPos { minGridStartPos + glm::vec2(cellSize) };

        // Only move the grid of loaded cells if it's outside of the lower / upper bounds.
        // This ensures that unnecessary loading doesn't occur.
        glm::vec2 newGridStartPos { clamp(cellGridStartPos, minGridStartPos, maxGridStartPos) };

        // Only do something if the grid of loaded cells needs to change.
        if (newGridStartPos != cellGridStartPos)
        {
            int bufferIndex { 0 };

            if (newGridStartPos.x > cellGridStartPos.x)
            {
                // Loop in case a cell was skipped over (only for very fast movement; usually will only run once).
                for (float x { cellGridStartPos.x + cellSize }; x <= newGridStartPos.x + 0.00001f; x += cellSize)
                {
                    // Add new cells to the right of the old active region
                    for (unsigned int j { 0 }; j < 2 * CELL_PAIRS_PER_DIMENSION; j++)
                    {
                        cellLoadQueue.push(glm::vec2(x, newGridStartPos.y) + glm::vec2(2 * CELL_PAIRS_PER_DIMENSION - 1, j) * cellSize);
                    }
                }
            }
            else if (newGridStartPos.x < cellGridStartPos.x)
            {
                // Loop in case a cell was skipped over (only for very fast movement; usually will only run once).
                for (float x { cellGridStartPos.x - cellSize }; x >= newGridStartPos.x - 0.00001f; x -= cellSize)
                {
                    // Add new cells to the left of the old active region
                    for (unsigned int j { 0 }; j < 2 * CELL_PAIRS_PER_DIMENSION; j++)
                    {
                        cellLoadQueue.push(glm::vec2(x, newGridStartPos.y + j * cellSize));
                    }
                }
            }

            // startX and endX for filling in new rows of cells.
            float startX, endX;

            // Don't do the top left corner if we just added an entire left-hand column of cells in the preceding code
            if (newGridStartPos.x < cellGridStartPos.x)
            {
                startX = cellGridStartPos.x;
            }
            else
            {
                startX = newGridStartPos.x;
            }

            if (newGridStartPos.x > cellGridStartPos.x)
            {
                endX = cellGridStartPos.x + (2 * CELL_PAIRS_PER_DIMENSION - 1) * cellSize;
            }
            else
            {
                endX = newGridStartPos.x + (2 * CELL_PAIRS_PER_DIMENSION - 1) * cellSize;
            }

            if (newGridStartPos.y > cellGridStartPos.y)
            {
                // Loop in case a cell was skipped over (only for very fast movement; usually will only run once).
                for (float y { cellGridStartPos.y + cellSize }; y <= newGridStartPos.y + 0.00001f; y += cellSize)
                {
                    // Add the top cells
                    for (float x { startX }; x <= endX + 0.00001f; x += cellSize)
                    {
                        cellLoadQueue.push(glm::vec2(x, y + (2 * CELL_PAIRS_PER_DIMENSION - 1) * cellSize));
                    }
                }
            }
            else if (newGridStartPos.y < cellGridStartPos.y)
            {
                // Loop in case a cell was skipped over (only for very fast movement; usually will only run once).
                for (float y { cellGridStartPos.y - cellSize }; y >= newGridStartPos.y - 0.00001f; y -= cellSize)
                {
                    // Add the bottom cells
                    for (float x { startX }; x <= endX + 0.00001f; x += cellSize)
                    {
                        cellLoadQueue.push(glm::vec2(x, y));
                    }
                }
            }

            // Update the starting coordinates of the new grid of loaded cells.
            cellGridStartPos = newGridStartPos;
        }
    }

    // This function iterates over all the available cells and draws all of them that are within the draw 
    // distance from the current camera position. This should be called from your ofApp::draw() function.  
    // The draw distance should be the same as the far plane from your projection matrix.
    void drawActiveCells(glm::vec3 camPosition, float drawDistance)
    {
        // Calculate an appropriate threshold for deciding if cells are too far away to draw.
        float threshold = drawDistance + cellSize * glm::sqrt(0.5f);

        for (Cell& cell : cellBuffer)
        {
            // Make sure the cell is live/active and check the distance from the cell center to the camera position
            if (cell.live && distance(glm::vec2(camPosition.x, camPosition.z), cell.startPos + cellSize * 0.5f) < threshold)
            {
                cell.terrainMesh.draw();
            }
        }
    }

    // This function stops the cell loading thread and should be called from ofApp::exit().
    void stop()
    {
        // Stop the cell load thread before other resources are destroyed.
        stopping = true;
        cellLoadThread.join();
    }

private:
    // The maximum number of cells that can be currently loaded at once.
    const static unsigned int CELL_BUFFER_SIZE { 4 * CELL_PAIRS_PER_DIMENSION * CELL_PAIRS_PER_DIMENSION };

    // The buffer of loaded cells.
    Cell cellBuffer[CELL_BUFFER_SIZE] {};

    // A reference to the pixel array containing the world heightmap.
    const ofShortImage& heightmap{ nullptr };

    float heightmapScale;

    // The size of each cell (assumed to be square).
    unsigned int cellSize;

    // The "first" cell that is currently loaded; the corner of the rectangle of loaded cells.
    glm::vec2 cellGridStartPos;

    // A queue containing the corners of cells that need to be loaded.
    std::queue<glm::vec2> cellLoadQueue {};

    // Thread to load cells in the background.
    std::thread cellLoadThread {};

    // Flag to signal the cell loading thread to stop.
    bool stopping {};

    bool isCellDistant(glm::vec2 cellStartPos)
    {
        // Distant cells are outside of the rectangle of cells currently loaded.
        return cellStartPos.x < cellGridStartPos.x - cellSize * 0.125f // subtract a little extra to account for round-off error
            || cellStartPos.y < cellGridStartPos.y - cellSize * 0.125f
            || cellStartPos.x + cellSize > cellGridStartPos.x + cellSize * (2 * CELL_PAIRS_PER_DIMENSION + 0.125f)
            || cellStartPos.y + cellSize > cellGridStartPos.y + cellSize * (2 * CELL_PAIRS_PER_DIMENSION + 0.125f);
    }

    bool isCellDuplicate(glm::vec2 cellStartPos, float tolerance)
    {
        for (Cell& otherCell : cellBuffer)
        {
            // If two cells' start position is within a certain tolerance, they are considered duplicates.
            if (distance(otherCell.startPos, cellStartPos) < tolerance)
            {
                return true;
            }
        }

        return false;
    }

    // Builds the mesh for a particular cell of the terrain.
    // The first parameter is a reference to the mesh to be initialized.
    // The second parameter is the coordinates (pixel indices) of the cell to load.
    // The third parameter is the dimensions (in pixels) of the cell to load.
    void buildMeshForTerrainCell(ofMesh& terrainMesh, glm::uvec2 startPos, glm::uvec2 size) const
    {
        if (startPos.x < heightmap.getWidth() && startPos.y < heightmap.getHeight())
        {
            // Clamp the size to the bounds of the heightmap
            size = glm::min(size, glm::uvec2(heightmap.getWidth(), heightmap.getHeight()) - startPos - 1u);

            // Use buildTerrainMesh() to initialize or re-initialize the mesh.
            // The scale parameter taken by buildTerrainMesh needs to be relative to the dimensions of the heightmap
            buildTerrainMesh(terrainMesh, heightmap.getPixels(), startPos.x, startPos.y, startPos.x + size.x, startPos.y + size.y, glm::vec3(1, heightmapScale, 1));
        }
    }
    
    void initCell(Cell& cell, glm::vec2 startPos)
    {
        // Set cell's starting position, it is current loading and not yet live.
        cell.startPos = startPos;
        cell.live = false;
        cell.loading = true;
        // Remap to the resolution of the heightmap and round to the nearest integer
        glm::uvec2 startIndices { round(glm::vec2(startPos.x, startPos.y)) };

        // Clear the old terrain mesh and rebuild it for the current cell.
        cell.terrainMesh.clear();
        buildMeshForTerrainCell(cell.terrainMesh, startIndices, glm::uvec2(cellSize, cellSize));

        // Once the cell has been successfully loaded, make it live.
        cell.loading = false;
        cell.live = true;
    }

    // This function is called from the cell loading thread.
    // This function is where the terrain meshes actually get created.
    void processLoadQueue()
    {
        if (!cellLoadQueue.empty())
        {
            // Deactivate cells that are now out of range.
            for (Cell& cell : cellBuffer)
            {
                if (isCellDistant(cell.startPos))
                {
                    cell.live = false;
                }
            }

            unsigned int bufferIndex { 0 };

            // Check to ensure that there are available cells in the buffer and that there are still cell requests left to process.
            if (bufferIndex < CELL_BUFFER_SIZE && !cellLoadQueue.empty())
            {
                // Find the next unused cell in the buffer.
                while (bufferIndex < CELL_BUFFER_SIZE && (cellBuffer[bufferIndex].live || cellBuffer[bufferIndex].loading))
                {
                    bufferIndex++;
                }

                if (bufferIndex < CELL_BUFFER_SIZE)
                {
                    if (!isCellDistant(cellLoadQueue.front())
                        && !isCellDuplicate(cellLoadQueue.front(), cellSize * 0.125f)) // Make sure we still want the cell
                    {

                        // Load the next requested cell.
                        initCell(cellBuffer[bufferIndex], cellLoadQueue.front());
                    }

                    cellLoadQueue.pop();
                }
            }
        }
    }
};
