/**
 * @file elf.cpp
 * @author Sean Kannanaikal
 * @brief implementation of Elf binary object type
 * @version 0.1
 * @date 2026-05-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include "elf.hpp"
#include "returncodes.hpp"


#define LABEL_WIDTH 40

/**
 * @brief Construct a new Elf:: Elf object
 * 
 * @param filename the file location for the elf binary 
 */
Elf::Elf(std::string filename) : m_filename(filename) 
{   
    //opening up file passed into constructor
	std::ifstream elf_file {filename, std::ios::binary};
	if (!elf_file)
	{
		std::cout << "[-] Unable to open up file " << filename << std::endl;
		std::exit(returncodes::FILE_READ_ERROR);
	}

	// Figuring out if its a 32 or 64 bit elf by reading the 5th byte from the e_ident
	std::uint8_t binary_type;
	elf_file.seekg(4);
	elf_file.read(reinterpret_cast<char *>(&binary_type), sizeof(binary_type));
	elf_file.seekg(0);

    /**
     * m_elf_header is a variant it can either be a `Elf_header32` or `Elf_header64`
     * we only know its value after reading the 5th byte from the e_ident array in the elf header
     * once we knowo what type it is we just go ahead and read the number of bytes corresponding to 
     * the right struct and fill it out automatically.
     */

	if (binary_type == 0x01)  //32 bit headers
	{   
        // getting elf header
		Elf_Header32 header;
		elf_file.read(reinterpret_cast<char *>(&header), sizeof(header));
		this->m_elf_header = header;
        
        // getting section headers
        Elf_S_Header32 s_header;
        uint32_t offset = header.e_shoff;
        for (uint16_t iterator = 0; iterator < header.e_shnum; iterator++) 
        {
            elf_file.seekg(offset);
            elf_file.read(reinterpret_cast<char *>(&s_header), sizeof(s_header));
            this->m_elf_s_headers.push_back(s_header);
            offset += sizeof(Elf_S_Header32);
        }
        
        
	}
	else if (binary_type == 0x02) //64 bit headers
	{
        // getting elf header
		Elf_Header64 header;
		elf_file.read(reinterpret_cast<char *>(&header), sizeof(header));
		this->m_elf_header = header;

        // getting section headers
        Elf_S_Header64 s_header;
        uint64_t offset = header.e_shoff;
        for (uint16_t iterator = 0; iterator < header.e_shnum; iterator++) 
        {
            elf_file.seekg(offset);
            elf_file.read(reinterpret_cast<char *>(&s_header), sizeof(s_header));
            this->m_elf_s_headers.push_back(s_header);
            offset += sizeof(Elf_S_Header64);
        }
	}

    //closing file after use
    elf_file.close();
}

/**
 * @brief Getter for Elf filename
 * 
 * @return std::string& 
 */
std::string& Elf::get_filename()
{
	return this->m_filename;
}

/**
 * @brief Getter for Elf header struct
 * 
 * @return std::variant<Elf_Header32, Elf_Header64>& 
 */
std::variant<Elf_Header32, Elf_Header64>& Elf::get_header()
{
	return this->m_elf_header;
}

std::vector<std::variant<Elf_S_Header32, Elf_S_Header64>>&   Elf::get_s_header()
{
    return this->m_elf_s_headers;
}

/**
 * @brief converts a byte value into a string containing a hex value
 * 
 * @param byte the byte value that is to be translated
 * @return std::string 
 */
static std::string _convert_to_hex(std::uint8_t byte)
{
	std::ostringstream output_stream;
	output_stream << std::hex
				  << std::setw(2) 
				  << std::setfill('0')
				  << static_cast<int>(byte);
	return output_stream.str();

}

/**
 * @brief translates the 'OS_ABI' byte value from elf header's e_ident array into the corresponding machine value
 * 
 * @param osabi the byte value for the osabi that will get converted to a string
 * @return std::string 
 */
