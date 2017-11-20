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

#include <algorithm>

#include <stdio.h>
#include <unistd.h>

#include <QDebug>

static int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

std::string AutGroup::gap_eval(const std::string _cmd, bool readOutput, std::string end) const
{
    const char *cmd = _cmd.c_str();
    std::string ret;

    const int endSize = end.size();

//    qDebug() << "GAP cmd:" << readOutput;
//    qDebug() << cmd;

    while (1) {
        if (write(m_writePipe, cmd, strlen(cmd)) == strlen(cmd)) {
            break;
        }
    }

    if(readOutput) {
        char byte = 0;
        while (read(m_readPipe, &byte, 1) == 1) {
            ret += byte;

            std::string comp;
            int retSize = ret.size();
            if (retSize < endSize) {
                continue;
            } else {
                comp = ret.substr(retSize - endSize);
            }
            if (!end.compare(comp)) {
                break;
            }
        }
//        qDebug() << "GAP result:";
//        qDebug() << ret.c_str();
    }
    ret.erase(std::remove(ret.begin(), ret.end(), '\n'), ret.end());

    return ret;
}

void AutGroup::gapCreateGroup(int d, bool semi)
{
    auto sDCmdGenerate = [d]() {
        std::string cmd;
        cmd += "Group((";
        for (int i = 1; i < d; i++) {
            cmd.append(std::to_string(i) + ",");
        }
        return cmd + std::to_string(d) + "),(1,2));;\n";
    };

    if (semi) {
        // semidirect product S_d with S_2
        gap_eval("Sd:=" + sDCmdGenerate(), false);
        gap_eval("S2:=Group((1,2));;\n", false);
        gap_eval("Sdhom:=GroupHomomorphismByFunction(Sd,Sd,function(g) return (1,2)*g*(1,2);end);;\n", false);
        gap_eval("hom:=GroupHomomorphismByImages(S2, AutomorphismGroup(Sd), [(1,2)], [Sdhom]);;\n", false);
        gap_eval("G:=SemidirectProduct(S2, hom, Sd);;\n", false);
    } else {
        // S_d
        gap_eval("G:=" + sDCmdGenerate(), false);
    }
}

AutGroup::AutGroup(int d, int k)
{
    int pipeStdIn[2];
    int pipeStdOut[2];
    pipe(pipeStdIn);
    pipe(pipeStdOut);

    pid_t pid = 0;
    pid = fork();

    if (pid == 0) {
        // child

      dup2(pipeStdIn[0], STDIN_FILENO);
      dup2(pipeStdOut[1], STDOUT_FILENO);

      close(pipeStdIn[0]);
      close(pipeStdOut[1]);
      close(pipeStdIn[1]);
      close(pipeStdOut[0]);

      std::string lineLength = std::to_string(50);
      int childRet = execlp("gap", "gap", "-q", "-m", "64M", "-x", lineLength.c_str(), (char*) NULL);

      qDebug() << "CHILD ERROR" << childRet;
      exit(childRet);
    }

    // parent
    close(pipeStdIn[0]);
    close(pipeStdOut[1]);

    m_readPipe = pipeStdOut[0];
    m_writePipe = pipeStdIn[1];

    if (d == 2*k) {
        // semidirect product S_d with S_2
        m_factorizations.reserve(2 * factorial(d));

    } else {
        // S_d
        m_factorizations.reserve(factorial(d));
    }

    gapCreateGroup(d, d == 2*k);
    m_gapName = gap_eval("G;\n", true, "\n");

    qDebug() << "Full automorphism group:" << m_gapName.c_str();

    createFactoredElements();
    calcSubgroups();
}

AutGroup::~AutGroup()
{
    delete[] m_subFactorizations;
    m_subFactorizations = nullptr;
}

void AutGroup::calcSubgroups()
{
    m_subgroups.clear();
    m_subgroups.push_back("Group(())");
    delete[] m_subFactorizations;
    m_subFactorizations = nullptr;

    std::string subgroupList = gap_eval("Subs:=AllSubgroups(G);\n", true, ") ]\n");

    std::size_t limit = 6;
    bool cont = true;

    while (cont) {
        std::size_t pos = subgroupList.find("Group", limit);

        limit = subgroupList.find("]),", pos);

        if (limit == std::string::npos) {
            cont = false;
            limit = subgroupList.find("]) ]", pos);
        }
        limit += 2;

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
    }
    return ret;
}

void AutGroup::createFactoredElements()
{
    m_factorizations.clear();

    gap_eval("l:=[];;\n", false);

    std::string facs = gap_eval("for g in G do Add(l, Factorization(G,g)); od;l;\n", true, "]");
    m_factorizations = splitFactoredElements(facs);

    qDebug() << "-----------------";
    qDebug() << "-----------------";
    qDebug() << "Factorizations Count" << m_factorizations.size();
//    for (auto f : m_factorizations)
//        qDebug() << "m_factorizations" << QString(f.c_str());
}

std::vector<std::string> AutGroup::getFactorizations(int subIndex) const
{
    if (subIndex == -1) {
        return m_factorizations;
    }

    std::string subgroup = m_subgroups[subIndex];

    qDebug() << "getFactorizations" << subgroup.c_str();

    if (gap_eval(subgroup + "=" + m_gapName + ";\n", true, "e").substr(0, 4) == "true") {
        return m_factorizations;
    }
    if (m_subFactorizations[subIndex].size() > 0) {
        return m_subFactorizations[subIndex];
    }

    gap_eval("l:=[];;\n", false);
    gap_eval("for g in " + subgroup + " do Add(l, Factorization(G,g)); od;\n", false);
    std::string facs = gap_eval("l;\n", true, "]");

//    qDebug() << "FACS" << QString(facs.c_str());

    m_subFactorizations[subIndex] = splitFactoredElements(facs);
    return m_subFactorizations[subIndex];
}
