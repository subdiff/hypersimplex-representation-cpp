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

#include "schlegel.h"

#include "hypersimplex.h"

#include <algorithm>

#include <QDebug>

#include <iostream>

Schlegel::Schlegel(Hypersimplex *hypers, int projFacet, bool projToLargerFacet, const MatrixXd &pts)
    : m_hypers(hypers),
      m_facetPairIndex(projFacet),
      m_projToLargerFacet(projToLargerFacet),
      m_pts(pts)
{
}

struct Point {
    Point(VectorXd _val, int _cI) : val(_val), cI(_cI) {}
    int size() const { return val.size(); }
    VectorXd val;
    int cI; // combinadic index
};

// After calling fp.first is the image plane - currently always the bigger one
void Schlegel::setImageProj(facet_pair &fp) const
{
    std::vector<Vertex> proj, imgPlane;
    bool projToLargerFacet = m_projToLargerFacet;

    int fSize = fp.first.size();
    int sSize = fp.second.size();

    if (fSize == 1 || sSize == 1) {
        projToLargerFacet = true;
    }

    bool fstSmSec = fSize < sSize;
    bool facetSwitch = (fstSmSec && projToLargerFacet) ||
            (!fstSmSec && !projToLargerFacet);

    if (facetSwitch) {
        imgPlane = fp.second;
        proj = fp.first;
    } else {
        imgPlane = fp.first;
        proj = fp.second;
    }
    fp = facet_pair(imgPlane, proj);
}

static Point average(std::vector<Point> points)
{
    Point ret(VectorXd(points[0].size()), -1);
    int count = points.size();

    for (int i = 0; i < ret.size(); i++) {
        double sum = 0;
        for (auto p : points) {
            sum += p.val[i];
        }
        ret.val[i] = sum / count;
    }
    return ret;
}

static VectorXd intersectLineHyperplane(VectorXd p1, VectorXd p2, VectorXd normal)
{
    VectorXd dir = p2 - p1;
    double sc = dir.dot(normal);
    assert(sc != 0);

    double coeff = - p1.dot(normal) / sc;
    return coeff * p1;
}

static MatrixXd gramSchmidt(const MatrixXd &basis)
{
    MatrixXd ret = basis;

    for(int i = 0; i < basis.cols(); ++i) {
        VectorXd vN = ret.col(i).normalized();

        for (int j = i+1; j < basis.cols(); ++j) {
            VectorXd w = ret.col(j);
            double sc = vN.dot(w);
            ret.col(j) = w - sc * vN;
        }
    }

    return ret;
}

