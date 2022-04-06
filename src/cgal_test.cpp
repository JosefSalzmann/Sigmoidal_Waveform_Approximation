#include "cgal_test.h"

// #include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/Aff_transformation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/IO/OFF.h>
#include <CGAL/IO/OFF_reader.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>
#include <CGAL/Polygon_mesh_processing/distance.h>
#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/transform.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Scale_space_reconstruction_3/Advancing_front_mesher.h>
#include <CGAL/Scale_space_reconstruction_3/Jet_smoother.h>
#include <CGAL/Scale_space_surface_reconstruction_3.h>
#include <CGAL/Side_of_triangle_mesh.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/poisson_surface_reconstruction.h>
#include <CGAL/remove_outliers.h>

#include <boost/range/empty.hpp>
#include <cstdlib>
#include <fstream>
#include <vector>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using FT = typename Kernel::FT;
using Point_3 = typename Kernel::Point_3;
using Vector_3 = typename Kernel::Vector_3;

using TAG = CGAL::Sequential_tag;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;
using Polyhedron = CGAL::Polyhedron_3<Kernel>;
using Affine_transformation_3 = CGAL::Aff_transformation_3<Kernel>;
// types
typedef Kernel::Point_3 Point_3;
typedef CGAL::Point_set_3<Point_3, Vector_3> Point_set;

