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

#include "nllsprepr.h"
#include "gimatrix.h"

#include <algorithm>

NllSpRepr::NllSpRepr(GiMatrix *matrix)
{
    m_reprMatrix = new GiMatrix(*matrix);
    calculate();
}

NllSpRepr::~NllSpRepr()
{
    delete m_reprMatrix;
}

std::vector<Eigen::VectorXd> NllSpRepr::calculate()
{
    m_points = m_reprMatrix->calcNullspaceRepr();
    return m_points;
}
