#include "vimBinds.h"
#include "MainWindow.h"
#include "Deck.h"

#include <QApplication>
#include <QKeyEvent>
#include <QStatusBar>
#include <QTabWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QDebug>
#include <qlistwidget.h>
#include <qnamespace.h>


//TODO going to implement / searching now



vimBinds::vimBinds(MainWindow* window)
    : m_window(window)
    , m_mode(Mode::Normal)
{
}

void vimBinds::setMode(Mode mode) {
    m_mode = mode;
}

void vimBinds::setupVim() {
    if (m_window) {
        qApp->installEventFilter(m_window);
        enterNormalMode();
    }
}

void vimBinds::enterNormalMode() {
    m_mode = Mode::Normal;
    m_pendingG = false;
    m_pendingD = false;
    m_commandBuffer.clear();

    if (!m_window) return;

    if (m_window->m_frontInput) m_window->m_frontInput->clearFocus();
    if (m_window->m_backInput)  m_window->m_backInput->clearFocus();

    if (m_window->m_tabWidget && m_window->m_tabWidget->currentIndex() == 0) {
        if (m_window->m_cardListWidget) {
            m_window->m_cardListWidget->setFocus();
            if (m_window->m_cardListWidget->currentRow() < 0 && m_window->m_cardListWidget->count() > 0) {
                m_window->m_cardListWidget->setCurrentRow(0);
            }
        }
    } else {
        m_window->setFocus();
    }

    if (m_window->statusBar()) {
        m_window->statusBar()->showMessage("-- NORMAL --", 2000);
    }
}

void vimBinds::enterInsertMode(bool clearInputs) {
    m_mode = Mode::Insert;
    m_pendingG = false;
    m_pendingD = false;
    m_commandBuffer.clear();

    if (!m_window) return;

    // Switch to editor tab (Tab 0: Manage Cards)
    if (m_window->m_tabWidget) {
        m_window->m_tabWidget->setCurrentIndex(0);
    }

    if (clearInputs) {
        if (m_window->m_frontInput) m_window->m_frontInput->clear();
        if (m_window->m_backInput)  m_window->m_backInput->clear();
    }

    if (m_window->m_frontInput) {
        m_window->m_frontInput->setFocus();
    }

    if (m_window->statusBar()) {
        m_window->statusBar()->showMessage("-- INSERT --");
    }
}

// void vimBinds::enterCommandMode() {
//     m_mode = Mode::Command;
//     m_pendingG = false;
//     m_pendingD = false;
//     m_commandBuffer.clear();
//
//     if (m_window && m_window->statusBar()) {
//         m_window->statusBar()->showMessage(":");
//     }
// }
void vimBinds::enterCommandMode() {
    enterCommandMode(':');
}
void vimBinds::enterCommandMode(char prompt) {
    m_mode = Mode::Command;
    m_promptChar = prompt;
    m_commandBuffer.clear();
    if (m_window && m_window->statusBar()) {
        m_window->statusBar()->showMessage(QString(prompt));
    }
}
void vimBinds::toggleCmdMode() {
    if (m_mode == Mode::Command) {
        enterNormalMode();
    } else {
        enterCommandMode();
    }
}

void vimBinds::processVimCmd(const QString& cmd) {
    if (!m_window) return;

    QString trimmed = cmd.trimmed();
    if (trimmed == ":w") {
        m_window->onExportDeck();
    } else if (trimmed == ":o") {
        m_window->onImportDeck();
    } else if (trimmed == ":q") {
        m_window->close();
    } else if (trimmed == ":wq" || trimmed == ":x") {
        m_window->onExportDeck();
        m_window->close();
    } else if (trimmed == ":1" || trimmed == ":manage" || trimmed == ":edit") {
        if (m_window->m_tabWidget) m_window->m_tabWidget->setCurrentIndex(0);
    } else if (trimmed == ":2" || trimmed == ":study") {
        if (m_window->m_tabWidget) m_window->m_tabWidget->setCurrentIndex(1);
    } else if (trimmed == ":shuf" || trimmed == ":shuffle") {
        m_window->toggleShuffle();
    } else if (trimmed.startsWith("/")) {
        //TODO: implement searching
        m_searchQuery = trimmed.mid(1).trimmed(); //cuts off / to get the search trim
        if (!m_searchQuery.isEmpty()) {
            if (m_window->m_tabWidget) {
                m_window->m_tabWidget->setCurrentIndex(0);
            }
            moveToSearchMatch(true);
        // if (query.isEmpty() || !m_window->m_cardListWidget) {
        //     return;
        // }
        //
        // if (m_window->m_tabWidget) {
        //     m_window->m_tabWidget->setCurrentIndex(0);
        // }
        //
        // const auto matches = m_window->m_cardListWidget->findItems(
        //     query, Qt::MatchContains | Qt::MatchFixedString
        // );
        //     if (!matches.isEmpty()) {
        //         m_window->m_cardListWidget->setCurrentItem(matches.first()); //highlight match
        //         m_window->statusBar()->showMessage(QString("Found: $1").arg(query), 3000);
        //     } else {
        //         m_window->statusBar()->showMessage(QString("Pattern not found: $1").arg(query), 3000);
        //     }
        }
    } else {
        if (m_window->statusBar()) {
            m_window->statusBar()->showMessage(QString("Unknown command: %1").arg(trimmed), 3000);
        }
    }
}

