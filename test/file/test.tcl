load ../../pyinter.so

puts $::env(PWD)

pyinter_import test
namespace import test::*

puts [hello_name world]

puts [pi]