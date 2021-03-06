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

#include "hypersimplex.h"

#include "combinadic.h"
#include "autgroup.h"
#include "gimatrix.h"

#include <algorithm>

#include <QDebug>

static void genAsymSd_1d(int dim, int *comp)
{
    int dComp = comp[dim - 1];

    for (int i = dim - 1; i > 0; i--) {
        comp[i] = comp[i-1];
    }
    comp[0] = dComp;
}
static void genAsymSd_1d_inv(int dim, int *comp)
{
    int dComp = comp[0];

    for (int i = 0; i < dim - 1; i++) {
        comp[i] = comp[i+1];
    }
    comp[dim - 1] = dComp;
}
static void genAsymSd_12(int dim, int *comp)
{
    int tmp = comp[0];
    comp[0] = comp[1];
    comp[1] = tmp;
}

static void genSymS2_12(int dim, int *comp)
{
    for (int i = 0; i < dim; i++) {
        comp[i] = 1 - comp[i];
    }
}
static void genSymSd_1d(int dim, int *comp)
{
    int dComp = comp[dim - 1];

    for (int i = dim - 1; i > 0; i--) {
        comp[i] = 1 - comp[i-1];
    }
    comp[0] = 1 - dComp;
}
static void genSymSd_1d_inv(int dim, int *comp)
{
    int dComp = comp[0];

    for (int i = 0; i < dim - 1; i++) {
        comp[i] = 1 - comp[i+1];
    }
    comp[dim - 1] = 1 - dComp;
}
static void genSymSd_12(int dim, int *comp)
{
    int tmp = comp[0];
    comp[0] = comp[1];
    comp[1] = tmp;
    genSymS2_12(dim, comp);
}

VtxTrnsSubgroup::VtxTrnsSubgroup(std::string sub, int index, AutGroup *parent)
    : m_gapName(sub),
      m_index(index),
      m_parent(parent)
{}

Edge::Edge(int _v, int _w)
{
    if (_v <= _w) {
        v = _v;
        w = _w;
    } else {
        v = _w;
        w = _v;
    }
}

EdgeEquivClass::EdgeEquivClass(std::vector<Edge> edges)
    : m_edges(edges)
{}

int EdgeEquivClass::calcMultiplicity()
{
    int mult = 0;
    auto v = m_edges[0].v;

    for (auto e : m_edges) {
        if (e.has(v)) {
            mult++;
        }
    }
    multiplicity = mult;

    return mult;
}

bool EdgeEquivClass::has(Edge edge)
{
    return std::any_of(m_edges.begin(), m_edges.end(), [&edge](Edge comp){return comp == edge;});
}

Hypersimplex::Hypersimplex(int d, int k)
    : m_d(d),
      m_k(k),
      m_vertexCount(binomCoeff(d, k))
{
    qDebug() << "Create H:" << d << k;

    initVertices();
    initEdges();
    initGroup();
}

Hypersimplex::~Hypersimplex()
{
    delete m_group;
}

std::vector<std::string> Hypersimplex::getVtxTrSubgroupNames()
{
    std::vector<std::string> ret;

    for (auto s : m_vtxTrnsSubgroups) {
        ret.push_back(s->m_gapName);
    }
    return ret;
}

bool Hypersimplex::haveEdge(int v, int w)
{
    int vComb[m_d] = {0};
    int wComb[m_d] = {0};

    combinadicComb(m_k, v, vComb);
    combinadicComb(m_k, w, wComb);

    int sum = 0;
    for (int i = 0; i < m_d; i++) {
        sum += vComb[i] && wComb[i];
    }

    return sum == m_k - 1;
}

void Hypersimplex::initVertices()
{
    for (int i = 0; i < m_vertexCount; i++) {
        m_vertices.push_back(Vertex(i, m_d, m_k));
    }
}

facet_pair Hypersimplex::getFacetPair(int index) const
{
    std::vector<Vertex> facet0, facet1;
    for (auto v : m_vertices) {
        if (v[index] == 0) {
            facet0.push_back(v);
        } else {
            facet1.push_back(v);
        }
    }
    return facet_pair(facet0, facet1);
}

