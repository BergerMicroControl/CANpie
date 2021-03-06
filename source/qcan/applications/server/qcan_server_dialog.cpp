//============================================================================//
// File:          qcan_server_dialog.cpp                                      //
// Description:   QCAN classes - CAN server dialog                            //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'LICENSE'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
//============================================================================//


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "qcan_server_dialog.hpp"

#include <QDebug>
#include <QDir>


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Class methods                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// QCanServer()                                                               //
// constructor                                                                //
//----------------------------------------------------------------------------//
QCanServerDialog::QCanServerDialog(QWidget * parent)
   : QDialog(parent)
{
   uint8_t        ubNetworkIdxT;
   QCanNetwork *  pclNetworkT;
   QString        clNetNameT;

   //----------------------------------------------------------------
   // create CAN networks
   //
   setupNetworks();


   //----------------------------------------------------------------
   // setup the user interface
   //
   ui.setupUi(this);

   //----------------------------------------------------------------
   // hide CAN server port
   //
   ui.pclLblSrvPortM->hide();
   ui.pclEdtSrvPortM->hide();


   //----------------------------------------------------------------
   // create toolbox for can interfaces
   //
   pclTbxNetworkP = new QToolBox(ui.pclTabConfigNetworkM);
   pclTbxNetworkP->setGeometry(QRect(0, 0, 101, 361));
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      apclCanIfWidgetP[ubNetworkIdxT]  = new QCanInterfaceWidget(ubNetworkIdxT);
      apclCanIfWidgetP[ubNetworkIdxT]->setGeometry(QRect(0, 0, 101, 145));
      pclTbxNetworkP->addItem(apclCanIfWidgetP[ubNetworkIdxT], 
                              ("CAN " + QString::number(ubNetworkIdxT+1,10)));
   }

   //----------------------------------------------------------------
   // connect widgets of dialog
   //
   connect(ui.pclCkbNetEnableM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfEnable(bool)));

   connect(ui.pclCkbNetErrorFramesM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfErrorFrames(bool)));

   connect(ui.pclCkbNetCanFdM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfCanFd(bool)));

   connect(ui.pclCkbNetListenOnlyM, SIGNAL(clicked(bool)),
           this, SLOT(onNetworkConfListenOnly(bool)));
   
   connect(pclTbxNetworkP, SIGNAL(currentChanged(int)),
           this, SLOT(onNetworkChange(int)));

   connect(ui.pclCbbNetNomBitrateM, SIGNAL(currentIndexChanged(int)),
           this, SLOT(onNetworkConfBitrateNom(int)));

   connect(ui.pclCbbNetDatBitrateM, SIGNAL(currentIndexChanged(int)),
           this, SLOT(onNetworkConfBitrateDat(int)));

   connect(ui.pclCbbServHostM, SIGNAL(currentIndexChanged(int)),
           this, SLOT(onServerConfAddress(int)));

   connect(ui.pclSpnSrvTimeM, SIGNAL(valueChanged(int)),
           this, SLOT(onServerConfTime(int)));

   //----------------------------------------------------------------
   // connect default signals / slots for statistic
   //
   pclNetworkT = pclCanServerP->network(0);
   connect(pclNetworkT, SIGNAL(showCanFrames(uint32_t)),
           this, SLOT(onNetworkShowCanFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showErrFrames(uint32_t)),
           this, SLOT(onNetworkShowErrFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showLoad(uint8_t, uint32_t)),
            this, SLOT(onNetworkShowLoad(uint8_t, uint32_t)) );

   //----------------------------------------------------------------
   // Intialise interface widgets for CAN interface selection
   //
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      connect( apclCanIfWidgetP[ubNetworkIdxT], 
               &QCanInterfaceWidget::interfaceChanged,
               this,                                       
               &QCanServerDialog::onInterfaceChange  );
   }


   //----------------------------------------------------------------
   // load settings
   //
   pclSettingsP = new QSettings( QSettings::NativeFormat,
                                 QSettings::UserScope,
                                 "microcontrol.net",
                                 "QCANserver");

   //-----------------------------------------------------------
   // settings for network
   //
   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      pclNetworkT = pclCanServerP->network(ubNetworkIdxT);
      clNetNameT  = "CAN " + QString("%1").arg(ubNetworkIdxT+1);
      pclSettingsP->beginGroup(clNetNameT);

      pclNetworkT->setBitrate(pclSettingsP->value("bitrateNom",
                                 eCAN_BITRATE_500K).toInt(),
                              pclSettingsP->value("bitrateDat",
                                    eCAN_BITRATE_NONE).toInt());

      pclNetworkT->setNetworkEnabled(pclSettingsP->value("enable",
                                 0).toBool());

      pclNetworkT->setErrorFramesEnabled(pclSettingsP->value("errorFrame",
                                 0).toBool());

      pclNetworkT->setFastDataEnabled(pclSettingsP->value("canFD",
                                 0).toBool());

      pclNetworkT->setListenOnlyEnabled(pclSettingsP->value("listenOnly",
                                 0).toBool());

      apclCanIfWidgetP[ubNetworkIdxT]->setInterface(pclSettingsP->value("interface"+QString::number(ubNetworkIdxT),"").toString());

      pclSettingsP->endGroup();
   }

   //-----------------------------------------------------------
   // settings for server
   //
   pclSettingsP->beginGroup("Server");
   QHostAddress clHostAddrT = QHostAddress(pclSettingsP->value("hostAddress",
                                             "127.0.0.1").toString());
   pclCanServerP->setServerAddress(clHostAddrT);
   pclCanServerP->setDispatcherTime(pclSettingsP->value("dispatchTime",
                                                         20).toInt());
   pclSettingsP->endGroup();

   //----------------------------------------------------------------
   // create actions and tray icon in system tray
   //
   createActions();
   createTrayIcon();
   setIcon();
   pclIconTrayP->show();

   //----------------------------------------------------------------
   // set title of dialog window
   //
   setWindowTitle("CANpie Server");
   
   //----------------------------------------------------------------
   // show CAN channel 1 as default and update user interface
   //
   slLastNetworkIndexP = 0;
   ui.pclTabConfigM->setCurrentIndex(0);
   pclTbxNetworkP->setCurrentIndex(0);
   this->updateUI(0);
}


