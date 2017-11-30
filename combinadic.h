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
static void combinadicComb(int k, int nInSet, int *comb)
{
    for (int i = k; i > 0; i--) {
        int diff;
        int combIndex = combinadicCombMax(i, nInSet, diff);

        comb[combIndex] = 1;
        nInSet = nInSet - diff;
    }
}

/*
 * Calculates the associated number to the
 * k-combination 'comb'.
 */
static int combinadicN(int d, const int *comb)
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
