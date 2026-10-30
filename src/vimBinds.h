#pragma once

#include <QString>
#include <QObject>

class MainWindow;
class QKeyEvent;

class vimBinds {
public:
    enum class Mode {
        Normal,
        Insert,
        Command
    };

    explicit vimBinds(MainWindow* window);

    void setupVim();
    bool handleKeyPress(QObject* watched, QKeyEvent* event);

    Mode mode() const { return m_mode; }
    void setMode(Mode mode);
    bool getMode() const { return m_mode == Mode::Command; }

    void enterNormalMode();
    void enterInsertMode(bool clearInputs = false);
    void enterCommandMode();
    void enterCommandMode(char prompt);
    void toggleCmdMode();

    void processVimCmd(const QString& cmd);

private:
    MainWindow* m_window;
    Mode m_mode = Mode::Normal;
    QString m_commandBuffer;
    QString m_searchQuery;
    void moveToSearchMatch(bool forward);
    char m_promptChar = ':';
    bool m_pendingG = false; // For 'gg'
    bool m_pendingD = false; // For 'dd'
};
