#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include <QRadioButton>
#include <qevent.h>
#include "Deck.h"

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
class MainWindow;
class vimBinds {

public:
    explicit vimBinds(MainWindow* window);

    void setupVim();
    void setupVimList();
    void toggleCmdMode();
    bool getMode();
    QString* getBuffer();
    void writeBuffer(const QString& cmd);
    void clearBuffer();
    void chopBuffer(int x);
    void saveQuit();
    void processVimCmd(const QString &cmd);
    // Application Data
    QAction* toggleCmdModeAction = nullptr;
    QShortcut* insertMode = nullptr;
    // Study state
private slots:
protected:

private:
    MainWindow* m_window;
    bool v_inCmdMode = true;
    QString* v_commandBuffer = nullptr;

};
