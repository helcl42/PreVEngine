
#include "prev/util/MathUtilsTests.h"
#include "prev/util/intersection/IntersectionTesterTests.h"

TEST(SampleTest, BasicAssertions)
{
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}