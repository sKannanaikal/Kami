/**
 * @file elf.hpp
 * @author Sean Kannanaikal
 * @brief  header file for elf implementation
 * @version 0.1
 * @date 2026-05-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <string>
#include <cstdint>
#include <variant>

#ifndef ELF_HPP
#define ELF_HPP

/**
 * @brief the structure of the 64 bit elf header
 * 
 */
typedef struct elf_header64
{
    unsigned char  e_ident[16]; // ELF identification:
                               // [0–3]  Magic bytes (0x7F 'E' 'L' 'F')
                               // [4]    Class (1 = 32-bit, 2 = 64-bit)
                               // [5]    Data encoding (1 = little endian, 2 = big endian)
                               // [6]    ELF version (should be 1)
                               // [7]    OS/ABI
                               // [8]    ABI version
                               // [9–15] Padding (unused)

    std::uint16_t  e_type;      // Object file type (relocatable, executable, shared, core)
    std::uint16_t  e_machine;   // Target architecture (e.g., x86-64, ARM)
    std::uint32_t  e_version;   // ELF version (should be 1)

    std::uint64_t  e_entry;     // Entry point virtual address (where execution starts)
    std::uint64_t  e_phoff;     // File offset to program header table
    std::uint64_t  e_shoff;     // File offset to section header table

    std::uint32_t  e_flags;     // Processor-specific flags (usually 0 on x86)

    std::uint16_t  e_ehsize;    // ELF header size in bytes (64 for ELF64)
    std::uint16_t  e_phentsize; // Size of one program header entry
    std::uint16_t  e_phnum;     // Number of program header entries

    std::uint16_t  e_shentsize; // Size of one section header entry
    std::uint16_t  e_shnum;     // Number of section header entries
    std::uint16_t  e_shstrndx;  // Index of section header string table

} Elf_Header64;

/**
 * @brief structure representing 32 bit elf header
 * 
 */
typedef struct elf_header32
{
    unsigned char  e_ident[16]; // Same structure as ELF64 (see above)

    std::uint16_t  e_type;      // Object file type
    std::uint16_t  e_machine;   // Target architecture
    std::uint32_t  e_version;   // ELF version

    std::uint32_t  e_entry;     // Entry point address (32-bit)
    std::uint32_t  e_phoff;     // Program header table offset
    std::uint32_t  e_shoff;     // Section header table offset

    std::uint32_t  e_flags;     // Processor-specific flags

    std::uint16_t  e_ehsize;    // ELF header size (52 for ELF32)
    std::uint16_t  e_phentsize; // Size of program header entry
    std::uint16_t  e_phnum;     // Number of program header entries

    std::uint16_t  e_shentsize; // Size of section header entry
    std::uint16_t  e_shnum;     // Number of section header entries
    std::uint16_t  e_shstrndx;  // Section name string table index

} Elf_Header32;

/**
 * @brief enum mapping of all 1 byte values for `os abi` in elf header
 * 
 */
enum class OSABI : std::uint8_t
{
    NONE      = 0x00, // UNIX System V ABI
    HPUX      = 0x01, // HP-UX
    NETBSD    = 0x02, // NetBSD
    LINUX     = 0x03, // GNU/Linux
    SOLARIS   = 0x06, // Solaris
    AIX       = 0x07, // AIX
    IRIX      = 0x08, // IRIX
    FREEBSD   = 0x09, // FreeBSD
    TRU64     = 0x0A, // Tru64 UNIX
    MODESTO   = 0x0B, // Novell Modesto
    OPENBSD   = 0x0C, // OpenBSD
    OPENVMS   = 0x0D, // OpenVMS
    NSK       = 0x0E  // HP NonStop Kernel
};

/**
 * @brief enum mapping of all 2 byte values for `type` in elf header
 * 
 */
enum class Object_File_Type : std::uint16_t
{
    NONE   = 0x0000, // No file type
    REL    = 0x0001, // Relocatable file
    EXEC   = 0x0002, // Executable file
    DYN    = 0x0003, // Shared object / PIE
    CORE   = 0x0004, // Core dump file

    LOOS   = 0xFE00, // OS-specific range start
    HIOS   = 0xFEFF, // OS-specific range end

    LOPROC = 0xFF00, // Processor-specific range start
    HIPROC = 0xFFFF  // Processor-specific range end
};

/**
 * @brief enum mapping of all 2 byte values for `machine` in elf header
 * 
 */
enum class Machine : std::uint16_t
{
    NONE        = 0x0000, // No machine
    M32         = 0x0001, // AT&T WE 32100
    SPARC       = 0x0002, // SPARC
    EM_386      = 0x0003, // Intel 80386
    EM_68K      = 0x0004, // Motorola 68000
    EM_88K      = 0x0005, // Motorola 88000
    IAMCU       = 0x0006, // Intel MCU
    EM_860      = 0x0007, // Intel 80860
    MIPS        = 0x0008, // MIPS I Architecture
    S370        = 0x0009, // IBM System/370
    MIPS_RS3_LE = 0x000A, // MIPS RS3000 Little-endian

    PARISC      = 0x000F, // HP PA-RISC
    VPP500      = 0x0011, // Fujitsu VPP500
    SPARC32PLUS = 0x0012, // Enhanced SPARC (SPARC32+)
    I960        = 0x0013, // Intel 80960
    PPC         = 0x0014, // PowerPC
    PPC64       = 0x0015, // PowerPC64
    S390        = 0x0016, // IBM System/390
    SPU         = 0x0017, // IBM SPU/SPC

