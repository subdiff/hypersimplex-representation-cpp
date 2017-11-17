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

import QtQuick 2.7
import QtQuick.Controls 1.4

Repeater {
    id: slidersRepeater

    model: backend.eecWraps
    delegate: VarSlider{}

    function distrVals(changedIndex) {
        if (count < 2) {
            return;
        }
        var chItem = itemAt(changedIndex);
        console.log("Changed val:", chItem.oldVal, "to", chItem.val);


        var diff = (chItem.val - chItem.oldVal) * chItem.mult;
        console.log("diff:", diff, chItem.val, chItem.oldVal, chItem.mult);

        chItem.oldVal = chItem.val;

        var avails = [];
        for (var i = 0; i < count; i++) {
            var itm = itemAt(i);

            if (itm == null) {
                return;
            }
            if (i == changedIndex) {
                continue;
            }

            var availVal = (diff > 0 ? itm.val : 1 - itm.val) * itm.mult;
            console.log("availVal", + i + ":", availVal);

            avails.push([i, availVal]);
        }

        if (chItem.val * chItem.mult == 1) {
            for (i = 0; i < avails.length; i++) {
                setItemVal(avails[i][0], 0);
            }
            return;
        }

        avails.sort(function(a, b){return a[1] - b[1]});

        fillOrDistr(avails, diff);
    }

    function fillOrDistr(avails, diff) {
        console.log("avails:", avails);

        if (!avails.length) {
            updateBackend();
            return;
        }
        console.log("fillOrDistr:", avails[0][1], diff);

        if (avails[0][1] >= Math.abs(diff) / avails.length) {
            // enough available on all - just distribute the rest
            distr(avails, diff);
        } else {
            fillFirst(avails, diff);
        }
    }

    function distr(avails, diff) {
        console.log("distr:", diff);
        var diffDistr = diff / avails.length;
        console.log("distr:", diffDistr);

        for (var i = 0; i < avails.length; i++) {
            var index = avails[i][0];
            setItemVal(index, itemAt(index).val - diffDistr / itemAt(index).mult);
        }
        updateBackend();
    }

    function fillFirst(avails, diff) {
        console.log("fillFirst:", diff);
        var newVal;
        var mp = avails[0][1];

        if (diff > 0) {
            newVal = 0;
            diff -= mp;
        } else {
            newVal = 1;
            diff += mp;
        }
        setItemVal(avails[0][0], newVal);
        avails.shift();
        fillOrDistr(avails, diff);
    }

    function setItemVal(index, val) {
        itemAt(index).oldVal = val;
        itemAt(index).val = val;
    }

    function updateBackend() {
        var vals = [];
        for (var i = 0; i < count - 1; i++) {
            vals.push(itemAt(i).val);
        }
        backend.setVars(vals);
    }
}
