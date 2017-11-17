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
    for (int i = 0; i < m_group->m_edgeEquivClasses.size(); i++) {
        m_mult.push_back(m_group->m_edgeEquivClasses[i]->multiplicity);
    }
}

void GiMatrix::init()
{
    m_matrix.resize(m_dim, m_dim);
    m_eecIndexMatrix.resize(m_dim, m_dim);


    // diagonal is always zero
    for (int i = 0; i < m_dim; i++) {
        m_matrix(i, i) = 0;
        m_eecIndexMatrix(i, i) = 0;
    }

    setVars(std::vector<double>());
    calculateEecIndexMatrix();
    calculateMatrix();
}

bool GiMatrix::setVars(const std::vector<double> set)
{
    std::vector<double> varsTmp, multVarsTmp;
    // first entry is always zero for more efficient queries
    varsTmp.push_back(0.);
    multVarsTmp.push_back(0.);

    auto setSize = set.size();
    if (!setSize) {
        // set variable defaults
        for (auto eec : m_group->m_edgeEquivClasses) {
            double var = 1. / m_hypers->degree();
            varsTmp.push_back(var);
            multVarsTmp.push_back(var / (double)eec->multiplicity);
        }
        m_vars.clear();
        m_multVars.clear();
        m_vars = varsTmp;
        m_multVars = multVarsTmp;
        return true;
    }

    if (setSize != m_group->m_edgeEquivClasses.size()) {
        return false;
    }

    double sum = 0;
    for (int i = 0; i < setSize; i++) {
        auto s = set[i];
        if (s < 0 || 1 < s) {
            return false;
        }
        varsTmp.push_back(s);

        double multVarTmp = m_group->m_edgeEquivClasses[i]->multiplicity * s;
        multVarsTmp.push_back(multVarTmp);
        sum += multVarTmp;
    }

    if (!qFuzzyCompare(sum,1.)) {
        qDebug() << "Error: Multiplied variable sum not one. It is:" << sum;
        return false;
    }
    m_vars = varsTmp;
    m_multVars = multVarsTmp;

    return true;
}

bool GiMatrix::setMultVars(std::vector<double> set)
{
    std::vector<double> multVarsTmp;
    multVarsTmp.push_back(0.);

    if (set.size() != m_group->m_edgeEquivClasses.size() - 1) {
        return false;
    }

    double sum = 0;
    for (auto s : set) {
        if (s < 0 || 1 < s) {
            return false;
        }
        multVarsTmp.push_back(s);
        sum += s;
    }

    if (sum != 1.) {
        qDebug() << "Error: Multiplied variable sum not one. It is:" << sum;
        return false;
    }
    m_multVars = multVarsTmp;
    return true;
}

void GiMatrix::calculateEecIndexMatrix()
{
    for (int row = 0; row < m_dim; row++) {
        for (int col = 0; col < row; col++) {
            const auto edge = Edge(col, row);

            int foundIndex = 0;
            int eecIndex = 1;
            for (auto eec : m_group->m_edgeEquivClasses) {
                if (eec->has(edge)) {
                    foundIndex = eecIndex;
                    break;
                }
                eecIndex++;
            }
            m_eecIndexMatrix(row, col) = foundIndex;
            m_eecIndexMatrix(col, row) = foundIndex;
        }
    }
}

void GiMatrix::calculateMatrix()
{
    for (int row = 0; row < m_dim; row++) {
        for (int col = 0; col < row; col++) {
            auto eecIndex = m_eecIndexMatrix(row, col);
            auto val = m_multVars[eecIndex];
            m_matrix(row, col) = val;
            m_matrix(col, row) = val;
        }
    }
}

void GiMatrix::calcNullspaceRepr()
{
    qDebug() << "----------------";
    qDebug() << "----------------";

    qDebug() << "EECIndexMatrix for" << m_group->m_gapName.c_str() << ":";
    std::cout << m_eecIndexMatrix  << std::endl;
    qDebug() << "GiMatrix:";
    std::cout << m_matrix  << std::endl;

    m_nullSpRepr.clear();

    SelfAdjointEigenSolver<MatrixXd> eigensolver(m_matrix);
    if (eigensolver.info() != Success) {
        return;
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

    std::vector<VectorXd> nullSpRepr;
    for (int i = 0; i < nullspReprs.cols(); i++) {
        nullSpRepr.push_back(nullspReprs.col(i));
    }

    m_nullSpRepr = nullSpRepr;
}
