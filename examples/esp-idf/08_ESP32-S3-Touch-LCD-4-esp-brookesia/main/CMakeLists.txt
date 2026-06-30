set(EXTRA_REQUIRES)
if(IDF_VERSION_MAJOR GREATER_EQUAL 6)
    list(APPEND EXTRA_REQUIRES esp_driver_ledc)
endif()

idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES ${EXTRA_REQUIRES})

if(IDF_VERSION_MAJOR GREATER_EQUAL 6)
    idf_component_get_property(BSP_LIB waveshare__esp32_s3_touch_lcd_4 COMPONENT_LIB)
    target_link_libraries(${BSP_LIB} PUBLIC idf::esp_driver_ledc)
endif()

target_compile_options(${COMPONENT_LIB} PUBLIC -Wno-missing-field-initializers)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_options(${LVGL_LIB} PRIVATE -Wno-error=attributes)

# If want to use custom memory allocation functions, refer to the following macros:
if(CONFIG_ESP_BROOKESIA_MEMORY_USE_CUSTOM)
    # Get the exact component name
    idf_build_get_property(build_components BUILD_COMPONENTS)
    foreach(COMPONENT ${build_components})
        if(COMPONENT MATCHES "esp-brookesia" OR COMPONENT MATCHES "espressif__esp-brookesia")
            set(TARGET_COMPONENT ${COMPONENT})
            break()
        endif()
    endforeach()

    # Get the component library
    if(TARGET_COMPONENT STREQUAL "")
        message(FATAL_ERROR "Component 'esp-brookesia' not found.")
    else()
        idf_component_get_property(ESP_BROOKESIA_LIB ${TARGET_COMPONENT} COMPONENT_LIB)
    endif()

    # Add the custom memory allocation functions
    target_compile_options(
        ${ESP_BROOKESIA_LIB}
        PUBLIC
            "-DESP_BROOKESIA_MEMORY_CUSTOM_MALLOC(x)=heap_caps_aligned_alloc(1, x, MALLOC_CAP_SPIRAM)"  "-DESP_BROOKESIA_MEMORY_CUSTOM_FREE=free"
    )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
