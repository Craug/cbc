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

#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "ui_Sensors.h"
#include <QDialog>
#include <QTimer>

#include "Cbob.h"
#include "CbobData.h"

class Sensors : public QDialog, private Ui::Sensors
{
    Q_OBJECT

public:
    Sensors(CbobData *cbobData = 0, QWidget *parent = 0);
    ~Sensors();

public slots:
    void updateSensors();

private:

    CbobData *m_cbobData;
    QTimer m_timer;
};

#endif
