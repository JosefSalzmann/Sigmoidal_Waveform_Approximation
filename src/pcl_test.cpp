#include "pcl_test.h"

#include <pcl/ModelCoefficients.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/surface/convex_hull.h>

#include <iostream>

void PCLTest::test() {
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	cloud->width = 60;
	cloud->height = 1;
	cloud->is_dense = false;
	cloud->resize(cloud->width * cloud->height);

	for (auto& point : *cloud) {
		point.x = 1024 * rand() / (RAND_MAX + 1.0f);
		point.y = 1024 * rand() / (RAND_MAX + 1.0f);
		point.z = 1024 * rand() / (RAND_MAX + 1.0f);
	}
	std::cout << "test" << std::endl;
	for (const auto& point : *cloud)
		std::cout << "    " << point.x << " " << point.y << " " << point.z << std::endl;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_hull(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::ConvexHull<pcl::PointXYZ> chull;
	chull.setInputCloud(cloud);
	chull.reconstruct(*cloud_hull);

	std::cerr << "Convex hull has: " << cloud_hull->size()
	          << " data points." << std::endl;

	pcl::PCDWriter writer;
	writer.write("table_scene_mug_stereo_textured_hull.pcd", *cloud_hull, false);
}
