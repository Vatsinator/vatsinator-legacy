cmake_minimum_required (VERSION 2.8)

execute_process (COMMAND env LC_ALL=C date "+%a, %-d %b %Y %H:%M:%S %z"
    OUTPUT_VARIABLE DEB_DATE OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/changelog.in
    ${CMAKE_CURRENT_SOURCE_DIR}/changelog
)
