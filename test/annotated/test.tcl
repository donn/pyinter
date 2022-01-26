load ../../pyinter.so

pyinter_import test
namespace import test::*

set result [add 4 2]
if { $result != 6 } {
    puts stderr "Error: unexpected result $result (expected 6)"
    exit 1
}