std::vector<VectorXd> Schlegel::getDiagram(int &error) const
{
    facet_pair fp = m_hypers->getFacetPair(m_facetPairIndex);

    int dim = m_hypers->d() - 1;
    if (dim > 4) {
        return std::vector<VectorXd>();
    }

    setImageProj(fp);

    std::vector<Point> plane;

    // first set all image plane points
    for (int i = 0; i < fp.first.size(); i++) {
        int cI = fp.first[i].combIndex();
        plane.push_back(Point(m_pts.col(cI), cI));
    }

    // substract first component - zero them
    std::vector<Point> planeZ;
    for (int i = 1; i < plane.size(); i++) {
        planeZ.push_back(Point(plane[i].val - plane[0].val, plane[i].cI));
    }

    // find middle point of image plane points
    Point planeMiddleZ = average(planeZ);

    // find basis of zeroed image plane points
    MatrixXd spanPlaneZ(planeZ[0].size(), planeZ.size());
    for (int i = 0; i < planeZ.size(); i++) {
        spanPlaneZ.block(0, i, spanPlaneZ.rows(), 1) = planeZ[i].val;
    }

    qDebug() << "spanPlaneZ:";
    std::cout << spanPlaneZ  << std::endl;

    FullPivLU<MatrixXd> luDecomp(spanPlaneZ);
    MatrixXd basisPlaneZ = luDecomp.image(spanPlaneZ);

    qDebug() << "Rank of basisPlaneZ:"<< luDecomp.rank();
    qDebug() << "basisPlaneZ:";
    std::cout << basisPlaneZ  << std::endl;

    if (basisPlaneZ.cols() != dim - 1) {
        error = 1;
        return std::vector<VectorXd>();
        qDebug() << "Warning: Schlegel diagram not possible. Image plane points lie not on one hyperplane.";
    }

    // find orthonormal basis of zeroed image plane points
    MatrixXd orthBasisPlaneZ = gramSchmidt(basisPlaneZ);

    qDebug() << "orthBasisPlaneZ:";
    std::cout << orthBasisPlaneZ  << std::endl;

    // find normal vector on image plane and normalize it
    FullPivLU<MatrixXd> luDecomp2(orthBasisPlaneZ.transpose());
    MatrixXd _normalPlaneZ = luDecomp2.kernel();
    VectorXd normalPlaneZ = _normalPlaneZ.col(0);
    normalPlaneZ.normalize();

    qDebug() << "normalPlaneZ:";
    std::cout << normalPlaneZ  << std::endl;

    // set all projected points
    std::vector<Point> proj;

    for (int i = 0; i < fp.second.size(); i++) {
        int cI = fp.second[i].combIndex();
        proj.push_back(Point(m_pts.col(cI), cI));
    }
    // substract from projected points first component of plane
    std::vector<Point> projZ;

    qDebug() << "proj size:" << proj.size();
    for (int i = 0; i < proj.size(); i++) {
        qDebug() << "---";
        std::cout << proj[i].val - plane[0].val  << std::endl;
        projZ.push_back(Point(proj[i].val - plane[0].val, proj[i].cI));
    }

    // set basis for R^d -> test such that normal vector is pointing away
    MatrixXd basisZ(dim, dim);

    qDebug() << "Schlegel::getDiagram0" << dim << orthBasisPlaneZ.cols();
    basisZ.block(0, 0, dim, dim - 1) = orthBasisPlaneZ;
    basisZ.block(0, dim - 1, dim, 1) = normalPlaneZ;

    qDebug() << "basisZ:";
    std::cout << basisZ  << std::endl;

    MatrixXd basisZinv = basisZ.inverse();

    qDebug() << "Schlegel::getDiagram1" << (basisZinv * proj[0].val)[dim - 1];
    if ((basisZinv * proj[0].val)[dim - 1] > 0) {
        // turn normal vector
        normalPlaneZ = -normalPlaneZ;
        basisZ.block(0, dim - 1, dim, 1) = normalPlaneZ;
        basisZinv = basisZ.inverse();
    }
    qDebug() << "Schlegel::getDiagram2";
    assert((basisZinv * proj[0].val)[dim - 1] < 0);

    // first iteration of projection center
    VectorXd projCenterZ = planeMiddleZ.val + normalPlaneZ;

    double normalPlaneCoeff = 0.001;    // TODO: determine better

    {
        /*
         * 1. Compute connecting lines between scTry * projCenterZ and every projected points
         * 2. Find intersection of these lines with planeZ
         * 3. See if all lines lie inside convex hull of image plane points (i = 0)
         *      a) yes -> set scMin = scTry, ++i and try scTry *= 2^i
         *      b) no -> set scMax = scTry and go to 4.
         * 4. Set scTry = (scMax + scMin) / 2 (bisect [scMin, scMax])
         * 5. do 1. and 2.
         * 6. As 3. with:
         *      a) no -> scMax = scTry and go to 4.
         *      b) yes -> scMax - scMin > epsilon ? scMin = scTry and go to 4. : exit and use scTry
         */
        double scTry, scMin, scMax;
        scMin = 0;
        scTry = 1;

        // calculate conv comb matrix of image plane points
        MatrixXd convCmbPlane(2 * dim + 2 * planeZ.size(), planeZ.size());

    }

    projCenterZ = planeMiddleZ.val + normalPlaneZ * normalPlaneCoeff;


    qDebug() << "projZ:" << projZ.size();

    for (auto p : projZ) {
        auto imgZ = intersectLineHyperplane(p.val, projCenterZ, normalPlaneZ);
        Point imgP(imgZ + plane[0].val, p.cI);

        Point imgPZ(imgZ, p.cI);
        planeZ.push_back(imgPZ);

//        qDebug() << "---";
//        std::cout << imgP.val << std::endl;

        plane.push_back(imgP);
    }

    planeZ.push_back(Point(VectorXd::Zero(dim), plane[0].cI));
    std::sort(planeZ.begin(), planeZ.end(), [](const Point &a, const Point &b) {return a.cI < b.cI;});
    std::sort(plane.begin(), plane.end(), [](const Point &a, const Point &b) {return a.cI < b.cI;});

    std::vector<Point> pts;
    for (auto p : planeZ) {
        VectorXd _val = basisZinv * p.val;
        VectorXd val = _val.head(_val.size() - 1);

        pts.push_back(Point(val, p.cI));
    }

    auto middlePtOfPts = average(pts);

    std::vector<VectorXd> ret;
    for (auto p : pts) {
        ret.push_back(p.val - middlePtOfPts.val);
    }

    return ret;
}
