#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("FlashcardStudyApp");
    QApplication::setApplicationDisplayName("Flashcards");
    QApplication::setOrganizationName("StudyTools");

    MainWindow window;
    MainWindow about_window;
    window.show();

    return app.exec();
}
