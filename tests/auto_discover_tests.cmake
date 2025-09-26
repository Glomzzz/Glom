file(GLOB TEST_SOURCES "*.cpp")

set(TEST_TARGETS "")

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_target ${test_source} NAME_WE)

    if (NOT test_target MATCHES "^test_")
        set(test_target "test_${test_target}")
    endif()
    message(STATUS "Found test: ${test_target} from ${test_source}")

    add_executable(${test_target} ${test_source})
    target_link_libraries(${test_target} glom GTest::gtest_main)

    gtest_discover_tests(${test_target}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
            PROPERTIES LABELS "UnitTest"
    )
    list(APPEND TEST_TARGETS ${test_target})

    set_target_properties(${test_target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endforeach()

add_custom_target(build-test DEPENDS ${TEST_TARGETS})