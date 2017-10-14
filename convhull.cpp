/*********************************************************************
Hypersimplex Representer
Copyright (C) 2017 Roman Gilg

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
*********************************************************************/

#include "convhull.h"
#include "hypersimplex.h"

#include <vector>
#include <algorithm>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polygon_2<K>                                  Polygon_2;
typedef K::Point_2                                          Point_2;
typedef CGAL::Polyhedron_3<K>                               Polyhedron_3;
typedef K::Point_3                                          Point_3;

ConvHull::ConvHull(MatrixXd &points)
{
    if (points.rows() == 2) {
        std::vector<Point_2> cgalPoints, result;

        for (int i = 0; i < points.cols(); i++) {
            cgalPoints.push_back(Point_2(points(0, i), points(1, i)));
        }

        CGAL::convex_hull_2(cgalPoints.begin(), cgalPoints.end(), std::back_inserter(result));

        Polygon_2 poly(result.begin(), result.end());

        std::cout << "The convex hull contains " << poly.size() << " vertices" << std::endl;

    } else if (points.rows() == 3) {
        std::vector<Point_3> cgalPoints;

        for (int i = 0; i < points.cols(); i++) {
            cgalPoints.push_back(Point_3(points(0, i), points(1, i), points(2, i)));
        }

        std::cout << "cgalPoints:" << std::endl;
        for (auto v : cgalPoints)
            std::cout << v  << std::endl;

        Polyhedron_3 poly;
        CGAL::convex_hull_3(cgalPoints.begin(), cgalPoints.end(), poly);

        std::cout << "The convex hull contains " << poly.size_of_vertices() << " vertices" << std::endl;
    } else {
         std::cout << "No The convex hull for dimension > 3 available yet." << std::endl;
    }
}
