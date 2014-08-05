/*
*  kis_tool_fill.cc - part of Krayon
*
*  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
*  Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
*  Copyright (c) 2004 Bart Coppens <kde@bartcoppens.be>
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

#include "kis_tool_fill.h"

#include <kis_debug.h>
#include <klocale.h>
#include <QLabel>
#include <QLayout>
#include <QCheckBox>
#include <QComboBox>

#include <QVector>
#include <QRect>
#include <QColor>

#include <knuminput.h>

#include <KoUpdater.h>
#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoProgressUpdater.h>

#include <kis_layer.h>
#include <kis_painter.h>
#include <KoPattern.h>
#include <kis_fill_painter.h>
#include <kis_selection.h>
#include <kis_system_locker.h>

#include <kis_view2.h>
#include <canvas/kis_canvas2.h>
#include <widgets/kis_cmb_composite.h>
#include <widgets/kis_slider_spin_box.h>
#include <kis_cursor.h>
#include <recorder/kis_recorded_fill_paint_action.h>
#include <recorder/kis_node_query_path.h>
#include <recorder/kis_action_recorder.h>
#include "kis_resources_snapshot.h"

#include <processing/fill_processing_visitor.h>
#include <kis_processing_applicator.h>


KisToolFill::KisToolFill(KoCanvasBase * canvas)
        : KisToolPaint(canvas, KisCursor::load("tool_fill_cursor.png", 6, 6))
{
    setObjectName("tool_fill");
    m_feather = 0;
    m_sizemod = 0;
    m_threshold = 80;
    m_usePattern = false;
    m_unmerged = false;
    m_fillOnlySelection = false;
}

KisToolFill::~KisToolFill()
{
}


void KisToolFill::beginPrimaryAction(KoPointerEvent *event)
{
    if (!nodeEditable()) {
        event->ignore();
        return;
    }

    setMode(KisTool::PAINT_MODE);

    m_startPos = convertToPixelCoord(event).toPoint();
}

void KisToolFill::endPrimaryAction(KoPointerEvent *event)
{
    Q_UNUSED(event);
    CHECK_MODE_SANITY_OR_RETURN(KisTool::PAINT_MODE);

    setMode(KisTool::HOVER_MODE);

    if (!currentNode() ||
        !currentImage()->bounds().contains(m_startPos)) {

        return;
    }

    // TODO: remove this block after recording refactoring
    if (image()) {
        KisNodeSP projectionNode;
        if(m_unmerged) {
            projectionNode = currentNode();
        } else {
            projectionNode = image()->root();
        }
        KisRecordedFillPaintAction paintAction(KisNodeQueryPath::absolutePath(currentNode()), m_startPos, KisNodeQueryPath::absolutePath(projectionNode));
        setupPaintAction(&paintAction);
        paintAction.setPattern(currentPattern());
        if(m_usePattern) {
            paintAction.setFillStyle(KisPainter::FillStylePattern);
        }
        image()->actionRecorder()->addAction(paintAction);
    }

    bool useFastMode = m_useFastMode->isChecked();

    KisProcessingApplicator applicator(currentImage(), currentNode(),
                                       KisProcessingApplicator::NONE,
                                       KisImageSignalVector() << ModifiedSignal,
                                       kundo2_i18n("Flood Fill"));

    KisResourcesSnapshotSP resources =
        new KisResourcesSnapshot(image(), currentNode(), 0, this->canvas()->resourceManager());

    KisProcessingVisitorSP visitor =
        new FillProcessingVisitor(m_startPos,
                                  resources->activeSelection(),
                                  resources,
                                  useFastMode,
                                  m_usePattern,
                                  m_fillOnlySelection,
                                  m_feather,
                                  m_sizemod,
                                  m_threshold,
                                  m_unmerged,
                                  false);

    applicator.applyVisitor(visitor,
                            KisStrokeJobData::SEQUENTIAL,
                            KisStrokeJobData::EXCLUSIVE);

    applicator.end();
}

QWidget* KisToolFill::createOptionWidget()
{
    QWidget *widget = KisToolPaint::createOptionWidget();
    widget->setObjectName(toolId() + " option widget");

    QLabel *lbl_fastMode = new QLabel(i18n("Fast mode: "), widget);
    m_useFastMode = new QCheckBox(QString(), widget);
    m_useFastMode->setToolTip(
        i18n("Fills area faster, but does not take composition "
             "mode into account. Selections and other extended "
             "features will also be disabled."));
    m_useFastMode->setChecked(false);

    QLabel *lbl_threshold = new QLabel(i18n("Threshold: "), widget);
    m_slThreshold = new KisSliderSpinBox(widget);
    m_slThreshold->setObjectName("int_widget");
    m_slThreshold->setRange(1, 100);
    m_slThreshold->setPageStep(3);
    m_slThreshold->setValue(m_threshold);

    QLabel *lbl_sizemod = new QLabel(i18n("Grow selection: "), widget);
    m_sizemodWidget = new KisSliderSpinBox(widget);
    m_sizemodWidget->setObjectName("sizemod");
    m_sizemodWidget->setRange(-40, 40);
    m_sizemodWidget->setSingleStep(1);
    m_sizemodWidget->setValue(0);
    m_sizemodWidget->setSuffix("px");

    QLabel *lbl_feather = new QLabel(i18n("Feathering radius: "), widget);
    m_featherWidget = new KisSliderSpinBox(widget);
    m_featherWidget->setObjectName("feather");
    m_featherWidget->setRange(0, 40);
    m_featherWidget->setSingleStep(1);
    m_featherWidget->setValue(0);
    m_featherWidget->setSuffix("px");

    QLabel *lbl_usePattern = new QLabel(i18n("Use pattern:"), widget);
    m_checkUsePattern = new QCheckBox(QString(), widget);
    m_checkUsePattern->setToolTip(i18n("When checked do not use the foreground color, but the gradient selected to fill with"));
    m_checkUsePattern->setChecked(m_usePattern);

    QLabel *lbl_sampleMerged = new QLabel(i18n("Limit to current layer:"), widget);
    m_checkSampleMerged = new QCheckBox(QString(), widget);
    m_checkSampleMerged->setChecked(m_unmerged);

    QLabel *lbl_fillSelection = new QLabel(i18n("Fill entire selection:"), widget);
    m_checkFillSelection = new QCheckBox(QString(), widget);
    m_checkFillSelection->setToolTip(i18n("When checked do not look at the current layer colors, but just fill all of the selected area"));
    m_checkFillSelection->setChecked(m_fillOnlySelection);

    connect (m_useFastMode       , SIGNAL(toggled(bool))    , this, SLOT(slotSetUseFastMode(bool)));
    connect (m_slThreshold       , SIGNAL(valueChanged(int)), this, SLOT(slotSetThreshold(int)));
    connect (m_sizemodWidget     , SIGNAL(valueChanged(int)), this, SLOT(slotSetSizemod(int)));
    connect (m_featherWidget     , SIGNAL(valueChanged(int)), this, SLOT(slotSetFeather(int)));
    connect (m_checkUsePattern   , SIGNAL(toggled(bool))    , this, SLOT(slotSetUsePattern(bool)));
    connect (m_checkSampleMerged , SIGNAL(toggled(bool))    , this, SLOT(slotSetSampleMerged(bool)));
    connect (m_checkFillSelection, SIGNAL(toggled(bool))    , this, SLOT(slotSetFillSelection(bool)));

    addOptionWidgetOption(m_useFastMode, lbl_fastMode);
    addOptionWidgetOption(m_slThreshold, lbl_threshold);
    addOptionWidgetOption(m_sizemodWidget      , lbl_sizemod);
    addOptionWidgetOption(m_featherWidget      , lbl_feather);

    addOptionWidgetOption(m_checkFillSelection, lbl_fillSelection);
    addOptionWidgetOption(m_checkSampleMerged, lbl_sampleMerged);
    addOptionWidgetOption(m_checkUsePattern, lbl_usePattern);

    updateGUI();

    widget->setFixedHeight(widget->sizeHint().height());

    return widget;
}

void KisToolFill::updateGUI()
{
    bool useAdvancedMode = !m_useFastMode->isChecked();
    bool selectionOnly = m_checkFillSelection->isChecked();

    m_useFastMode->setEnabled(!selectionOnly);
    m_slThreshold->setEnabled(!selectionOnly);

    m_sizemodWidget->setEnabled(!selectionOnly && useAdvancedMode);
    m_featherWidget->setEnabled(!selectionOnly && useAdvancedMode);
    m_checkSampleMerged->setEnabled(!selectionOnly && useAdvancedMode);
    m_checkUsePattern->setEnabled(useAdvancedMode);
}

void KisToolFill::slotSetUseFastMode(bool)
{
    updateGUI();
}

void KisToolFill::slotSetThreshold(int threshold)
{
    m_threshold = threshold;
}

void KisToolFill::slotSetUsePattern(bool state)
{
    m_usePattern = state;
}

void KisToolFill::slotSetSampleMerged(bool state)
{
    m_unmerged = state;
}

void KisToolFill::slotSetFillSelection(bool state)
{
    m_fillOnlySelection = state;
    updateGUI();
}

void KisToolFill::slotSetSizemod(int sizemod)
{
    m_sizemod = sizemod;
}

void KisToolFill::slotSetFeather(int feather)
{
    m_feather = feather;
}
#include "kis_tool_fill.moc"
