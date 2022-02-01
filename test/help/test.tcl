load [lindex $argv 0]

set ::env(PYINTER_SEARCH_PATHS) [file dirname [file normalize [info script]]]

pyinter_import module
namespace import module::*

set original_stdout [dup $stdout]

close $stdout

set fval [function_with_help -help]

if { $fval != "" } {
    exit 1
}