//----------------------------------------------------------------------------//
// QCanServer()                                                               //
// destructor                                                                 //
//----------------------------------------------------------------------------//
QCanServerDialog::~QCanServerDialog()
{
   uint8_t        ubNetworkIdxT;
   QCanNetwork *  pclNetworkT;
   QString        clNetNameT;

   //----------------------------------------------------------------
   // store settings
   //
   //----------------------------------------------------------------

   for(ubNetworkIdxT = 0; ubNetworkIdxT < QCAN_NETWORK_MAX; ubNetworkIdxT++)
   {
      //--------------------------------------------------------
      // settings for network
      //
      pclNetworkT = pclCanServerP->network(ubNetworkIdxT);
      clNetNameT  = "CAN " + QString("%1").arg(ubNetworkIdxT+1);
      pclSettingsP->beginGroup(clNetNameT);
      pclSettingsP->setValue("bitrateNom", pclNetworkT->nominalBitrate());
      pclSettingsP->setValue("bitrateDat", pclNetworkT->dataBitrate());
      pclSettingsP->setValue("enable",     pclNetworkT->isNetworkEnabled());
      pclSettingsP->setValue("errorFrame", pclNetworkT->isErrorFramesEnabled());
      pclSettingsP->setValue("canFD",      pclNetworkT->isFastDataEnabled());
      pclSettingsP->setValue("listenOnly", pclNetworkT->isListenOnlyEnabled());

      pclSettingsP->setValue("interface"+QString::number(ubNetworkIdxT), 
                              apclCanIfWidgetP[ubNetworkIdxT]->name());

      pclSettingsP->endGroup();
   }
   //-----------------------------------------------------------
   // settings for server
   //
   pclSettingsP->beginGroup("Server");
   pclSettingsP->setValue("hostAddress",
                           pclCanServerP->serverAddress().toString());

   pclSettingsP->setValue("dispatchTime",
                           pclCanServerP->dispatcherTime());
   pclSettingsP->endGroup();

   delete(pclSettingsP);
}


