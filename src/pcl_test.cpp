/*
  File: pcl_test.cpp
  Author: Josef Salzmann <josef.salzmann@tuwien.ac.at>
  Copyright (c) 2022

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

void PCLTest::ComputeConcaveHull(const std::string& pcd_file_name) {
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>),
	    cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>),
	    cloud_projected(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PCDReader reader;

	reader.read(pcd_file_name, *cloud);

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_hull(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::ConcaveHull<pcl::PointXYZ> chull;
	std::vector<pcl::Vertices> polygons;
	chull.setInputCloud(cloud);
	chull.setAlpha(20);  // concave hull parameter (how far can points be away from each other and still be counted as connected)
	chull.reconstruct(*cloud_hull, polygons);

	std::cerr << "Concave hull has: " << cloud_hull->size()
	          << " data points." << std::endl;

	pcl::PCDWriter writer;
	// writer.write("rising_input_LUT_concave_hull.pcd", *cloud_hull, false);

	std::stringstream ss;
	ss << "OFF" << std::endl
	   << (int)cloud_hull->points.size() << " " << polygons.size() << " 0" << std::endl;

	ss << std::endl;
	for (int i = 0; i < (int)cloud_hull->points.size(); i++) {
		ss << cloud_hull->points[i].x << " " << cloud_hull->points[i].y << " " << cloud_hull->points[i].z << std::endl;
	}

	for (int i = 0; i < (int)polygons.size(); i++) {
		ss << "3  " << polygons[i].vertices[0] << " " << polygons[i].vertices[1] << " " << polygons[i].vertices[2] << std::endl;
	}

	std::ofstream output_file;
	output_file.open("output.off");
	output_file << ss.str();
	output_file.close();
}