int CGALTest::test() {
	Point_set points;
	// std::string fname = CGAL::data_file_path("points_3/kitten.xyz");
	CGAL::Surface_mesh<Point_3> test_mash;
	std::vector<Kernel::Point_3> points;
	std::vector<std::vector<std::size_t>> polygons;
	CGAL::IO::read_OFF("falling_input.off", points, polygons);
	CGAL::Polygon_mesh_processing::orient_polygon_soup(points, polygons);
	// CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, test_mash);

	std::cout << "Number of vertices: " << test_mash.vertices().size()
	          << ", Number of faces: " << test_mash.faces().size() << std::endl;

	auto mypoint = Point_3(1.0891, -12.7, -10.357);
	auto location = CGAL::Polygon_mesh_processing::locate(mypoint, test_mash);
	auto location_point = CGAL::Polygon_mesh_processing::construct_point(location, test_mash);
	std::ofstream f("out_af.off");
	f << "x: " << location_point.x() << std::endl;
	f << "y: " << location_point.y() << std::endl;
	f << "z: " << location_point.z() << std::endl;

	typedef typename boost::graph_traits<CGAL::Surface_mesh<Point_3>>::halfedge_descriptor halfedge_descriptor;
	for (halfedge_descriptor hd : halfedges(test_mash)) {
		if (CGAL::is_border(hd, test_mash)) {
			f << "halfedge detected" << std::endl;
		}
	}

	// CGAL::Side_of_triangle_mesh<CGAL::Surface_mesh<Point_3>, Kernel> inside(test_mash);
	// auto res = inside(mypoint);

	// if (res == CGAL::ON_BOUNDARY || res == CGAL::ON_BOUNDED_SIDE) {
	// 	f << "point is inside" << std::endl;
	// }
	// if (res == CGAL::ON_UNBOUNDED_SIDE) {
	// 	f << "point is outside" << std::endl;
	// }

	// f << location.second[0] << std::endl;
	// f << location.second[1] << std::endl;
	// f << location.second[2] << std::endl;
	// f << location.first << std::endl;
	// std::ifstream stream(fname, std::ios_base::binary);
	// if (!stream) {
	// 	std::cerr << "Error: cannot read file " << fname << std::endl;
	// 	return EXIT_FAILURE;
	// }
	// stream >> points;
	// std::cout << "Read " << points.size() << " point(s)" << std::endl;
	// if (points.empty())
	// 	return EXIT_FAILURE;
	// // typename Point_set::iterator rout_it = CGAL::remove_outliers<CGAL::Sequential_tag>(points,
	// //    24,                                           // Number of neighbors considered for evaluation
	// //    points.parameters().threshold_percent(5.0));  // Percentage of points to remove
	// // points.remove(rout_it, points.end());
	// // std::cout << points.number_of_removed_points()
	// //           << " point(s) are outliers." << std::endl;
	// // Applying point set processing algorithm to a CGAL::Point_set_3
	// // object does not erase the points from memory but place them in
	// // the garbage of the object: memory can be freeed by the user.
	// points.collect_garbage();
	// // Compute average spacing using neighborhood of 6 points
	// // double spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag>(points, 6);
	// // // Simplify using a grid of size 2 * average spacing
	// // typename Point_set::iterator gsim_it = CGAL::grid_simplify_point_set(points, 2. * spacing);
	// // points.remove(gsim_it, points.end());
	// // std::cout << points.number_of_removed_points()
	// //           << " point(s) removed after simplification." << std::endl;
	// // points.collect_garbage();
	// // CGAL::jet_smooth_point_set<CGAL::Sequential_tag>(points, 24);
	// int reconstruction_choice = 1;
	// if (reconstruction_choice == 0 || reconstruction_choice == -1)  // Poisson
	// {
	// 	// CGAL::jet_estimate_normals<CGAL::Sequential_tag>(points, 24);  // Use 24 neighbors
	// 	// // Orientation of normals, returns iterator to first unoriented point
	// 	// typename Point_set::iterator unoriented_points_begin =
	// 	//     CGAL::mst_orient_normals(points, 24);  // Use 24 neighbors
	// 	// points.remove(unoriented_points_begin, points.end());
	// 	// CGAL::Surface_mesh<Point_3> output_mesh;
	// 	// CGAL::poisson_surface_reconstruction_delaunay(points.begin(), points.end(),
	// 	//                                               points.point_map(), points.normal_map(),
	// 	//                                               output_mesh, spacing);
	// 	// std::ofstream f("out_poisson.ply", std::ios_base::binary);
	// 	// CGAL::IO::set_binary_mode(f);
	// 	// CGAL::IO::write_PLY(f, output_mesh);
	// 	// f.close();
	// } else if (reconstruction_choice == 1 || reconstruction_choice == -1)  // Advancing front
	// {
	// 	typedef std::array<std::size_t, 3> Facet;  // Triple of indices
	// 	std::vector<Facet> facets;
	// 	// The function is called using directly the points raw iterators
	// 	CGAL::advancing_front_surface_reconstruction(points.points().begin(),
	// 	                                             points.points().end(),
	// 	                                             std::back_inserter(facets));
	// 	std::cout << facets.size()
	// 	          << " facet(s) generated by reconstruction." << std::endl;
	// 	// copy points for random access
	// 	std::vector<Point_3> vertices;
	// 	vertices.reserve(points.size());
	// 	std::copy(points.points().begin(), points.points().end(), std::back_inserter(vertices));
	// 	CGAL::Surface_mesh<Point_3> output_mesh;
	// 	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(vertices, facets, output_mesh);
	// 	std::ofstream f("out_af.off");
	// 	f << output_mesh;
	// 	auto mypoint = Point_3(0, 0, 0);
	// 	auto location = CGAL::Polygon_mesh_processing::locate(mypoint, output_mesh);
	// 	// f << mypoint.x() << std::endl;
	// 	// f << mypoint.y() << std::endl;
	// 	// f << mypoint.z() << std::endl;
	// 	f << location.second[0] << std::endl;
	// 	f << location.second[1] << std::endl;
	// 	f << location.second[2] << std::endl;
	// 	f << location.first << std::endl;

	// 	// location.first.i
	// 	// output_mesh.f
	// 	f.close();
	// } else if (reconstruction_choice == 2 || reconstruction_choice == -1)  // Scale space
	// {
	// 	CGAL::Scale_space_surface_reconstruction_3<Kernel> reconstruct(points.points().begin(), points.points().end());
	// 	// Smooth using 4 iterations of Jet Smoothing
	// 	reconstruct.increase_scale(4, CGAL::Scale_space_reconstruction_3::Jet_smoother<Kernel>());
	// 	// Mesh with the Advancing Front mesher with a maximum facet length of 0.5
	// 	reconstruct.reconstruct_surface(CGAL::Scale_space_reconstruction_3::Advancing_front_mesher<Kernel>(0.5));
	// 	std::ofstream f("out_sp.off");
	// 	f << "OFF" << std::endl
	// 	  << points.size() << " "
	// 	  << reconstruct.number_of_facets() << " 0" << std::endl;
	// 	for (Point_set::Index idx : points)
	// 		f << points.point(idx) << std::endl;
	// 	for (const auto& facet : CGAL::make_range(reconstruct.facets_begin(), reconstruct.facets_end()))
	// 		f << "3 " << facet << std::endl;
	// 	f.close();
	// } else  // Handle error
	// {
	// 	std::cerr << "Error: invalid reconstruction id: " << reconstruction_choice << std::endl;
	// 	return EXIT_FAILURE;
	// }
	return EXIT_SUCCESS;
}
