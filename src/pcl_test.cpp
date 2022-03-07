#include "pcl_test.h"

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#include <iostream>

void PCLTest::test() {
	pcl::PointCloud<pcl::PointXYZ> cloud;
	cloud.width = 5;
	cloud.height = 1;
	cloud.is_dense = false;
	cloud.resize(cloud.width * cloud.height);

	for (auto& point : cloud) {
		point.x = 1024 * rand() / (RAND_MAX + 1.0f);
		point.y = 1024 * rand() / (RAND_MAX + 1.0f);
		point.z = 1024 * rand() / (RAND_MAX + 1.0f);
	}
	std::cout << "test" << std::endl;
	for (const auto& point : cloud)
		std::cout << "    " << point.x << " " << point.y << " " << point.z << std::endl;
}
