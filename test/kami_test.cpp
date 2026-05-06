#include <gtest/gtest.h>
#include "../src/lib/elf.hpp"

/**
 * @brief exemplar test for comparison and understanding features of
 * gtest
 * 
 */
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

/**
 * @brief tests wheter or not the header extracted continues to output
 * the correct and consistent data
 * 
 */
TEST(ElfTests, ElfHeaderTest) {
  std::string expected = "E_ident array values: 7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00\n"
                         "Binary Type:                            64 bit elf\n"
                         "Endianess:                              Least Significant Bit (LSB)\n"
                         "Version:                                Current Version (0x01)\n"
                         "OS/ABI:                                 UNIX - System V\n"
                         "Type:                                   Executable file\n"
                         "Machine:                                AMD x86-64\n"
                         "Entry Point Address:                    0x400380\n"
                         "Program Headers Offset:                 64\n"
                         "Section Headers Offset:                 10472\n"
                         "Program Headers Offset:                 64\n"
                         "Flags:                                  0x0\n"
                         "Elf Headers Size:                       64\n"
                         "Program Header's Entry Size:            56\n"
                         "Program Header's Entry Count:           13\n"
                         "Section Header's Entry Size:            64\n"
                         "Section Header's Entry Count:           32\n"
                         "Section Header's String table index:    31\n";
  
  std::string testBin_filepath = "./test/bins/hello";
  Elf testBinary(testBin_filepath);
  std::string result = testBinary.print_headers();

  EXPECT_EQ(expected, result);
}
