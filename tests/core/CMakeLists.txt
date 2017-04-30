find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)

add_executable (servertracking servertracking.cpp servertracking.h)
target_link_libraries (servertracking Qt5::Test vtrcore)
add_test (NAME ServerTracking COMMAND servertracking)

add_executable (vatsimdocumentsparsing vatsimdocumentsparsing.cpp vatsimdocumentsparsing.h)
target_link_libraries (vatsimdocumentsparsing Qt5::Test vtrcore)
add_test (NAME VatsimDocumentsParsing COMMAND vatsimdocumentsparsing)
