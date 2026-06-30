set(EXTRA_REQUIRES)
list(APPEND EXTRA_REQUIRES nvs_flash)

idf_component_register(
    SRCS main.c
    INCLUDE_DIRS .
    REQUIRES ${EXTRA_REQUIRES})

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_options(
    ${LVGL_LIB} 
    PRIVATE 
        -Wno-error=attributes
        -DLV_LVGL_H_INCLUDE_SIMPLE
        -DLV_USE_DEMO_MUSIC
)
