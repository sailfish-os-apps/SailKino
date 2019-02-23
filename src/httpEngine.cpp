#include "httpEngine.h"

HTTPEngine::HTTPEngine(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&nam_, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finished(QNetworkReply*)));

}

void HTTPEngine::getEvents(HTTPEngine::queryItemList &queryItems,
                           EventModelType type)
{
    QUrl api_url(baseUrl_ + lang_ + "/xml/Events/?" + generateUrl(queryItems));
    GET(api_url, events, type);
}

void HTTPEngine::getSchedule(HTTPEngine::queryItemList &queryItems)
{
    QUrl api_url(baseUrl_ + lang_ + "/xml/Schedule/?" + generateUrl(queryItems));
    GET(api_url, schedule, InTheatres);
}

void HTTPEngine::getScheduleDates(HTTPEngine::queryItemList &queryItems)
{
    QUrl api_url(baseUrl_ + lang_ + "/xml/ScheduleDates/?" + generateUrl(queryItems));
    GET(api_url, scheduledates, InTheatres);
}

void HTTPEngine::getLanguages()
{
    QUrl api_url(baseUrl_ + lang_ + "/xml/Languages/");
    GET(api_url, languages, InTheatres);
}

QString HTTPEngine::getCurrentLang()
{
    return lang_;
}

void HTTPEngine::getAreas()
{
    QUrl api_url(baseUrl_ + lang_ + "/xml/TheatreAreas/");
    GET(api_url, areas, InTheatres);
}

void HTTPEngine::setLanguage(QString language)
{
    lang_ = language;
}

void HTTPEngine::setLocation(SettingsManager::Country code)
{
    switch(code)
    {
    case SettingsManager::Country::FI:
        baseUrl_ = baseUrlFI;
        break;
    case SettingsManager::Country::EE:
        baseUrl_ = baseUrlEE;
        break;
    case SettingsManager::Country::LV:
        baseUrl_ = baseUrlLV;
        break;
    case SettingsManager::Country::LT:
        baseUrl_ = baseUrlLT;
        break;
    default:
        baseUrl_ = baseUrlFI;
        break;
    }
}

QString HTTPEngine::generateUrl(HTTPEngine::queryItemList &queryItems)
{
    QUrlQuery url;
    url.setQueryItems(queryItems);
    return url.toString();
}

void HTTPEngine::GET(QUrl &url,
                     HTTPEngine::GetMethod method,
                     EventModelType type)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = nam_.get(request);
    hash_[reply] = QPair<GetMethod, EventModelType>(method, type);
}

void HTTPEngine::finished(QNetworkReply *reply)
{
    if(hash_.contains(reply)) {

        switch(hash_[reply].first)
        {
        case events:
            eventsRequest(reply, hash_[reply].second);
            break;
        case schedule:
            scheduleRequest(reply);
            break;
        case scheduledates:
            scheduleDatesRequest(reply);
            break;
        case languages:
            LanguagesRequest(reply);
            break;
        case areas:
            areasRequest(reply);
            break;
        }
        hash_.remove(reply);

    }
}

bool HTTPEngine::checkError(QNetworkReply *finished)
{
    if (finished->error() != QNetworkReply::NoError)
    {
        emit networkError(finished->error());
        return true;
    }
    return false;
}

void HTTPEngine::eventsRequest(QNetworkReply *finished, HTTPEngine::EventModelType type)
{
    if (checkError(finished))
        return;
    QByteArray data = finished->readAll();
    emit eventsReady(data, type);
    finished->deleteLater();
}

void HTTPEngine::scheduleRequest(QNetworkReply *finished)
{
    if (checkError(finished))
        return;
    QByteArray data = finished->readAll();
    emit scheludesReady(data);
    finished->deleteLater();
}

void HTTPEngine::scheduleDatesRequest(QNetworkReply *finished)
{
    if (checkError(finished))
        return;
    QByteArray data = finished->readAll();
    emit scheludeDatesReady(data);
    finished->deleteLater();
}

void HTTPEngine::LanguagesRequest(QNetworkReply *finished)
{
    if (checkError(finished))
        return;
    QByteArray data = finished->readAll();
    emit languagesReady(data);
    finished->deleteLater();
}

void HTTPEngine::areasRequest(QNetworkReply *finished)
{
    if (checkError(finished))
        return;
    QByteArray data = finished->readAll();
    emit areasReady(data);
    finished->deleteLater();
}
