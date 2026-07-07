#include <zephyr/linker/iterable_sections.h>

/* Defines the required ROM iterable symbol bindings for 'angle_service' */
ITERABLE_SECTION_ROM(http_resource_desc_angle_service, Z_LINK_ITERABLE_SUBALIGN)