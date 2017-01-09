#ifndef HTMLNAMEDENTITYRESOLVER_H
#define HTMLNAMEDENTITYRESOLVER_H

#include <QMap>
#include <QXmlStreamEntityResolver>

class HtmlNamedEntityResolver : public QXmlStreamEntityResolver{
private:
    QMap<QString,QString> map;
public:
    HtmlNamedEntityResolver();
    virtual	~HtmlNamedEntityResolver();
    virtual QString resolveUndeclaredEntity(const QString & name);
};

#endif // HTMLNAMEDENTITYRESOLVER_H
