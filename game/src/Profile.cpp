#include "include/TileArray.hpp"
#include "include/TileTree.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
   // Read meta data from level file
    std::ifstream in(argv[1]);
    std::string texFileName, actorFileName;

    // Get path from given .lvl file
  
    int ir, ig, ib;
    int m_tileWidth, m_tileHeight, m_arrayWidth, m_arrayHeight, m_tileOffset, m_tilesPerRow, m_numRows;
    unsigned char keyR, keyG, keyB;

    if (in.good())
    {
        in >> texFileName;
        in >> actorFileName;
        in >> m_tileWidth >> m_tileHeight >> m_tilesPerRow >> m_numRows;
        in >> m_tileOffset >> ir >> ig >> ib;
        in >> m_arrayWidth >> m_arrayHeight;
    }
    

    //jumper::TileTree* m_tiles = new jumper::TileTree(0, 0, m_arrayHeight, m_arrayWidth);	  
    jumper::TileArray* m_tiles = new jumper::TileArray(0, 0, m_arrayWidth, m_arrayHeight);  


    // Read tile indices
    for (int i = 0; i < m_arrayHeight; i++)
    {
        for (int j = 0; j < m_arrayWidth; j++)
        {
            int tileID;
            in >> tileID;
            m_tiles->insert(i, j, tileID);
        }
    }

    // Close file
    in.close();

    delete m_tiles;
    
    return 0;
}
