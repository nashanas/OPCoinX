// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2015-2018 The OPCX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bootstrapdialog.h"
#include "bootstrapmodel.h"
#include "chainparams.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "tinyformat.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QDebug>

using namespace std;

static void BootstrapModelChanged(BootstrapDialog *dialog)
{
    qDebug() << "BootstrapModelChanged()";
    QMetaObject::invokeMethod(dialog, "onBootstrapModelChanged", Qt::QueuedConnection);
}

static void BootstrapProgress(BootstrapDialog *dialog, const std::string& title, int nProgress)
{
    qDebug() << "BootstrapProgress()";
    QMetaObject::invokeMethod(dialog, "onBootstrapProgress", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(title)),
                              Q_ARG(int, nProgress));
}

static void BootstrapCompleted(BootstrapDialog *dialog, bool success, const std::string& error)
{
    qDebug() << "BootstrapCompleted()";
    QMetaObject::invokeMethod(dialog, "onBootstrapCompleted", Qt::QueuedConnection,
                              Q_ARG(bool, success),
                              Q_ARG(QString, QString::fromStdString(error)));
}

BootstrapDialog::BootstrapDialog(BootstrapModelPtr model, QWidget *parent):
    QDialog(parent),
    model_(model)
{
    memset(&ui, 0, sizeof(ui));

    if (!model_)
        throw runtime_error(strprintf("%s: model is nullptr", __func__));

    setupUI();
    setupLayout();
    updateUI();

    model_->NotifyModelChanged.connect(boost::bind(BootstrapModelChanged, this));
    model_->NotifyBootstrapProgress.connect(boost::bind(BootstrapProgress, this, _1, _2));
    model_->NotifyBootstrapCompletedI.connect(boost::bind(BootstrapCompleted, this, _1, _2));
}

BootstrapDialog::~BootstrapDialog()
{
    model_->NotifyModelChanged.disconnect(boost::bind(BootstrapModelChanged, this));
    model_->NotifyBootstrapProgress.disconnect(boost::bind(BootstrapProgress, this, _1, _2));
    model_->NotifyBootstrapCompletedI.disconnect(boost::bind(BootstrapCompleted, this, _1, _2));
}

void BootstrapDialog::setupUI()
{
    if (ui.labelTitle) // must be nullptr
        throw std::runtime_error("BootstrapDialog: ui has already been initialized");

    this->setWindowTitle(tr("Bootstrap Blockchain"));

    ui.labelTitle = new QLabel(this);
    ui.labelTitle->setWordWrap(true);
    ui.labelTitle->setText(tr("OPCX Core can bootstrap the blockchain from the cloud or from a file. Please select an option and press Run."));

    ui.radioCloud = new QRadioButton(this);
    ui.radioCloud->setText(tr("Bootstrap from the cloud"));

    ui.radioFile = new QRadioButton(this);
    ui.radioFile->setText(tr("Bootstrap from a file"));

    ui.editFilePath = new QLineEdit(this);

    ui.labelLink = new QLabel(this);
    ui.labelLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    ui.labelLink->setOpenExternalLinks(true);
    string url = strprintf("<a href=\"%s\">%s</a>", Params().GetBootstrapUrl(), Params().GetBootstrapUrl());
    ui.labelLink->setText(QString::fromStdString(url));

    ui.btnSelectFile = new QPushButton(this);
    ui.btnSelectFile->setAutoDefault(false);
    ui.btnSelectFile->setText(tr("..."));

    ui.btnRun = new QPushButton(this);
    ui.btnRun->setAutoDefault(false);
    ui.btnRun->setText(tr("Run"));

    ui.btnHide = new QPushButton(this);
    ui.btnHide->setAutoDefault(false);
    ui.btnHide->setText(tr("Hide"));

    ui.btnCancel = new QPushButton(this);
    ui.btnCancel->setAutoDefault(false);
    ui.btnCancel->setText(tr("Cancel"));

    ui.progress = new GUIUtil::ProgressBar(this);
    ui.progress->setAlignment(Qt::AlignCenter);
    ui.progress->setMaximum(100);
    ui.progress->hide();

    // Override style sheet for progress bar for styles that have a segmented progress bar,
    // as they make the text unreadable (workaround for issue #1071)
    // See https://qt-project.org/doc/qt-4.8/gallery.html
    QString curStyle = QApplication::style()->metaObject()->className();
    if (curStyle == "QWindowsStyle" || curStyle == "QWindowsXPStyle")
        ui.progress->setStyleSheet(PROGRESS_BAR_STYLE);

    connect(ui.radioCloud, SIGNAL (released()), this, SLOT (onButtonRadioCloud()));
    connect(ui.radioFile, SIGNAL (released()), this, SLOT (onButtonRadioFile()));
    connect(ui.btnSelectFile, SIGNAL (released()), this, SLOT (onButtonSelectFile()));
    connect(ui.btnRun, SIGNAL (released()), this, SLOT (onButtonRun()));
    connect(ui.btnHide, SIGNAL (released()), this, SLOT (onButtonHide()));
    connect(ui.btnCancel, SIGNAL (released()), this, SLOT (onButtonCancel()));
    connect(ui.editFilePath, SIGNAL (textEdited(const QString&)), this, SLOT (onEditFilePathChanged(const QString&)));
}

