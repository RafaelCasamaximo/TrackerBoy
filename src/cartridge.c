#include <cartridge.h>
#include <log.h>

typedef struct {
    char file_name[1024];
    u32 rom_size;
    u8* rom_data;
    bool use_new_licensee_code;
    cartridge_header* header;
} cartridge_ctx;

static cartridge_ctx ctx;


static char* NEW_LICENSEE_CODE[0xA5] = {
    [0x00] = "None",
    [0x01] = "Nintendo R&D1",
    [0x08] = "Capcom",
    [0x13] = "Electronic Arts",
    [0x18] = "Hudson Soft",
    [0x19] = "b-ai",
    [0x20] = "kss",
    [0x22] = "pow",
    [0x24] = "PCM Complete",
    [0x25] = "san-x",
    [0x28] = "Kemco Japan",
    [0x29] = "seta",
    [0x30] = "Viacom",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Ocean/Acclaim",
    [0x34] = "Konami",
    [0x35] = "Hector",
    [0x37] = "Taito",
    [0x38] = "Hudson",
    [0x39] = "Banpresto",
    [0x41] = "Ubi Soft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "angel",
    [0x47] = "Bullet-Proof",
    [0x49] = "irem",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "American sammy",
    [0x54] = "Konami",
    [0x55] = "Hi tech entertainment",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x58] = "Mattel",
    [0x59] = "Milton Bradley",
    [0x60] = "Titus",
    [0x61] = "Virgin",
    [0x64] = "LucasArts",
    [0x67] = "Ocean",
    [0x69] = "Electronic Arts",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbound",
    [0x73] = "sculptured",
    [0x75] = "sci",
    [0x78] = "THQ",
    [0x79] = "Accolade",
    [0x80] = "misawa",
    [0x83] = "lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x87] = "Tsukuda Original",
    [0x91] = "Chunsoft",
    [0x92] = "Video system",
    [0x93] = "Ocean/Acclaim",
    [0x95] = "Varie",
    [0x96] = "Yunezawa/s'pal",
    [0x97] = "Kaneko",
    [0x99] = "Pack in soft",
    [0xA4] = "Konami (Yu-Gi-Oh!)",
};

