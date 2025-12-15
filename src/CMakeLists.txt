# 遍历 src 下的所有子目录
file(GLOB SRC_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(dir ${SRC_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
        # 遍历该子目录下的所有 .cpp
        file(GLOB DEMOS "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.cpp")

        foreach(src ${DEMOS})
            get_filename_component(target ${src} NAME_WE)
            add_executable(${target} ${src})

            # 输出到 bin/子目录名
            set_target_properties(${target} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${dir}"
            )
        endforeach()
    endif()
endforeach()
