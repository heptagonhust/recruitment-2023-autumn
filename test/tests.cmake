list(APPEND CMAKE_TEST_ARGUMENTS "--output-on-failure")

getFiles(Inputs "/tmp/dataset/APSP" ".*raw.*")
getCheckFiles(CheckFiles Inputs)

foreach(TestName TestExe IN ZIP_LISTS SubDirs Test_Executables)
    foreach(Input Check IN ZIP_LISTS Inputs CheckFiles)
        getCaseSize(Size ${Input})
        add_test(NAME "APSP_${TestName}_${Size}" COMMAND ${TestExe} ${Input} ${Check})
    endforeach()
endforeach()