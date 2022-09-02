#include "../../Utilities/GridBase2D.h"
#include "Grid2D.h"

int main() {
    // This checks if the grid 2d base functions.
    int problem = test_Grid2DBase<Utilities::GridBase2D<int>>();
    
    return problem;
}
