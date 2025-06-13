#add_library(Communication_Lib BasePlayer.cpp BasePlayer2.cpp GameBridge.cpp)
add_library(Communication_Lib STATIC BasePlayer.cpp BasePlayer2.cpp GameBridge.cpp EpsilonGreedyPlayer.cpp)

#target_link_libraries(Communication_Lib PUBLIC -static MJBase_Lib)
target_link_libraries(Communication_Lib PUBLIC Base_Lib MJBase_Lib)
target_include_directories(Communication_Lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
