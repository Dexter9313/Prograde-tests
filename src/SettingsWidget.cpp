/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "SettingsWidget.hpp"

SettingsWidget::SettingsWidget(QWidget* parent)
    : QTabWidget(parent)
{
	addGroup("window", tr("Window"));
	addUIntSetting("width", 1500, tr("Window Width"), 0, 10000);
	addUIntSetting("height", 800, tr("Window Height"), 0, 10000);
	addBoolSetting("fullscreen", false, tr("Window Fullscreen"));
	addBoolSetting("hdr", true, tr("High Dynamic Range"));

	addGroup("vr", tr("Virtual Reality"));
	addBoolSetting("enabled", true, tr("Enable VR"));

	addGroup("scripting", tr("Scripting"));
	addDirPathSetting(
	    "rootdir",
	    QFileInfo(settings.fileName()).absoluteDir().absolutePath()
	        + "/scripts",
	    tr("Scripts Root Directory"));

	addGroup("debugcamera", tr("Debug Camera"));
	addBoolSetting("enabled", false, tr("Enable Debug Camera"));
	addBoolSetting("followhmd", false, tr("Follow HMD Movement"));
	addBoolSetting("debuginheadset", false, tr("Show Debug In HMD"));
}

void SettingsWidget::addGroup(QString const& name, QString const& label)
{
	currentGroup = name;

	auto newTab = new QWidget(this);
	QTabWidget::addTab(newTab, label);
	currentForm = new QFormLayout(newTab);
}

void SettingsWidget::insertGroup(QString const& name, QString const& label,
                                 int index)
{
	currentGroup = name;

	QWidget* newTab = new QWidget(this);
	QTabWidget::insertTab(index, newTab, label);
	currentForm = new QFormLayout(newTab);
}

void SettingsWidget::addBoolSetting(QString const& name, bool defaultVal,
                                    QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto checkBox = new QCheckBox(this);
	checkBox->setCheckState(settings.value(fullName).toBool() ? Qt::Checked
	                                                          : Qt::Unchecked);
	connect(checkBox, &QCheckBox::stateChanged, this, [this, fullName](int s) {
		updateValue(fullName, s != Qt::Unchecked);
	});

	currentForm->addRow(label + " :", checkBox);
}

void SettingsWidget::addUIntSetting(QString const& name,
                                    unsigned int defaultVal,
                                    QString const& label, unsigned int minVal,
                                    unsigned int maxVal)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto sbox = new QSpinBox(this);
	sbox->setRange(minVal, maxVal);
	sbox->setValue(settings.value(fullName).toUInt());

	connect(sbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
	        this, [this, fullName](int v) { updateValue(fullName, v); });

	currentForm->addRow(label + " :", sbox);
}

void SettingsWidget::addStringSetting(QString const& name,
                                      QString const& defaultVal,
                                      QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());

	connect(lineEdit, &QLineEdit::textChanged, this,
	        [this, fullName](QString const& t) { updateValue(fullName, t); });

	currentForm->addRow(label + " :", lineEdit);
}

