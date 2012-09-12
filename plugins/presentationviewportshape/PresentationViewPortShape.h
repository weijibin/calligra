/* This file is part of the KDE project
 * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PRESENTATIONVIEWPORTSHAPE_H
#define PRESENTATIONVIEWPORTSHAPE_H

#include <KoShapeContainer.h>
#include <qpainterpath.h>
#include <KoShape.h>
#include <KoPathShape.h>
#include <SvgShape.h>


#define PresentationViewPortShapeId "PresentationViewPortShape"


class KoParameterShape;

class PresentationViewPortShape : public KoShape, public SvgShape
{
public:
  /**
   * @brief Constructor
   * @brief initializes a basic PresentationViewPortShape shaped like a [ ]
   */
    
    PresentationViewPortShape();
    ~PresentationViewPortShape();
    
    /** Reimplemented methods */
    
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext);

    virtual void saveOdf(KoShapeSavingContext &context) const;

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

        /// reimplemented from SvgShape
    virtual bool saveSvg(SvgSavingContext &context);

    /// reimplemented from SvgShape
    virtual bool loadSvg(const KoXmlElement &element, SvgLoadingContext &context);

    virtual QPainterPath outline() const;

    virtual QSizeF size() const;

    
    void initializeAnimationProperties(); 
    void initializeTransitionProfiles();
    /**
     * Parses frame information from a KoXmlElement,
     * And saves it into this frame.
     */
    void parseAnimationProperties(const KoXmlElement& e); 
    bool saveAnimationAttributes(SvgSavingContext& context);
    
    void setRefId(const QString& refid);//TODO:Redundant?
    
    //int sequence();//TODO:Needed?
    QString attribute(const QString& attrName);
    int transitionProfileIndex(const QString& profile);
    
    bool setAttribute(const QString& attrName, const QString& attrValue);
    
    static const QString title;
    static const QString refid;
    static const QString transitionProfile;
    static const QString transitionZoomPercent;
    static const QString transitionDurationMs;
    static const QString sequence;
    static const QString timeoutEnable;
    static const QString timeoutMs;
    static const QString hide;
    static const QString clip;
    static const QString ns;
    
    
private:     
     /**
     * @return a default path in the shape of '[ ]'
     */
     QPainterPath createShapePath(const QSizeF& size);

     QString m_ns;
     QMap<QString, QString> m_animationAttributes;
     QMap<QString, int> m_transitionProfiles;
};
#endif

    