# add the executable
add_executable(DummyPlayer _DummyPlayer.cpp TestMode.cpp DummyPlayer.h)
set_target_properties(DummyPlayer PROPERTIES WIN32_EXECUTABLE OFF)
#target_link_libraries(DummyPlayer PUBLIC -static Base_Lib MJBase_Lib)
target_link_libraries(DummyPlayer PUBLIC Base_Lib MJBase_Lib Communication_Lib)
