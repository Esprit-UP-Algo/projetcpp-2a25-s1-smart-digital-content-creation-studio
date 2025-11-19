#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>

class Connection
{
public:
    static Connection* instance();
    bool createConnect();

private:
    Connection();
    ~Connection();
    Connection(const Connection&)=delete;
    Connection& operator=(const Connection&)=delete;

    static Connection* p_instance;
};

#endif // CONNECTION_H
