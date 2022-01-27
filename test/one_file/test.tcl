load [lindex $argv 0]

set ::env(PYINTER_SEARCH_PATHS) [file dirname [file normalize [info script]]]

pyinter_import module
namespace import module::*

puts [hello_name world]

puts [pi]