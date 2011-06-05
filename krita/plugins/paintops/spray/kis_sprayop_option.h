/*
 *  Copyright (c) 2008,2009,2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_SPRAYOP_OPTION_H
#define KIS_SPRAYOP_OPTION_H

#include <kis_paintop_option.h>
#include <krita_export.h>

const QString SPRAY_DIAMETER = "Spray/diameter";
const QString SPRAY_ASPECT = "Spray/aspect";
const QString SPRAY_COVERAGE = "Spray/coverage";
const QString SPRAY_SCALE = "Spray/scale";
const QString SPRAY_ROTATION = "Spray/rotation";
const QString SPRAY_PARTICLE_COUNT = "Spray/particleCount";
const QString SPRAY_JITTER_MOVE_AMOUNT = "Spray/jitterMoveAmount";
const QString SPRAY_JITTER_MOVEMENT = "Spray/jitterMovement";
const QString SPRAY_SPACING = "Spray/spacing";
const QString SPRAY_GAUSS_DISTRIBUTION = "Spray/gaussianDistribution";
const QString SPRAY_USE_DENSITY = "Spray/useDensity";

class KisSprayOpOptionsWidget;

class KisSprayOpOption : public KisPaintOpOption
{
public:
    KisSprayOpOption();
    ~KisSprayOpOption();

    void setDiameter(int diameter) const;
    int diameter() const;

    qreal brushAspect() const;

    void writeOptionSetting(KisPropertiesConfiguration* setting) const;
    void readOptionSetting(const KisPropertiesConfiguration* setting);

private:
    KisSprayOpOptionsWidget * m_options;

};

class KisSprayProperties {
public:
    quint16 diameter;
    quint16 radius;
    quint16 particleCount;
    qreal aspect;
    qreal coverage;
    qreal amount;
    qreal spacing;
    qreal scale;
    qreal brushRotation;
    bool jitterMovement;
    bool useDensity;
    bool gaussian;

public:
    void loadSettings(const KisPropertiesConfiguration* settings){
        diameter = settings->getInt(SPRAY_DIAMETER);
        radius =  qRound(0.5 * diameter);
        aspect = settings->getDouble(SPRAY_ASPECT);
        particleCount = settings->getDouble(SPRAY_PARTICLE_COUNT);
        coverage = (settings->getDouble(SPRAY_COVERAGE) / 100.0);
        amount = settings->getDouble(SPRAY_JITTER_MOVE_AMOUNT);
        spacing = settings->getDouble(SPRAY_SPACING);
        scale = settings->getDouble(SPRAY_SCALE);
        brushRotation = settings->getDouble(SPRAY_ROTATION);
        jitterMovement = settings->getBool(SPRAY_JITTER_MOVEMENT);
        useDensity = settings->getBool(SPRAY_USE_DENSITY);
        gaussian = settings->getBool(SPRAY_GAUSS_DISTRIBUTION);
    }
};

#endif
