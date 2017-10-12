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

#include "autgroup.h"

extern "C" {
#include <gap/system.h>
#include <gap/gasman.h>
#include <gap/objects.h>
#include <gap/code.h>
#include <gap/calls.h>
#include <gap/gap.h>

#include <gap/libgap.h>
#include <gap/libgap_internal.h>

#include <gap/vars.h>
#include <gap/read.h>
}

#include <QDebug>

static int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

static void gap_error_handler(char *msg)
{
    qDebug() << "GAP Error:" << QString(msg);
}

//static std::string gap_eval(const char *_cmd)
static std::string gap_eval(const std::string _cmd, bool readOutput = true)
{
    char *cmd = const_cast<char*>(_cmd.c_str());

    std::string ret;

//    qDebug() << "GAP Input:" << cmd;
    libgap_enter();
    libgap_start_interaction(cmd);

    auto status = libGAP_ReadEvalCommand(libGAP_BottomLVars, NULL);
    if (status != libGAP_STATUS_END) {
        qDebug() << "GAP STATUS ERROR";
    } else {
//        qDebug() << "GAP SUCCESS";
    }

    char *out = nullptr;
    if (readOutput) {
        libGAP_ViewObjHandler(libGAP_ReadEvalResult);
        out = libgap_get_output();
    }
    if (out) {
        ret = std::string(out);
//        qDebug() << "GAP Output:" << QString(out);
    }
    libgap_exit();
    libgap_finish_interaction();

    return ret;
}

static bool gapIsInitialized = false;
static void initGap()
{
    if (gapIsInitialized) {
        return;
    }

    libgap_set_error_handler(&gap_error_handler);

    char const *argv[8];
    argv[0] = "gap";
    argv[1] = "-l";
    argv[2] = "/home/roman/Studium/Masterarbeit/Software/gap/gap4r8/";//"/usr/bin"; // TODOX
    argv[3] = "-m";
    argv[4] = "64M";
    argv[5] = "-q";
    argv[6] = "-T";
    argv[7] = NULL;
    int argc=7;

    libgap_initialize(argc, const_cast<char**>(argv));
    qDebug() << "GAP initialized";

    gapIsInitialized = true;
}

static void gapCreateGroup(int d, bool semi)
{
    auto sDCmdGenerate = [d]() {
        std::string cmd;
        cmd += "Group((";
        for (int i = 1; i < d; i++) {
            cmd.append(std::to_string(i) + ",");
        }
        return cmd + std::to_string(d) + "),(1,2));\n";
    };

    if (semi) {
        // semidirect product S_d with S_2
        gap_eval("Sd:=" + sDCmdGenerate());
        gap_eval("S2:=Group((1,2));\n", false);
        gap_eval("Sdhom:=GroupHomomorphismByFunction(Sd,Sd,function(g) return (1,2)*g*(1,2);end);\n", false);
        gap_eval("hom:=GroupHomomorphismByImages(S2, AutomorphismGroup(Sd), [(1,2)], [Sdhom]);\n", false);
        gap_eval("G:=SemidirectProduct(S2, hom, Sd);\n", false);
    } else {
        // S_d
        gap_eval("G:=" + sDCmdGenerate(), false);
    }
}

AutGroup::AutGroup(int d, int k)
{
    if (d == 2*k) {
        // semidirect product S_d with S_2
        m_factorizations.reserve(2 * factorial(d));

    } else {
        // S_d
        m_factorizations.reserve(factorial(d));
    }

    initGap();

    libgap_enter()
    libGAP_CollectBags(0,1);
    libgap_exit()

    gapCreateGroup(d, d == 2*k);
    m_gapName = gap_eval("G;\n", true);

    qDebug() << "Full automorphism group:" << m_gapName.c_str();

    createFactoredElements();
    calcSubgroups();
}

AutGroup::~AutGroup()
{
    libgap_finalize();
    delete[] m_subFactorizations;
    m_subFactorizations = nullptr;
}