//----------------------------------------------------------------------------//
// createActions()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::createActions(void)
{
   pclActionCnfgP = new QAction(tr("&Configuration ..."), this);
   connect(pclActionCnfgP, &QAction::triggered, this, &QWidget::showNormal);


   pclActionQuitP = new QAction(tr("&Quit"), this);
   connect(pclActionQuitP, &QAction::triggered, qApp, &QCoreApplication::quit);
}


//----------------------------------------------------------------------------//
// createTrayIcon()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::createTrayIcon(void)
{
   pclMenuTrayP = new QMenu(this);
   pclMenuTrayP->addAction(pclActionCnfgP);
   pclMenuTrayP->addSeparator();
   pclMenuTrayP->addAction(pclActionQuitP);

   pclIconTrayP = new QSystemTrayIcon(this);
   pclIconTrayP->setContextMenu(pclMenuTrayP);
}


//----------------------------------------------------------------------------//
// onInterfaceChange()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onInterfaceChange( uint8_t ubIdxV, 
                                          QCanInterface *pclCanIfV)
{
   QCanNetwork *  pclNetworkT;

   qDebug() << "QCanServerDialog:onInterfaceChange() ToolBox Index " << QString::number(ubIdxV);


   //----------------------------------------------------------------
   // get selected QCanNetwork
   //
   pclNetworkT = pclCanServerP->network(ubIdxV);

   //----------------------------------------------------------------
   // add / remove physical CAN interface
   //
   if (pclCanIfV == Q_NULLPTR)
   {
      pclNetworkT->removeInterface();
      apclCanIfWidgetP[ubIdxV]->setIcon(QIcon(QCAN_IF_VCAN_ICON));
      qDebug() << " set vCAN icon";
   }
   else
   {
      qDebug() << " set interface icon";
      if(pclNetworkT->addInterface(pclCanIfV) == true)
      {
         apclCanIfWidgetP[ubIdxV]->setIcon(pclCanIfV->icon());
      } else
      {
         qWarning() << "Fail to add interface!";
      }
   }

}


//----------------------------------------------------------------------------//
// onNetworkChange()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkChange(int slIndexV)
{
   QCanNetwork *  pclNetworkT;

   qDebug() << "onNetworkChange()" << slIndexV;

   //----------------------------------------------------------------
   // reconnect the signals / slots for statistic
   //
   pclNetworkT = pclCanServerP->network(slLastNetworkIndexP);
   disconnect(pclNetworkT, SIGNAL(showCanFrames(uint32_t)),
           this, SLOT(onNetworkShowCanFrames(uint32_t)) );

   disconnect(pclNetworkT, SIGNAL(showErrFrames(uint32_t)),
           this, SLOT(onNetworkShowErrFrames(uint32_t)) );

   disconnect(pclNetworkT, SIGNAL(showLoad(uint8_t, uint32_t)),
            this, SLOT(onNetworkShowLoad(uint8_t, uint32_t)) );


   pclNetworkT = pclCanServerP->network(slIndexV);
   connect(pclNetworkT, SIGNAL(showCanFrames(uint32_t)),
           this, SLOT(onNetworkShowCanFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showErrFrames(uint32_t)),
           this, SLOT(onNetworkShowErrFrames(uint32_t)) );

   connect(pclNetworkT, SIGNAL(showLoad(uint8_t, uint32_t)),
            this, SLOT(onNetworkShowLoad(uint8_t, uint32_t)) );

   //----------------------------------------------------------------
   // update user interface
   //
   this->updateUI(slIndexV);

   //----------------------------------------------------------------
   // store index for future access
   //
   slLastNetworkIndexP = slIndexV;
}


