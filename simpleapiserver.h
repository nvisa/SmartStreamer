#ifndef SIMPLEAPISERVER_H
#define SIMPLEAPISERVER_H

#include <QObject>

class SimpleApiServer : public QObject
{
	Q_OBJECT
public:
	explicit SimpleApiServer(QObject *parent = 0);

signals:

public slots:
};

#endif // SIMPLEAPISERVER_H
