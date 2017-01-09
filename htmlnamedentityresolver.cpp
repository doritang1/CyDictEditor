//엔티티캐릭터가 있으면 에러가 난다.
//이를 해결하기 위해 QXmlStreamReader에게 해결 방법을 알려주어야 한다.

#include "htmlnamedentityresolver.h"

HtmlNamedEntityResolver::HtmlNamedEntityResolver()
{
    map["Auml"] = "Ä";      // "\u00C4"
    map["Ouml"] = "Ö";      // "\u00D6"
    map["Uuml"] = "Ü";      // "\u00DC"
    map["auml"] = "ä";      // "\u00E4"
    map["ouml"] = "ö";      // "\u00F6"
    map["uuml"] = "ü";      // "\u00FC"
    map["szlig"] = "ß";     // "\u00DF"
    map["middot"] = "·";    // "\u00B7";
    map["copy"] = "©";      // "\u00A9";
}

HtmlNamedEntityResolver::~HtmlNamedEntityResolver() { }

QString HtmlNamedEntityResolver::resolveUndeclaredEntity(const QString & name)
{
    return map.value(name, "");
}

