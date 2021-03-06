/**************************************************************************
 *  Copyright 2008,2009 KISS Institute for Practical Robotics             *
 *                                                                        *
 *  This file is part of CBC Firmware.                                    *
 *                                                                        *
 *  CBC Firmware is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  CBC Firmware is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this copy of CBC Firmware.  Check the LICENSE file         *
 *  in the project root.  If not, see <http://www.gnu.org/licenses/>.     *
 **************************************************************************/

#include "Sensors.h"

Sensors::Sensors(CbobData *cbobData, QWidget *parent) : QDialog(parent), m_cbobData(cbobData)
{
    setupUi(this);

#ifdef QT_ARCH_ARM
    setWindowState(windowState() | Qt::WindowFullScreen);
#endif

    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateSensors()));

    m_timer.start(50);
}

Sensors::~Sensors()
{
}

void Sensors::updateSensors()
{
    if (isVisible()) {

        ui_Digital0->setText(QString::number(m_cbobData->digital(0)));
        ui_Digital1->setText(QString::number(m_cbobData->digital(1)));
        ui_Digital2->setText(QString::number(m_cbobData->digital(2)));
        ui_Digital3->setText(QString::number(m_cbobData->digital(3)));
        ui_Digital4->setText(QString::number(m_cbobData->digital(4)));
        ui_Digital5->setText(QString::number(m_cbobData->digital(5)));
        ui_Digital6->setText(QString::number(m_cbobData->digital(6)));
        ui_Digital7->setText(QString::number(m_cbobData->digital(7)));

        ui_Analog8->setText (QString::number(m_cbobData->analog(0)));
        ui_Analog9->setText (QString::number(m_cbobData->analog(1)));
        ui_Analog10->setText(QString::number(m_cbobData->analog(2)));
        ui_Analog11->setText(QString::number(m_cbobData->analog(3)));
        ui_Analog12->setText(QString::number(m_cbobData->analog(4)));
        ui_Analog13->setText(QString::number(m_cbobData->analog(5)));
        ui_Analog14->setText(QString::number(m_cbobData->analog(6)));
        ui_Analog15->setText(QString::number(m_cbobData->analog(7)));

        ui_Motor0Power->setText(QString::number(m_cbobData->motorPWM(0)));
        ui_Motor1Power->setText(QString::number(m_cbobData->motorPWM(1)));
        ui_Motor2Power->setText(QString::number(m_cbobData->motorPWM(2)));
        ui_Motor3Power->setText(QString::number(m_cbobData->motorPWM(3)));

        ui_Motor0Position->setText(QString::number(m_cbobData->motorPosition(0)/160));
        ui_Motor1Position->setText(QString::number(m_cbobData->motorPosition(1)/160));
        ui_Motor2Position->setText(QString::number(m_cbobData->motorPosition(2)/160));
        ui_Motor3Position->setText(QString::number(m_cbobData->motorPosition(3)/160));
        
        ui_AccelerometerX->setText(QString::number(m_cbobData->accelerometerX()));
        ui_AccelerometerY->setText(QString::number(m_cbobData->accelerometerY()));
        ui_AccelerometerZ->setText(QString::number(m_cbobData->accelerometerZ()));
        
        ui_BatteryVoltage->setText(QString::number(m_cbobData->batteryVoltage()));
    }
}

