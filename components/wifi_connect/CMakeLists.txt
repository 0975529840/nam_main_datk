set(app_src wifi_connect.c)
set(pri_req nvs_flash mqtt sd_card_lib mqtt_protocol)
idf_component_register(SRCS ${app_src}
					INCLUDE_DIRS "."
					REQUIRES ${pri_req})