void BootstrapDialog::updateUI()
{
    if (model_->GetBootstrapMode() == BootstrapMode::cloud) {
        ui.radioCloud->setChecked(true);
        ui.radioFile->setChecked(false);
    } else {
        ui.radioCloud->setChecked(false);
        ui.radioFile->setChecked(true);
    }

    ui.editFilePath->setText(GUIUtil::boostPathToQString(model_->GetBootstrapFilePath()));

    const bool running = model_->IsBootstrapRunning();
    ui.radioCloud->setEnabled(!running);
    ui.radioFile->setEnabled(!running);

    if (running) {
        ui.editFilePath->setEnabled(false);
        ui.btnSelectFile->setEnabled(false);
    } else if (model_->GetBootstrapMode() == BootstrapMode::cloud) {
        ui.editFilePath->setEnabled(false);
        ui.btnSelectFile->setEnabled(false);
    } else {
        ui.editFilePath->setEnabled(true);
        ui.btnSelectFile->setEnabled(true);
    }

    ui.btnHide->setEnabled(!startup_ && running);
    ui.btnRun->setEnabled(!running &&
        (model_->GetBootstrapMode() == BootstrapMode::cloud || model_->BootstrapFilePathOk()));

    if (running)
        ui.progress->show();
    else
        ui.progress->hide();

    update();
}

void BootstrapDialog::setupLayout()
{
    this->setFixedWidth(480);

    QHBoxLayout *layoutSelectFile = new QHBoxLayout;
    layoutSelectFile->addItem(new QSpacerItem(10, 50, QSizePolicy::Fixed, QSizePolicy::Fixed));
    layoutSelectFile->addWidget(ui.editFilePath);
    layoutSelectFile->addWidget(ui.btnSelectFile);

    QHBoxLayout *layoutButtons = new QHBoxLayout;
    layoutButtons->addWidget(ui.btnRun);
    layoutButtons->addWidget(ui.btnHide);
    layoutButtons->addWidget(ui.btnCancel);

    QVBoxLayout *layoutLink = new QVBoxLayout;
    layoutLink->setContentsMargins(20, 0, 0, 0);
    layoutLink->addWidget(ui.labelLink);

    QVBoxLayout *layoutCloudLink = new QVBoxLayout;
    layoutCloudLink->setContentsMargins(0, 0, 0, 0);
    layoutCloudLink->setSpacing(0);
    layoutCloudLink->addWidget(ui.radioCloud);
    layoutCloudLink->addLayout(layoutLink);

    // Combine all
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(ui.labelTitle);
    mainLayout->addLayout(layoutCloudLink);
    mainLayout->addWidget(ui.radioFile);
    mainLayout->addLayout(layoutSelectFile);
    mainLayout->addWidget(ui.progress);
    mainLayout->addLayout(layoutButtons);
    this->setLayout(mainLayout);
}

void BootstrapDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
}

