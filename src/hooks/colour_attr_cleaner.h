#pragma once
#include <QObject>
#include <QPointer>
#include <QWidget>

// Qt 5.2 (used by the Kobo toolchain) does not support connecting a SIGNAL()
// macro to a lambda. This Q_OBJECT relay receives SelectionController::closeFooterMenu()
// and clears the colour EPD waveform attrs from the ReadingView.
// See the colour flash fix comment in hooks/reading_view.cc for full context.
class ColourAttrCleaner : public QObject {
    Q_OBJECT
public:
    explicit ColourAttrCleaner(QWidget* view, QObject* parent = nullptr);

public slots:
    void onFooterMenuClosed();

private:
    QPointer<QWidget> m_view;
};
