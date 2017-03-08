/*
	This file is part of the UrgBenri application.

	Copyright (c) 2016 Mehrez Kristou.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Please contact kristou@hokuyo-aut.jp for more details.

*/

#ifndef SENSORINFORMATIONWIDGET_H
#define SENSORINFORMATIONWIDGET_H

#include "HelperPluginInterface.h"

#include <QTranslator>

#include "RangeSensor.h"
#include "UrgLogHandler.h"

using namespace qrk;

namespace Ui
{
class SensorInformationHelperPlugin;
}

class SensorInformationHelperPlugin : public HelperPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(HelperPluginInterface)
    Q_PLUGIN_METADATA(IID "org.kristou.UrgBenri.SensorInformationHelperPlugin")
    PLUGIN_FACTORY(SensorInformationHelperPlugin)

public:
    explicit SensorInformationHelperPlugin(QWidget* parent = 0);
    virtual ~SensorInformationHelperPlugin();

    QString pluginName() const{return tr("Sensor Information");}
    QIcon pluginIcon() const {return QIcon(":/SensorInformationHelperPlugin/tabIcon");}
    QString pluginDescription() const {return tr("Sensor Information");}
    PluginVersion pluginVersion() const {return PluginVersion(1, 0, 0);}
    QString pluginAuthorName() const {return "Mehrez Kristou";}
    QString pluginAuthorContact() const {return "mehrez@kristou.com";}
    int pluginLoadOrder() const {return 0;}
    bool pluginIsExperimental() const { return false; }

    QString pluginLicense() const { return "GPL"; }
    QString pluginLicenseDescription() const { return "GPL"; }

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void loadTranslator(const QString &locale);


public slots:
    void onLoad(PluginManagerInterface *manager);
    void onUnload();

signals:
    void error(const QString &header, const QString &message);
    void warning(const QString &header, const QString &message);
    void information(const QString &header, const QString &message);

protected:
    void changeEvent(QEvent* e);

private slots:
    void saveButtonClicked();
    void reloadButtonClicked();
    void copy();
    void showSensorHomePage();

private:
    Ui::SensorInformationHelperPlugin* ui;
    RangeSensor* m_sensor;
    QString m_model;
    QTranslator m_translator;

    void updateUI(RangeSensorParameter pp,
                  RangeSensorInformation vv,
                  RangeSensorInternalInformation ii);
    void updateUI(UrgLogHandler *logger);
    void resetTable(int rows);

    void setRangeSensor(RangeSensor* sensor);
    void setLogger(UrgLogHandler* logger);
    void noReload(bool state);

    //Functions interface
    QVariant setDeviceMethod(const QVariantList &vars);
    QVariant setLoggerMethod(const QVariantList &vars);
    QVariant noReloadMethod(const QVariantList &vars);

};

#endif // SENSORINFORMATIONWIDGET_H

