#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTabWidget>
#include <QMenuBar>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QFrame>
#include <fstream>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_deck("Starter Deck")
{
    setWindowTitle("Flashcard Study App");
    resize(700, 550);

    setupMenu();
    setupUi();

    // Starter seed cards for demonstration
    m_deck.addCard(Flashcard("Who is Charlie Kirk?",
                             "He saved America"));
    m_deck.addCard(Flashcard("What is Arch Linux?",
                             "A Linux distro."));

    refreshCardList();
    updateStudyView();
    statusBar()->showMessage("Welcome! You can add cards, import/export decks, or switch to Study mode.", 5000);
}

void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* newAction = fileMenu->addAction("&New Deck");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewDeck);

    QAction* importAction = fileMenu->addAction("&Import Deck (JSON)...");
    importAction->setShortcut(QKeySequence::Open);
    connect(importAction, &QAction::triggered, this, &MainWindow::onImportDeck);

    QAction* exportAction = fileMenu->addAction("&Export Deck (JSON)...");
    exportAction->setShortcut(QKeySequence::Save);
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportDeck);

    QAction* exportPDF = fileMenu->addAction("&Exprot Deck (PDF)...");
    exportPDF->setShortcut(QKeySequence::Print);
    connect(exportPDF, &QAction::triggered, this, &MainWindow::onExportPDF);

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::setupUi() {
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // ==========================================
    // TAB 1: MANAGE CARDS
    // ==========================================
    QWidget* manageTab = new QWidget();
    QVBoxLayout* manageLayout = new QVBoxLayout(manageTab);

    // Form to create a card
    QFormLayout* formLayout = new QFormLayout();
    m_frontInput = new QLineEdit();
    m_frontInput->setPlaceholderText("Ask a question!");
    m_backInput = new QLineEdit();
    m_backInput->setPlaceholderText("You get an answer.");

    formLayout->addRow("<b>Question (Front):</b>", m_frontInput);
    formLayout->addRow("<b>Answer (Back):</b>", m_backInput);
    manageLayout->addLayout(formLayout);

    // Add Card button
    QPushButton* addCardBtn = new QPushButton("➕ Add Flashcard");
    addCardBtn->setStyleSheet("font-weight: bold; padding: 6px;");
    connect(addCardBtn, &QPushButton::clicked, this, &MainWindow::onAddCardClicked);
    manageLayout->addWidget(addCardBtn);

    // List of cards in the deck
    manageLayout->addSpacing(10);
    QLabel* listLabel = new QLabel("<b>Cards in Deck:</b>");
    manageLayout->addWidget(listLabel);

    m_cardListWidget = new QListWidget();
    manageLayout->addWidget(m_cardListWidget);

    // Delete Card button and Deck count
    QHBoxLayout* listActionLayout = new QHBoxLayout();
    QPushButton* deleteCardBtn = new QPushButton("🗑️ Remove Selected Card");
    connect(deleteCardBtn, &QPushButton::clicked, this, &MainWindow::onDeleteCardClicked);
    listActionLayout->addWidget(deleteCardBtn);

    listActionLayout->addStretch();
    m_statusLabel = new QLabel();
    listActionLayout->addWidget(m_statusLabel);
    manageLayout->addLayout(listActionLayout);

    m_tabWidget->addTab(manageTab, "🗂️ Manage Cards");

    // ==========================================
    // TAB 2: STUDY MODE
    // ==========================================
    QWidget* studyTab = new QWidget();
    QVBoxLayout* studyLayout = new QVBoxLayout(studyTab);

    m_studyProgressLabel = new QLabel("Card 0 of 0");
    m_studyProgressLabel->setAlignment(Qt::AlignCenter);
    m_studyProgressLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #555;");
    studyLayout->addWidget(m_studyProgressLabel);

    // Flashcard Display Box
    m_studyCardDisplay = new ClickableLabel(this);
    m_studyCardDisplay->setAlignment(Qt::AlignCenter);
    m_studyCardDisplay->setWordWrap(true);
    m_studyCardDisplay->setFrameShape(QFrame::StyledPanel);
    m_studyCardDisplay->setMinimumHeight(200);
    m_studyCardDisplay->setStyleSheet(
        "background-color: #fcfcfc; "
        "border: 2px solid #aaa; "
        "border-radius: 12px; "
        "font-size: 18px; "
        "padding: 20px;"
    );
    studyLayout->addWidget(m_studyCardDisplay, 1);

    // Flip Card Button
    m_flipButton = new QPushButton("🔄 Flip Card");
    m_flipButton->setStyleSheet("font-size: 14px; padding: 8px; font-weight: bold;");
    connect(m_flipButton, &QPushButton::clicked, this, &MainWindow::onFlipCardClicked);
    connect(m_studyCardDisplay, &ClickableLabel::clicked, this, &MainWindow::onFlipCardClicked);
    studyLayout->addWidget(m_flipButton);

    // Navigation Buttons (Prev / Next)
    QHBoxLayout* navLayout = new QHBoxLayout();
    m_prevButton = new QPushButton("⬅️ Previous");
    m_nextButton = new QPushButton("Next ➡️");
    connect(m_prevButton, &QPushButton::clicked, this, &MainWindow::onPrevCardClicked);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextCardClicked);
    navLayout->addWidget(m_prevButton);
    navLayout->addWidget(m_nextButton);
    studyLayout->addLayout(navLayout);

    m_tabWidget->addTab(studyTab, "📖 Study Mode");

    // Update study view whenever user switches to the Study tab
    connect(m_tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == 1) {
            m_isShowingAnswer = false;
            updateStudyView();
        }
    });
}

