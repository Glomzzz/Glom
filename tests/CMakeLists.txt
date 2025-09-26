
if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )
    if (WIN32)
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(test_tokenizer tokenizer.cpp)
    add_executable(test_parser parser.cpp)
#    add_executable(test_expr expr.cpp)

    include(GoogleTest)
    # Link lib and dependencies
    foreach(test_target test_tokenizer test_parser)
        target_link_libraries(${test_target}
                glom
                GTest::gtest_main
        )
        gtest_discover_tests(${test_target})
    endforeach()

    set_target_properties(test_tokenizer test_parser
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
    endif()