void Hypersimplex::initEdges()
{
    for (int i = 0; i < m_vertexCount; i++) {
        for (int j = i + 1; j < m_vertexCount; j++) {
            if (haveEdge(i, j)) {
                m_edges.push_back(Edge(i, j));
            }
        }
    }

    qDebug() << "Edges:";
    for (auto e: m_edges) {
        qDebug() << "[" << e.v << "--" << e.w << "]";
    }
}

void Hypersimplex::initGroup()
{
    m_group = new AutGroup(m_d, m_k);
}

void Hypersimplex::initCalculations()
{
    m_degree = getEdgesToVertex(0).size();

    qDebug() << "---------------";
    calcVtxTrnsSubgroups();
    qDebug() << "---------------";
    qDebug() << "---------------";
    qDebug() << "vtxtrns subs:" << m_vtxTrnsSubgroups.size();
    for (auto s : m_vtxTrnsSubgroups)
        qDebug() << QString(s->m_gapName.c_str());

    qDebug() << "---------------";
    qDebug() << "---------------";
    calcEdgeEquivClasses();

    qDebug() << "---------------";
    qDebug() << "---------------";
    qDebug() << "EECs of subgroups:";
    for (auto s : m_vtxTrnsSubgroups) {
        qDebug() << "---------------";
        qDebug() << QString(s->m_gapName.c_str());

        qDebug() << "EEC count:" << s->m_edgeEquivClasses.size();
        int index = 1;
        for (auto eec : s->m_edgeEquivClasses) {
            std::string debug = "class " + std::to_string(index) + " (mult " + std::to_string(eec->multiplicity) + ")" + ": ";
            std::vector<Edge>::const_iterator it = eec->m_edges.cbegin();
            while (it != eec->m_edges.cend()) {
                if (it != eec->m_edges.cbegin()) {
                    debug +=  + " | ";
                }
                debug += std::to_string(it->v) + "--" + std::to_string(it->w);
                it++;
            }
            qDebug() << debug.c_str();
            index++;
        }
    }

    qDebug() << "initCalculations DONE";
}

GiMatrix Hypersimplex::getGiMatrix(int subgroup)
{
    return GiMatrix(this, m_vtxTrnsSubgroups[subgroup]);
}

bool Hypersimplex::isEdge(int vertex1, int vertex2)
{
    Edge edge(vertex1, vertex2);

    auto compOnFirstVertex = [](const Edge &e1, const Edge &e2) -> bool {
        return e1.v <= e2.v;
    };
    auto range = std::equal_range(m_edges.begin(), m_edges.end(), edge, compOnFirstVertex);
    auto ptr = std::lower_bound(range.first, range.second, edge);

    return ptr != range.second && *ptr == edge;
}

void Hypersimplex::calcVtxTrnsSubgroups()
{
    int index = 0;
    for (auto sub : m_group->getSubgroups()) {
        if (isVtxTrnsSubgroup(index)) {
            m_vtxTrnsSubgroups.push_back(new VtxTrnsSubgroup(sub, index, m_group));
        }
        index++;
    }
}

bool Hypersimplex::isVtxTrnsSubgroup(int sub)
{
    bool vertexHits[m_vertexCount] = {false};

    for (auto factored : m_group->getFactorizations(sub)) {
        int res[m_vertexCount];
        startPermutate(factored, res);

        vertexHits[res[0]] = true;
    }

    for (auto hit : vertexHits) {
        if (!hit) {
            return false;
        }
    }
    return true;
}

std::vector<Edge> Hypersimplex::getEdgesToVertex(int vertex)
{
    std::vector<Edge> ret;
    for (auto e : m_edges) {
        if (e.has(vertex)) {
            ret.push_back(e);
        }
    }
    return ret;
}

