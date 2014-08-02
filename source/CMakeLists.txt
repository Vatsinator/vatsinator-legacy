set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include (Utils)

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)
# Auto rcc feature does not work for object libraries as for now
# set (CMAKE_AUTORCC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (vatsinator_SRCS
  netconfig.cpp
  vatsinatorapplication.cpp
)

set (vatsinator_HEADERS
  netconfig.h
  vatsinatorapplication.h
)

# icon for Windows
if (WIN32)
  list (APPEND vatsinator_SRCS
      ${PROJECT_SOURCE_DIR}/dist/vatsinator.rc)
endif ()

add_subdirectory (db)
add_subdirectory (events)
add_subdirectory (network)
add_subdirectory (plugins)
add_subdirectory (storage)
add_subdirectory (ui)
add_subdirectory (vatsimdata)

if (ANDROID) # on Android we want to build library libvatsinator.so
  add_library (vatsinator SHARED
    main.cpp
    ${vatsinator_SRCS}
    ${vatsinator_HEADERS}
    $<TARGET_OBJECTS:db>
    $<TARGET_OBJECTS:events>
    $<TARGET_OBJECTS:network>
    $<TARGET_OBJECTS:plugins>
    $<TARGET_OBJECTS:storage>
    $<TARGET_OBJECTS:ui>
    $<TARGET_OBJECTS:vatsimdata>
    
    $<TARGET_OBJECTS:PolygonTriangulator>
  )
else (ANDROID)
  add_executable (vatsinator MACOSX_BUNDLE WIN32
    main.cpp
    ${vatsinator_SRCS}
    ${vatsinator_HEADERS}
    $<TARGET_OBJECTS:db>
    $<TARGET_OBJECTS:events>
    $<TARGET_OBJECTS:network>
    $<TARGET_OBJECTS:plugins>
    $<TARGET_OBJECTS:storage>
    $<TARGET_OBJECTS:ui>
    $<TARGET_OBJECTS:vatsimdata>
    
    $<TARGET_OBJECTS:PolygonTriangulator>
  )
endif ()

target_link_libraries (vatsinator
  Qt5::Widgets
  Qt5::Network
  Qt5::OpenGL
  ${OPENGL_LIBRARY}
  ${QUAZIP_LIBRARIES}
  ${vatsinator_STATIC_PLUGINS}
)

if (APPLE)
  target_link_libraries (vatsinator Qt5::MacExtras)
endif ()

if (LINUX)
  set (vatsinator_PREFIX "${CMAKE_INSTALL_PREFIX}/share/vatsinator/")
elseif (APPLE OR WIN32 OR ANDROID)
  set (vatsinator_PREFIX "")
endif ()

if (NOT vatsinator_UI_IMPLEMENTATION)
  message (FATAL_ERROR "No valid user interface implementation specified")
  message (STATUS "Known user interface implementations: widgets")
endif ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vatsinator-config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

add_subdirectory (i18n)
add_dependencies (vatsinator i18n)

if (LINUX)
  # Qt is always already deployed on Linux
  install (TARGETS vatsinator RUNTIME DESTINATION bin)
elseif (APPLE OR WIN32)
  install (TARGETS vatsinator DESTINATION .)
  
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
  include(InstallRequiredSystemLibraries)
  
  query_qmake (QT_INSTALL_LIBS QT_INSTALL_BINS)
  get_platform_qpa_plugin (QT_QPA_PLUGIN)
  
  if (NOT QUAZIP_LIBRARY_DIR)
    get_filename_component(QUAZIP_LIBRARY_DIR ${QUAZIP_LIBRARIES} DIRECTORY)
  endif ()
  
  get_target_location (EXECUTABLE vatsinator)
  
  include (DeployQt5)
  install_qt5_executable ("${EXECUTABLE}"
      "${QT_QPA_PLUGIN}" # qtplugins
      "" # libs
      "${QT_INSTALL_LIBS};${QUAZIP_LIBRARY_DIR}" # dirs
  )
elseif (ANDROID)
  install (TARGETS vatsinator DESTINATION bin)
endif ()
