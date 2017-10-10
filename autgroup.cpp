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
        qDebug() << "ERROR";
    } else {
//        qDebug() << "SUCCESS";
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

AutGroup::AutGroup(int d, int k)
{
    if (d == 2*k) {
        // semidirect product S_d with S_2
        m_factorizations.reserve(2 * factorial(d));

    } else {
        // S_d
        m_factorizations.reserve(factorial(d));
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

    libgap_enter()
    libGAP_CollectBags(0,1);
    libgap_exit()

//    gap_eval("1+2+3;\n");
//      gap_eval("g:=FreeGroup(2);\n");

    gapCreateGroup(d, d == 2*k);
    gap_eval("G;\n", true);

    createFactoredElements();
    calcSubgroups();
}

AutGroup::~AutGroup()
{
    libgap_finalize();
}

void AutGroup::calcSubgroups()
{
    m_subgroups.clear();

    std::string subgroupList = gap_eval("Subs:=AllSubgroups(G);\n", true);
//    gap_eval("Subs:=AllSubgroups(G);\n", false);

    std::size_t limit = 0;
    bool cont = true;
    while (cont) {
        std::size_t pos = subgroupList.find("Group", limit);
        limit = subgroupList.find("), Group", pos);

//        qDebug()<< "S"<< pos << limit;

        if (limit == std::string::npos) {
            cont = false;
            limit = subgroupList.find(") ]\n", pos);
        }
        limit++;

        std::string subgr = subgroupList.substr(pos, limit - pos);
        m_subgroups.push_back(subgr);
//        qDebug()<< "SUB"<< QString(subgr.c_str());
    }

    qDebug() << "-----------------";
    qDebug() << "-----------------";
    qDebug() << "m_subgroups Count" << m_subgroups.size();
    for (auto s : m_subgroups)
        qDebug() << "m_subgroups" << QString(s.c_str());

    qDebug() << "-----------------";
//    qDebug() << "calcSubgroups" << QString(subgroupList.c_str());
}

static std::vector<std::string> splitFactoredElements(std::string elements)
{
    std::vector<std::string> ret;

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
    return ret;
}

void AutGroup::createFactoredElements()
{
    m_factorizations.clear();

//    gap_eval("hom:=EpimorphismFromFreeGroup(G:names:=[\"x\",\"y\"]);\n", true);
    gap_eval("l:=[];\n", false);

//    gap_eval("for g in G do Add(l, PreImagesRepresentative(hom,g)); od;\n", false);
    gap_eval("for g in G do Add(l, Factorization(G,g)); od;\n", false);

    std::string facs = gap_eval("l;\n");
    m_factorizations = splitFactoredElements(facs);

    qDebug() << "-----------------";
    qDebug() << "-----------------";
    qDebug() << "m_factorizations Count" << m_factorizations.size();
    for (auto f : m_factorizations)
        qDebug() << "m_factorizations" << QString(f.c_str());

//    std::size_t start = facs.find('<');
//    if (start == std::string::npos) {
//        return;
//    }

//    facs.erase(0, start);

//    bool cont = true;
//    while (cont) {
//        std::size_t limit = facs.find(',');
//        if (limit == std::string::npos) {
//            cont = false;
//            limit = facs.find(']') - 1;
//        }
//        std::string fac = facs.substr(0, limit);
//        m_factorizations.push_back(fac);

//        facs.erase(0, limit + 2);

////        qDebug() << "TEST" << QString(fac.c_str()) << "|||" << QString(facs.c_str());
//    }
}

std::vector<std::string> AutGroup::getSubgroupFactorizations(std::string subgroup) const
{
//    gap_eval("hom:=EpimorphismFromFreeGroup(G:names:=[\"x\",\"y\"]);\n", true);
    gap_eval("l:=[];\n", false);

//    gap_eval("for g in G do Add(l, PreImagesRepresentative(hom,g)); od;\n", false);

    // TODO: This is multiple times Factorization of elements. Find saved factorization in m_factorizations instead?
    gap_eval("for g in " + subgroup + " do Add(l, Factorization(G,g)); od;\n", false);

    std::string facs = gap_eval("l;\n");

    qDebug() << "FACS" << QString(facs.c_str());

    return splitFactoredElements(facs);

//    std::size_t start = facs.find('<');
//    if (start == std::string::npos) {
//        return;
//    }

//    facs.erase(0, start);

//    bool cont = true;
//    while (cont) {
//        std::size_t limit = facs.find(',');
//        if (limit == std::string::npos) {
//            cont = false;
//            limit = facs.find(']') - 1;
//        }
//        std::string fac = facs.substr(0, limit);
//        m_factorizations.push_back(fac);

//        facs.erase(0, limit + 2);

//        qDebug() << "TEST" << QString(fac.c_str()) << "|||" << QString(facs.c_str());
//    }
}

void AutGroup::gapCreateGroup(int d, bool semi)
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
        gap_eval("S2:=Group((1,2));\n", true);
        gap_eval("Sdhom:=GroupHomomorphismByFunction(Sd,Sd,function(g) return (1,2)*g*(1,2);end);\n", true);
        gap_eval("hom:=GroupHomomorphismByImages(S2, AutomorphismGroup(Sd), [(1,2)], [Sdhom]);\n", true);
        gap_eval("G:=SemidirectProduct(S2, hom, Sd);\n", true);
    } else {
        // S_d
        gap_eval("G:=" + sDCmdGenerate());
    }
}
