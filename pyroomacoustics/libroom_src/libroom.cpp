/*
 * This file contains the bindings for the libroom ISM model
 */
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <Eigen/Dense>

#include "geometry.hpp"
#include "utility.hpp"
#include "wall.hpp"
#include "room.hpp"

namespace py = pybind11;

float libroom_eps = 1e-5;  // epsilon is set to 0.01 millimeter (10 um)

Room *create_room(py::list _walls, py::list _obstructing_walls, const Eigen::MatrixXf &_microphones)
{
  /*
   * This is a factory method to isolate the Room class from pybind code
   */
  Room *room = new Room();

  room->microphones = _microphones;
  

  for (auto wall : _walls)
    room->walls.push_back(wall.cast<Wall>());

  for (auto owall : _obstructing_walls)
    room->obstructing_walls.push_back(owall.cast<int>());

  room->dim = room->walls[0].dim;
  
  // Specific to ray tracing
  room->mic_pos = _microphones.col(0);
  room->max_dist = room->get_max_distance();

  return room;
}

PYBIND11_MODULE(libroom, m) {
    m.doc() = "Libroom room simulation extension plugin"; // optional module docstring

    // The Room class
    py::class_<Room>(m, "Room")
      //.def(py::init<py::list, py::list, const Eigen::MatrixXf &>())
      .def(py::init(&create_room))
      .def("image_source_model", &Room::image_source_model)
      .def("image_source_shoebox", &Room::image_source_shoebox)
      .def("get_wall", &Room::get_wall)
      .def("get_max_distance", &Room::get_max_distance)
      .def("next_wall_hit", &Room::next_wall_hit)
      .def("scat_ray", &Room::scat_ray)
      .def("simul_ray", &Room::simul_ray)
      .def("get_rir_entries", &Room::get_rir_entries)  
      .def("contains", &Room::contains)   
      .def_readonly("sources", &Room::sources)
      .def_readonly("orders", &Room::orders)
      .def_readonly("attenuations", &Room::attenuations)
      .def_readonly("gen_walls", &Room::gen_walls)
      .def_readonly("visible_mics", &Room::visible_mics)
      .def_readonly("walls", &Room::walls)
      .def_readonly("obstructing_walls", &Room::obstructing_walls)
      .def_readonly("microphones", &Room::microphones)
      .def_readonly("mic_pos", &Room::mic_pos)
      .def_readonly("max_dist", &Room::max_dist)
      
      ;

    // The Wall class
    py::class_<Wall> wall_cls(m, "Wall");

    wall_cls
        .def(py::init<const Eigen::MatrixXf &, float, const std::string &>(),
            py::arg("corners"), py::arg("absorption") = 0., py::arg("name") = "")
        .def("area", &Wall::area)
        .def("intersection", &Wall::intersection)
        .def("intersects", &Wall::intersects)
        .def("side", &Wall::side)
        .def("reflect", &Wall::reflect)
        .def("same_as", &Wall::same_as)
        .def_readonly("dim", &Wall::dim)
        .def_readwrite("absorption", &Wall::absorption)
        .def_readwrite("name", &Wall::name)
        .def_readonly("corners", &Wall::corners)
        .def_readonly("origin", &Wall::origin)
        .def_readonly("normal", &Wall::normal)
        .def_readonly("basis", &Wall::basis)
        .def_readonly("flat_corners", &Wall::flat_corners)
        ;

    py::enum_<Wall::Isect>(wall_cls, "Isect")
      .value("NONE", Wall::Isect::NONE)
      .value("VALID", Wall::Isect::VALID)
      .value("ENDPT", Wall::Isect::ENDPT)
      .value("BNDRY", Wall::Isect::BNDRY)
      .export_values();

    // The different wall intersection cases
    m.attr("WALL_ISECT_NONE") = WALL_ISECT_NONE;
    m.attr("WALL_ISECT_VALID") = WALL_ISECT_VALID;
    m.attr("WALL_ISECT_VALID_ENDPT") = WALL_ISECT_VALID_ENDPT;
    m.attr("WALL_ISECT_VALID_BNDRY") = WALL_ISECT_VALID_BNDRY;

    // getter and setter for geometric epsilon
    m.def("set_eps", [](const float &eps) { libroom_eps = eps; });
    m.def("get_eps", []() { return libroom_eps; });

    // Routines for the geometry packages
    m.def("ccw3p", &ccw3p, "Determines the orientation of three points");

    m.def("check_intersection_2d_segments",
        &check_intersection_2d_segments,
        "A function that checks if two line segments intersect");

    m.def("intersection_2d_segments",
        &intersection_2d_segments,
        "A function that finds the intersection of two line segments");

    m.def("intersection_3d_segment_plane",
        &intersection_3d_segment_plane,
        "A function that finds the intersection between a line segment and a plane");

    m.def("cross", &cross, "Cross product of two 3D vectors");

    m.def("is_inside_2d_polygon", &is_inside_2d_polygon,
        "Checks if a 2D point lies in or out of a planar polygon");

    m.def("area_2d_polygon", &area_2d_polygon,
        "Compute the signed area of a planar polygon");
		
	m.def("cos_angle_between", &cos_angle_between,
		"Computes the angle between two 2D or 3D vectors");
	
	m.def("dist_line_point", &dist_line_point,
		"Computes the distance between a point and an infinite line");
	
	
	// Routines for the utility packages
	m.def("equation", &equation,
		"Computes the a and b coefficients in the expression y=ax+b given two points lying on that line.");
		
	m.def("compute_segment_end", &compute_segment_end,
		"Computes the end point of a segment given the start point, the length, and the orientation");
		
	m.def("compute_reflected_end", &compute_reflected_end,
		"This function operates when we know the vector [start, hit_point]. This function computes the end point E so that [hit_point, E] is the reflected vector of [start, hit_point] with the correct magnitude");
	
	m.def("intersects_mic", &intersects_mic,
		"Determines if a segment intersects the microphone of specified center and radius");
		
	m.def("solve_quad", &solve_quad,
		"Solves the quadratic system and outputs real roots");
		
	m.def("mic_intersection", &mic_intersection,
		"Computes the intersection point between the ray and the microphone");
	
}

