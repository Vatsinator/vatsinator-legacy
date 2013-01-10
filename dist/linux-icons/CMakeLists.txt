file (GLOB LINUX_ICONS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/vatsinator-*.png"
)


foreach (ICON ${LINUX_ICONS})
  string (REGEX
    REPLACE "^vatsinator-([0-9]+).png$"
    "\\1"
    SIZE ${ICON}
  )
  
  install (FILES ${ICON}
    DESTINATION "share/icons/hicolor/${SIZE}x${SIZE}/apps"
    RENAME vatsinator.png
  )
endforeach (ICON)
