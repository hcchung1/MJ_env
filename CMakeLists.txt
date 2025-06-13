cmake_minimum_required(VERSION 3.15)

# set the project name and version
project(MeowCaTS VERSION 1.0)

# specify the C++ standard
add_library(MeowCaTS_compiler_flags INTERFACE)
target_compile_features(MeowCaTS_compiler_flags INTERFACE cxx_std_17)

# configure a header file to pass some of the CMake settings to the source code
configure_file(MeowCaTSVersionConfig.h.in MeowCaTSVersionConfig.h)

# add the library
#add_subdirectory(MyTest)
#add_subdirectory(TestLib)
add_subdirectory(MJLibrary)
add_subdirectory(CommunicationLibrary)
add_subdirectory(DummyPlayer)
#add_subdirectory(SamplingLibrary)
#add_subdirectory(FlatMC_Player)
#add_subdirectory(MctsLibrary)
#add_subdirectory(Mcts_Player)
#add_subdirectory(ServerLibrary)
#add_subdirectory(Server)

