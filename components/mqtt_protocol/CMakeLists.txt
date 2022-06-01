set(app_src mqtt_protocol.c)
set(pri_req mqtt sd_card_lib wifi_connect_lib)
idf_component_register(SRCS ${app_src}
					INCLUDE_DIRS "."
					REQUIRES ${pri_req})
