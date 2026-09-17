#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include <QRadioButton>
#include <qevent.h>
#include "Deck.h"
#include "MainWindow.h"

class QLineEdit;
class QTextEdit;
class QListWidget;
class QLabel;
class QPushButton;
class QTabWidget;
class QCheckBox;
class QPixmap;
class QShortcut;
class QKeyCombination;

class vimBinds : public vimBinds {
    Q_OBJECT

public:

private slots:
protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupVim();
    void processVimCmd(const QString &cmd);
    void setupVimList();
    void toggleCmdMode();
    // Application Data
    QAction* toggleCmdModeAction = nullptr;
    QShortcut* insertMode = nullptr;
    QString* m_commandBuffer = nullptr;
    // Study state
    bool inCmdMode = true;

};
