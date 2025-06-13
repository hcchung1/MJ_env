#add_library(MJBase_Lib HandTile.cpp HandTile_old.cpp HandTileDecomposer.cpp MahjongBaseConfig.h MeldTile.cpp MJBaseTypedef.h MJStateHashKeyCalculator.cpp Move.cpp PlayerTile.cpp RemainTile.cpp RemainTile_old.cpp Tile.cpp TranspositionTable.cpp Wind.h WinTable.cpp)

add_library(MJBase_Lib STATIC HandTile.cpp RemainTile.cpp Tile.cpp HandTileDecomposer.cpp MahjongBaseConfig.h MJBaseTypedef.h MeldTile.cpp MJStateHashKeyCalculator.cpp Move.cpp PlayerTile.cpp Wind.h MinLackTable/MinLackEntry.cpp MinLackTable/MinLackTable.cpp MinLackTable/MinLackTable_old.cpp MinLackTable/TileGroup.cpp TilePartCombination/TilePart.cpp TilePartCombination/TilePartCombination.cpp TilePartCombination/TilePartEntry.cpp TilePartCombination/TilePartSet.cpp TilePartCombination/TilePartTable.cpp)

#add_subdirectory(MinLackTable)
#add_subdirectory(TilePartCombination)

#target_link_libraries(MJBase_Lib PUBLIC -static Base_Lib)
target_link_libraries(MJBase_Lib PUBLIC Base_Lib)
target_include_directories(MJBase_Lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
