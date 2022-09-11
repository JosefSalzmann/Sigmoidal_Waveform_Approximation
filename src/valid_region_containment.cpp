#include "valid_region_containment.h"

void ValidRegionContainment::LoadOffFile(std::string file) {
	std::cout << "Reading " << file << std::endl;
	std::vector<K::Point_3> points;
	std::vector<std::vector<std::size_t>> polygons;
	// read in off file in single points and polygons

	// TODO: test if file is present
	CGAL::IO::read_OFF(file, points, polygons);
	// make sure the orientation of the polygons is correct
	CGAL::Polygon_mesh_processing::orient_polygon_soup(points, polygons);
	// finally convert the points and oriented polygons to a mesh
	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, parameter_mesh);

	// std::cout << "Number of vertices: " << test_mash.vertices().size()
	//           << ", Number of faces: " << test_mash.faces().size() << std::endl;

	// convert mesh into AABB tree for better performance
	CGAL::Polygon_mesh_processing::build_AABB_tree(parameter_mesh, parameter_aabb_tree);
}

bool ValidRegionContainment::ParametersAreOutsideValidRegion(const std::vector<float>& parameters) {
	auto current_point = BuildPointFromParameters(parameters);
	auto point_outside = Point_3(100000, 0, 0);

	Segment segment_query(current_point, point_outside);

	// check how many faces are intersected by the ray form the current to outside the parameter mesh
	// an even number means that the current point is outside the valid region
	if (parameter_aabb_tree.number_of_intersected_primitives(segment_query) % 2 == 0)
		return true;
	else
		return false;
}

std::vector<float> ValidRegionContainment::GetClosestInsideValidRegion(const std::vector<float>& parameters) {
	std::vector<float> multiplied_parameters = parameters;
	multiplied_parameters[0] = multiplied_parameters[0] * T_MULTIPLIER;
	auto current_point = BuildPointFromParameters(multiplied_parameters);

	auto location = CGAL::Polygon_mesh_processing::locate_with_AABB_tree(current_point, parameter_aabb_tree, parameter_mesh);
	auto location_point = CGAL::Polygon_mesh_processing::construct_point(location, parameter_mesh);

	return {(float)location_point.x() / (float)T_MULTIPLIER, (float)location_point.y(), (float)location_point.z()};
}

Point_3 ValidRegionContainment::BuildPointFromParameters(const std::vector<float>& parameters) {
	return Point_3(parameters[0], parameters[1], parameters[2]);
}