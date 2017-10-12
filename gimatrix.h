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

#include <vector>

class Hypersimplex;
class VtxTrnsSubgroup;

// Group invariant matrix with vanishing diagonal
class GiMatrix {
public:
    GiMatrix(Hypersimplex *hypers, VtxTrnsSubgroup *group);

    // TODOX:
    // setVariables()
    // getPossibleVariableCombinations()
    // calcEigen()
    // calcNullspaceRepr()

private:
    void calculateMatrix();

    Hypersimplex *m_hypers;
    VtxTrnsSubgroup *m_group;

    int **m_matrix;
    std::vector<double> m_vars;
};
