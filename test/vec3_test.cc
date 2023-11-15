#include <gtest/gtest.h>

#include "../src/vec3.h"

point3 p_test = point3(3.0, 6.0, 21.0);

TEST(RotateTest, rotate3dxtest) {
  point3 p_result = rotate3d_x(p_test, pi/3);
  ASSERT_NEAR(p_result.x(), 3.0, 1.0e-3);
  ASSERT_NEAR(p_result.y(), 3.0-(21.0*sqrt(3.0)/2.0), 1.0e-3);
  ASSERT_NEAR(p_result.z(), (21.0/2.0)+(3.0*sqrt(3.0)), 1.0e-3);
}

TEST(RotateTest, rotate3dytest) {
  point3 p_result = rotate3d_y(p_test, pi/3);
  ASSERT_NEAR(p_result.x(), (3.0/2.0)*(1.0+7.0*sqrt(3.0)), 1.0e-3);
  ASSERT_NEAR(p_result.y(), 6.0, 1.0e-3);
  ASSERT_NEAR(p_result.z(), (-3.0/2.0)*(-7.0+sqrt(3.0)), 1.0e-3);
}

TEST(RotateTest, rotate3dztest) {
  point3 p_result = rotate3d_z(p_test, pi/3);
  ASSERT_NEAR(p_result.x(), (3.0/2.0)-(3.0*sqrt(3.0)), 1.0e-3);
  ASSERT_NEAR(p_result.y(), (3.0/2.0)*(2.0+sqrt(3.0)), 1.0e-3);
  ASSERT_NEAR(p_result.z(), 21.0, 1.0e-3);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}