void Hypersimplex::calcEdgeEquivClasses()
{

    auto hasEdge = [](std::vector<Edge> &edges, Edge &e) {
        return std::any_of(edges.begin(), edges.end(), [&e](Edge comp){return comp == e;});
    };
    for (auto sub : m_vtxTrnsSubgroups) {
        qDebug() << QString(sub->m_gapName.c_str());

        std::vector<EdgeEquivClass *> eecs;

        std::vector<int*> imgList;

        for (auto fac : m_group->getFactorizations(sub->m_index)) {
            int *vertices = new int[m_vertexCount];
            startPermutate(fac, vertices);

            imgList.push_back(vertices);
        }

        std::vector<int> vertexHits;

        for (int vertex = 0; vertex < m_vertexCount; vertex++) {
            auto testOnHit = [this, &vertexHits](int v) {
                for (auto w : vertexHits) {
                    Edge e(v, w);
                    if (std::any_of(m_edges.begin(), m_edges.end(), [&e](Edge comp){return comp == e;})) {
                        return true;
                    }
                }
                return false;
            };

            /* we don't need to test vertices which have an edge
             * to a vertex whose edges already have been tested
             */
            if (testOnHit(vertex)) {
                continue;
            }
            vertexHits.push_back(vertex);
            std::vector<Edge> vertexEdges = getEdgesToVertex(vertex);

            for (auto vEdge : vertexEdges) {
                std::vector<Edge> edgeImgs;

                for (auto el :  m_group->getFactorizations(sub->m_index)) {
                    auto edgeImg = [this](Edge e, std::string factoredPerm) {
                        auto imgV = permutateVertex(factoredPerm, e.v);
                        auto imgW = permutateVertex(factoredPerm, e.w);
                        return Edge(imgV, imgW);
                    };
                    auto eI = edgeImg(vEdge, el);
                    if (!hasEdge(edgeImgs, eI)) {
                        edgeImgs.push_back(eI);
                    }
                }

                // add edges to existing class or create new one
                bool addToClass = false;
                for (auto c : eecs) {
                    // test first if one of the mapped edges is in this class already
                    for (auto e : edgeImgs) {
                        if (hasEdge(c->m_edges, e)) {
                            addToClass = true;
                            break;
                        }
                    }

                    // test also if one of the edges already in class 'c' is in the image
                    if (!addToClass) {
                        for (auto e : c->m_edges) {
                            if (hasEdge(edgeImgs, e)) {
                                addToClass = true;
                                break;
                            }
                        }
                    }

                    if (addToClass) {
                        // add new edges to class 'c'

                        std::string debug;
                        for (auto e : c->m_edges)
                            debug += std::to_string(e.v) + std::to_string(e.w) + "|";

                        for (auto eI : edgeImgs) {
                            if (!hasEdge(c->m_edges, eI)) {
                                c->m_edges.push_back(eI);
                            }
                        }
                        break;
                    }
                }

                if (!addToClass) {
                    // class is not yet listed in class_list
                    auto eec = new EdgeEquivClass(edgeImgs);
                    eecs.push_back(eec);
                }
            }
        }

        for (auto c : eecs) {
            std::sort (c->m_edges.begin(), c->m_edges.end(), [](Edge e, Edge l) { return e.v < l.v || e.v == l.v && e.w <= l.w; });
            c->calcMultiplicity();
        }

        for_each(sub->m_edgeEquivClasses.begin(), sub->m_edgeEquivClasses.end(), [](EdgeEquivClass *ptr){delete ptr;});
        sub->m_edgeEquivClasses = eecs;

        for_each(imgList.begin(), imgList.end(), [](int *ptr){delete ptr;});
    }
}

void Hypersimplex::startPermutate(std::string factoredPerm, int *result)
{
    for (int i = 0; i < m_vertexCount; i++) {
        result[i] = i;
    }
    permutateVertices(factoredPerm, result);
}

void Hypersimplex::permutateVertices(std::string factoredPerm, int *vertices)
{
    if (factoredPerm[0] == '<') {
        return;
    }

    int input[m_vertexCount];
    std::copy(vertices, vertices + m_vertexCount, input);

    std::vector<int *>parsedFactoredPerm = parsePermutation(factoredPerm);

    for (auto factor : parsedFactoredPerm) {
        int input[m_vertexCount];
        std::copy(vertices, vertices + m_vertexCount, input);
        for (int i=0; i < m_vertexCount; i++) {
            vertices[factor[i]] = input[i];
        }
    }
}

int Hypersimplex::permutateVertex(std::string factoredPerm, int vertex)
{
    int vertices[m_vertexCount];
    startPermutate(factoredPerm, vertices);
    return vertices[vertex];
}

