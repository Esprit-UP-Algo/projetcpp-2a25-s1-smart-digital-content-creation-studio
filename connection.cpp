#include "connection.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

Connection* Connection::p_instance = nullptr;

Connection::Connection() {}
Connection* Connection::instance()
{
    if(!p_instance)
        p_instance = new Connection();
    return p_instance;
}

bool Connection::createConnect()
{
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("projet");
    db.setUserName("smart_content");
    db.setPassword("smart_content123");

    if(db.open())
    {
        test = true;
        qDebug() << "Database connected successfully!";
    }
    else
        qDebug() << "Database connection failed:" << db.lastError().text();

    return test;
}

Connection::~Connection()
{
    QSqlDatabase db = QSqlDatabase::database();
    if(db.isOpen())
    {
        db.close();
        qDebug() << "Database connection closed.";
    }
}
