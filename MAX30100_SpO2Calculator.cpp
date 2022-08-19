/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>

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
*/

#include <math.h>

#include "MAX30100_SpO2Calculator.h"

// SaO2 Look-up Table
// http://www.ti.com/lit/an/slaa274b/slaa274b.pdf
const uint8_t SpO2Calculator::spO2LUT [160] ={     

                                  99,99,99,99,99,98,98,98,98,98,97,97,97,97,97,96,96,96,96,96,95,95,95,94,94,94,
                                  93,93,93,92,92,92,91,91,91,91,90,90,90,90,89,89,89,89,88,88,88,88,87,87,87,87,
                                  86,86,86,86,85,85,85,85,84,84,84,84,83,83,83,83,82,82,82,82,81,81,81,81,80,80,
                                  80,80,79,79,79,79,78,78,78,78,77,77,77,77,76,76,76,76,75,75,75,75,74,74,74,73,
                                  73,73,72,72,72,71,71,71,70,70,70,69,69,69,68,68,67,67,67,67,66,66,66,65,65,65,
                                  64,64,64,63,63,63,62,62,62,62,61,61,61,61,60,60,60,59,59,59,58,58,58,57,57,57,
                                  56,56,56,56
                                  };              

SpO2Calculator::SpO2Calculator() :
    irACValueSqSum(0),
    redACValueSqSum(0),
    beatsDetectedNum(0),
    samplesRecorded(0),
    spO2(0)
{
}

void SpO2Calculator::update(float irACValue, float redACValue, bool beatDetected)
{
    irACValueSqSum += irACValue * irACValue;
    redACValueSqSum += redACValue * redACValue;
    ++samplesRecorded;

    if (beatDetected) {
        ++beatsDetectedNum;
        if (beatsDetectedNum == CALCULATE_EVERY_N_BEATS) {
            float acSqRatio = 100.0 * log(redACValueSqSum/samplesRecorded) / log(irACValueSqSum/samplesRecorded);
            uint8_t index = 0;

            if (acSqRatio > 66) {
                index = (uint8_t)acSqRatio - 66;
            } else if (acSqRatio > 50) {
                index = (uint8_t)acSqRatio - 50;
            }
            reset();

            if (index >= 160 || index < 0)
                spO2 = 0; // invalid
            else
                spO2 = spO2LUT[index];
        }
    }
}

void SpO2Calculator::reset()
{
    samplesRecorded = 0;
    redACValueSqSum = 0;
    irACValueSqSum = 0;
    beatsDetectedNum = 0;
    spO2 = 0;
}

uint8_t SpO2Calculator::getSpO2()
{
    return spO2;
}
