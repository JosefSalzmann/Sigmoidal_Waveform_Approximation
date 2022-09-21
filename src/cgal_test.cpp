/*
  File: cgal_test.h
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

#include "cgal_test.h"

#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/Dynamic_property_map.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Side_of_triangle_mesh.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/helpers.h>

#include <fstream>
#include <iostream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

using Point_3 = typename K::Point_3;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;

typedef CGAL::Polyhedron_3<K, CGAL::Polyhedron_items_with_id_3> Polyhedron;
typedef K::Segment_3 Segment;
typedef CGAL::Surface_mesh<Point_3> Mesh;
typedef CGAL::AABB_face_graph_triangle_primitive<Mesh> AABB_face_graph_primitive;
typedef CGAL::AABB_traits<K, AABB_face_graph_primitive> AABB_face_graph_traits;

int CGALTest::test() {
	// Point_set points;
	// std::string fname = CGAL::data_file_path("points_3/kitten.xyz");
	CGAL::Surface_mesh<Point_3> test_mash;
	std::vector<K::Point_3> points;
	std::vector<std::vector<std::size_t>> polygons;
	CGAL::IO::read_OFF("falling_input.off", points, polygons);
	CGAL::Polygon_mesh_processing::orient_polygon_soup(points, polygons);
	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, test_mash);

	std::cout << "Number of vertices: " << test_mash.vertices().size()
	          << ", Number of faces: " << test_mash.faces().size() << std::endl;

	auto mypoint = Point_3(1.2, -12.3008, -10.068);
	auto mypoint_outside = Point_3(1000, 0, 0);

	Segment segment_query(mypoint, mypoint_outside);

	CGAL::AABB_tree<AABB_face_graph_traits> tree;
	CGAL::Polygon_mesh_processing::build_AABB_tree(test_mash, tree);
	if (tree.do_intersect(segment_query))
		std::cout << "intersection(s)" << std::endl;
	else
		std::cout << "no intersection" << std::endl;
	// computes #intersections with segment query
	std::cout << tree.number_of_intersected_primitives(segment_query)
	          << " intersection(s)" << std::endl;

	auto location = CGAL::Polygon_mesh_processing::locate_with_AABB_tree(mypoint, tree, test_mash);
	auto location_point = CGAL::Polygon_mesh_processing::construct_point(location, test_mash);
	std::ofstream f("out_af.off");
	f << "x: " << location_point.x() << std::endl;
	f << "y: " << location_point.y() << std::endl;
	f << "z: " << location_point.z() << std::endl;

	return EXIT_SUCCESS;
}