AsymHypers::AsymHypers(int d, int k)
    : Hypersimplex(d, k)
{
    m_genAsymSd_1d = new int[m_vertexCount];
    m_genAsymSd_1d_inv = new int[m_vertexCount];
    m_genAsymSd_12 = new int[m_vertexCount];

    int genAsymSd_1d_result[m_vertexCount][m_d];
    int genAsymSd_1d_inv_result[m_vertexCount][m_d];
    int genAsymSd_12_result[m_vertexCount][m_d];

    for (int i = 0; i < m_vertexCount; i++) {
        int comps[m_d] = {0};
        combinadicComb(m_k, i, comps);

        std::copy(comps, comps + m_d, genAsymSd_1d_result[i]);
        std::copy(comps, comps + m_d, genAsymSd_1d_inv_result[i]);
        std::copy(comps, comps + m_d, genAsymSd_12_result[i]);

        genAsymSd_1d(m_d, genAsymSd_1d_result[i]);
        genAsymSd_1d_inv(m_d, genAsymSd_1d_inv_result[i]);
        genAsymSd_12(m_d, genAsymSd_12_result[i]);
    }

    for (int i = 0; i < m_vertexCount; i++) {
        m_genAsymSd_1d[i] = combinadicN(m_d, genAsymSd_1d_result[i]);
        m_genAsymSd_1d_inv[i] = combinadicN(m_d, genAsymSd_1d_inv_result[i]);
        m_genAsymSd_12[i] = combinadicN(m_d, genAsymSd_12_result[i]);
    }

    initCalculations();
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

    int genSymS2_12_result[m_vertexCount][m_d];
    int genSymSd_1d_result[m_vertexCount][m_d];
    int genSymSd_1d_inv_result[m_vertexCount][m_d];
    int genSymSd_12_result[m_vertexCount][m_d];

    for (int i = 0; i < m_vertexCount; i++) {
        int comps[m_d] = {0};
        combinadicComb(m_k, i, comps);

        std::copy(comps, comps + m_d, genSymS2_12_result[i]);
        std::copy(comps, comps + m_d, genSymSd_1d_result[i]);
        std::copy(comps, comps + m_d, genSymSd_1d_inv_result[i]);
        std::copy(comps, comps + m_d, genSymSd_12_result[i]);

        genSymS2_12(m_d, genSymS2_12_result[i]);
        genSymSd_1d(m_d, genSymSd_1d_result[i]);
        genSymSd_1d_inv(m_d, genSymSd_1d_inv_result[i]);
        genSymSd_12(m_d, genSymSd_12_result[i]);
    }

    for (int i = 0; i < m_vertexCount; i++) {
        m_genSymS2_12[i] = combinadicN(m_d, genSymS2_12_result[i]);
        m_genSymSd_1d[i] = combinadicN(m_d, genSymSd_1d_result[i]);
        m_genSymSd_1d_inv[i] = combinadicN(m_d, genSymSd_1d_inv_result[i]);
        m_genSymSd_12[i] = combinadicN(m_d, genSymSd_12_result[i]);
    }

    initCalculations();
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

        if (perm[pos] == '(') {
            int brackets = 1;
            std::size_t subpos = pos;
            while (brackets > 0) {
                partEnd = perm.find_first_of("()", subpos + 1);
                if (perm[partEnd] == ')') {
                    brackets--;
                } else {
                    brackets++;
                }
                subpos = partEnd + 1;
            }
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
            part = perm.substr(pos, partEnd - pos);
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

    while (pos != std::string::npos) {
        char symbol = perm[pos + 1];
        int times = 1;
        bool inverse = false;

        if (perm[pos + 2] == '^') {
            if (perm[pos + 3] == '-') {
                inverse = true;
                times = perm[pos + 4] - '0';
            } else {
                times = perm[pos + 3] - '0';
            }
        }

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

        pos = perm.find('x', pos + 1);
    }

    return ret;
}

std::vector<int *> SymHypers::parsePermutation(std::string perm)
{
    std::vector<int *> ret;
    std::size_t pos = 0;

    perm = prepareForParsing(perm);

    while (pos != std::string::npos) {
        char symbol = perm[pos + 1];
        int times = 1;
        bool inverse = false;

        if (perm[pos + 2] == '^') {
            if (perm[pos + 3] == '-') {
                inverse = true;
                times = perm[pos + 4] - '0';
            } else {
                times = perm[pos + 3] - '0';
            }
        }

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

        pos = perm.find('x', pos + 1);
    }

    return ret;
}

