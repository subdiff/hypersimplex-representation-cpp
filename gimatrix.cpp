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
#include <eigen3/Eigen/Eigenvalues>

#include <QDebug>

#include <iostream>

GiMatrix::GiMatrix(Hypersimplex *hypers, VtxTrnsSubgroup *group)
    : m_dim(hypers->vertexCount()),
      m_hypers(hypers),
      m_group(group)
{
}

void GiMatrix::init()
{
    m_matrix.resize(m_dim, m_dim);

    for (int i = 0; i < m_dim; i++) {
        m_matrix(i, i) = 0;
    }

    setVars(std::vector<double>());
    calculateMatrix();
}

bool GiMatrix::setVars(std::vector<double> set)
{
    std::vector<double> varsTmp, multVarsTmp;

    auto getCellVal = [this, &varsTmp](Edge e) -> double {
        int index = 0;
        for (auto eec : m_group->m_edgeEquivClasses) {
            if (eec->has(e)) {
                return eec->multiplicity * varsTmp[index];
            }
            index++;
        }
        return 0.;
    };

    auto setSize = set.size();
    if (!setSize) {
        // set variable defaults
        for (auto eec : m_group->m_edgeEquivClasses) {
            varsTmp.push_back(1. / (double)eec->multiplicity / m_hypers->degree());
        }
        for (int col = 0; col < m_dim; col++) {
            multVarsTmp.push_back(getCellVal(Edge(col, m_dim - 1)));
        }
        m_vars.clear();
        m_multVars.clear();
        m_vars = varsTmp;
        m_multVars = multVarsTmp;

        return true;
    }

    if (setSize != m_group->m_edgeEquivClasses.size() - 1) {
        return false;
    }

    for (auto s : set) {
        if (s < 0 || 1 < s) {
            return false;
        }
        varsTmp.push_back(s);
    }

    double sum = 0;
    for (int col = 0; col < m_dim; col++) {
        auto multVarTmp = getCellVal(Edge(col, m_dim - 1));
        multVarsTmp.push_back(multVarTmp);
        sum += multVarTmp;
    }

    if (sum != 1.) {
        qDebug() << "Error: Multiplied variable sum not one. It is:" << sum;
        return false;
    }
    m_vars = varsTmp;
    m_multVars = multVarsTmp;
    return true;
}

void GiMatrix::calculateMatrix()
{
    auto getCellVal = [this](Edge e) -> double {
        int index = 0;
        for (auto eec : m_group->m_edgeEquivClasses) {
            if (eec->has(e)) {
                return m_multVars[index];
            }
            index++;
        }
        return 0.;
    };

    for (int row = 0; row < m_dim; row++) {
        for (int col = 0; col < row; col++) {
            auto val = getCellVal(Edge(col, row));
            m_matrix(row, col) = val;
            m_matrix(col, row) = val;
        }
    }
}

std::vector<VectorXd> GiMatrix::calcNullspaceRepr()
{
    qDebug() << "----------------";
    qDebug() << "----------------";

    qDebug() << "GiMatrix for" << m_group->m_gapName.c_str() << ":";
    std::cout << m_matrix  << std::endl;

    SelfAdjointEigenSolver<MatrixXd> eigensolver(m_matrix);
    if (eigensolver.info() != Success) {
        return std::vector<VectorXd>();
    }

    VectorXd eVals = eigensolver.eigenvalues();
    MatrixXd eVcts = eigensolver.eigenvectors();

    qDebug() << "Eigenvalues:";
    std::cout << eVals.transpose() << std::endl;

    qDebug() << "Eigenvectors:";
    std::cout << eVcts  << std::endl;

    qDebug() << "----------------";
    qDebug() << "Null Space Representation:";
    MatrixXd nullspReprs = eVcts.block(0, m_dim - m_hypers->d(), m_dim, m_hypers->d() - 1).transpose();
    std::cout << nullspReprs  << std::endl;;

    std::vector<VectorXd> ret;
    for (int i = 0; i < nullspReprs.cols(); i++) {
        ret.push_back(nullspReprs.col(i));
    }
    return ret;
}
