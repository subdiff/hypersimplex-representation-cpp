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

Column {
    id: slidersCol

    spacing: 5

Repeater {
    id: slidersRepeater

    model: backend.eecWraps
    delegate: VarSlider{}

    function hasManualChanges() {
        for (var i = 0; i < count; i++) {
            var itm = itemAt(i);
            if (itm.val != itm.text) {
                return manualTextIsValid();
            }
        }
        return false;
    }

    function manualTextIsValid() {
        var sum = 0;
        for (var i = 0; i < count; i++) {
            var itm = itemAt(i);
            if (itm.text < 0 || itm.text > 1) {
                // TODO: error message
                return false;
            }
            sum += itm.text * itm.mult;
        }
        return sum == 1;
    }

    function applyManualVals() {
        if (manualTextIsValid()) {
            for (var i = 0; i < count; i++) {
                var itm = itemAt(i);
                itm.oldVal = itm.text;
                itm.val = itm.text;
            }
            updateBackend();
        } else {
            console.log("Error: manual values are not valid.");
        }
    }

    function distrVals(changedIndex) {
        if (count < 2) {
            return;
        }
        var chItem = itemAt(changedIndex);
        var diff = (chItem.val - chItem.oldVal) * chItem.mult;
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
            avails.push([i, availVal]);
        }

        var multVal = chItem.val * chItem.mult;
        if (multVal == 1) {
            for (i = 0; i < avails.length; i++) {
                setItemVal(avails[i][0], 0);
            }
            return;
        }
        avails.sort(function(a, b){return a[1] - b[1]});
        fillOrDistr(avails, diff, 1 - multVal);
    }

    function fillOrDistr(avails, diff, restSum) {
        if (!avails.length) {
            updateBackend();
            return;
        }

        if (avails[0][1] >= Math.abs(diff) / avails.length) {
            // enough available on all - just distribute the rest
            distr(avails, diff, restSum);
        } else {
            fillFirst(avails, diff, restSum);
        }
    }

    function distr(avails, diff, restSum) {
        var diffDistr = diff / avails.length;

        for (var i = 0; i < avails.length; i++) {
            var index = avails[i][0];
            var mult = itemAt(index).mult;

            // make sure we don't add up too much through rounding errors
            var nv = Math.min(itemAt(index).val - diffDistr / mult, restSum / mult);
            setItemVal(index, nv);
            restSum = restSum - nv * mult;
        }
        if (restSum > 0) {
            // if through rounding error we have not distributed everything
            for (i = 0; i < count; i++) {
                var itm = itemAt(i);
                if (1. - itm.val * itm.mult >= restSum) {
                    // just put the little rest somewhere
                    setItemVal(i, itm.val + restSum * 1. / itm.mult)
                    break;
                }
            }
        }
        updateBackend();
    }

    function fillFirst(avails, diff, restSum) {
        var nv;
        var mnv = avails[0][1];

        if (diff > 0) {
            nv = 0;
            diff -= mnv;
        } else {
            nv = 1;
            diff += mnv;
        }
        setItemVal(avails[0][0], nv);
        avails.shift();

        restSum = restSum - mnv;
        fillOrDistr(avails, diff, restSum);
    }

    function setItemVal(index, val) {
        itemAt(index).oldVal = val;
        itemAt(index).val = val;
    }

    function updateBackend() {
        var vals = [];
        for (var i = 0; i < count; i++) {
            vals.push(itemAt(i).val);
        }
        backend.setVars(vals);
    }
}
Button {
    visible: slidersRepeater.count > 1
    text: "Set"
    width: slidersCol.width * 0.25
    anchors.right: slidersCol.right
    enabled: slidersRepeater.hasManualChanges()
    onClicked: slidersRepeater.applyManualVals()
}
}
