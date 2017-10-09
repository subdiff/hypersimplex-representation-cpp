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

    qDebug() << "GAP Input:" << cmd;
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
        qDebug() << "GAP Output:" << QString(out);
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

    gap_eval(gapCreateGroupCmd(d, d == 2*k));


    createFactoredElements();
}

AutGroup::~AutGroup()
{
    libgap_finalize();
}

void AutGroup::calcSubgroups()
{

}

void AutGroup::createFactoredElements()
{
//    m_factorizations.clear();
//    std::string elementList = gap_eval("AsList(G);\n");

    gap_eval("hom:=EpimorphismFromFreeGroup(G:names:=[\"x\",\"y\"]);\n", true);

    gap_eval("l:=[];\n", false);
    gap_eval("for g in G do Add(l, PreImagesRepresentative(hom,g)); od;\n", false);

    std::string factorlist = gap_eval("l;\n");


}

std::string AutGroup::gapCreateGroupCmd(int d, bool semi)
{
    std::string cmd;

    auto sDCmdGenerate = [&cmd, d]() {
        cmd += "G:=Group((";
        for (int i = 1; i < d; i++) {
            cmd.append(std::to_string(i) + ",");
        }
        cmd += std::to_string(d) + "),(1,2));\n";
    };

    if (semi) {
        // semidirect product S_d with S_2
        sDCmdGenerate();
        // TODOX
    } else {
        // S_d
        sDCmdGenerate();
    }
    qDebug() << "Create cmd:" << QString(cmd.c_str());
    return cmd;
}

//std::string AutGroup::gapCreateGroupCmd(int d, bool semi)
//{

//}
