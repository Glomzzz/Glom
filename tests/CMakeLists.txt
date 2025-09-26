if(BUILD_TESTING)
    # Try to find the system-installed GTest first
    find_package(GTest QUIET)

    if (NOT GTest_FOUND)
        # If not found, use FetchContent to download and build GTest
        message(STATUS "System GTest not found, fetching via FetchContent...")
        include(FetchContent)

        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG v1.17.0
        )
        if (WIN32)
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        endif()
        FetchContent_MakeAvailable(googletest)

        set(GTEST_USING_FETCH_CONTENT TRUE)
    else()
        message(STATUS "Found system GTest.")
        # Ensure the GTest::gtest_main target is available
        if (NOT TARGET GTest::gtest_main)
            add_library(GTest::gtest_main INTERFACE IMPORTED)
            target_link_libraries(GTest::gtest_main INTERFACE GTest::GTest GTest::Main)
        endif()
    endif()

    enable_testing()

    include(GoogleTest)

    # Link the test executables with GTest and the glom library
    # by auto discovery
    include(auto_discover_tests.cmake)

endif()