set(EXTRA_REQUIRES)
if(IDF_VERSION_MAJOR GREATER_EQUAL 6)
    list(APPEND EXTRA_REQUIRES esp_driver_ledc)
endif()

idf_component_register(
    SRCS main.c
    INCLUDE_DIRS .
    REQUIRES ${EXTRA_REQUIRES})

if(IDF_VERSION_MAJOR GREATER_EQUAL 6)
    idf_component_get_property(BSP_LIB waveshare__esp32_s3_touch_lcd_4 COMPONENT_LIB)
    target_link_libraries(${BSP_LIB} PUBLIC idf::esp_driver_ledc)
endif()

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_options(${LVGL_LIB} PRIVATE -Wno-error=attributes)
