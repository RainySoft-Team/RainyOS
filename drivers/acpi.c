#include "acpi.h"
#include "heap.h"
#include "common.h"

static acpi_rsdp_t *rsdp;
static acpi_rsdt_t *rsdt;
static acpi_fadt_t *fadt;

static bool checksum(uint8_t *addr, uint32_t len)
{
    uint8_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += addr[i];
    }
    return !sum;
}

static acpi_rsdp_t *acpi_find_rsdp()
{
    uint32_t *addr;
    for (addr = (uint32_t *) RSDP_START_ADDR; addr < (uint32_t *) RSDP_END_ADDR; addr++) {
        if (!memcmp(addr, "RSD PTR ", 8)) {
            if (checksum((uint8_t *) addr, ((acpi_rsdp_t *) addr)->Length)) {
                return (acpi_rsdp_t *) addr;
            }
        }
    }
    return NULL;
}

static uint32_t acpi_find_table(char *signature)
{
    int len = (rsdt->header.Length - sizeof(rsdt->header)) / 4;
    acpi_sdt_header_t *header;
    for (int i = 0; i < len; i++) {
        header = (acpi_sdt_header_t *) ((uint32_t) rsdt->Entry + i * 4);
        if (!memcmp(header->Signature, signature, 4)) return (uint32_t) header;
    }
    return 0;
}

void init_acpi()
{
    rsdp = acpi_find_rsdp();
    if (!rsdp) return;
    rsdt = (acpi_rsdt_t *) rsdp->RsdtAddress;
    if (!checksum((uint8_t *) rsdt, rsdt->header.Length)) return;
    fadt = (acpi_fadt_t *) acpi_find_table("FACP");
    if (!checksum((uint8_t *) fadt, fadt->header.Length)) return;
    if (!(inw(fadt->PM1aControlBlock) & 1)) {
        if (fadt->SMI_CommandPort && fadt->AcpiEnable) {
            outb(fadt->SMI_CommandPort, fadt->AcpiEnable);
            while (!(inw(fadt->PM1aControlBlock) & 1));
            if (fadt->PM1bControlBlock) {
                while (!(inw(fadt->PM1bControlBlock) & 1));
            }
        }
    }
}

int acpi_shutdown()
{
    int i;
    uint16_t SLP_TYPa, SLP_TYPb;
    acpi_sdt_header_t *header = (acpi_sdt_header_t *) acpi_find_table("DSDT");
    char *S5Addr = (char *) header;
    int dsdtLength = (header->Length - sizeof(acpi_sdt_header_t)) / 4;
    for (i = 0; i < dsdtLength; i++) {
        if (!memcmp(S5Addr, "_S5_", 4)) break;
        S5Addr++;
    }
    if (i < dsdtLength) {
        if ((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12) {
            S5Addr += 5;
            S5Addr += ((*S5Addr & 0xC0) >> 6) + 2;
            if (*S5Addr == 0x0a) S5Addr++;
            SLP_TYPa = *(S5Addr) << 10;
            S5Addr++;
            if (*S5Addr == 0x0a) S5Addr++;
            SLP_TYPb = *(S5Addr) << 10;
            S5Addr++;
        }
        outw(fadt->PM1aControlBlock, SLP_TYPa | (1 << 13));
        if (fadt->PM1bControlBlock) {
            outw(fadt->PM1bControlBlock, SLP_TYPb | (1 << 13));
        }
    }
    return -1;
}

void reboot(){
    outb(0x64, 0xfe);
}
