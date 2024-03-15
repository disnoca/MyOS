#pragma once

#include <kernel/list.h>

#include <stdint.h>


typedef struct pci_device_descriptor_s {
	list_t list;

	uint32_t port_base;
	uint32_t interrupt;

	uint16_t bus;
	uint16_t device;
	uint16_t function;	
	uint16_t vendor_id;
	uint16_t device_id;

	uint8_t class_id;
	uint8_t subclass_id;
	uint8_t interface_id;	
	uint8_t revision;
} pci_device_descriptor_t;


/**
 * Returns all devices connected to the PCI.
 * 
 * @return the head of a list containing all devices connected to the PCI
*/
list_t* pci_get_connected_devices(void);