static char* OLD_LICENSEE_CODE[0x100] = {
    [0x00] = "None",
    [0x01] = "Nintendo",
    [0x08] = "Capcom",
    [0x09] = "Hot-B",
    [0x0A] = "Jaleco",
    [0x0B] = "Coconuts Japan",
    [0x0C] = "Elite Systems",
    [0x13] = "EA (Electronic Arts)",
    [0x18] = "Hudsonsoft",
    [0x19] = "ITC Entertainment",
    [0x1A] = "Yanoman",
    [0x1D] = "Japan Clary",
    [0x1F] = "Virgin Interactive",
    [0x24] = "PCM Complete",
    [0x25] = "San-X",
    [0x28] = "Kotobuki Systems",
    [0x29] = "Seta",
    [0x30] = "Infogrames",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Indicates that the New licensee code should be used instead.",
    [0x34] = "Konami",
    [0x35] = "HectorSoft",
    [0x38] = "Capcom",
    [0x39] = "Banpresto",
    [0x3C] = ".Entertainment i",
    [0x3E] = "Gremlin",
    [0x41] = "Ubisoft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "Angel",
    [0x47] = "Spectrum Holoby",
    [0x49] = "Irem",
    [0x4A] = "Virgin Interactive",
    [0x4D] = "Malibu",
    [0x4F] = "U.S. Gold",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "American Sammy",
    [0x54] = "GameTek",
    [0x55] = "Park Place",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x59] = "Milton Bradley",
    [0x5A] = "Mindscape",
    [0x5B] = "Romstar",
    [0x5C] = "Naxat Soft",
    [0x5D] = "Tradewest",
    [0x60] = "Titus",
    [0x61] = "Virgin Interactive",
    [0x67] = "Ocean Interactive",
    [0x69] = "EA (Electronic Arts)",
    [0x6E] = "Elite Systems",
    [0x6F] = "Electro Brain",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbund",
    [0x73] = "Sculptered Soft",
    [0x75] = "The Sales Curve",
    [0x78] = "t.hq",
    [0x79] = "Accolade",
    [0x7A] = "Triffix Entertainment",
    [0x7C] = "Microprose",
    [0x7F] = "Kemco",
    [0x80] = "Misawa Entertainment",
    [0x83] = "Lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x8B] = "Bullet-Proof Software",
    [0x8C] = "Vic Tokai",
    [0x8E] = "Ape",
    [0x8F] = "I’Max",
    [0x91] = "Chunsoft Co.",
    [0x92] = "Video System",
    [0x93] = "Tsubaraya Productions Co.",
    [0x95] = "Varie Corporation",
    [0x96] = "Yonezawa/S’Pal",
    [0x97] = "Kaneko",
    [0x99] = "Arc",
    [0x9A] = "Nihon Bussan",
    [0x9B] = "Tecmo",
    [0x9C] = "Imagineer",
    [0x9D] = "Banpresto",
    [0x9F] = "Nova",
    [0xA1] = "Hori Electric",
    [0xA2] = "Bandai",
    [0xA4] = "Konami",
    [0xA6] = "Kawada",
    [0xA7] = "Takara",
    [0xA9] = "Technos Japan",
    [0xAA] = "Broderbund",
    [0xAC] = "Toei Animation",
    [0xAD] = "Toho",
    [0xAF] = "Namco",
    [0xB0] = "acclaim",
    [0xB1] = "ASCII or Nexsoft",
    [0xB2] = "Bandai",
    [0xB4] = "Square Enix",
    [0xB6] = "HAL Laboratory",
    [0xB7] = "SNK",
    [0xB9] = "Pony Canyon",
    [0xBA] = "Culture Brain",
    [0xBB] = "Sunsoft",
    [0xBD] = "Sony Imagesoft",
    [0xBF] = "Sammy",
    [0xC0] = "Taito",
    [0xC2] = "Kemco",
    [0xC3] = "Squaresoft",
    [0xC4] = "Tokuma Shoten Intermedia",
    [0xC5] = "Data East",
    [0xC6] = "Tonkinhouse",
    [0xC8] = "Koei",
    [0xC9] = "UFL",
    [0xCA] = "Ultra",
    [0xCB] = "Vap",
    [0xCC] = "Use Corporation",
    [0xCD] = "Meldac",
    [0xCE] = ".Pony Canyon or",
    [0xCF] = "Angel",
    [0xD0] = "Taito",
    [0xD1] = "Sofel",
    [0xD2] = "Quest",
    [0xD3] = "Sigma Enterprises",
    [0xD4] = "ASK Kodansha Co.",
    [0xD6] = "Naxat Soft",
    [0xD7] = "Copya System",
    [0xD9] = "Banpresto",
    [0xDA] = "Tomy",
    [0xDB] = "LJN",
    [0xDD] = "NCS",
    [0xDE] = "Human",
    [0xDF] = "Altron",
    [0xE0] = "Jaleco",
    [0xE1] = "Towa Chiki",
    [0xE2] = "Yutaka",
    [0xE3] = "Varie",
    [0xE5] = "Epcoh",
    [0xE7] = "Athena",
    [0xE8] = "Asmik ACE Entertainment",
    [0xE9] = "Natsume",
    [0xEA] = "King Records",
    [0xEB] = "Atlus",
    [0xEC] = "Epic/Sony Records",
    [0xEE] = "IGS",
    [0xF0] = "A Wave",
    [0xF3] = "Extreme Entertainment",
    [0xFF] = "LJN",
};

static char* CARTRIDGE_TYPE[0x100] = {
    [0x00] = "ROM ONLY",
    [0x01] = "MBC1",
    [0x02] = "MBC1+RAM",
    [0x03] = "MBC1+RAM+BATTERY",
    [0x05] = "MBC2",
    [0x06] = "MBC2+BATTERY",
    [0x08] = "ROM+RAM*",
    [0x09] = "ROM+RAM+BATTERY*",
    [0x0B] = "MMM01",
    [0x0C] = "MMM01+RAM",
    [0x0D] = "MMM01+RAM+BATTERY",
    [0x0F] = "MBC3+TIMER+BATTERY",
    [0x10] = "MBC3+TIMER+RAM+BATTERY**",
    [0x11] = "MBC3",
    [0x12] = "MBC3+RAM**",
    [0x13] = "MBC3+RAM+BATTERY**",
    [0x19] = "MBC5",
    [0x1A] = "MBC5+RAM",
    [0x1B] = "MBC5+RAM+BATTERY",
    [0x1C] = "MBC5+RUMBLE",
    [0x1D] = "MBC5+RUMBLE+RAM",
    [0x1E] = "MBC5+RUMBLE+RAM+BATTERY",
    [0x20] = "MBC6",
    [0x22] = "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
    [0xFC] = "POCKET CAMERA",
    [0xFD] = "BANDAI TAMA5",
    [0xFE] = "HuC3",
    [0xFF] = "HuC1+RAM+BATTERY",
};

static char* RAM_SIZE[0x06] = {
    [0x00] = "0",
    [0x01] = "0",
    [0x02] = "8 KiB",
    [0x03] = "32 KiB",
    [0x04] = "128 KiB",
    [0x05] = "64 KiB",
};

static char* DESTINATION_CODE[0x2] = {
    [0x00] = "Japan (and possibly overseas)",
    [0x01] = "Overseas only",
};

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