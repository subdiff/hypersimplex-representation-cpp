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

#ifndef SCHLEGEL_H
#define SCHLEGEL_H

#include "vertex.h"

#include <vector>
#include <eigen3/Eigen/Dense>

class Hypersimplex;

using namespace Eigen;

typedef std::pair<std::vector<Vertex>, std::vector<Vertex> > facet_pair;

class Schlegel {
public:
    Schlegel(Hypersimplex *hypers, int projFacet, const MatrixXd &pts);

    std::vector<VectorXd> getDiagram(int &error) const;

    void setFacetPairIndex(int index) {
        m_facetPairIndex = index;
    }

private:
    Hypersimplex *m_hypers = nullptr;
    int m_facetPairIndex = 0;
    facet_pair m_facetPair;
    MatrixXd m_pts;
};

#endif // SCHLEGEL_H
