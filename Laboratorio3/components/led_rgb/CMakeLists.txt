
idf_component_register(SRCS "src/led_rgb.c"                
                        INCLUDE_DIRS "include"              
                        REQUIRES driver led_strip esp_driver_rmt
                    )
                    