static std::string _convert_abi(std::uint8_t osabi)
{
    switch (static_cast<OSABI>(osabi))
    {
    case OSABI::NONE:      return "UNIX - System V";
    case OSABI::HPUX:      return "HP-UX";
    case OSABI::NETBSD:    return "NetBSD";
    case OSABI::LINUX:     return "Linux";
    case OSABI::SOLARIS:   return "Solaris";
    case OSABI::AIX:       return "AIX";
    case OSABI::IRIX:      return "IRIX";
    case OSABI::FREEBSD:   return "FreeBSD";
    case OSABI::TRU64:     return "Tru64";
    case OSABI::MODESTO:   return "Novell Modesto";
    case OSABI::OPENBSD:   return "OpenBSD";
    case OSABI::OPENVMS:   return "OpenVMS";
    case OSABI::NSK:       return "HP NonStop Kernel";
    default:               return "Unknown";
    }
}

/**
 * @brief converts the 2 byte 'type' value from the elf header into a string 
 * 
 * @param type 2 byte `type` value
 * @return std::string 
 */
static std::string _convert_type(std::uint16_t type)
{
    switch (static_cast<Object_File_Type>(type))
    {
    case Object_File_Type::NONE:   return "No file type";
    case Object_File_Type::REL:    return "Relocatable file";
    case Object_File_Type::EXEC:   return "Executable file";
    case Object_File_Type::DYN:    return "Shared object file";
    case Object_File_Type::CORE:   return "Core file";
    case Object_File_Type::LOOS:   return "OS-specific (low)";
    case Object_File_Type::HIOS:   return "OS-specific (high)";
    case Object_File_Type::LOPROC: return "Processor-specific (low)";
    case Object_File_Type::HIPROC: return "Processor-specific (high)";
    default:                       return "Unknown";
    }
}

/**
 * @brief converts the 2 byte 'machine' value from the elf header into a string
 * 
 * @param machine 2 byte `machine` value
 * @return std::string 
 */
