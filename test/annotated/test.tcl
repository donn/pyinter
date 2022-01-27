load ../../pyinter.so

pyinter_import test
namespace import test::*

set result [math -a 4 -b 2 -add]

if { $result != 6 } {
    puts stderr "Error: unexpected result $result (expected 6)"
    exit 1
}

set result [math -a 4 -b 2]

if { $result != 2 } {
    puts stderr "Error: unexpected result $result (expected 2)"
    exit 1
}