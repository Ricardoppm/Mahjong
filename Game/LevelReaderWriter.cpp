//
//  LevelReaderWriter.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 06/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "LevelReaderWriter.hpp"

#include <Bengine/ResourceManager.hpp>

#include <fstream>

bool LevelReaderWriter::saveAsText(const std::string& filePath, const std::vector<uint8_t> board , const glm::ivec4& borders, const int width) {
    // Keep this updated with newest version
    return saveAsTextV0(filePath,board, borders, width);
}

bool LevelReaderWriter::saveAsTextV0(const std::string& filePath, const std::vector<uint8_t> board, const glm::ivec4& borders, const int width) {
    // Open file and error check
    std::ofstream file(filePath);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }
    int xRange = (borders.y%2 == 1)? borders.y + 1 : borders.y;
    int yRange = (borders.w%2 == 1)? borders.w + 1 : borders.w;
    
    

    // Write player
    file << std::ceil( (borders.y-borders.x + 2)/2.f) << ' ' << std::ceil( (borders.w - borders.z + 2)/2.f) << '\n';
    for (size_t y = borders.z; y <= yRange+1; y++) {
        for (size_t x = borders.x; x <= xRange+1; x++) {
            uint8_t value = board[(y*width*2) + x]+1;
            file << (int)value;
        }
        file << '\n';
    }
    return true;
}

bool LevelReaderWriter::saveAsBinary(const std::string& filePath, const std::vector<uint8_t> board, const int width, const int height) {
    puts("saveBinary not implemented.");
    return false;
}
