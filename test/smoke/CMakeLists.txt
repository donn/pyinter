configure_file(test.tcl ${CMAKE_CURRENT_BINARY_DIR}/test.tcl COPYONLY)

add_test(
    NAME SmokeTest
    COMMAND
        tclsh test.tcl $<TARGET_FILE:pyinter>
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)