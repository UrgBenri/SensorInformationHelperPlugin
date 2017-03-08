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

#include "SensorInformationHelperPlugin.h"
#include "ui_SensorInformationHelperPlugin.h"

#include <QPlainTextEdit>
#include <QClipboard>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>
#include <QDebug>
#include <QDateTime>

using namespace std;

SensorInformationHelperPlugin::SensorInformationHelperPlugin(QWidget* parent)
    : HelperPluginInterface(parent)
    , ui(new Ui::SensorInformationHelperPlugin)
{
    REGISTER_FUNCTION(setDeviceMethod);
    REGISTER_FUNCTION(setLoggerMethod);
    REGISTER_FUNCTION(noReloadMethod);

    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setWindowTitle(pluginName());
    setWindowIcon(pluginIcon());

    connect(ui->closeButton, &QAbstractButton::clicked,
            this, [&](){
        QDialog *parentWidget = qobject_cast<QDialog *>(this->parent());
        if(parentWidget){
            parentWidget->close();
        }else{
            close();
        }
    });

    connect(ui->saveButton, &QAbstractButton::clicked,
            this, &SensorInformationHelperPlugin::saveButtonClicked);
    connect(ui->reloadButton, &QAbstractButton::clicked,
            this, &SensorInformationHelperPlugin::reloadButtonClicked);

    connect(ui->homeButton, &QAbstractButton::clicked,
            this, &SensorInformationHelperPlugin::showSensorHomePage);

    QAction* copyAction = new QAction(tr("Copy"), this);
    connect(copyAction, &QAction::triggered, this, &SensorInformationHelperPlugin::copy);
    ui->propertiesTable->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->propertiesTable->addAction(copyAction);
    //    addAction( pasteAction );

    //        ui->reloadButton->setEnabled(false);

    ui->propertiesTable->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->homeButton->setVisible(false);

    m_sensor = NULL;
}

SensorInformationHelperPlugin::~SensorInformationHelperPlugin()
{
    delete ui;
}

void SensorInformationHelperPlugin::saveState(QSettings &settings)
{    
    settings.setValue("widgetGeometry", saveGeometry());
}

void SensorInformationHelperPlugin::restoreState(QSettings &settings)
{
    restoreGeometry(settings.value("widgetGeometry", saveGeometry()).toByteArray());
}

void SensorInformationHelperPlugin::loadTranslator(const QString &locale)
{
    qApp->removeTranslator(&m_translator);
    if (m_translator.load(QString("plugin.%1").arg(locale), ":/SensorInformationHelperPlugin")) {
        qApp->installTranslator(&m_translator);
    }
}

void SensorInformationHelperPlugin::noReload(bool state)
{
    ui->reloadButton->setEnabled(state);
    if (m_sensor){
        updateUI(m_sensor->parameter(), m_sensor->information(), m_sensor->internalInformation());
    }
}

QVariant SensorInformationHelperPlugin::setDeviceMethod(const QVariantList &vars)
{
    if(vars.size() == 1){
        setRangeSensor((RangeSensor*)vars[0].value<void *>());
    }
    return QVariant();
}

QVariant SensorInformationHelperPlugin::setLoggerMethod(const QVariantList &vars)
{
    if(vars.size() == 1){
        setLogger((UrgLogHandler*)vars[0].value<void *>());
    }
    return QVariant();
}

QVariant SensorInformationHelperPlugin::noReloadMethod(const QVariantList &vars)
{
    if(vars.size() == 1){
        noReload(vars[0].toBool());
    }
    return QVariant();
}

void SensorInformationHelperPlugin::onLoad(PluginManagerInterface *manager)
{

}

void SensorInformationHelperPlugin::onUnload()
{

}

void SensorInformationHelperPlugin::setRangeSensor(RangeSensor* sensor)
{
    m_sensor = sensor;
}

void SensorInformationHelperPlugin::setLogger(UrgLogHandler* logger)
{
    if (logger->isOpen()) {
        updateUI(logger);
        show();
    }
}

void SensorInformationHelperPlugin::changeEvent(QEvent* e)
{
    HelperPluginInterface::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if(ui) ui->retranslateUi(this);
        setWindowTitle(pluginName());
        break;
    default:
        break;
    }
}

