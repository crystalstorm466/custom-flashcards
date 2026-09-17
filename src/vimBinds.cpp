#include "vimBinds.h"
#include "MainWindow.h"
#include "Deck.h"


vimBinds::vimBinds(MainWindow* window) : m_window(window) {}


void vimBinds::toggleCmdMode() {
    if (&v_inCmdMode) { v_inCmdMode = false; } else { v_inCmdMode = true; }
    qDebug() << "cmd status: " << v_inCmdMode;
}

bool vimBinds::getMode() {
    return v_inCmdMode;
}
QString* vimBinds::getBuffer() {
    return v_commandBuffer;
}

void vimBinds::writeBuffer(const QString& buf) {
    v_commandBuffer->append(buf);
}
void vimBinds::clearBuffer() {
    v_commandBuffer->clear();
}

void vimBinds::chopBuffer(int x) {
    v_commandBuffer->chop(x);
}

void vimBinds::processVimCmd(const QString &cmd) {
    //insert mode -> card builder: i mode
    //Escape - return to normal mode
    //delete a card under cursor: x
    //undo: u (require saving deleted cards - depends)
    //save: w
    //quit: q
    //quit w/o saving: q!
    //Command mode which changes mode and imports and export


    //C++ doesnt support strings in switch statments :(
    if (cmd == ":w") {
        qDebug() << "write command called";
        m_window->onExportDeck();
    } else if (cmd == ":o") {
        qDebug() << "open command called";
        m_window->onImportDeck();
    }  else if (cmd == ":q") {
        qDebug() << "quit command called";
        m_window->close();
    } else if (cmd == ":wq") {
        m_window->onExportDeck();
        m_window->close();
    }
    // switch(&cmd) {
    //     case ":w":
    //         onExportDeck();
    //         break;
    //     case ":wq":
    //         onExportDeck();
    //     case ":q":
    //         close();
    //         break;
    //
    // }
}
void vimBinds::saveQuit() {
    m_window->onExportDeck();
    m_window->close();
    qDebug() << "savequit() called";
    QCoreApplication::quit();
}
void vimBinds::setupVim() {

    qDebug() << "insert mode pressed";
}





