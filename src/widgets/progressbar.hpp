#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include <QWidget>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget* parent = nullptr);

    void setText(QString text);
    void setPercent(double value);

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    QString m_text{};
    double m_percent{};
};

#endif // PROGRESSBAR_HPP