void vimBinds::moveToSearchMatch(bool forward) {
    auto* list = m_window ? m_window->m_cardListWidget : nullptr;
    if (!list || m_searchQuery.isEmpty() || list->count() == 0) {
        return;
    }
    const int count = list->count();
    int row = list->currentRow();

    //start at beginning if nothing is selected
    if (row < 0 ) {
        row = forward ? -1 : 0;
    }
    const int direction = forward ? 1 : -1;

    //skip current row, wrap around and select next match
    for (int step = 1; step <= count; ++step) {
        const int candidate = (row + direction * step + count) % count;
        auto* item =list->item(candidate);

        if (item && item->text().contains(m_searchQuery, Qt::CaseInsensitive)) {
            list->setCurrentRow(candidate);
            return;
        }
    }

    if (m_window->statusBar()) {
        m_window->statusBar()->showMessage(
            QString("Pattern not found: %1").arg(m_searchQuery), 3000
        );

    }
}

bool vimBinds::handleKeyPress(QObject* watched, QKeyEvent* event) {
    if (!m_window) return false;

    int key = event->key();
    QString text = event->text();

    // ==========================================
    // 1. INSERT MODE HANDLING
    // ==========================================
    if (m_mode == Mode::Insert) {
        if (key == Qt::Key_Escape) {
            enterNormalMode();
            return true;
        }

        bool inFront = (watched == m_window->m_frontInput);
        bool inBack  = (watched == m_window->m_backInput);

        if (inFront || inBack) {
            if (key == Qt::Key_Return || key == Qt::Key_Enter) {
                if (inFront) {
                    if (m_window->m_backInput) {
                        m_window->m_backInput->setFocus();
                    }
                    return true;
                } else if (inBack) {
                    m_window->onAddCardClicked();
                    if (m_window->m_frontInput) {
                        m_window->m_frontInput->setFocus();
                    }
                    return true;
                }
            }
            // Allow normal typing inside input fields
            return false;
        }

        // If focus shifted away from inputs, switch back to normal
        enterNormalMode();
    }

    // ==========================================
    // 2. COMMAND MODE HANDLING (Started with ':')
    // ==========================================
    if (m_mode == Mode::Command) {
        if (key == Qt::Key_Escape) {
            enterNormalMode();
            return true;
        }
        if (key == Qt::Key_Return || key == Qt::Key_Enter) {
            processVimCmd(QString(m_promptChar) + m_commandBuffer);
            enterNormalMode();
            return true;
        }
        if (key == Qt::Key_Backspace) {
            if (m_commandBuffer.isEmpty()) {
                enterNormalMode();
            } else {
                m_commandBuffer.chop(1);
                if (m_window->statusBar()) {
                    m_window->statusBar()->showMessage(":" + m_commandBuffer);
                }
            }
            return true;
        }
        if (!text.isEmpty()) {
            m_commandBuffer.append(text);
            if (m_window->statusBar()) {
                m_window->statusBar()->showMessage(":" + m_commandBuffer);
            }
            return true;
        }
        return false;
    }

    // ==========================================
    // 3. NORMAL MODE HANDLING
    // ==========================================
    if (key == Qt::Key_Escape) {
        enterNormalMode();
        return true;
    }

    if (key == Qt::Key_Slash) {
        enterCommandMode('/');
        return true;
    }

    // If an input field receives typing while in Normal mode, auto-switch to Insert
    if (watched == m_window->m_frontInput || watched == m_window->m_backInput) {
        m_mode = Mode::Insert;
        if (m_window->statusBar()) {
            m_window->statusBar()->showMessage("-- INSERT --");
        }
        return false;
    }

    int currentTab = m_window->m_tabWidget ? m_window->m_tabWidget->currentIndex() : 0;

    // --- Tab Switching: 'gt' or Tab ---
    if ((key == Qt::Key_Tab && event->modifiers() == Qt::NoModifier) ||
        (m_pendingG && (key == Qt::Key_T || text == "t"))) {
        m_pendingG = false;
        if (m_window->m_tabWidget) {
            int nextTab = (currentTab + 1) % m_window->m_tabWidget->count();
            m_window->m_tabWidget->setCurrentIndex(nextTab);
            if (nextTab == 0 && m_window->m_cardListWidget) {
                m_window->m_cardListWidget->setFocus();
            }
        }
        return true;
    }

    // --- Command Mode: ':' ---
    if (text == ":") {
        enterCommandMode();
        return true;
    }

    // --- Insert Mode: 'i', 'a', 'o' ---
    if (key == Qt::Key_I || text == "i" || text == "a") {
        enterInsertMode(false);
        return true;
    }
    if (key == Qt::Key_O || text == "o") {
        enterInsertMode(true); // 'o' opens a new blank card
        return true;
    }

    // --- Deleting: 'x' or 'dd' ---
    if (text == "x") {
        if (currentTab == 0 && m_window->m_cardListWidget) {
            m_window->onDeleteCardClicked();
        }
        return true;
    }
    if (text == "d") {
        if (m_pendingD) {
            m_pendingD = false;
            if (currentTab == 0 && m_window->m_cardListWidget) {
                m_window->onDeleteCardClicked();
            }
            return true;
        } else {
            m_pendingD = true;
            return true;
        }
    }
    if (text == "n" && !m_searchQuery.isEmpty()) {
        moveToSearchMatch(true);
        return true;
    }

    if (text == "N" && !m_searchQuery.isEmpty()) {
        moveToSearchMatch(false);
        return true;
    }
    m_pendingD = false;

    // --- Top/Bottom Jump: 'gg' / 'G' ---
    if (text == "G") {
        if (currentTab == 0 && m_window->m_cardListWidget && m_window->m_cardListWidget->count() > 0) {
            m_window->m_cardListWidget->setCurrentRow(m_window->m_cardListWidget->count() - 1);
        }
        return true;
    }
    if (text == "g") {
        if (m_pendingG) {
            m_pendingG = false;
            if (currentTab == 0 && m_window->m_cardListWidget && m_window->m_cardListWidget->count() > 0) {
                m_window->m_cardListWidget->setCurrentRow(0);
            }
            return true;
        } else {
            m_pendingG = true;
            return true;
        }
    }
    m_pendingG = false;

    // --- Navigation: 'j' (Down / Next) ---
    if (key == Qt::Key_J || text == "j" || key == Qt::Key_Down) {
        if (currentTab == 0 && m_window->m_cardListWidget) {
            int row = m_window->m_cardListWidget->currentRow();
            if (row < 0 && m_window->m_cardListWidget->count() > 0) {
                m_window->m_cardListWidget->setCurrentRow(0);
            } else if (row + 1 < m_window->m_cardListWidget->count()) {
                m_window->m_cardListWidget->setCurrentRow(row + 1);
            }
        } else if (currentTab == 1) {
            m_window->onNextCardClicked();
        }
        return true;
    }

    // --- Navigation: 'k' (Up / Prev) ---
    if (key == Qt::Key_K || text == "k" || key == Qt::Key_Up) {
        if (currentTab == 0 && m_window->m_cardListWidget) {
            int row = m_window->m_cardListWidget->currentRow();
            if (row - 1 >= 0) {
                m_window->m_cardListWidget->setCurrentRow(row - 1);
            }
        } else if (currentTab == 1) {
            m_window->onPrevCardClicked();
        }
        return true;
    }

    // --- Navigation: 'l' (Next in card view) ---
    if (key == Qt::Key_L || text == "l" || key == Qt::Key_Right) {
        if (currentTab == 1) {
            m_window->onNextCardClicked();
            return true;
        }
    }

    // --- Navigation: 'h' (Prev in card view) ---
    if (key == Qt::Key_H || text == "h" || key == Qt::Key_Left) {
        if (currentTab == 1) {
            m_window->onPrevCardClicked();
            return true;
        }
    }

    // --- Flip Card: Space or 'f' or Enter (in Study Mode) ---
    if (currentTab == 1 && (key == Qt::Key_Space || text == "f" || key == Qt::Key_Return || key == Qt::Key_Enter)) {
        m_window->onFlipCardClicked();
        return true;
    }

    return false;
}