    V800        = 0x0024, // NEC V800
    FR20        = 0x0025, // Fujitsu FR20
    RH32        = 0x0026, // TRW RH-32
    RCE         = 0x0027, // Motorola RCE (embedded CPU)
    ARM         = 0x0028, // ARM (AArch32)
    ALPHA       = 0x0029, // DEC Alpha
    SH          = 0x002A, // Hitachi SuperH
    SPARCV9     = 0x002B, // SPARC v9 (64-bit)
    TRICORE     = 0x002C, // Siemens TriCore
    ARC         = 0x002D, // Argonaut RISC Core
    H8_300      = 0x002E, // Hitachi H8/300
    H8_300H     = 0x002F, // Hitachi H8/300H
    H8S         = 0x0030, // Hitachi H8S
    H8_500      = 0x0031, // Hitachi H8/500
    IA_64       = 0x0032, // Intel Itanium (IA-64)
    MIPS_X      = 0x0033, // Stanford MIPS-X
    COLDFIRE    = 0x0034, // Motorola ColdFire
    M68HC12     = 0x0035, // Motorola M68HC12
    MMA         = 0x0036, // Fujitsu MMA Multimedia Accelerator
    PCP         = 0x0037, // Siemens PCP
    NCPU        = 0x0038, // Sony nCPU embedded processor
    NDR1        = 0x0039, // Denso NDR1
    STARCORE    = 0x003A, // Motorola Star*Core DSP
    ME16        = 0x003B, // Toyota ME16 processor
    ST100       = 0x003C, // STMicroelectronics ST100
    TINYJ       = 0x003D, // Advanced Logic TinyJ (Java processor)
    X86_64      = 0x003E, // AMD x86-64 (x86_64)
    PDSP        = 0x003F, // Sony DSP

    PDP10       = 0x0040, // DEC PDP-10
    PDP11       = 0x0041, // DEC PDP-11
    FX66        = 0x0042, // Siemens FX66 microcontroller
    ST9PLUS     = 0x0043, // STMicroelectronics ST9+
    ST7         = 0x0044, // STMicroelectronics ST7
    MC68HC16    = 0x0045, // Motorola MC68HC16
    MC68HC11    = 0x0046, // Motorola MC68HC11
    MC68HC08    = 0x0047, // Motorola MC68HC08
    MC68HC05    = 0x0048, // Motorola MC68HC05
    SVX         = 0x0049, // Silicon Graphics SVx
    ST19        = 0x004A, // STMicroelectronics ST19
    VAX         = 0x004B, // DEC VAX
    CRIS        = 0x004C, // Axis Communications CRIS
    JAVELIN     = 0x004D, // Infineon Javelin
    FIREPATH    = 0x004E, // Element 14 FirePath DSP
    ZSP         = 0x004F, // LSI Logic ZSP DSP
    MMIX        = 0x0050, // Donald Knuth's educational MMIX
    HUANY       = 0x0051, // Harvard University machine (experimental)
    PRISM       = 0x0052, // SiTera Prism
    AVR         = 0x0053, // Atmel AVR 8-bit microcontroller
    FR30        = 0x0054, // Fujitsu FR30
    D10V        = 0x0055, // Mitsubishi D10V
    D30V        = 0x0056, // Mitsubishi D30V
    V850        = 0x0057, // NEC V850
    M32R        = 0x0058, // Mitsubishi M32R
    MN10300     = 0x0059, // Panasonic MN10300
    MN10200     = 0x005A, // Panasonic MN10200
    PJ          = 0x005B, // PicoJava (Java processor)
    OPENRISC    = 0x005C, // OpenRISC 32-bit
    ARC_COMPACT = 0x005D, // ARC Compact ISA
    XTENSA      = 0x005E, // Tensilica Xtensa
    VIDEOCORE   = 0x005F, // Broadcom VideoCore
    TMM_GPP     = 0x0060, // Thompson Multimedia GPP
    NS32K       = 0x0061, // National Semiconductor 32K
    TPC         = 0x0062, // Tenor Network TPC
    SNP1K       = 0x0063, // Trebia SNP 1000
    ST200       = 0x0064, // STMicroelectronics ST200
    IP2K        = 0x0065, // Ubicom IP2xxx
    MAX         = 0x0066, // MAX processor
    CR          = 0x0067, // National Semiconductor CompactRISC
    F2MC16      = 0x0068, // Fujitsu F2MC16
    MSP430      = 0x0069, // Texas Instruments MSP430
    BLACKFIN    = 0x006A, // Analog Devices Blackfin DSP
    SE_C33      = 0x006B, // Seiko Epson C33
    SEP         = 0x006C, // Sharp embedded processor
    ARCA        = 0x006D, // Arca RISC
    UNICORE     = 0x006E, // PKU-Unity UniCore
    EXCESS      = 0x006F, // eXcess configurable CPU
    DXP         = 0x0070, // Icera Deep Execution Processor
    ALTERA_NIOS2= 0x0071, // Altera Nios II soft-core CPU
    CRX         = 0x0072  // National Semiconductor CRX
};

/**
 * @brief class representing elf binary
 * 
 */
class Elf 
{
	private:
		std::string m_filename;
		std::variant<Elf_Header32, Elf_Header64> m_elf_header;
		
		void extract_headers(std::ifstream elf_file);

	public:
		// basic constructor
		Elf(std::string filename);
		
		std::string print_headers();
		
		// getters
		std::string& 								get_filename();
		std::variant<Elf_Header32, Elf_Header64>& 	get_header();
};

#endif /* ELF_HPP */