void SensorInformationHelperPlugin::updateUI(RangeSensorParameter pp,
                                       RangeSensorInformation vv,
                                       RangeSensorInternalInformation ii)
{
    if (m_sensor) {
        resetTable(21);
        m_model = QString::fromStdString(pp.model).trimmed().toLower().split('(')[0];
        if (m_model.startsWith("ubg-04lx-f01")) {
            ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/ubg-04lx-f01")));
        }
        else if (m_model.startsWith("urg-04lx-ug01")) {
            ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/urg-04lx-ug01")));
        }
        else if (m_model.startsWith("urg-04lx")) {
            ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/urg-04lx")));
        }
        else if (m_model.startsWith("utm-30lx-ew")) {
            ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/utm-30lx-ew")));
        }
        else if (m_model.startsWith("utm-30lx")) {
            ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/utm-30lx")));
        }
        else if (m_model.startsWith("uxm-30lx-ew")) {
            ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/uxm-30lx-ew")));
        }
        else {
            ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/unknown")));
        }

        //PP
        ui->propertiesTable->setItem(0, 0, new QTableWidgetItem(QString("Model")));
        ui->propertiesTable->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(pp.model)));

        ui->propertiesTable->setItem(1, 0, new QTableWidgetItem(QString("Area front")));
        ui->propertiesTable->setItem(1, 1, new QTableWidgetItem(QString::number(pp.area_front)));

        ui->propertiesTable->setItem(2, 0, new QTableWidgetItem(QString("Area min")));
        ui->propertiesTable->setItem(2, 1, new QTableWidgetItem(QString::number(pp.area_min)));

        ui->propertiesTable->setItem(3, 0, new QTableWidgetItem(QString("Area max")));
        ui->propertiesTable->setItem(3, 1, new QTableWidgetItem(QString::number(pp.area_max)));

        ui->propertiesTable->setItem(4, 0, new QTableWidgetItem(QString("Area total")));
        ui->propertiesTable->setItem(4, 1, new QTableWidgetItem(QString::number(pp.area_total)));

        ui->propertiesTable->setItem(5, 0, new QTableWidgetItem(QString("Min distance [mm]")));
        ui->propertiesTable->setItem(5, 1, new QTableWidgetItem(QString::number(pp.distance_min)));

        ui->propertiesTable->setItem(6, 0, new QTableWidgetItem(QString("Max distance [mm]")));
        ui->propertiesTable->setItem(6, 1, new QTableWidgetItem(QString::number(pp.distance_max)));

        ui->propertiesTable->setItem(7, 0, new QTableWidgetItem(QString("Motor speed [rpm]")));
        ui->propertiesTable->setItem(7, 1, new QTableWidgetItem(QString::number(pp.scan_rpm)));

        //VV
        ui->propertiesTable->setItem(8, 0, new QTableWidgetItem(QString("Product")));
        ui->propertiesTable->setItem(8, 1, new QTableWidgetItem(QString::fromStdString(vv.product)));

        ui->propertiesTable->setItem(9, 0, new QTableWidgetItem(QString("Protocol")));
        ui->propertiesTable->setItem(9, 1, new QTableWidgetItem(QString::fromStdString(vv.protocol)));

        ui->propertiesTable->setItem(10, 0, new QTableWidgetItem(QString("Firmware")));
        ui->propertiesTable->setItem(10, 1, new QTableWidgetItem(QString::fromStdString(vv.firmware)));

        ui->propertiesTable->setItem(11, 0, new QTableWidgetItem(QString("Serial number")));
        ui->propertiesTable->setItem(11, 1, new QTableWidgetItem(QString::fromStdString(vv.serial_number)));

        ui->propertiesTable->setItem(12, 0, new QTableWidgetItem(QString("Vendor")));
        ui->propertiesTable->setItem(12, 1, new QTableWidgetItem(QString::fromStdString(vv.vendor)));

        //II

        ui->propertiesTable->setItem(13, 0, new QTableWidgetItem(QString("Model")));
        ui->propertiesTable->setItem(13, 1, new QTableWidgetItem(QString::fromStdString(ii.model)));

        ui->propertiesTable->setItem(14, 0, new QTableWidgetItem(QString("Communication type")));
        ui->propertiesTable->setItem(14, 1, new QTableWidgetItem(QString::fromStdString(ii.communicationType)));

        ui->propertiesTable->setItem(15, 0, new QTableWidgetItem(QString("Raw timestamp [Hex]")));
        ui->propertiesTable->setItem(15, 1, new QTableWidgetItem(QString::fromStdString(ii.internalTime)));

        ui->propertiesTable->setItem(16, 0, new QTableWidgetItem(QString("Laser status")));
        ui->propertiesTable->setItem(16, 1, new QTableWidgetItem(QString::fromStdString(ii.laserStatus)));

        ui->propertiesTable->setItem(17, 0, new QTableWidgetItem(QString("Motor desired speed")));
        ui->propertiesTable->setItem(17, 1, new QTableWidgetItem(QString::fromStdString(ii.motorDesiredSpeed)));

        ui->propertiesTable->setItem(18, 0, new QTableWidgetItem(QString("Current internal state")));
        ui->propertiesTable->setItem(18, 1, new QTableWidgetItem(QString::fromStdString(ii.stateID)));

        ui->propertiesTable->setItem(19, 0, new QTableWidgetItem(QString("Sensor situation")));
        ui->propertiesTable->setItem(19, 1, new QTableWidgetItem(QString::fromStdString(ii.sensorSituation)));

        //---------------------------------------------------------
        QStringList commandsList = m_sensor->supportedModes();
        QString commands;
        for (int i = 0; i < commandsList.size(); ++i) {
            commands.append(QString(" [%1]").arg(commandsList[i]));
        }

        ui->propertiesTable->setItem(20, 0, new QTableWidgetItem(QString("Supported Modes")));
        ui->propertiesTable->setItem(20, 1, new QTableWidgetItem(commands));

        //-------------------------------------------------------------
    }
}

