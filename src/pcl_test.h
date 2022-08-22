/*
 *   PCL test class
 */
#ifndef PCL_test_H
#define PCL_test_H

#include <string>

class PCLTest {
   public:
	PCLTest(){};
	void ComputeConcaveHull(const std::string& pcd_file_name);
};
#endif