void BootstrapDialog::closeEvent(QCloseEvent *event)
{
    if (cancel())
        event->accept();
    else
        event->ignore();
}

void BootstrapDialog::onBootstrapModelChanged()
{
    try {
        updateUI();
    } catch (const exception& e) {
        ShowError(e.what());
    }
}

void BootstrapDialog::onBootstrapProgress(const QString& title, int nProgress)
{
    ui.progress->setValue(nProgress);
    ui.progress->setFormat(tr(title.toStdString().c_str()));
}

void BootstrapDialog::onBootstrapCompleted(bool success, const QString& error)
{
    try {
        if (model_->IsCancelled())
            this->close();
        else {
            if (success) {
                if (startup_)
                    QMessageBox::information(this, this->windowTitle(), tr("Bootstrap successfully completed."), QMessageBox::Ok, QMessageBox::Ok);
                else
                    QMessageBox::information(this, this->windowTitle(), tr("Bootstrap successfully completed. Please restart your wallet."), QMessageBox::Ok, QMessageBox::Ok);

                this->close();
            }
            else {
                ShowError(error.toStdString());
                updateUI();
            }
        }
    } catch (const exception& e) {
        ShowError(e.what());
    }
}

void BootstrapDialog::onButtonRadioCloud()
{
    try {
        string err;
        model_->SetBootstrapMode(BootstrapMode::cloud, err);
        ShowError(err);
    } catch (const exception& e) {
        ShowError(e.what());
    }
}

void BootstrapDialog::onButtonRadioFile()
{
    try {
        string err;
        model_->SetBootstrapMode(BootstrapMode::file, err);
        ShowError(err);
    } catch (const exception& e) {
        ShowError(e.what());
    }
}

void BootstrapDialog::onEditFilePathChanged(const QString& text)
{
    try {
        boost::filesystem::path p = GUIUtil::qstringToBoostPath(text);

        string err;
        model_->SetBootstrapFilePath(p, err);
        ShowError(err);
    } catch (const exception& e) {
        ShowError(e.what());
    }
}

void BootstrapDialog::onButtonSelectFile()
{
    try {
        QFileDialog dialog(this);
        dialog.setWindowTitle(tr("Select please blockchain .zip archive"));
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter(tr("Bootstrap (*.zip)"));
        dialog.setViewMode(QFileDialog::Detail);
        if (dialog.exec()) {
            QStringList fileNames = dialog.selectedFiles();
            if (fileNames.empty()) {
                ShowError("File has not been selected, please try again");
            } else {
                boost::filesystem::path p = GUIUtil::qstringToBoostPath(fileNames.first());
                string err;
                model_->SetBootstrapFilePath(p, err);
                ShowError(err);
            }
        }
    } catch (const exception& e) {
        ShowError(e.what());
    }
}

void BootstrapDialog::onButtonRun()
{
    try {
        string err;
        if (!model_->RunStageIPossible(err) || !model_->RunStageI(err))
            ShowError(err);
        else; //successfully ran
    } catch (const exception& e) {
        ShowError(e.what());
    }
}

void BootstrapDialog::onButtonHide()
{
    this->hide();
}

void BootstrapDialog::onButtonCancel()
{
    cancel();
}

void BootstrapDialog::ShowError(const string& msg)
{
    if (msg.empty())
        return;

    QMessageBox::warning(this, this->windowTitle(), tr(msg.c_str()), QMessageBox::Ok, QMessageBox::Ok);
}

bool BootstrapDialog::cancel()
{
    try {
        if (model_->IsBootstrapRunning()) {
            model_->Cancel();
            model_->Wait();
            return false;
        } else {
            this->close();
            return true;
        }
    } catch (const exception& e) {
        ShowError(e.what());
        return false;
    }
}

void BootstrapDialog::bootstrapBlockchain(BootstrapModelPtr model)
{
    if (!model)
        throw runtime_error(strprintf("%s: model is nullptr", __func__));

    BootstrapDialog dlg(model);
    dlg.SetStartupFlag();
    dlg.exec();
}

void BootstrapDialog::SetStartupFlag()
{
    startup_ = true;
}