void SensorInformationHelperPlugin::updateUI(UrgLogHandler *logger)
{
    resetTable(12);
    m_model = logger->getModel().trimmed().toLower().split('(')[0];
    if (m_model.startsWith("ubg-04lx-f01")) {
        ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/ubg-04lx-f01")));
    }
    else if (m_model.startsWith("urg-04lx-ug01")) {
        ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/urg-04lx-ug01")));
    }
    else if (m_model.startsWith("urg-04lx")) {
        ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/urg-04lx")));
    }
    else if (m_model.startsWith("utm-30lx-ew")) {
        ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/utm-30lx-ew")));
    }
    else if (m_model.startsWith("utm-30lx")) {
        ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/utm-30lx")));
    }
    else if (m_model.startsWith("uxm-30lx-ew")) {
        ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/uxm-30lx-ew")));
    }
    else {
        ui->imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/SensorInformationHelperPlugin/unknown")));
    }

    ui->propertiesTable->setItem(0, 0, new QTableWidgetItem(QString("Application name")));
    ui->propertiesTable->setItem(0, 1, new QTableWidgetItem(logger->getAppName()));

    ui->propertiesTable->setItem(1, 0, new QTableWidgetItem(QString("Application version")));
    ui->propertiesTable->setItem(1, 1, new QTableWidgetItem(logger->getAppVersion()));

    ui->propertiesTable->setItem(2, 0, new QTableWidgetItem(QString("Model")));
    ui->propertiesTable->setItem(2, 1, new QTableWidgetItem(logger->getModel()));

    ui->propertiesTable->setItem(3, 0, new QTableWidgetItem(QString("Serial number")));
    ui->propertiesTable->setItem(3, 1, new QTableWidgetItem(logger->getSerialNumber()));

    ui->propertiesTable->setItem(4, 0, new QTableWidgetItem(QString("Area front")));
    ui->propertiesTable->setItem(4, 1, new QTableWidgetItem(QString::number(logger->getFrontStep())));

    ui->propertiesTable->setItem(5, 0, new QTableWidgetItem(QString("Area min")));
    ui->propertiesTable->setItem(5, 1, new QTableWidgetItem(QString::number(logger->getStartStep())));

    ui->propertiesTable->setItem(6, 0, new QTableWidgetItem(QString("Area max")));
    ui->propertiesTable->setItem(6, 1, new QTableWidgetItem(QString::number(logger->getEndStep())));

    ui->propertiesTable->setItem(7, 0, new QTableWidgetItem(QString("Area total")));
    ui->propertiesTable->setItem(7, 1, new QTableWidgetItem(QString::number(logger->getTotalSteps())));

    ui->propertiesTable->setItem(8, 0, new QTableWidgetItem(QString("Step grouping")));
    ui->propertiesTable->setItem(8, 1, new QTableWidgetItem(QString::number(logger->getGrouping())));

    ui->propertiesTable->setItem(9, 0, new QTableWidgetItem(QString("Min distance [mm]")));
    ui->propertiesTable->setItem(9, 1, new QTableWidgetItem(QString::number(logger->getMinDistance())));

    ui->propertiesTable->setItem(10, 0, new QTableWidgetItem(QString("Max distance [mm]")));
    ui->propertiesTable->setItem(10, 1, new QTableWidgetItem(QString::number(logger->getMaxDistance())));

    ui->propertiesTable->setItem(11, 0, new QTableWidgetItem(QString("Motor speed [rpm]")));
    ui->propertiesTable->setItem(11, 1, new QTableWidgetItem(QString::number(logger->getMotorSpeed())));

    //-------------------------------------------------------------
}