void MainWindow::refreshCardList() {
    m_cardListWidget->clear();
    for (int i = 0; i < m_deck.count(); ++i) {
        const Flashcard& card = m_deck.at(i);
        QString itemText = QString("[%1] %2 -> %3")
                               .arg(i + 1)
                               .arg(card.front())
                               .arg(card.back());
        m_cardListWidget->addItem(itemText);
    }
    m_statusLabel->setText(QString("Total: %1 cards").arg(m_deck.count()));
}

void MainWindow::onAddCardClicked() {
    QString front = m_frontInput->text().trimmed();
    QString back = m_backInput->text().trimmed();

    if (front.isEmpty() || back.isEmpty()) {
        QMessageBox::warning(this, "Missing Input", "Please fill in both the Question (Front) and Answer (Back).");
        return;
    }

    m_deck.addCard(Flashcard(front, back));
    m_frontInput->clear();
    m_backInput->clear();
    m_frontInput->setFocus();

    refreshCardList();
    updateStudyView();
    statusBar()->showMessage("Card added successfully.", 3000);
}

void MainWindow::onDeleteCardClicked() {
    int row = m_cardListWidget->currentRow();
    if (row < 0 || row >= m_deck.count()) {
        QMessageBox::information(this, "Select Card", "Please select a card from the list to remove.");
        return;
    }

    m_deck.removeCard(row);
    if (m_currentStudyIndex >= m_deck.count()) {
        m_currentStudyIndex = std::max(0, m_deck.count() - 1);
    }

    refreshCardList();
    updateStudyView();
    statusBar()->showMessage("Card removed.", 3000);
}

void MainWindow::onCardSelected(int currentRow) {
    Q_UNUSED(currentRow);
}

void MainWindow::onNewDeck() {
    auto reply = QMessageBox::question(this, "New Deck",
                                       "Are you sure you want to clear current cards and create a new deck?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_deck.clear();
        m_deck.setTitle("Untitled Deck");
        m_currentStudyIndex = 0;
        m_isShowingAnswer = false;
        refreshCardList();
        updateStudyView();
        statusBar()->showMessage("Created a new empty deck.", 3000);
    }
}

void MainWindow::onImportDeck() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Import Flashcards",
        "",
        "Flashcard Files (*.json);;All Files (*)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString errorMsg;
    if (!m_deck.loadFromFile(filePath, &errorMsg)) {
        QMessageBox::critical(this, "Import Error", "Failed to import flashcards:\n" + errorMsg);
        return;
    }

    m_currentStudyIndex = 0;
    m_isShowingAnswer = false;
    refreshCardList();
    updateStudyView();
    statusBar()->showMessage(QString("Imported deck successfully with %1 cards.").arg(m_deck.count()), 5000);
}

