# httpclient

## Introduction

`httpclient.h` is a C++ header file that provides a wrapper around the QNetworkAccessManager class in Qt. It simplifies performing HTTP requests in Qt by supporting both synchronous and asynchronous methods.

## Table of Contents

- [Introduction](#introduction)
- [Classes](#classes)
  - [NetworkException](#networkexception)
  - [HttpClient](#httpclient)
- [Functions](#functions)
  - [writeFile](#writefile)
  - [imageFromBytes](#imagefrombytes)
- [Usage](#usage)
  - [Syncronous APIs](#syncronous-apis)
  - [Asyncronous APIs](#asyncronous-apis)
- [Linking with CMAKE](#linking-with-cmake)

## Classes

### NetworkException

Custom exception thrown when synchronous network calls fail. The caller must catch this exception to avoid segmentation faults.

#### Public Methods

- `NetworkException(int statusCode, const QString &message)`: Constructs a new NetworkException object.
- `int getStatusCode() const`: Gets the status code of the response.
- `const char *what() const noexcept`: Virtual override for the std::exception.

### HttpClient

Wrapper class around the QNetworkAccessManager to simplify performing HTTP requests in Qt.

#### Public Methods

- `HttpClient(QObject *parent = nullptr)`
  Constructs a new HttpClient object.
- `HttpClient(QObject *parent, const QMap<QString, QString> &headers)`
  Constructs a new HttpClient object with parent and default HTTP headers.
- `virtual ~HttpClient()`
  Destroys the HttpClient object.
- `static void setRootCA(QString certPath)`
  Sets the Root CA certificate.
- `static void setBearerToken(const QString &jwtToken)`
  Sets the Bearer Token for authentication.
- `void get(const QString &url) noexcept`
  Performs a GET request asynchronously.
- `void post(const QString &url, const QByteArray &data) noexcept`
  Performs a POST request asynchronously.
- `void put(const QString &url, const QByteArray &data) noexcept`
  Performs a PUT request asynchronously.
- `void patch(const QString &url, const QByteArray &data) noexcept`
  Performs a PATCH request asynchronously.
- `void del(const QString &url) noexcept`
  Performs a DELETE request asynchronously.
- `QByteArray get_sync(const QString &url)`
  Performs a synchronous GET request and blocks until the response arrives.
- `QByteArray post_sync(const QString &url, const QByteArray &data)`
  Performs a synchronous POST request and blocks until the response arrives.
- `QByteArray put_sync(const QString &url, const QByteArray &data)`
  Performs a synchronous PUT request and blocks until the response arrives.
- `QByteArray patch_sync(const QString &url, const QByteArray &data)`
  Performs a synchronous PATCH request and blocks until the response arrives.
- `QByteArray del_sync(const QString &url)`
  Performs a synchronous DELETE request and blocks until the response arrives.

#### Signals

- `success(const QByteArray &data)`: Signal emitted when an asynchronous network call succeeds.
- `error(const QString &errorString)`: Signal emitted when an asynchronous network call fails.

#### Private Slots

- `onReplyFinished()`: Slot for internal use to process network replies.

## Functions

### writeFile

Writes data to a file.

#### Parameters

- `const QString &path`: The path of the file to write.
- `const QByteArray &data`: The data to write.

### imageFromBytes

Creates a QImage from a byte array.

#### Parameters

- `const QByteArray &data`: The byte array containing image data.

## Header File

[httpclient.h](http://link-to-your-httpclient-header-file)

## Usage

### Asyncronous APIs

```cpp

#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <iostream>

#include <httpclient/httpclient.h>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    QMap<QString, QString> headers;
    headers.insert("Content-Type", "application/json");

    // Create an instance of the HttpClient
    HttpClient httpClient(nullptr, headers);

    // Make a GET request
    QString url = "https://api.mysite.com/api/login";

    QJsonObject jsonObject;
    jsonObject.insert("username", "");
    jsonObject.insert("password", "");

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();
    httpClient.post(url, jsonData);

    // Connect signals from the HttpClient
    QObject::connect(&httpClient, &HttpClient::success, [](const QByteArray& data) {
        QTextStream(stdout) << data << "\n";
    });

    QObject::connect(&httpClient, &HttpClient::error, [](const QString& errorString) {
        QTextStream(stderr) << errorString << "\n";
    });

    // Get verified user
    url = "https://api.mysite.com/api/verify-user";
    httpClient.setBearerToken("JWT TOKEN HERE");
    httpClient.get(url);

    HttpClient client2;
    client2.get("https://mysite.com/logo.png");

    QObject::connect(&client2, &HttpClient::success, [](const QByteArray& data) {
        writeFile("logo.png", data);
    });
    return app.exec();
}
```

### Syncronous APIs

```cpp

#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <iostream>

#include <httpclient/httpclient.h>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    QMap<QString, QString> headers;
    headers.insert("Content-Type", "application/json");

    // Create an instance of the HttpClient
    HttpClient httpClient(nullptr, headers);

    // Make a GET request
    QString url = "https://api.mysite.com/api/login";

    QJsonObject jsonObject;
    jsonObject.insert("username", "");
    jsonObject.insert("password", "");

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();

    QByteArray data;

    try {
        data = httpClient.post_sync(url, jsonData);
        QTextStream(stdout) << data << "\n";
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    try {
        // Get verified user
        url = "https://api.mysite.com/api/verify-user";
        httpClient.setBearerToken("token here");
        data = httpClient.get_sync(url);
        QTextStream(stdout) << data << "\n";
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    try {
        HttpClient client2;
        data = client2.get_sync("https://mysite.com/logo.png");
        writeFile("logo-sync.png", data);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return app.exec();
}

```

### Linking with CMAKE

```cmake

cmake_minimum_required(VERSION 3.2)

project(application LANGUAGES CXX)
include(GNUInstallDirs)

find_package(httpclient REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE httpclient::httpclient)

```
