#ifndef DEFODAQSTREAMER_H
#define DEFODAQSTREAMER_H

#include <QObject>
#include <QTextStream>
#include <QFile>

#include <DefoDAQModel.h>

class DefoDAQStreamer : public QObject
{
    Q_OBJECT
public:

    explicit DefoDAQStreamer(DefoDAQModel* model, QObject* parent=0);

    const QString& getFilename() const { return ofilename_; }
    void startDAQ(const QString& ofilename);
    
protected slots:

    void handleDAQMessage(const QString& message);

protected:

  DefoDAQModel* model_;
  bool isStreaming_;
  QString ofilename_;
  QFile* ofile_;
  QTextStream* stream_;
};

#endif // DEFODAQSTREAMER_H