void MainWindow::onExportDeck() {
    if (m_deck.count() == 0) {
        QMessageBox::warning(this, "Export Deck", "The deck is currently empty. Add cards before exporting.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Flashcards",
        "deck.json",
        "Flashcard Files (*.json);;All Files (*)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString errorMsg;
    if (!m_deck.saveToFile(filePath, &errorMsg)) {
        QMessageBox::critical(this, "Export Error", "Failed to save flashcards:\n" + errorMsg);
        return;
    }

    statusBar()->showMessage(QString("Deck exported to %1").arg(filePath), 5000);
}


void MainWindow::onExportPDF() {
    if (m_deck.count() == 0) {
        QMessageBox::warning(this, "Export PDF", "The deck is currently empty. Add cards before exporting.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Flashcards",
        "deck.pdf",
        "Portable Document Files (*.pdf);;All files (*)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString errorMsg;
    //PDF write //handles cross-platform path conversion
    #if defined(Q_OS_WIN)
    std::ofstream file(filePath.toStdWString(), std::ios::binary);
    #else
    std::ofstream file(filePath.toStdString(), std::ios::binary);
    #endif

    std::vector<long> offsets;

    file << "%PDF-1.4\n";
    auto record_offset = [&]() {
        offsets.push_back(file.tellp());
    };

    record_offset();
    file << "1 0 obj\n << /Type /Catalog /Pages 2 0 R >>\nendobj\n";

    record_offset();
    file << "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n";

    // Object 3: Page Definition
    record_offset();
    file << "3 0 obj\n"
        << "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] "
        << "/Contents 4 0 R /Resources << /Font << /F1 5 0 R >> >> >>\n"
        << "endobj\n";

//   std::string stream_data = "BT /F1 24 Tf 100 700 Td (Hello from Qt and standard C++) Tj ET"; //this is our stream that we write to it.
        double fontSize = 10.0;
        double lineHeight = 14.0;
        QFont font("Helvetica", static_cast<int>(fontSize));
        QFontMetrics fm(font);

        QString ques = m_deck.at(m_currentStudyIndex).front();
        QString ans = m_deck.at(m_currentStudyIndex).back();

        // Page layout: Letter size is 612 x 792 points
        double startX = 40.0;
        double startY = 680.0;
        double gap = 20.0;
        double boxWidth = (612.0 - (2 * startX) - gap) / 2.0; // 256 pt each
        double innerPadding = 12.0;
        int maxTextWidth = static_cast<int>(boxWidth - (2 * innerPadding));

        // Helper: split text into multiple lines so it fits within maxTextWidth
        auto wrapText = [](const QString& text, const QFontMetrics& metrics, int maxWidth) -> QStringList {
            QStringList words = text.split(' ', Qt::SkipEmptyParts);
            QStringList lines;
            QString currentLine;
            for (const QString& word : words) {
                QString candidate = currentLine.isEmpty() ? word : currentLine + " " + word;
                if (metrics.horizontalAdvance(candidate) <= maxWidth) {
                    currentLine = candidate;
                } else {
                    if (!currentLine.isEmpty()) lines.append(currentLine);
                    currentLine = word;
                }
            }
            if (!currentLine.isEmpty()) lines.append(currentLine);
            return lines;
        };

        QStringList quesLines = wrapText(ques, fm, maxTextWidth);
        QStringList ansLines  = wrapText(ans, fm, maxTextWidth);

        // Adjust box height to accommodate the box with the most lines
        int maxLineCount = std::max({1, static_cast<int>(quesLines.size()), static_cast<int>(ansLines.size())});
        double boxHeight = std::max(60.0, (innerPadding * 2) + (maxLineCount * lineHeight));

        double box1X = startX;
        double box2X = box1X + boxWidth + gap;

        // Draw the two side-by-side boxes
        QString streamData = QString(
            "0.5 w 0 G\n"
            // Box 1 (Question border)
            "%1 %2 %3 %4 re S\n"
            // Box 2 (Answer border)
            "%5 %2 %3 %4 re S\n"
        )
        .arg(box1X)
        .arg(startY)
        .arg(boxWidth)
        .arg(boxHeight)
        .arg(box2X);

        // Helper: generate PDF text commands for wrapped lines
        auto formatPdfTextBlock = [](const QStringList& lines, double x, double topY, double size, double leading) -> QString {
            QString block = QString("BT /F1 %1 Tf %2 %3 Td\n").arg(size).arg(x).arg(topY);
            for (int i = 0; i < lines.size(); ++i) {
                QString safeLine = lines[i];
                safeLine.replace("\\", "\\\\").replace("(", "\\(").replace(")", "\\)");
                if (i > 0) {
                    block += QString("0 -%1 Td\n").arg(leading); // Move down for next line
                }
                block += QString("(%1) Tj\n").arg(safeLine);
            }
            block += "ET\n";
            return block;
        };

        // Render Question and Answer text inside their respective boxes
        double textTopY = startY + boxHeight - innerPadding - (fontSize * 0.85);
        streamData += formatPdfTextBlock(quesLines, box1X + innerPadding, textTopY, fontSize, lineHeight);
        streamData += formatPdfTextBlock(ansLines,  box2X + innerPadding, textTopY, fontSize, lineHeight);

        std::string stream_data = streamData.toStdString();
        record_offset();
        file << "4 0 obj\n<< /Length " << stream_data.length() << " >>\n"
        << "stream\n" << stream_data << "\nendstream\nendobj\n"; //prints to PDF

        // Object 5: Font Resource
        record_offset();
        file << "5 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n";

        long xref_offset = file.tellp();

        // Write Cross-Reference Table
        file << "xref\n0 " << (offsets.size() + 1) << "\n";
        file << "0000000000 65535 f \n";
        for (long off : offsets) {
            char num_buf[32];
            snprintf(num_buf, sizeof(num_buf), "%010ld", off);
            file << num_buf << " 00000 n \n";
        }

        // Write Trailer and EOF marker
        file << "trailer\n<< /Size " << (offsets.size() + 1) << " /Root 1 0 R >>\n"
        << "startxref\n" << xref_offset << "\n%%EOF\n";

        file.close();
        qDebug() << "PDF generated successfully at:" << filePath;

}

void MainWindow::updateStudyView() {
    if (m_deck.count() == 0) {
        m_studyProgressLabel->setText("No cards available. Add some in 'Manage Cards'!");
        m_studyCardDisplay->setText("<i>No cards in deck.</i>");
        m_studyCardDisplay->setStyleSheet(
            "background-color: #f7f7f7; "
            "border: 2px dashed #bbb; "
            "border-radius: 12px; "
            "font-size: 16px; "
            "color: #888; "
            "padding: 20px;"
        );
        m_flipButton->setEnabled(false);
        m_prevButton->setEnabled(false);
        m_nextButton->setEnabled(false);
        return;
    }

    m_flipButton->setEnabled(true);
    m_prevButton->setEnabled(m_currentStudyIndex > 0);
    m_nextButton->setEnabled(m_currentStudyIndex < m_deck.count() - 1);

    if (m_currentStudyIndex < 0) m_currentStudyIndex = 0;
    if (m_currentStudyIndex >= m_deck.count()) m_currentStudyIndex = m_deck.count() - 1;

    const Flashcard& currentCard = m_deck.at(m_currentStudyIndex);

    m_studyProgressLabel->setText(
        QString("Card %1 of %2  (%3)")
            .arg(m_currentStudyIndex + 1)
            .arg(m_deck.count())
            .arg(m_isShowingAnswer ? "Answer" : "Question")
    );

    if (m_isShowingAnswer) {
        m_studyCardDisplay->setText(currentCard.back());
        m_studyCardDisplay->setStyleSheet(
            "background-color: #f0fdf4; " // subtle green tint for answer
            "border: 2px solid #86efac; "
            "border-radius: 12px; "
            "font-size: 18px; "
            "color: #14532d; "
            "padding: 20px;"
        );
        m_flipButton->setText("🔄 Flip to Question");
    } else {
        m_studyCardDisplay->setText(currentCard.front());
        m_studyCardDisplay->setStyleSheet(
            "background-color: #eff6ff; " // subtle blue tint for question
            "border: 2px solid #93c5fd; "
            "border-radius: 12px; "
            "font-size: 18px; "
            "color: #1e3a8a; "
            "padding: 20px;"
        );
        m_flipButton->setText("🔄 Flip to Answer");
    }
}

void MainWindow::onFlipCardClicked() {
    if (m_deck.count() == 0) return;
    m_isShowingAnswer = !m_isShowingAnswer;
    updateStudyView();
}

void MainWindow::onPrevCardClicked() {
    if (m_currentStudyIndex > 0) {
        --m_currentStudyIndex;
        m_isShowingAnswer = false; // Reset to question side
        updateStudyView();
    }
}

void MainWindow::onNextCardClicked() {
    if (m_currentStudyIndex < m_deck.count() - 1) {
        ++m_currentStudyIndex;
        m_isShowingAnswer = false; // Reset to question side
        updateStudyView();
    }
}
