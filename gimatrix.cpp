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
#include "schlegel.h"

#include <algorithm>
#include <eigen3/Eigen/Eigenvalues>

#include <QDebug>

#include <cmath>
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
    m_multMatrix.resize(m_dim, m_dim);
    m_eecIndexMatrix.resize(m_dim, m_dim);


    // diagonal is always zero
    for (int i = 0; i < m_dim; i++) {
        m_matrix(i, i) = 0;
        m_eecIndexMatrix(i, i) = 0;
    }

    calculateEecIndexMatrix();
    setVars(std::vector<double>());
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

        calculateMatrix();
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

    calculateMatrix();
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
            auto val = m_vars[eecIndex];
            auto multVal = m_multVars[eecIndex];
            m_matrix(row, col) = val;
            m_matrix(col, row) = val;
            m_multMatrix(row, col) = multVal;
            m_multMatrix(col, row) = multVal;
        }
    }
    calcNullspaceRepr();
}

MatrixXd GiMatrix::getMaxDimensionalNullspBasis(const VectorXd &eVals, const MatrixXd &eVcts)
{
    int nullSpDim = m_hypers->d() - 1;

    if (m_selEigenvectMode == 0) {
        auto nearIndices = [nullSpDim, &eVals, &eVcts](double epsilon) {
            std::vector<int> hits;
            for (int i = eVals.rows() - 2; i >= 0; i--) {
                hits = std::vector<int>();
                for (int j = i; j >= 0; j--) {
                    if (std::abs(eVals(i) - eVals(j)) < epsilon) {
                        hits.push_back(j);
                    }
                }
                if (hits.size() == nullSpDim) {
                    return hits[nullSpDim - 1];
                }
            }
            return -1;
        };

        int hit = nearIndices(0.01);
        // first try with small tolerance
        if (hit != -1) {
            return eVcts.block(0, hit, m_dim, nullSpDim).transpose();
        }
        // try with larger tolerance
        hit = nearIndices(0.1);
        if (hit != -1) {
            return eVcts.block(0, hit, m_dim, nullSpDim).transpose();
        }
    }
    // highest indices after the first one
    int count = nullSpDim;
    if (m_selEigenvectMode == 2) {
        // only for same eigenvalue
        count = 1;
        double ev = eVals(eVals.rows() - 2);
        for (int i = eVals.rows() - 3; i >= 0; i--) {
            if (std::abs(eVals(i) - ev) < std::numeric_limits<double>::epsilon()) {
                count++;
            } else {
                break;
            }
        }
    }
    return eVcts.block(0, m_dim - 1 - count, m_dim, count).transpose();
}

void GiMatrix::calcNullspaceRepr()
{
    qDebug() << "----------------";
    qDebug() << "----------------";

    qDebug() << "EECIndexMatrix for" << m_group->m_gapName.c_str() << ":";
    std::cout << m_eecIndexMatrix  << std::endl;
    qDebug() << "GiMatrix (doubly stochastic matrix):";
    std::cout << m_matrix  << std::endl;
    qDebug() << "GiMatrix multiplied (this matrix stores values already multiplied by eec multiplicity -> it is not the doubly stochastic matrix!):";
    std::cout << m_multMatrix  << std::endl;

    m_nullSpReprList.clear();

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
    MatrixXd nullSpRepr = getMaxDimensionalNullspBasis(eVals, eVcts);
    std::cout << nullSpRepr  << std::endl;;
    m_nullSpRepr = nullSpRepr;

    std::vector<VectorXd> nullSpReprList;
    for (int i = 0; i < nullSpRepr.cols(); i++) {
        nullSpReprList.push_back(nullSpRepr.col(i));
    }

    m_nullSpReprList = nullSpReprList;
}

std::vector<VectorXd> GiMatrix::getSchlegelDiagram(int projFacet, bool projToLargerFacet, int &error)
{
    Schlegel s(m_hypers, projFacet, projToLargerFacet, m_nullSpRepr);
    return s.getDiagram(error);
}