void SettingsWidget::addFilePathSetting(QString const& name,
                                        QString const& defaultVal,
                                        QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());

	auto dirModel = new QFileSystemModel(this);
	dirModel->setRootPath(QDir::currentPath());
	auto completer = new QCompleter(dirModel, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	lineEdit->setCompleter(completer);

	auto browsePb = new QPushButton(this);
	browsePb->setText(tr("..."));
	connect(browsePb, &QPushButton::clicked, this,
	        [this, label, lineEdit](bool) {
		        QString result(QFileDialog::getOpenFileName(this, label,
		                                                    lineEdit->text()));
		        if(result != "")
		        {
			        lineEdit->setText(result);
		        }
	        });

	auto w      = new QWidget(this);
	auto layout = new QHBoxLayout(w);
	layout->addWidget(lineEdit);
	layout->addWidget(browsePb);

	connect(lineEdit, &QLineEdit::textChanged, this,
	        [this, fullName](QString const& t) { updateValue(fullName, t); });

	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addDirPathSetting(QString const& name,
                                       QString const& defaultVal,
                                       QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	auto lineEdit = new QLineEdit(this);
	lineEdit->setText(settings.value(fullName).toString());

	auto dirModel = new QFileSystemModel(this);
	dirModel->setRootPath(QDir::currentPath());
	auto completer = new QCompleter(dirModel, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	lineEdit->setCompleter(completer);

	auto browsePb = new QPushButton(this);
	browsePb->setText(tr("..."));
	connect(browsePb, &QPushButton::clicked, this,
	        [this, label, lineEdit](bool) {
		        QString result(QFileDialog::getExistingDirectory(
		            this, label, lineEdit->text()));
		        if(result != "")
		        {
			        lineEdit->setText(result);
		        }
	        });

	auto w      = new QWidget(this);
	auto layout = new QHBoxLayout(w);
	layout->addWidget(lineEdit);
	layout->addWidget(browsePb);

	connect(lineEdit, &QLineEdit::textChanged, this,
	        [this, fullName](QString const& t) { updateValue(fullName, t); });

	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addVector3DSetting(QString const& name,
                                        QVector3D const& defaultVal,
                                        QString const& label,
                                        std::array<QString, 3> componentLabels,
                                        float minVal, float maxVal)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	QVector3D stored(settings.value(fullName).value<QVector3D>());

	auto w                                = new QWidget(this);
	auto layout                           = new QHBoxLayout(w);
	std::array<QDoubleSpinBox*, 3> sboxes = {{nullptr, nullptr, nullptr}};
	unsigned int i(0);
	for(auto& sbox : sboxes)
	{
		sbox = new QDoubleSpinBox(this);
		sbox->setRange(minVal, maxVal);
		sbox->setSingleStep((maxVal - minVal) / 100.f);
		sbox->setValue(stored[i]);
		layout->addWidget(new QLabel(componentLabels.at(i) + " :", this));
		layout->addWidget(sbox);
		++i;
	}
	for(auto sbox : sboxes)
	{
		connect(sbox,
		        static_cast<void (QDoubleSpinBox::*)(double)>(
		            &QDoubleSpinBox::valueChanged),
		        this, [this, fullName, sboxes](double) {
			        updateValue(fullName, QVector3D(sboxes[0]->value(),
			                                        sboxes[1]->value(),
			                                        sboxes[2]->value()));
		        });
	}

	currentForm->addRow(label + " :", w);
}

void SettingsWidget::addColorSetting(QString const& name,
                                     QColor const& defaultVal,
                                     QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	QColor stored(settings.value(fullName).value<QColor>());

	auto button = new QPushButton(this);

	button->setStyleSheet("QPushButton{ \
    background-color: " + stored.name()
	                      + ";        \
    border-style: inset;                     \
    }");

	connect(button, &QPushButton::clicked, this,
	        [this, fullName, button, label](bool) {
		        QColor result(QColorDialog::getColor(
		            settings.value(fullName).value<QColor>(), this, label));
		        if(!result.isValid())
		        {
			        return;
		        }

		        button->setStyleSheet("QPushButton{ \
    background-color: " + result.name()
		                              + ";        \
    border-style: inset;                     \
    }");
		        updateValue(fullName, result);
	        });

	currentForm->addRow(label + " :", button);
}

void SettingsWidget::addDateTimeSetting(QString const& name,
                                        QDateTime const& defaultVal,
                                        QString const& label)
{
	QString fullName(currentGroup + '/' + name);

	if(!settings.contains(fullName))
	{
		settings.setValue(fullName, defaultVal);
	}

	QDateTime stored(settings.value(fullName).value<QDateTime>().toTimeSpec(
	    Qt::OffsetFromUTC));

	auto dtEdit = new QDateTimeEdit(stored, this);
	dtEdit->setCalendarPopup(true);
	dtEdit->setDisplayFormat("dd.MM.yyyy hh:mm");

	connect(dtEdit, &QDateTimeEdit::dateTimeChanged, this,
	        [this, fullName](QDateTime dt) { updateValue(fullName, dt); });

	currentForm->addRow(label + " :", dtEdit);
}
