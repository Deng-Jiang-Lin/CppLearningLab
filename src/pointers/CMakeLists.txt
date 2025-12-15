# 收集当前目录下所有 .cpp demo
file(GLOB OOD_DEMOS "*.cpp")

foreach(src ${OOD_DEMOS})
    # 取文件名（不带 .cpp）
    get_filename_component(target ${src} NAME_WE)

    add_executable(${target} ${src})

    # 可选：统一输出到 bin/OOD
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/OOD"
    )
endforeach()
