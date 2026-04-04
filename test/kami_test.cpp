#include <gtest/gtest.h>
#include "../src/lib/test.h"

// Demonstrate some basic assertions.
TEST(KamiTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(KamiTest, BasicArithmetic) {
  EXPECT_EQ(add(3, 2), 5);

  EXPECT_EQ(subtract(3, 2), 1);
  
  EXPECT_EQ(multiply(3, 2), 6);
}
