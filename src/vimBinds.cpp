#include "vimBinds.h"
#include "MainWindow.h"
#include "Deck.h"

void vimBinds::toggleCmdMode() {
    if (&inCmdMode) { inCmdMode = false; } else { inCmdMode = true; }
    qDebug() << "cmd status: " << inCmdMode;
}

void vimBinds::keyPressEvent(QKeyEvent *event) {
    // TODO: implement state checking for normal mode and command mode
    if (inCmdMode) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            processVimCmd(*m_commandBuffer);
            m_commandBuffer->clear();
        } else if (event->key() == Qt::Key_Backspace) {
            if (!m_commandBuffer->isEmpty()) {
                m_commandBuffer->chop(1); //handle backspace
            }
        } else if (!event->text().isEmpty()) {
            m_commandBuffer->append(event->text());
        }
        return;
    }
    QMainWindow::keyPressEvent(event); //pass unhandled key presses back to QMainWindow
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
        onExportDeck();
    } else if (cmd == ":o") {
        qDebug() << "open command called";
        onImportDeck();
    }  else if (cmd == ":q") {
        qDebug() << "quit command called";
        close();
    } else if (cmd == ":wq") {
        onExportDeck();
        close();
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
    onExportDeck();
    this->close();
    qDebug() << "savequit() called";
    QCoreApplication::quit();
}
void vimBinds::setupVim() {

    qDebug() << "insert mode pressed";
}


void vimBinds::setupVimList() {
    m_cardListWidget->installEventFilter(this);
}



