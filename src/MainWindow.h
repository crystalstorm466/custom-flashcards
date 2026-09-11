#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include <QRadioButton>
#include "Deck.h"

class QLineEdit;
class QTextEdit;
class QListWidget;
class QLabel;
class QPushButton;
class QTabWidget;

class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel (QWidget* parent = nullptr) : QLabel(parent) {}
    ~ClickableLabel() = default;

signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit clicked();
        QLabel::mousePressEvent(event);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    // Card Management Slots
    void onAddCardClicked();
    void onDeleteCardClicked();
    void onCardSelected(int currentRow);

    // Import / Export Slots
    void onImportDeck();
    void onExportDeck();
    void onExportPDF();
    void onNewDeck();

    // Study Mode Slots
    void onFlipCardClicked();
    void onNextCardClicked();
    void onPrevCardClicked();
    void onShuffle();

private:
    void setupUi();
    void setupMenu();
    void refreshCardList();
    void updateStudyView();

    // Application Data
    Deck m_deck;

    // UI Widgets - Manage Tab
    QLineEdit* m_frontInput = nullptr;
    QLineEdit* m_backInput = nullptr;
    QListWidget* m_cardListWidget = nullptr;
    QLabel* m_statusLabel = nullptr;

    // UI Widgets - Study Tab
    QTabWidget* m_tabWidget = nullptr;
    ClickableLabel* m_studyCardDisplay = nullptr;
    QLabel* m_studyProgressLabel = nullptr;
    QRadioButton* m_shuffleCards = nullptr;
    QPushButton* m_flipButton = nullptr;
    QPushButton* m_prevButton = nullptr;
    QPushButton* m_nextButton = nullptr;

    // Study state
    int m_currentStudyIndex = 0;
    bool m_isShowingAnswer = false;
    bool shuffleYes = false;

};
