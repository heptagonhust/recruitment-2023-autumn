if("baseline" IN_LIST SubDirs)
    list(REMOVE_ITEM SubDirs "baseline")
    list(INSERT SubDirs 0 "baseline")
endif()

foreach(SubDir ${SubDirs})
    add_executable(apsp_test_${SubDir} apsp_test.cc)
    target_link_libraries(apsp_test_${SubDir} PRIVATE graph apsp_${SubDir})
    list(APPEND Test_Executables "apsp_test_${SubDir}")
endforeach()

include("tests.cmake")