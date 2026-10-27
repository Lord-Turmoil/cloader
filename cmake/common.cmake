function(cloader_enable_warnings target)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Cannot enable warnings: target '${target}' does not exist")
    endif()

    if(MSVC)
        target_compile_options(
            "${target}"
            PRIVATE
                /W4
                /permissive-
        )
    else()
        target_compile_options(
            "${target}"
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()
endfunction()

function(cloader_configure_compiler target)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Cannot apply compiler settings: target '${target}' does not exist")
    endif()

    target_compile_features("${target}" PUBLIC cxx_std_20)

    if(${CLOADER_IS_TOP_LEVEL})
        cloader_enable_warnings("${target}")
    endif()
endfunction()
