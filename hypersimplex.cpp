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

#include <QDebug>

static void genAsymSd_1d(int dim, bool *comp)
{
    bool dComp = comp[dim - 1];

    for (int i = dim - 1; i > 0; i--) {
        comp[i] = comp[i-1];
    }
    comp[0] = dComp;
}
static void genAsymSd_1d_inv(int dim, bool *comp)
{
    bool dComp = comp[0];

    for (int i = 0; i < dim - 1; i++) {
        comp[i] = comp[i+1];
    }
    comp[dim - 1] = dComp;
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
static void genSymSd_1d_inv(int dim, bool *comp)
{
    bool dComp = comp[0];

    for (int i = 0; i < dim - 1; i++) {
        comp[i] = !comp[i+1];
    }
    comp[dim - 1] = !dComp;
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
      m_k(k),
      m_vertexCount(binomCoeff(d, k))
{
    qDebug() << "Create H:" << d << k;

    initGraph();
    initGroup();
}

Hypersimplex::~Hypersimplex()
{
    delete m_graph;
    delete m_group;
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
    qDebug() << "calcVtxTrnsSubgroups";

//    auto sub = m_group->getSubgroups()[97];   //TODOX
    for (auto sub : m_group->getSubgroups()) {
        if (isVtxTrnsSubgroup(sub)) {
            m_vtxTrnsSubgroups.push_back(sub);
        }
    }
}

bool Hypersimplex::isVtxTrnsSubgroup(std::string sub)
{
    bool vertexHits[m_vertexCount] = {false};

    qDebug() << "isVtxTrnsSubgroup1:" << QString(sub.c_str());

    for (auto factored : m_group->getSubgroupFactorizations(sub)) {
        qDebug() << "-----------------";
        qDebug() << "FACTORED:" << QString(factored.c_str());

        int res[m_vertexCount];
        startPermutate(factored, res);

        qDebug() << "permutated:" << res[0];

        vertexHits[res[0] - 1] = true;

    }

    for (auto hit : vertexHits) {
        if (!hit) {
            return false;
        }
    }
    return true;
}

void Hypersimplex::startPermutate(std::string factoredPerm, int *result)
{
    for (int i = 0; i < m_vertexCount; i++) {
        result[i] = i + 1;
    }
//    qDebug() << "startPermutate!";
    permutateVertices(factoredPerm, result);
}

void Hypersimplex::permutateVertices(std::string factoredPerm, int *vertices)
{
    if (factoredPerm[0] == '<') {
        return;
    }

    int input[m_vertexCount];
    std::copy(vertices, vertices + m_vertexCount, input);

//    qDebug() << "permutateVertices!" << QString(factoredPerm.c_str());

    std::vector<int *>parsedFactoredPerm = parsePermutation(factoredPerm);

//    for (int i = 0; i < m_vertexCount; i++) {
//        qDebug() << "Perm:" << parsedFactoredPerm[0][i];
//    }

//    for (int i = 0; i < m_vertexCount; i++) {
//        qDebug() << "Perm:" << parsedFactoredPerm[0][i];
//    }

    for (auto factor : parsedFactoredPerm) {
        int input[m_vertexCount];
        std::copy(vertices, vertices + m_vertexCount, input);
        for (int i=0; i < m_vertexCount; i++) {
            vertices[factor[i]] = input[i];
        }
    }

//    for (int i = 0; i < m_vertexCount; i++) {
//        qDebug() << "Perm Vert:" << input[i] << vertices[i];
//    }
}

AsymHypers::AsymHypers(int d, int k)
    : Hypersimplex(d, k)
{
    m_genAsymSd_1d = new int[m_vertexCount];
    m_genAsymSd_1d_inv = new int[m_vertexCount];
    m_genAsymSd_12 = new int[m_vertexCount];

    bool genAsymSd_1d_result[m_vertexCount][m_d];
    bool genAsymSd_1d_inv_result[m_vertexCount][m_d];
    bool genAsymSd_12_result[m_vertexCount][m_d];

    for (int i = 0; i < m_vertexCount; i++) {
        bool comps[m_d] = {false};
        combinadicComb(m_k, i, comps);

        std::copy(comps, comps + m_d, genAsymSd_1d_result[i]);
        std::copy(comps, comps + m_d, genAsymSd_1d_inv_result[i]);
        std::copy(comps, comps + m_d, genAsymSd_12_result[i]);

        genAsymSd_1d(m_d, genAsymSd_1d_result[i]);
        genAsymSd_1d_inv(m_d, genAsymSd_1d_inv_result[i]);
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

    for (int i = 0; i < m_vertexCount; i++) {
        m_genAsymSd_1d[i] = combinadicN(m_d, genAsymSd_1d_result[i]);
        m_genAsymSd_1d_inv[i] = combinadicN(m_d, genAsymSd_1d_inv_result[i]);
        m_genAsymSd_12[i] = combinadicN(m_d, genAsymSd_12_result[i]);
    }

//    int test[m_vertexCount];
//    qDebug() << "---------";
//    qDebug() << "---------";
//    startPermutate(m_group->getFactorizations()[4], test);

    calcVtxTrnsSubgroups();
    qDebug() << "vtxtrns subs:" << m_vtxTrnsSubgroups.size();
    for (auto s : m_vtxTrnsSubgroups)
        qDebug() << QString(s.c_str());
}

AsymHypers::~AsymHypers()
{
    delete[] m_genAsymSd_1d;
    delete[] m_genAsymSd_1d_inv;
    delete[] m_genAsymSd_12;
}

SymHypers::SymHypers(int d, int k)
    : Hypersimplex(d, k)
{
    m_genSymS2_12 = new int[m_vertexCount];
    m_genSymSd_1d = new int[m_vertexCount];
    m_genSymSd_1d_inv = new int[m_vertexCount];
    m_genSymSd_12 = new int[m_vertexCount];

    bool genSymS2_12_result[m_vertexCount][m_d];
    bool genSymSd_1d_result[m_vertexCount][m_d];
    bool genSymSd_1d_inv_result[m_vertexCount][m_d];
    bool genSymSd_12_result[m_vertexCount][m_d];

    for (int i = 0; i < m_vertexCount; i++) {
        bool comps[m_d] = {false};
        combinadicComb(m_k, i, comps);

        std::copy(comps, comps + m_d, genSymS2_12_result[i]);
        std::copy(comps, comps + m_d, genSymSd_1d_result[i]);
        std::copy(comps, comps + m_d, genSymSd_1d_inv_result[i]);
        std::copy(comps, comps + m_d, genSymSd_12_result[i]);

        genSymS2_12(m_d, genSymS2_12_result[i]);
        genSymSd_1d(m_d, genSymSd_1d_result[i]);
        genSymSd_1d_inv(m_d, genSymSd_1d_inv_result[i]);
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

    for (int i = 0; i < m_vertexCount; i++) {
        m_genSymS2_12[i] = combinadicN(m_d, genSymS2_12_result[i]);
        m_genSymSd_1d[i] = combinadicN(m_d, genSymSd_1d_result[i]);
        m_genSymSd_1d_inv[i] = combinadicN(m_d, genSymSd_1d_inv_result[i]);
        m_genSymSd_12[i] = combinadicN(m_d, genSymSd_12_result[i]);
    }

//    int test[m_vertexCount];
//    qDebug() << "---------";
//    qDebug() << "---------";
//    startPermutate(m_group->getFactorizations()[38], test);

    calcVtxTrnsSubgroups();
    qDebug() << "vtxtrns subs:" << m_vtxTrnsSubgroups.size();
    for (auto s : m_vtxTrnsSubgroups)
        qDebug() << QString(s.c_str());
}

SymHypers::~SymHypers()
{
    delete[] m_genSymS2_12;
    delete[] m_genSymSd_1d;
    delete[] m_genSymSd_1d_inv;
    delete[] m_genSymSd_12;
}

std::string Hypersimplex::prepareForParsing(const std::string &perm)
{
    std::string ret;
    std::size_t pos = 0;

//    qDebug() << "prepareForParsing" << QString(perm.c_str());

    auto inverteParsedPart = [](const std::string part) {
        std::size_t pos = 0;
        std::string ret;
        while (pos != std::string::npos) {
            std::string cur = part.substr(pos, 2) + '^';
            if (part[pos + 2] == '^') {
                if (part[pos + 3] == '-') {
                    // TODO: only reads in exponents with one digit
                    cur += part[pos + 4];
                    pos += 5;
                } else {
                    cur += '-';
                    // TODO: only reads in exponents with one digit
                    cur += part[pos + 3];
                    pos += 4;
                }
            } else {
                cur += '1';
                pos += 2;
            }
            ret = cur + ret;
        }
        return ret;
    };

    while (pos != std::string::npos) {
        std::size_t partEnd;
        std::string part;

//        qDebug() << "IN" << pos << perm[pos];

        if (perm[pos] == '(') {
            partEnd = perm.rfind(')');
            part = prepareForParsing(perm.substr(pos + 1, partEnd - (pos + 1)));
            int times;
            bool inverse;

            if (perm[partEnd + 2] == '-') {
                // TODO: only reads in exponents with one digit
                times = perm[partEnd + 3] - '0';
                inverse = true;
                part = inverteParsedPart(part);
                pos = partEnd + 4;
            } else {
                // TODO: only reads in exponents with one digit
                times = perm[partEnd + 2] - '0';
                inverse = false;
                pos = partEnd + 3;
            }

            while (times > 0) {
                ret += part;
                times--;
            }
        } else {
            partEnd = perm.find('(', pos + 1);
            if (partEnd != std::string::npos) {
                partEnd--;
            }
            part = perm.substr(pos, partEnd);
            ret += part;
            pos = partEnd;
        }
    }
    return ret;
}

std::vector<int *> AsymHypers::parsePermutation(std::string perm)
{
    std::vector<int *> ret;
    std::size_t pos = 0;

    perm = prepareForParsing(perm);

//    qDebug() << "parsePermutation1" << QString(perm.c_str());

//    qDebug() << m_genAsymSd_1d << m_genAsymSd_1d_inv << m_genAsymSd_12;

    while (pos != std::string::npos) {
        char symbol = perm[pos + 1];
        int times = 1;
        bool inverse = false;

//        qDebug() << "IN1" << pos << symbol;

        if (perm[pos + 2] == '^') {
            if (perm[pos + 3] == '-') {
                inverse = true;
                times = perm[pos + 4] - '0';
            } else {
                times = perm[pos + 3] - '0';
            }
        }

//        qDebug() << "IN2" << inverse << times;

        int *ptr;
        if (symbol == '1') {
            ptr = inverse ? m_genAsymSd_1d_inv : m_genAsymSd_1d;
        } else {
            ptr = m_genAsymSd_12;
        }

        while (times > 0) {
            ret.push_back(ptr);
            times--;
        }

//        qDebug() << "IN3" << ret[0];

        pos = perm.find('x', pos + 1);
    }

//    for (auto i : ret)
//        qDebug() << "pPRet:" << i;

    return ret;
}

std::vector<int *> SymHypers::parsePermutation(std::string perm)
{
    std::vector<int *> ret;
    std::size_t pos = 0;

//    qDebug() << "parsePermutation VORHER" << QString(perm.c_str());


    perm = prepareForParsing(perm);

//    qDebug() << "parsePermutation NACHHER" << QString(perm.c_str());

    while (pos != std::string::npos) {
        char symbol = perm[pos + 1];
        int times = 1;
        bool inverse = false;

//        qDebug() << "IN1" << pos << symbol;

        if (perm[pos + 2] == '^') {
            if (perm[pos + 3] == '-') {
                inverse = true;
                times = perm[pos + 4] - '0';
            } else {
                times = perm[pos + 3] - '0';
            }
        }

//        qDebug() << "IN2" << inverse << times;

        int *ptr;
        if (symbol == '3') {
            while (times > 0) {
                ret.push_back(m_genSymS2_12);
                ret.push_back(m_genSymSd_12);
                times--;
            }
        } else {
            if (symbol == '1') {
                ptr = inverse ? m_genSymSd_1d_inv : m_genSymSd_1d;
            } else {
                ptr = m_genSymSd_12;
            }
            while (times > 0) {
                ret.push_back(ptr);
                times--;
            }
        }

//        if (symbol == '1') {
//            ptr = inverse ? m_genSymSd_1d_inv : m_genSymSd_1d;
//        } else if (symbol == '2') {
//            ptr = m_genSymSd_12;
//        } else {
//            ptr = m_genSymS2_12;
//        }
//        while (times > 0) {
//            ret.push_back(ptr);
//        }

        pos = perm.find('x', pos + 1);
    }

//    qDebug() << "parsePermutation END";
    return ret;
}

