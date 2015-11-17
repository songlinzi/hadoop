#include "fs/filesystem.h"
#include "fs/bad_datanode_tracker.h"

#include <gmock/gmock.h>

using ::testing::_;
using ::testing::InvokeArgument;
using ::testing::Return;

using namespace hdfs;

/**
 * Unit test for the tracker
 **/

/* make sure nodes can be added */
TEST(NodeExclusionTest, AddBadNode) {
  auto tracker = std::make_shared<BadDataNodeTracker>();

  ASSERT_FALSE(tracker->IsBadNode("dn1"));
  tracker->AddBadNode("dn1");
  ASSERT_TRUE(tracker->IsBadNode("dn1"));
  ASSERT_FALSE(tracker->IsBadNode("dn2"));
  tracker->AddBadNode("dn2");
  ASSERT_TRUE(tracker->IsBadNode("dn2"));
}

/* Make sure nodes get removed when time elapses */
TEST(NodeExclusionTest, RemoveOnTimeout) {
  auto tracker = std::make_shared<BadDataNodeTracker>();

  /* add node and make sure only that node is marked bad */
  std::string bad_dn("this_dn_died");
  tracker->AddBadNode(bad_dn);
  ASSERT_TRUE(tracker->IsBadNode(bad_dn));
  ASSERT_FALSE(tracker->IsBadNode("good_dn"));

  tracker->TEST_set_clock_shift(1000000);

  /* node should be removed on lookup after time shift */
  ASSERT_FALSE(tracker->IsBadNode(bad_dn));
}

/**
 * Unit tests for ExcludeSet
 **/

TEST(NodeExclusionTest, ExcludeSet) {
  /* empty case */
  auto exclude_set = std::make_shared<ExclusionSet>(std::set<std::string>());
  ASSERT_FALSE(exclude_set->IsBadNode("any_node"));

  /* common case */
  exclude_set =
      std::make_shared<ExclusionSet>(std::set<std::string>({"dn_1", "dn_3"}));
  ASSERT_TRUE(exclude_set->IsBadNode("dn_1"));
  ASSERT_FALSE(exclude_set->IsBadNode("dn_2"));
  ASSERT_TRUE(exclude_set->IsBadNode("dn_3"));
}

int main(int argc, char *argv[]) {
  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
