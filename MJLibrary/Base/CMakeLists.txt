add_library(Base_Lib STATIC ClientSocket.cpp ConfigManager.cpp DebugLogger.cpp Ini.cpp MyFutureHandler.cpp MyTimer.cpp OpenAddressHashTable.h OpenAddressHashTable2.h ReserviorOneSampler.h SGF.cpp SgfManager.cpp Tools.cpp Type.h)
#add_library(Base_Lib ClientSocket.cpp)

#target_link_libraries(Base_Lib PUBLIC -static MeowCaTS_compiler_flags)
#target_link_libraries(Base_Lib PUBLIC -static-libgcc -static-libstdc++ MeowCaTS_compiler_flags)
target_link_libraries(Base_Lib PUBLIC MeowCaTS_compiler_flags ws2_32.lib)

target_include_directories(Base_Lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#Ini, Config library
#add_library(Ini_Lib Ini.cpp)
#target_include_directories(Ini_Lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#add_library(Config_Lib ConfigManager.cpp)
#target_include_directories(Config_Lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
