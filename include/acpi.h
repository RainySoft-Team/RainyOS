#ifndef _ACPI_H_
#define _ACPI_H_

#include "common.h"

typedef struct ACPI_RSDP {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
    uint32_t Length;
    uint32_t XsdtAddress[2];
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} acpi_rsdp_t;

typedef struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} acpi_sdt_header_t;

typedef struct ACPI_RSDT {
    acpi_sdt_header_t header;
    uint32_t Entry;
} acpi_rsdt_t;

// 注：此FADT非标准规定，舍去了大量没有用到的成员，慎用
typedef struct ACPI_FADT {
    acpi_sdt_header_t header;
    uint8_t padding_0[12];
    uint32_t SMI_CommandPort;
    uint8_t AcpiEnable;
    uint8_t padding_1[11];
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
} acpi_fadt_t;

#define RSDP_START_ADDR 0x000e0000
#define RSDP_END_ADDR   0x00100000

void init_acpi();
int acpi_shutdown();
void reboot();

#endif
