#include <cartridge.h>
#include <log.h>
#include <lookup.h>

typedef struct {
    char file_name[1024];
    u32 rom_size;
    u8* rom_data;
    bool use_new_licensee_code;
    cartridge_header* header;
} cartridge_ctx;

static cartridge_ctx ctx;

extern char* NEW_LICENSEE_CODE[0xA5];
extern char* OLD_LICENSEE_CODE[0x100];
extern char* CARTRIDGE_TYPE[0x100];
extern char* RAM_SIZE[0x06];
extern char* DESTINATION_CODE[0x2];

/*
    Open ROM
    Read its content into rom data
    Save header in the header
    (Check if new_licensee_code should be used)
    Functions for boot and global checksums
*/

u8 get_checksum()
{
    uint8_t checksum = 0;
    for (uint16_t address = 0x0134; address <= 0x014C; address++) {
        checksum = checksum - ctx.rom_data[address] - 1;
    return checksum;
}
}

u16 get_global_checksum()
{
    NO_IMPL
}

bool use_new_licensee_code()
{
    if(ctx.header->lic_code == 0x33)
    {
        return true;
    }
    return false;
}

char* get_licensee_name()
{
    if(ctx.use_new_licensee_code && ctx.header->new_licensee_code < 0xA5)
    {
        return NEW_LICENSEE_CODE[ctx.header->new_licensee_code];
    }
    if(!ctx.use_new_licensee_code && ctx.header->lic_code < 0x100)
    {
        return OLD_LICENSEE_CODE[ctx.header->lic_code];
    }
    return "UNKNOWN";
}

char* get_cartridge_type()
{
    if(ctx.header->type < 0x100)
    {
        return CARTRIDGE_TYPE[ctx.header->type];
    }
    return "UNKNOWN";
}

char* get_ram_size()
{
    if(ctx.header->ram_size < 0x06)
    {
        return RAM_SIZE[ctx.header->ram_size];
    }

    return "UNKNOWN";
}

char* get_destination_region()
{
    if(ctx.header->dest_code < 0x2)
    {
        return DESTINATION_CODE[ctx.header->dest_code];
    }

    return "UNKNOWN";
}

int cartridge_load(char* file_name)
{
    snprintf(ctx.file_name, sizeof(ctx.file_name), "%s", file_name);

    // Open file and check
    FILE* fp = fopen(file_name, "r");
    if(!fp)
    {
        log_error("Could not open ROM: %s", file_name);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    ctx.rom_size = ftell(fp);
    rewind(fp);

    ctx.rom_data = malloc(ctx.rom_size);
    fread(ctx.rom_data, ctx.rom_size, 1, fp);

    fclose(fp);

    ctx.header = (cartridge_header*)(ctx.rom_data + 0x0100);
    ctx.header->title[15] = 0;

    ctx.use_new_licensee_code = use_new_licensee_code();

    ctx.header->global_checksum = get_global_checksum();

    // Debug dos valores
    printf("Title: %s\n", ctx.header->title);
    printf("Licensee: %s\n", get_licensee_name());
    printf("SGB Flag: %x\n", ctx.header->sgb_flag);
    printf("Type: %s\n", get_cartridge_type());
    printf("ROM Size: %d KB\n", 32 << ctx.rom_size);
    printf("RAM Size: %s\n", get_ram_size());
    printf("Dest Code: %s\n", get_destination_region());
    printf("Version: %x\n", ctx.header->version);
    printf("Checksum: %x (%s)\n", ctx.header->checksum, (get_checksum() & 0xFF) ? "PASSED" : "FAILED");
}

void cartridge_exit()
{
    free(ctx.rom_data);
}

u8 cartridge_read(u16 address)
{
    // Reads from cartridge and return address value (1 byte)
    return ctx.rom_data[address];
}

void cartridge_write(u16 address, u8 value)
{
    // Writes value to the address on the cartridge
    // ROM Only for now
    NO_IMPL
}