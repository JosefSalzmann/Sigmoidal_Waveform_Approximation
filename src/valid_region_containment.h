/*
*   ValidRegionContainment class
*/
#ifndef ValidRegionContainment_H
#define ValidRegionContainment_H

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
#include <vector>

#include "logic_gate.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

using Point_3 = typename K::Point_3;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;

typedef CGAL::Polyhedron_3<K, CGAL::Polyhedron_items_with_id_3> Polyhedron;
typedef K::Segment_3 Segment;
typedef CGAL::Surface_mesh<Point_3> Mesh;
typedef CGAL::AABB_face_graph_triangle_primitive<Mesh> AABB_face_graph_primitive;
typedef CGAL::AABB_traits<K, AABB_face_graph_primitive> AABB_face_graph_traits;

class ValidRegionContainment {
   private:
	double T_MULTIPLIER = 1000;
	CGAL::Surface_mesh<Point_3> parameter_mesh;
	CGAL::AABB_tree<AABB_face_graph_traits> parameter_aabb_tree;
	Point_3 BuildPointFromParameters(const std::vector<float>& parameters);

   public:
	ValidRegionContainment() : parameter_mesh{}, parameter_aabb_tree{} {};
	void LoadOffFile(std::string file);
	bool ParametersAreOutsideValidRegion(const std::vector<float>& parameters);
	std::vector<float> GetClosestInsideValidRegion(const std::vector<float>& parameters);
};
#endif