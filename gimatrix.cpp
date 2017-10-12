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

#include "gimatrix.h"
#include "hypersimplex.h"

#include <algorithm>

#include <QDebug>

GiMatrix::GiMatrix(Hypersimplex *hypers, VtxTrnsSubgroup *group)
    : m_hypers(hypers),
      m_group(group)
{
    // define matrix
    // TODOX

    for (int i = 0; i < hypers->d(); i++) {
        m_matrix[i][i] = 0;
    }

    // set variables defaults
    for (auto eec : m_group->m_edgeEquivClasses) {
        m_vars.push_back(1. / (double)eec->multiplicity / m_hypers->degree());
    }
    calculateMatrix();
}

void GiMatrix::calculateMatrix()
{
    auto getCellVal = [this](Edge e) -> double {
        int index = 0;
        for (auto eec : m_group->m_edgeEquivClasses) {
            if (eec->has(e)) {
                return eec->multiplicity * m_vars[index];
            }
            index++;
        }
        return 0.;
    };

    int dim = m_hypers->d();

    for (int row = 0; row < dim; row++) {
        for (int col = 0; col < row; col++) {
            auto val = getCellVal(Edge(col, row));
            m_matrix[row][col] = val;
            m_matrix[col][row] = val;
        }
    }
}
