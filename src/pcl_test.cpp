#include "pcl_test.h"

#include <pcl/ModelCoefficients.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/surface/concave_hull.h>

#include <iostream>

void PCLTest::test() {
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>),
	    cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>),
	    cloud_projected(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PCDReader reader;

	reader.read("rising_input_LUT_test.pcd", *cloud);

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_hull(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::ConcaveHull<pcl::PointXYZ> chull;
	std::vector<pcl::Vertices> polygons;
	chull.setInputCloud(cloud);
	chull.setAlpha(5);
	chull.reconstruct(*cloud_hull, polygons);

	std::cerr << "Concave hull has: " << cloud_hull->size()
	          << " data points." << std::endl;

	pcl::PCDWriter writer;
	writer.write("rising_input_LUT_concave_hull.pcd", *cloud_hull, false);

	std::cout << "OFF" << std::endl
	          << (int)cloud_hull->points.size() << " " << polygons.size() << " 0" << std::endl;

	std::cout << std::endl;
	for (int i = 0; i < (int)cloud_hull->points.size(); i++) {
		std::cout << cloud_hull->points[i].x << " " << cloud_hull->points[i].y << " " << cloud_hull->points[i].z << std::endl;
	}

	for (int i = 0; i < (int)polygons.size(); i++) {
		std::cout << "3  " << polygons[i].vertices[0] << " " << polygons[i].vertices[1] << " " << polygons[i].vertices[2] << std::endl;
	}
}