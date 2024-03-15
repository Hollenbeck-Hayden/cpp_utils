macro(set_CppUtils_library_name LIBRARY_POSTFIX)
    set(LIBRARY_NAME "${CMAKE_PROJECT_NAME}${LIBRARY_POSTFIX}")
endmacro()

function(make_CppUtils_library LIBRARY_NAME FOLDER_NAME)
    file(GLOB HEADER_LIST CONFIGURE_DEPENDS "./*.h")
    file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "./*.cpp")

    add_library(${LIBRARY_NAME} ${HEADER_LIST})
    target_sources(${LIBRARY_NAME} PRIVATE ${SOURCE_LIST})

    target_include_directories(${LIBRARY_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CppUtils_BUILD_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_14)
endfunction()

function(install_CppUtils_library LIBRARY_NAME FOLDER_NAME)
    set(TARGETS_NAME "${LIBRARY_NAME}Targets")

    install(
        TARGETS ${LIBRARY_NAME}
        EXPORT ${TARGETS_NAME}
        LIBRARY DESTINATION ${CppUtils_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${CppUtils_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${CppUtils_INSTALL_BIN_DIR}
    )

    install(
        FILES ${HEADER_LIST}
        DESTINATION ${CppUtils_INSTALL_INCLUDE_DIR}/${FOLDER_NAME}
    )

    install(
        EXPORT ${TARGETS_NAME}
        FILE ${TARGETS_NAME}.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
endfunction()