static std::string _convert_machine(std::uint16_t machine)
{
    switch (static_cast<Machine>(machine))
    {
    case Machine::NONE:         return "No machine";
    case Machine::M32:          return "AT&T WE 32100";
    case Machine::SPARC:        return "SPARC";
    case Machine::EM_386:       return "Intel 80386";
    case Machine::EM_68K:       return "Motorola 68000";
    case Machine::EM_88K:       return "Motorola 88000";
    case Machine::IAMCU:        return "Intel MCU";
    case Machine::EM_860:       return "Intel 80860";
    case Machine::MIPS:         return "MIPS";
    case Machine::S370:         return "IBM System/370";
    case Machine::MIPS_RS3_LE:  return "MIPS RS3000 Little-endian";

    case Machine::PARISC:       return "HP/PA";
    case Machine::VPP500:       return "Fujitsu VPP500";
    case Machine::SPARC32PLUS:  return "SPARC32+";
    case Machine::I960:         return "Intel 80960";
    case Machine::PPC:          return "PowerPC";
    case Machine::PPC64:        return "PowerPC64";
    case Machine::S390:         return "IBM S/390";
    case Machine::SPU:          return "SPU";

    case Machine::V800:         return "NEC V800";
    case Machine::FR20:         return "Fujitsu FR20";
    case Machine::RH32:         return "TRW RH-32";
    case Machine::RCE:          return "Motorola RCE";
    case Machine::ARM:          return "ARM";
    case Machine::ALPHA:        return "DEC Alpha";
    case Machine::SH:           return "Hitachi SH";
    case Machine::SPARCV9:      return "SPARC v9";
    case Machine::TRICORE:      return "Siemens TriCore";
    case Machine::ARC:          return "ARC";
    case Machine::H8_300:       return "Hitachi H8/300";
    case Machine::H8_300H:      return "Hitachi H8/300H";
    case Machine::H8S:          return "Hitachi H8S";
    case Machine::H8_500:       return "Hitachi H8/500";
    case Machine::IA_64:        return "Intel IA-64";
    case Machine::MIPS_X:       return "Stanford MIPS-X";
    case Machine::COLDFIRE:     return "Motorola ColdFire";
    case Machine::M68HC12:      return "Motorola M68HC12";
    case Machine::MMA:          return "Fujitsu MMA";
    case Machine::PCP:          return "Siemens PCP";
    case Machine::NCPU:         return "Sony nCPU";
    case Machine::NDR1:         return "Denso NDR1";
    case Machine::STARCORE:     return "Motorola Star*Core";
    case Machine::ME16:         return "Toyota ME16";
    case Machine::ST100:        return "STMicro ST100";
    case Machine::TINYJ:        return "Advanced Logic TinyJ";
    case Machine::X86_64:       return "AMD x86-64";
    case Machine::PDSP:         return "Sony DSP";

    case Machine::PDP10:        return "PDP-10";
    case Machine::PDP11:        return "PDP-11";
    case Machine::FX66:         return "FX66";
    case Machine::ST9PLUS:      return "ST9+";
    case Machine::ST7:          return "ST7";
    case Machine::MC68HC16:     return "MC68HC16";
    case Machine::MC68HC11:     return "MC68HC11";
    case Machine::MC68HC08:     return "MC68HC08";
    case Machine::MC68HC05:     return "MC68HC05";
    case Machine::SVX:          return "SVX";
    case Machine::ST19:         return "ST19";
    case Machine::VAX:          return "VAX";
    case Machine::CRIS:         return "CRIS";
    case Machine::JAVELIN:      return "Javelin";
    case Machine::FIREPATH:     return "FirePath";
    case Machine::ZSP:          return "ZSP";
    case Machine::MMIX:         return "MMIX";
    case Machine::HUANY:        return "Huany";
    case Machine::PRISM:        return "Prism";
    case Machine::AVR:          return "Atmel AVR";
    case Machine::FR30:         return "Fujitsu FR30";
    case Machine::D10V:         return "D10V";
    case Machine::D30V:         return "D30V";
    case Machine::V850:         return "NEC V850";
    case Machine::M32R:         return "M32R";
    case Machine::MN10300:      return "MN10300";
    case Machine::MN10200:      return "MN10200";
    case Machine::PJ:           return "PicoJava";
    case Machine::OPENRISC:     return "OpenRISC";
    case Machine::ARC_COMPACT:  return "ARC Compact";
    case Machine::XTENSA:       return "Xtensa";
    case Machine::VIDEOCORE:    return "VideoCore";
    case Machine::TMM_GPP:      return "TMM GPP";
    case Machine::NS32K:        return "NS32K";
    case Machine::TPC:          return "TPC";
    case Machine::SNP1K:        return "SNP1K";
    case Machine::ST200:        return "ST200";
    case Machine::IP2K:         return "IP2K";
    case Machine::MAX:          return "MAX";
    case Machine::CR:           return "CompactRISC";
    case Machine::F2MC16:       return "F2MC16";
    case Machine::MSP430:       return "MSP430";
    case Machine::BLACKFIN:     return "Blackfin";
    case Machine::SE_C33:       return "SE C33";
    case Machine::SEP:          return "SEP";
    case Machine::ARCA:         return "ARCA";
    case Machine::UNICORE:      return "UniCore";
    case Machine::EXCESS:       return "eXcess";
    case Machine::DXP:          return "DXP";
    case Machine::ALTERA_NIOS2: return "Altera Nios II";
    case Machine::CRX:          return "CRX";

    default:                    return "Unknown";
    }
}

/**
 * @brief Converts the ELF section header `sh_type` value into a readable string
 * 
 * @param sh_type 4-byte section type value from Elf32_Shdr / Elf64_Shdr
 * @return std::string Human-readable section type
 */
