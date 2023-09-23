#include "database.hpp"
#include <iostream>
#include <memory>
#include <fstream>

void database_t::connect(const char* address, const char* username, const char* password, const char* schema)
{
    try{
        if(!database_t::driver)
        {
            database_t::driver = sql::mysql::get_driver_instance();
        }
        connection = database_t::driver->connect(address, username, password);
        connection->setSchema(schema);
    } catch(sql::SQLException& e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() <<
            " )" << std::endl;
    }
}

void database_t::_create_tables()
{
    std::ifstream file("typesql.txt");
    std::string line;

    while(std::getline(file, line))
    {

        bool hasSymbols = false;
        for(const auto& s: line)
        {
            if(s != ' ' || s != '\r' || s != '\n') {
                hasSymbols = true;
                break;
            }
        }
        if(!hasSymbols) continue;

        std::string queryStr = "CREATE TABLE IF NOT EXISTS ";
        queryStr += line;

        std::cout << line << "\n";

        auto stat = cptr(connection->prepareStatement(queryStr));
        auto res = cptr(stat->executeQuery());

        stat->close();
        res->close();


        line = "";
    }
}

database_t::~database_t()
{
    if(connection) {
        delete connection;
    }
}
