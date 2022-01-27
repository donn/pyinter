load ../../pyinter.so

puts $::env(PWD)

pyinter_import module
namespace import module::*

puts [hello_name world]

puts [pi]