static std::string _convert_stype(std::uint32_t sh_type)
{
    switch (static_cast<SectionHeaderType>(sh_type))
    {
    case SectionHeaderType::SHT_NULL:           return "Unused section entry";
    case SectionHeaderType::SHT_PROGBITS:       return "Program data";
    case SectionHeaderType::SHT_SYMTAB:         return "Symbol table";
    case SectionHeaderType::SHT_STRTAB:         return "String table";
    case SectionHeaderType::SHT_RELA:           return "Relocation entries with addends";
    case SectionHeaderType::SHT_HASH:           return "Symbol hash table";
    case SectionHeaderType::SHT_DYNAMIC:        return "Dynamic linking information";
    case SectionHeaderType::SHT_NOTE:           return "Note section";
    case SectionHeaderType::SHT_NOBITS:         return "No file contents (.bss)";
    case SectionHeaderType::SHT_REL:            return "Relocation entries";
    case SectionHeaderType::SHT_SHLIB:          return "Reserved";
    case SectionHeaderType::SHT_DYNSYM:         return "Dynamic linker symbol table";
    case SectionHeaderType::SHT_INIT_ARRAY:     return "Constructor function array";
    case SectionHeaderType::SHT_FINI_ARRAY:     return "Destructor function array";
    case SectionHeaderType::SHT_PREINIT_ARRAY:  return "Pre-constructor function array";
    case SectionHeaderType::SHT_GROUP:          return "Section group";
    case SectionHeaderType::SHT_SYMTAB_SHNDX:   return "Extended symbol indices";
    case SectionHeaderType::SHT_LOOS:           return "OS-specific (low bound)";
    case SectionHeaderType::SHT_HIOS:           return "OS-specific (high bound)";
    case SectionHeaderType::SHT_LOPROC:         return "Processor-specific (low bound)";
    case SectionHeaderType::SHT_HIPROC:         return "Processor-specific (high bound)";
    case SectionHeaderType::SHT_LOUSER:         return "Application-specific (low bound)";
    case SectionHeaderType::SHT_HIUSER:         return "Application-specific (high bound)";
    default:                                    return "Unknown section type";
    }
}

/**
 * @brief Converts ELF section header flags into a readable string
 * 
 * @param sh_flags Section header flags field (`sh_flags`)
 * @return std::string Human-readable flag list
 */
static std::string _convert_sflags(std::uint32_t sh_flags)
{
    std::string flags = "";

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_WRITE))
    {
        flags += "SHF_WRITE";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_ALLOC))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_ALLOC";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_EXECINSTR))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_EXECINSTR";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_MERGE))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_MERGE";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_STRINGS))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_STRINGS";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_INFO_LINK))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_INFO_LINK";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_LINK_ORDER))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_LINK_ORDER";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_OS_NONCONFORMING))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_OS_NONCONFORMING";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_GROUP))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_GROUP";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_TLS))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_TLS";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_MASKOS))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_MASKOS";
    }

    if (sh_flags & static_cast<std::uint32_t>(SectionHeaderFlags::SHF_MASKPROC))
    {
        if (!flags.empty())
            flags += " | ";

        flags += "SHF_MASKPROC";
    }

    if (flags.empty())
        return "NONE";

    return flags;
}

/**
 * @brief will output the elf headers in a neat and user friendly manner
 * 
 */
