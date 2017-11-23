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
#include <eigen3/Eigen/Dense>

class Hypersimplex;
class VtxTrnsSubgroup;

using namespace Eigen;

// Group invariant matrix with vanishing diagonal
class GiMatrix {
public:
    GiMatrix(Hypersimplex *hypers, VtxTrnsSubgroup *group);
    void init();

    // TODOX:
    // getPossibleVariableCombinations()

    std::vector<double> getVars() const {
        return std::vector<double>(m_vars.begin() + 1, m_vars.end());
    }
    std::vector<int> getMultiplicities() const {
        return m_mult;
    }

    bool setVars(const std::vector<double> set);

    void calcNullspaceRepr();
    std::vector<VectorXd> getNullspaceRepr() const {
        return m_nullSpReprList;
    }

    std::vector<VectorXd> getSchlegelDiagram(int projFacet, bool projToLargerFacet, int &error);

    MatrixXd getMatrix() const {
        return m_matrix;
    }

    void setSelEigenvectByMult(bool set) {
        m_selEigenvectByMult = set;
    }

private:
    void calculateEecIndexMatrix();
    void calculateMatrix();
    MatrixXd getMaxDimensionalNullspBasis(const VectorXd &eVals, const MatrixXd &eVcts);

    Hypersimplex *m_hypers;
    VtxTrnsSubgroup *m_group;

    MatrixXd m_matrix;          // Matrix<double, Dynamic, Dynamic>
    MatrixXd m_multMatrix;          // Matrix<double, Dynamic, Dynamic>
    MatrixXi m_eecIndexMatrix;  // Matrix<double, Dynamic, Dynamic>

    std::vector<int> m_mult;
    std::vector<double> m_vars;
    std::vector<double> m_multVars;

    MatrixXd m_nullSpRepr;
    std::vector<VectorXd> m_nullSpReprList;

    int m_dim;

    bool m_selEigenvectByMult = true;
};