void SensorInformationHelperPlugin::resetTable(int rows)
{
    ui->propertiesTable->clear();

    ui->propertiesTable->setRowCount(rows);
    ui->propertiesTable->setColumnCount(2);

    QStringList labels;
    labels << tr("Property") << tr("Value");

    ui->propertiesTable->setHorizontalHeaderLabels(labels);
    ui->propertiesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->propertiesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void SensorInformationHelperPlugin::saveButtonClicked()
{
    QString defaulName = QString("/") +
            QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss_zzz") +
            ".txt";
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save information file as"),
                                                    QDir::currentPath() + defaulName,
                                                    "*.txt");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            stream << QApplication::applicationName() << " = " <<
                      QApplication::applicationVersion() << endl;

            for (int row = 0; row < ui->propertiesTable->rowCount(); ++row) {
                stream << ui->propertiesTable->item(row, 0)->text() << " = " <<
                          ui->propertiesTable->item(row, 1)->text() << endl;
            }

            file.close();
            emit information(QApplication::applicationName(),
                             tr("Sensor information were saved successfully."));
        }
        else {
            emit error(QApplication::applicationName(),
                       tr("Output file could not be created."));
        }
    }
}

void SensorInformationHelperPlugin::reloadButtonClicked()
{
    if (m_sensor && !m_sensor->isWorking()) {
        updateUI(m_sensor->parameterNow(), m_sensor->informationNow(), m_sensor->internalInformationNow());
    }
    else {
        emit information(QApplication::applicationName(),
                         tr("The sensor is working."));
    }
}

void SensorInformationHelperPlugin::copy()
{
    QItemSelectionModel* selection = ui->propertiesTable->selectionModel();
    QModelIndexList indexes = selection->selectedIndexes();

    if (indexes.size() < 1) {
        return;
    }

    // QModelIndex::operator < sorts first by row, then by column.
    // this is what we need
    std::sort(indexes.begin(), indexes.end());

    // You need a pair of indexes to find the row changes
    QModelIndex previous = indexes.first();
    indexes.removeFirst();
    QString selected_text;
    QModelIndex current;
    Q_FOREACH(current, indexes) {
        QVariant data = ui->propertiesTable->model()->data(previous);
        QString text = data.toString();
        // At this point `text` contains the text in one cell
        selected_text.append(text);
        // If you are at the start of the row the row number of the previous index
        // isn't the same.  Text is followed by a row separator, which is a newline.
        if (current.row() != previous.row()) {
            selected_text.append(QLatin1Char('\n'));
        }
        // Otherwise it's the same row, so append a column separator, which is a tab.
        else {
            selected_text.append(QLatin1Char('\t'));
        }
        previous = current;
    }

    // add last element
    selected_text.append(ui->propertiesTable->model()->data(current).toString());
    selected_text.append(QLatin1Char('\n'));
    qApp->clipboard()->setText(selected_text);
}

void SensorInformationHelperPlugin::showSensorHomePage()
{
    // format systems language
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini", QSettings::IniFormat);
    QString defaultLocale = settings.value("currentLanguage", QLocale::system().name()).toString();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"
    QString suffix = defaultLocale == "ja" ? "co." : "";

    QDesktopServices::openUrl(QUrl("http://www.hokuyo-aut." + suffix + "jp/02sensor/07scanner/download/products/" + m_model, QUrl::TolerantMode));
}

