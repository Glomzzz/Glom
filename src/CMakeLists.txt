file(GLOB PRIMITIVE_SOURCES "primitives/*.cpp")
message(STATUS "Found PRIMITIVE_SOURCES files: ${PRIMITIVE_SOURCES}")
add_library(glom
        tokenizer.cpp
        parser.cpp
        expr.cpp
        context.cpp
        eval.cpp
        primitive.cpp
        error.cpp
        bigint.cpp
        number.cpp
        module.cpp
        ${PRIMITIVE_SOURCES}
)
add_executable(glom_exe main.cpp)

target_link_libraries(glom_exe glom)

install(TARGETS glom_exe DESTINATION bin)