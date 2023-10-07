function(add_shader TARGET SHADER)

  add_custom_target(shader_${SHADER}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER} | xxd -i > ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders/${SHADER}.data
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER}
  )
  add_dependencies(${TARGET} shader_${SHADER})
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER})
endfunction()