void AutGroup::calcSubgroups()
{
    m_subgroups.clear();
    delete[] m_subFactorizations;
    m_subFactorizations = nullptr;

    std::string subgroupList = gap_eval("Subs:=AllSubgroups(G);\n", true);
//    gap_eval("Subs:=AllSubgroups(G);\n", false);

    std::size_t limit = 0;
    bool cont = true;
    while (cont) {
        std::size_t pos = subgroupList.find("Group", limit);
        limit = subgroupList.find("), Group", pos);

        if (limit == std::string::npos) {
            cont = false;
            limit = subgroupList.find(") ]\n", pos);
        }
        limit++;

        std::string subgr = subgroupList.substr(pos, limit - pos);
        m_subgroups.push_back(subgr);
    }
    m_subFactorizations = new std::vector<std::string>[m_subgroups.size()];

    qDebug() << "-----------------";
    qDebug() << "-----------------";
    qDebug() << "Subgroup Count" << m_subgroups.size();
//    for (auto s : m_subgroups)
//        qDebug() << "m_subgroups" << QString(s.c_str());

    qDebug() << "-----------------";
}

static std::vector<std::string> splitFactoredElements(std::string elements)
{
    std::vector<std::string> ret;

//    qDebug() << "FACS DO" << elements.c_str();

    std::size_t start = elements.find_first_of("<x(");
    if (start == std::string::npos) {
        return ret;
    }
    elements.erase(0, start);

    bool cont = true;
    while (cont) {
        std::size_t limit = elements.find(',');
        if (limit == std::string::npos) {
            cont = false;
            limit = elements.find(']') - 1;
        }
        std::string el = elements.substr(0, limit);
        ret.push_back(el);

        elements.erase(0, limit + 2);

//        qDebug() << "TEST" << QString(fac.c_str()) << "|||" << QString(facs.c_str());
    }

//    std::string debug;
//    for (auto s : ret)
//        debug += s;
//    qDebug() << "FACS ->" << debug.c_str();
    return ret;
}

void AutGroup::createFactoredElements()
{
    m_factorizations.clear();

    gap_eval("l:=[];\n", false);

//    gap_eval("hom:=EpimorphismFromFreeGroup(G:names:=[\"x\",\"y\"]);\n", true);
//    gap_eval("for g in G do Add(l, PreImagesRepresentative(hom,g)); od;\n", false);

    gap_eval("for g in G do Add(l, Factorization(G,g)); od;\n", false);

    std::string facs = gap_eval("l;\n");
    m_factorizations = splitFactoredElements(facs);

    qDebug() << "-----------------";
    qDebug() << "-----------------";
    qDebug() << "Factorizations Count" << m_factorizations.size();
//    for (auto f : m_factorizations)
//        qDebug() << "m_factorizations" << QString(f.c_str());
}

std::string AutGroup::getFactorization(std::string element) const
{
    // TODO: Faster if we use the list elements.
    return gap_eval("Factorization(G, " + element + " ));\n", true);
}

std::vector<std::string> AutGroup::getFactorizations(int subIndex) const
{
//    qDebug() << "getFactorizations" << subgroup.c_str();

    if (subIndex == -1) {
        return m_factorizations;
    }

    std::string subgroup = m_subgroups[subIndex];

    if (gap_eval(subgroup + "=" + m_gapName + ";\n").substr(0, 4) == "true") {
        return m_factorizations;
    }
    if (m_subFactorizations[subIndex].size() > 0) {
        return m_subFactorizations[subIndex];
    }

    gap_eval("l:=[];\n", false);
    gap_eval("for g in " + subgroup + " do Add(l, Factorization(G,g)); od;\n", false);
    std::string facs = gap_eval("l;\n");

//    qDebug() << "FACS" << QString(facs.c_str());

    m_subFactorizations[subIndex] = splitFactoredElements(facs);
    return m_subFactorizations[subIndex];
}
