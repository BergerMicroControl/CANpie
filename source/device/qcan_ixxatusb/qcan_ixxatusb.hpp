//============================================================================//
// File:          qcan_ixxatusb.hpp                                           //
// Description:   CAN plugin for IXXAT USB device                             //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//

#ifndef QCAN_IXXAT_USB_H_
#define QCAN_IXXAT_USB_H_

#include <QObject>
#include <QtPlugin>
#include <..\..\qcan\qcan_interface.hpp>


//! [0]
class QCanIxxatUsb : public QObject, QCanInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.microcontrol.Qt.qcan.QCanInterface" FILE "plugin.json")
    Q_INTERFACES(QCanInterface)

public:
    QString echo(const QString &message) Q_DECL_OVERRIDE;
    int32_t setBitrate(uint32_t ulBitrateV, uint32_t ulBrsClockV) Q_DECL_OVERRIDE;
    int32_t	setMode(const uint32_t ulModeV) Q_DECL_OVERRIDE;
    int32_t	state(void) Q_DECL_OVERRIDE;
    int32_t	statistic(QCanStatistic_ts &clStatisticR) Q_DECL_OVERRIDE;
    int32_t	read(QCanFrame &clFrameR) Q_DECL_OVERRIDE;
    int32_t	write(const QCanFrame &clFrameR) Q_DECL_OVERRIDE;
    int32_t connect(void) Q_DECL_OVERRIDE;
    int32_t disconnect(void) Q_DECL_OVERRIDE;

signals:
    void errorOccurred(int32_t slCanBusErrorV);
};
//! [0]

#endif