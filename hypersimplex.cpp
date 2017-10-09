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

#include "graph.h"
#include "autgroup.h"

#include "hypersimplex.h"

#include <algorithm>

#include <QByteArray>
#include <QDebug>

static void genAsymSd_1d(int dim, bool *comp)
{
    bool dComp = comp[dim - 1];

    for (int i = dim - 1; i > 0; i--) {
        comp[i] = comp[i-1];
    }
    comp[0] = dComp;
}
static void genAsymSd_12(int dim, bool *comp)
{
    bool tmp = comp[0];
    comp[0] = comp[1];
    comp[1] = tmp;
}

static void genSymS2_12(int dim, bool *comp)
{
    for (int i = 0; i < dim; i++) {
        comp[i] = !comp[i];
    }
}
static void genSymSd_1d(int dim, bool *comp)
{
    bool dComp = comp[dim - 1];

    for (int i = dim - 1; i > 0; i--) {
        comp[i] = !comp[i-1];
    }
    comp[0] = !dComp;
}
static void genSymSd_12(int dim, bool *comp)
{
    bool tmp = comp[0];
    comp[0] = comp[1];
    comp[1] = tmp;
    genSymS2_12(dim, comp);
}

static int binomCoeff(int d, int k)
{
    if (d < k || k <= 0) {
        return 0;
    }
    double ret = 1.;
    for (int i = 1; i <= k; i++) {
        ret *= (d + 1 - i) / (double) i;
    }
    return (int)(ret + 0.5);
}

/*
 * Greedy algorithm for determining position
 * of k-combination.
 */
static int combinadicCombMax(int k, int n, int &max)
{
    int i = k - 1;
    max = 0;

    while (true) {
        int nextTry = binomCoeff(i + 1, k);
        if (nextTry > n) {
            break;
        }
        max = nextTry;
        i++;
    }
    return i;
}

/*
 * Calculates the ninSet's k-combination 'comb' in the
 * lexicographic combinadic ordering.
 *
 * nInSet starts counting at 0.
 */
static void combinadicComb(int k, int nInSet, bool *comb)
{
    int n1 = nInSet;

    for (int i = k; i > 0; i--) {
        int diff;
        int combIndex = combinadicCombMax(i, n1, diff);
        comb[combIndex] = true;
        n1 = n1 - diff;
    }
}

static int combinadicN(int d, const bool *comb)
{
    int n = 0;
    int j = 1;

    for (int i = 0; i < d; i++) {
        if (comb[i]) {
            n += binomCoeff(i, j);
            j++;
        }
    }
    return n;
}

Hypersimplex::Hypersimplex(int d, int k)
    : m_d(d),
      m_k(k)
{
    int vcnt = binomCoeff(d, k);

    qDebug() << "Create H:" << d << k;

    if (d == 2 * k) {
        initSym();
    } else {
        initAsym();
    }

    initGraph();
    initGroup();
}

Hypersimplex::~Hypersimplex()
{
    delete[] m_genAsymSd_1d;
    delete[] m_genAsymSd_12;

    delete[] m_genSymS2_12;
    delete[] m_genSymSd_1d;
    delete[] m_genSymSd_12;

    delete m_graph;
    delete m_group;
}

void Hypersimplex::initSym()
{
    int vcnt = binomCoeff(m_d, m_k);

    m_genSymS2_12 = new int[vcnt];
    m_genSymSd_1d = new int[vcnt];
    m_genSymSd_12 = new int[vcnt];

    bool genSymS2_12_result[vcnt][m_d];
    bool genSymSd_1d_result[vcnt][m_d];
    bool genSymSd_12_result[vcnt][m_d];

//    qDebug() << "initSym:" << vcnt;

    for (int i = 0; i < vcnt; i++) {
        bool comps[m_d] = {false};
        combinadicComb(m_k, i, comps);

        std::copy(comps, comps + m_d, genSymS2_12_result[i]);
        std::copy(comps, comps + m_d, genSymSd_1d_result[i]);
        std::copy(comps, comps + m_d, genSymSd_12_result[i]);

        genSymS2_12(m_d, genSymS2_12_result[i]);
        genSymSd_1d(m_d, genSymSd_1d_result[i]);
        genSymSd_12(m_d, genSymSd_12_result[i]);

//        qDebug() << "---------------";
//        qDebug() << "------" << i << "------";
//        for (int j = 0; j < m_d; j++) {
//            qDebug() << j << ":" << comps[j] << genSymS2_12_result[i][j]
//                        << genSymSd_1d_result[i][j]
//                           << genSymSd_12_result[i][j];
//        }
//        qDebug() << "---------------";
//        qDebug() << "TEST:" << combinadicN(m_d, comps);
//        qDebug() << "---------------";
    }

    for (int i = 0; i < vcnt; i++) {
        m_genSymS2_12[i] = combinadicN(m_d, genSymS2_12_result[i]);
        m_genSymSd_1d[i] = combinadicN(m_d, genSymSd_1d_result[i]);
        m_genSymSd_12[i] = combinadicN(m_d, genSymSd_12_result[i]);
    }
}

void Hypersimplex::initAsym()
{
    int vcnt = binomCoeff(m_d, m_k);

    m_genAsymSd_1d = new int[vcnt];
    m_genAsymSd_12 = new int[vcnt];

    bool genAsymSd_1d_result[vcnt][m_d];
    bool genAsymSd_12_result[vcnt][m_d];

//    qDebug() << "initAsym:" << vcnt;

    for (int i = 0; i < vcnt; i++) {
        bool comps[m_d] = {false};
        combinadicComb(m_k, i, comps);

        std::copy(comps, comps + m_d, genAsymSd_1d_result[i]);
        std::copy(comps, comps + m_d, genAsymSd_12_result[i]);

        genAsymSd_1d(m_d, genAsymSd_1d_result[i]);
        genAsymSd_12(m_d, genAsymSd_12_result[i]);

//        qDebug() << "---------------";
//        qDebug() << "------" << i << "------";
//        for (int j = 0; j < m_d; j++) {
//            qDebug() << j << ":" << comps[j]
//                        << genAsymSd_1d_result[i][j]
//                           << genAsymSd_12_result[i][j];
//        }
//        qDebug() << "---------------";
//        qDebug() << "TEST:" << combinadicN(m_d, comps);
//        qDebug() << "---------------";
    }

    for (int i = 0; i < vcnt; i++) {
        m_genAsymSd_1d[i] = combinadicN(m_d, genAsymSd_1d_result[i]);
        m_genAsymSd_12[i] = combinadicN(m_d, genAsymSd_12_result[i]);
    }
}

void Hypersimplex::initGraph()
{
    m_graph = new Graph(m_d);
}

void Hypersimplex::initGroup()
{
    m_group = new AutGroup(m_d, m_k);
}

void Hypersimplex::calcVtxTrnsSubgroups()
{
    m_vtxTrnsSubgroups.clear();

    for (auto sub : m_group->getSubgroups()) {
        if (isVtxTrnsSubgroup(sub)) {
            m_vtxTrnsSubgroups.push_back(sub);
        }
    }
}

bool Hypersimplex::isVtxTrnsSubgroup(std::string sub)
{

}

//Vertex::Vertex(int dim, bool *components)
//    : m_dim(dim)
//{
//    m_comp = new bool[dim];
//    for (int i = 0; i < dim; i++) {
//        m_comp[i] = components[i];
//    }
//}

//Vertex::~Vertex()
//{
//    delete[] m_comp;
//}
