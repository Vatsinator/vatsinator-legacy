set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

# exec file
set (vatsinator_TARGET
    vatsinator
)

if (APPLE) # bundle looks better
  set (vatsinator_TARGET
      Vatsinator
  )
endif (APPLE)

set (vatsinator_SRCS
    netconfig.cpp
    vatsinatorapplication.cpp
)

set (vatsinator_SUBDIRS
    db
    events
    glutils
    modules
    network
    storage
    ui
    vatsimdata
)

include (Macros)
if (CMAKE_COMPILER_IS_GNUCC)
    include (GccVersion)
        
    # workaround for gcc <= 4.6
    if (NOT GCC_VERSION_47)
      add_definitions("-Doverride=" "")
    endif (NOT GCC_VERSION_47)
endif (CMAKE_COMPILER_IS_GNUCC)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (${vatsinator_TARGET} WIN32
    main.cpp
    ${vatsinator_SRCS}
)

# fix for older versions of qjson library
if (qjson_LIBRARIES)
    set (QJSON_LIBRARIES ${qjson_LIBRARIES})
endif (qjson_LIBRARIES)

# Set PREFIX for defines.h and necessary libs
if (LINUX)
    set (vatsinator_PREFIX "${CMAKE_INSTALL_PREFIX}/share/vatsinator/")

    set (vatsinator_LIBS
        Qt5::Widgets
        Qt5::Network
        Qt5::Xml
        Qt5::Svg
        Qt5::OpenGL
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
        ${QJSON_LIBRARIES}
    )
elseif (APPLE)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        ${QT_LIBRARIES}
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
        ${QJSON_LIBRARIES}
    )
else (WIN32)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        Qt5::Widgets
        Qt5::Network
        Qt5::Xml
        Qt5::Svg
        Qt5::OpenGL
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
        ${QJSON_LIBRARIES}
    )
    
    if (MINGW)
      set (vatsinator_LIBS
        mingw32
        ${vatsinator_LIBS}
      )
    elseif (MSVC)
      set (vatsinator_LIBS
        ${vatsinator_LIBS}
        Qt5::WinMain
      )
    endif (MINGW)
	# disable VBO on Win32 build - Vatsinator doesn't handle it properly
	  set (CONFIG_NO_VBO 1)
endif (LINUX)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vatsinator-config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

foreach (DIR ${vatsinator_SUBDIRS})
   add_subdirectory (${DIR})
    add_dependencies (${vatsinator_TARGET}
        ${DIR}
    )
endforeach (DIR)

add_subdirectory (i18n)
add_dependencies (${vatsinator_TARGET} i18n)

if (APPLE)
    target_link_libraries (${vatsinator_TARGET}
        -Wl,-all_load ${vatsinator_SUBDIRS}
    )
else (APPLE)
  if (CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries (${vatsinator_TARGET}
      -Wl,--start-group
      -Wl,--whole-archive
      ${vatsinator_SUBDIRS}
      -Wl,--no-whole-archive
      -Wl,--end-group
    )
  elseif (MSVC)
    target_link_libraries (${vatsinator_TARGET} ${vatsinator_SUBDIRS})
  endif (CMAKE_COMPILER_IS_GNUCC)
endif (APPLE)

target_link_libraries (${vatsinator_TARGET} ${vatsinator_LIBS})

#get_property (vatsinator_EXECUTABLE TARGET ${vatsinator_TARGET} PROPERTY LOCATION)
#get_filename_component (vatsinator_EXECUTABLE ${vatsinator_EXECUTABLE} NAME)
set (vatsinator_EXECUTABLE vatsinator.exe)


if (LINUX)

    install (TARGETS ${vatsinator_TARGET} RUNTIME DESTINATION bin)
    
elseif (APPLE)

    # move all files to .app bundle
    set (vatsinator_BUNDLE ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app)
    
    add_custom_command (TARGET ${vatsinator_TARGET} POST_BUILD
        COMMAND mkdir ARGS -p
        ${vatsinator_BUNDLE}/Contents/Resources/
        COMMAND cp ARGS -R
        ${MACOSX_BUNDLE_ICON} ${vatsinator_BUNDLE}/Contents/Resources/
        COMMAND cp ARGS -R
        ${CMAKE_BINARY_DIR}/vatsinator.resources/* ${vatsinator_BUNDLE}/Contents/Resources/
    )
    
    install (TARGETS ${vatsinator_TARGET} DESTINATION .)
    
    #include (DeployQt4)
    #install_qt4_executable ("${vatsinator_TARGET}.app"
    #    "" # qtplugins
    #    "" # libs
    #    "/usr/local/lib" # dirs
    #)
    
elseif (WIN32)

    install (TARGETS ${vatsinator_TARGET} DESTINATION .)
    
    if (MSVC)
      get_property (qjson_LIBRARY TARGET qjson PROPERTY IMPORTED_LOCATION_RELEASE)
      get_filename_component (QJSON_LIBRARY_DIR ${qjson_LIBRARY} PATH)
	
	    if (NOT QT_QMAKE_EXECUTABLE)
			get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
		endif (NOT QT_QMAKE_EXECUTABLE)
		exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_BINS"
			OUTPUT_VARIABLE QT_INSTALL_LIBS)
      
      include (DeployQt5)
      install_qt5_executable ("${vatsinator_EXECUTABLE}"
          "" # qtplugins
          "" # libs
          "${QT_INSTALL_LIBS};${QUAZIP_LIBRARY_DIR};${QJSON_LIBRARY_DIR}"
        )
    endif (MSVC)
    
endif (LINUX)