std::string Elf::print_headers()
{
	std::ostringstream cli_output;

    /**
     * because elf_header is a variant need to use visit along with a general nameless function
     * to be able to  safely/easily handle the 2 elf header types and printing out their values
     * helpful because the elf headers are the same aside from a couple of value
     */
	std::visit([&cli_output](auto && elf_header)
	{	
		//printing out hex values of 16 bytes in e_ident array
		cli_output << "E_ident array values: ";
		for (int i = 0; i < 16; i++)
		{
            
			cli_output << _convert_to_hex(elf_header.e_ident[i]);

            if (i < 15) {
                cli_output << " ";
            }
		}
		cli_output<< std::endl;

		cli_output << std::left << std::setw(LABEL_WIDTH) << "Binary Type: ";
		if (elf_header.e_ident[4] == 0x01)
			cli_output << "32 bit elf";
		else if (elf_header.e_ident[4] == 0x02)
			cli_output << "64 bit elf";
		else
			cli_output << "unknown";
		cli_output<< std::endl;

		cli_output << std::left << std::setw(LABEL_WIDTH) << "Endianess: " ;
		if (elf_header.e_ident[5] == 0x01)
			cli_output << "Least Significant Bit (LSB)";
		else if (elf_header.e_ident[5] == 0x02)
			cli_output << "Most Significant Bit (MSB)";
		else
			cli_output << "unknown";
		cli_output<< std::endl;

		cli_output << std::left << std::setw(LABEL_WIDTH) << "Version: " ;
		if (elf_header.e_ident[6] == 0x01)
			cli_output << "Current Version (0x01)";
		else
			cli_output << "invalid";
		cli_output<< std::endl;

		cli_output << std::left << std::setw(LABEL_WIDTH) << "OS/ABI: " 								<< _convert_abi(elf_header.e_ident[7]) << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Type: "  									<< _convert_type(elf_header.e_type) << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Machine: "  								<< _convert_machine(elf_header.e_machine) << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Entry Point Address: "  					<< std::hex << std::showbase << elf_header.e_entry << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Program Headers Offset:  "  				<< std::dec <<  elf_header.e_phoff << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Section Headers Offset:  "  				<< std::dec <<  elf_header.e_shoff << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Program Headers Offset:  "  				<< std::dec <<  elf_header.e_phoff << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Flags: "  								<< std::hex << "0x"<< elf_header.e_flags << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Elf Headers Size:  "  					<< std::dec <<  elf_header.e_ehsize << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Program Header's Entry Size:  "  			<< std::dec <<  elf_header.e_phentsize << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Program Header's Entry Count:  "  		<< std::dec <<  elf_header.e_phnum << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Section Header's Entry Size:  "  			<< std::dec <<  elf_header.e_shentsize << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Section Header's Entry Count:  "  		<< std::dec <<  elf_header.e_shnum << std::endl;
		cli_output << std::left << std::setw(LABEL_WIDTH) << "Section Header's String table index:  "  	<< std::dec <<  elf_header.e_shstrndx << std::endl;

	}, this->m_elf_header);

	return cli_output.str();
}

std::string Elf::get_section_name(std::uint32_t string_t_offset) {
    std::string section_name;
    
    std::uint16_t shstrndx;
    std::visit([&](auto&& elf_header)
    {
        shstrndx = elf_header.e_shstrndx;
    }, this->m_elf_header);

    std::variant<Elf_S_Header32, Elf_S_Header64> string_table_sheader = this->m_elf_s_headers.at(shstrndx);
    std::string filename = get_filename();
    std::visit([&section_name, &filename, &string_t_offset](auto && string_table_sheader) {
        
        //opening up file passed into constructor
        std::ifstream elf_file {filename, std::ios::binary};
        if (!elf_file)
        {
            std::cout << "[-] Unable to open up file " << filename << std::endl;
            std::exit(returncodes::FILE_READ_ERROR);
        }

        // get to the file offset of the section header table and read the corresponding string_t_offset
        std::vector<char> string_table(string_table_sheader.sh_size);
        elf_file.seekg(string_table_sheader.sh_offset);
        elf_file.read(string_table.data(), string_table.size());

        section_name = &string_table[string_t_offset]; // not sure if this is going to read it up until the null character?

        elf_file.close();
    }, string_table_sheader);

    return section_name; // not sure if this is going to be some scope issue now though
}

std::string Elf::print_s_headers()
{
    std::ostringstream cli_output;

     /**
     * because elf_header is a variant need to use visit along with a general nameless function
     * to be able to  safely/easily handle the 2 elf header types and printing out their values
     * helpful because the elf headers are the same aside from a couple of value
     */
    
    for (int iterator = 0; iterator < this->m_elf_s_headers.size(); iterator++)
    {
        std::visit([this, &cli_output](auto && elf_s_header)
        {	
            cli_output << "Name: " << get_section_name(elf_s_header.sh_name) << "\n";
            cli_output << "\t" << "Type: "<< _convert_stype(elf_s_header.sh_type) << "\n";
            cli_output << "\t" << "Flags: "<< _convert_sflags(elf_s_header.sh_flags) << "\n";
            cli_output << "\t" << "Virtual Address: "<< std::hex << elf_s_header.sh_addr << "\n";
            cli_output << "\t" << "File offset: "<< std::dec << elf_s_header.sh_offset << " (bytes)" << "\n";
            cli_output << "\t" << "Section Size: "<< std::dec << elf_s_header.sh_size << " (bytes)" << "\n";
            
        },this->m_elf_s_headers.at(iterator));
    }
    
    return cli_output.str();
}