//----------------------------------------------------------------------------//
// onNetworkConfBitrate()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfBitrateDat(int slBitrateSelV)
{
   QCanNetwork *  pclNetworkT;
   int32_t        slBitrateT;

   qDebug() << "Change data bit-rate" << slBitrateSelV;
   switch(slBitrateSelV)
   {
      case 0:
         slBitrateT = eCAN_BITRATE_NONE;
         break;

      case 1:
         slBitrateT = 1000000;
         break;

      case 2:
         slBitrateT = 2000000;
         break;

      case 3:
         slBitrateT = 4000000;
         break;

      default:
         slBitrateT = eCAN_BITRATE_NONE;
         break;

   }
   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setBitrate(pclNetworkT->nominalBitrate(), slBitrateT);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfBitrate()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfBitrateNom(int slBitrateSelV)
{
   QCanNetwork * pclNetworkT;

   qDebug() << "Change nominal bit-rate" << slBitrateSelV;
   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setBitrate(slBitrateSelV, pclNetworkT->dataBitrate());
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfCanFd()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfCanFd(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setFastDataEnabled(btEnableV);
   }

   //----------------------------------------------------------------
   // update user interface
   //
   if(btEnableV == true)
   {
      ui.pclLblNetDatBitrateM->setEnabled(true);
      ui.pclCbbNetDatBitrateM->setEnabled(true);
   }
   else
   {
      ui.pclLblNetDatBitrateM->setEnabled(false);
      ui.pclCbbNetDatBitrateM->setEnabled(false);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfEnable()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfEnable(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setNetworkEnabled(btEnableV);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfErrorFrames()                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfErrorFrames(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setErrorFramesEnabled(btEnableV);
   }
}


//----------------------------------------------------------------------------//
// onNetworkConfListenOnly()                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkConfListenOnly(bool btEnableV)
{
   QCanNetwork * pclNetworkT;

   pclNetworkT = pclCanServerP->network(pclTbxNetworkP->currentIndex());
   if(pclNetworkT != Q_NULLPTR)
   {
      pclNetworkT->setListenOnlyEnabled(btEnableV);
   }
}


//----------------------------------------------------------------------------//
// onNetworkShowCanFrames()                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowCanFrames(uint32_t ulFrameCntV)
{
   ui.pclCntStatCanM->setText(QString("%1").arg(ulFrameCntV));
}

//----------------------------------------------------------------------------//
// onNetworkShowErrFrames()                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowErrFrames(uint32_t ulFrameCntV)
{
   ui.pclCntStatErrM->setText(QString("%1").arg(ulFrameCntV));
}


//----------------------------------------------------------------------------//
// onNetworkShowLoad()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onNetworkShowLoad(uint8_t ubLoadV, uint32_t ulMsgPerSecV)
{
   ui.pclCntStatMsgM->setText(QString("%1").arg(ulMsgPerSecV));
   ui.pclPgbStatLoadM->setValue(ubLoadV);
}


//----------------------------------------------------------------------------//
// onServerConfAddress()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onServerConfAddress(int slIndexV)
{
   QHostAddress clHostAddressT;

   qDebug() << "onServerConfAddress" << slIndexV;
   if(slIndexV == 0)
   {
      clHostAddressT = QHostAddress(QHostAddress::LocalHost);
   }
   else
   {
      clHostAddressT = QHostAddress(QHostAddress::Any);
   }
   pclCanServerP->setServerAddress(clHostAddressT);
}

//----------------------------------------------------------------------------//
// onServerConfTime()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::onServerConfTime(int slValueV)
{
   qDebug() << "onServerConfTime" << slValueV;
   pclCanServerP->setDispatcherTime(slValueV);
}


//----------------------------------------------------------------------------//
// setupNetworks()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::setupNetworks(void)
{
   pclCanServerP = new QCanServer(  this,
                                    QCAN_TCP_DEFAULT_PORT,
                                    QCAN_NETWORK_MAX);
}


//----------------------------------------------------------------------------//
// setIcon()                                                                  //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::setIcon(void)
{
    QIcon clIconT = QIcon(":images/network-icon.png");

    pclIconTrayP->setIcon(clIconT);
    pclIconTrayP->setToolTip("CAN network configuration");
}


//----------------------------------------------------------------------------//
// updateUI()                                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanServerDialog::updateUI(uint8_t ubNetworkIdxV)
{
   QCanNetwork * pclNetworkT;


   //----------------------------------------------------------------
   // update network tab
   //
   pclNetworkT = pclCanServerP->network(ubNetworkIdxV);
   if(pclNetworkT != Q_NULLPTR)
   {
      //--------------------------------------------------------
      // Is network enabled?
      //
      ui.pclCkbNetEnableM->setChecked(pclNetworkT->isNetworkEnabled());

      //--------------------------------------------------------
      // Does it support error frames?
      //
      if(pclNetworkT->hasErrorFramesSupport())
      {
         ui.pclCkbNetErrorFramesM->setEnabled(true);
         ui.pclCkbNetErrorFramesM->setChecked(pclNetworkT->isErrorFramesEnabled());
      }
      else
      {
         ui.pclCkbNetErrorFramesM->setChecked(false);
         ui.pclCkbNetErrorFramesM->setEnabled(false);
      }
      
      //--------------------------------------------------------
      // Does it support CAN-FD frames?
      //
      if(pclNetworkT->hasFastDataSupport())
      {
         ui.pclCkbNetCanFdM->setEnabled(true);
         ui.pclCkbNetCanFdM->setChecked(pclNetworkT->isFastDataEnabled());
      }
      else
      {
         ui.pclCkbNetCanFdM->setChecked(false);
         ui.pclCkbNetCanFdM->setEnabled(false);
      }

      //--------------------------------------------------------
      // Is CAN-FD activated?
      //
      if(ui.pclCkbNetCanFdM->isChecked())
      {
         ui.pclLblNetDatBitrateM->setEnabled(true);
         ui.pclCbbNetDatBitrateM->setEnabled(true);
      }
      else
      {
         ui.pclLblNetDatBitrateM->setEnabled(false);
         ui.pclCbbNetDatBitrateM->setEnabled(false);
      }

      //--------------------------------------------------------
      // Does it support Listen-Only mode?
      //
      if(pclNetworkT->hasListenOnlySupport())
      {
         ui.pclCkbNetListenOnlyM->setEnabled(true);
         ui.pclCkbNetListenOnlyM->setChecked(pclNetworkT->isListenOnlyEnabled());
      }
      else
      {
         ui.pclCkbNetListenOnlyM->setChecked(false);
         ui.pclCkbNetListenOnlyM->setEnabled(false);
      }
      

      //--------------------------------------------------------
      // show current nominal bit-rate
      //
      ui.pclCbbNetNomBitrateM->setCurrentIndex(pclNetworkT->nominalBitrate());
      int32_t slDatBitRateSelT = 0;
      switch(pclNetworkT->dataBitrate())
      {
         case eCAN_BITRATE_NONE:
            slDatBitRateSelT = 0;
            break;

         case 1000000:
            slDatBitRateSelT = 1;
            break;

         case 2000000:
            slDatBitRateSelT = 2;
            break;

         case 4000000:
            slDatBitRateSelT = 3;
            break;

         case 5000000:
            slDatBitRateSelT = 4;
            break;

         default:
            slDatBitRateSelT = 0;
            break;
      }
      ui.pclCbbNetDatBitrateM->setCurrentIndex(slDatBitRateSelT);
      
   }

   //----------------------------------------------------------------
   // update server tab
   //
   if(pclNetworkT != Q_NULLPTR)
   {
      QHostAddress clHostAddrT = pclNetworkT->serverAddress();
      if(clHostAddrT == QHostAddress(QHostAddress::LocalHost))
      {
         ui.pclCbbServHostM->setCurrentIndex(0);
      }
      else
      {
         ui.pclCbbServHostM->setCurrentIndex(1);
      }
      ui.pclSpnSrvTimeM->setValue(pclNetworkT->dispatcherTime());
   }
}
