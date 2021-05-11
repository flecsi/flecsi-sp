option(ENABLE_EXODUSII "Enable Exodus II" OFF)

if(ENABLE_EXODUSII)
  find_package(EXODUSII REQUIRED)

  if(NOT EXODUSII_FOUND)
    message(FATAL_ERROR "Exodus II is required for this configuration")
  endif()

  list(APPEND TPL_INCLUDES ${EXODUSII_INCLUDE_DIRS})
  list(APPEND TPL_LIBRARIES ${EXODUSII_LIBRARIES})
endif()
