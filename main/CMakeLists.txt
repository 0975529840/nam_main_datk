idf_component_register(SRCS "main.c" 
                    INCLUDE_DIRS "."
                    REQUIRES mqtt_protocol nvs_flash json sd_card_lib wifi_connect_lib)