include (ExternalProject)
    
ExternalProject_Add (QuaZIP
    URL ${CMAKE_CURRENT_SOURCE_DIR}/quazip-0.5.zip
    PATCH_COMMAND patch -p1 -t -N < ${CMAKE_CURRENT_SOURCE_DIR}/quazip-zlib.patch
    
    CMAKE_ARGS -D CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    
    INSTALL_COMMAND ""
    
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

ExternalProject_Get_Property (QuaZIP source_dir)
ExternalProject_Get_Property (QuaZIP binary_dir)

set (QUAZIP_SOURCE_DIR ${source_dir} PARENT_SCOPE)
set (QUAZIP_LIBRARY_RELEASE ${binary_dir}/libquazip.dll PARENT_SCOPE)
set (QUAZIP_LIBRARIES ${binary_dir}/libquazip.dll PARENT_SCOPE)
