idf_component_register(SRCS "main.c" "PCF85063A.c" 
                        INCLUDE_DIRS "."
                        REQUIRES esp_driver_i2c
                    )
