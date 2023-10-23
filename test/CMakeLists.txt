if("baseline" IN_LIST SubDirs)
    list(REMOVE_ITEM SubDirs "baseline")
    list(INSERT SubDirs 0 "baseline")
endif()

foreach(SubDir ${SubDirs})
    add_executable(conway_test_${SubDir} conway_test.cc)
    target_link_libraries(conway_test_${SubDir} PRIVATE conway conway_${SubDir})
    list(APPEND Test_Executables "conway_test_${SubDir}")
endforeach()

include("tests.cmake")