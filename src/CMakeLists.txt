add_library(glom
        tokenizer.cpp
        parser.cpp
        expr.cpp
        context.cpp
)
target_include_directories(glom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(glom_exe main.cpp)
target_link_libraries(glom_exe glom)


install(TARGETS glom DESTINATION bin)