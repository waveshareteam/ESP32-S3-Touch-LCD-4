set(CAN_DIR ./)
file(GLOB_RECURSE CAN_C_SRCS ${CAN_DIR}/*.c)
file(GLOB_RECURSE CAN_CPP_SRCS ${CAN_DIR}/*.cpp)

idf_component_register(
    SRCS ${CAN_C_SRCS} ${CAN_CPP_SRCS}
    INCLUDE_DIRS ${CAN_DIR}
    REQUIRES driver)

target_compile_options(
    ${COMPONENT_LIB}
    PRIVATE
        -Wno-format
        -Wno-incompatible-pointer-types
        -Wno-int-conversion
        -DLV_LVGL_H_INCLUDE_SIMPLE)
