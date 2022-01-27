configure_file(test.tcl ${CMAKE_CURRENT_BINARY_DIR}/test.tcl COPYONLY)
configure_file(module.py ${CMAKE_CURRENT_BINARY_DIR}/module.py COPYONLY)

add_test(
    NAME "Simple External Python File"
    COMMAND
        tclsh test.tcl $<TARGET_FILE:pyinter>
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)