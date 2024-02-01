add_custom_target(multiprotocol_rpmsg_shared_property_target)
add_custom_target(net_core_shared_property_target)
add_custom_target(CPUNET_shared_property_target)
set_property(TARGET multiprotocol_rpmsg_shared_property_target  PROPERTY KERNEL_HEX_NAME;zephyr.hex)
set_property(TARGET multiprotocol_rpmsg_shared_property_target  PROPERTY ZEPHYR_BINARY_DIR;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr)
set_property(TARGET multiprotocol_rpmsg_shared_property_target  PROPERTY KERNEL_ELF_NAME;zephyr.elf)
set_property(TARGET multiprotocol_rpmsg_shared_property_target  PROPERTY BUILD_BYPRODUCTS;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr/zephyr.hex;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr/zephyr.elf)
set_property(TARGET net_core_shared_property_target  PROPERTY SOC;nRF5340_CPUNET_QKAA)
set_property(TARGET net_core_shared_property_target  PROPERTY VERSION;1)
include(/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/b0n/shared_vars.cmake)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_DOMAIN_PARTITIONS;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/partitions_CPUNET.yml)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_DOMAIN_REGIONS;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/regions_CPUNET.yml)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_DOMAIN_HEADER_FILES;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/b0n/zephyr/include/generated/pm_config.h;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr/include/generated/pm_config.h)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_DOMAIN_IMAGES;CPUNET:b0n;CPUNET:multiprotocol_rpmsg)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_HEX_FILE;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr/merged_CPUNET.hex)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_DOTCONF_FILES;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/pm_CPUNET.config)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_APP_HEX;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr/app.hex)
set_property(TARGET multiprotocol_rpmsg_shared_property_target APPEND PROPERTY BUILD_BYPRODUCTS;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr/merged_CPUNET.hex)
set_property(TARGET CPUNET_shared_property_target  PROPERTY PM_SIGNED_APP_HEX;/work/nrfconnect/nrf/applications/matter_weather_station/debug/multiprotocol_rpmsg/zephyr/signed_by_b0_app.hex)
