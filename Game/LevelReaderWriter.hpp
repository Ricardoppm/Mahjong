//
//  LevelReaderWriter.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 06/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef LevelReaderWriter_hpp
#define LevelReaderWriter_hpp

#include <string>
#include <vector>
#include <stdio.h>
#include "Tile.hpp"


class LevelReaderWriter {
public:
    static bool saveAsText(const std::string& filePath, const std::vector<uint8_t> board, const glm::ivec4& borders, const int width);
    static bool saveAsBinary(const std::string& filePath, const std::vector<uint8_t> board, const int width, const int height);
private:
    static bool saveAsTextV0(const std::string& filePath,
                             const std::vector<uint8_t> board,
                             const glm::ivec4& borders,
                             const int width);
};
#endif /* LevelReaderWriter_hpp */
