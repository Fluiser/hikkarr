#pragma once
#include <mysql/jdbc.h>
#include <iostream>
#include "database_type.h"
#include <boost/visit_each.hpp>
#include <boost/type_index.hpp>
#include <util/util.hpp>
#include <variant>
#include <initializer_list>
#include <string>
#include <format>

#define GG_TO_STRING(Type, howtocast)   template<class T>\
                                        static std::string to_string(const Type& s) { return howtocast; }
#define S_HANDLESQL(Type, howtocast)    template<class T>\
                                         void handleSQL(const char* n, Type* p) { howtocast; }

namespace {
    struct separator {
        std::string& str;
        separator(std::string& _str): str(_str) {}
        ~separator() {}

        GG_TO_STRING(std::string, ('\'' + s + '\''))
        GG_TO_STRING(dpp::snowflake, fast_tostring((u64)s))
        GG_TO_STRING(i32, fast_tostring(s))
        GG_TO_STRING(u32, fast_tostring(s))
        GG_TO_STRING(i64, fast_tostring(s))
        GG_TO_STRING(u64, fast_tostring(s))
        GG_TO_STRING(f32, fast_tostring(s))
        GG_TO_STRING(f64, fast_tostring(s))

        template<class T>
        static std::string to_string(const T& v) { return std::to_string(v); }


        template<class T>
        static bool isEmpty(const T& v) { return (!(bool)v); }
        static bool isEmpty(const dpp::snowflake& s) { return s.empty(); }
        static bool isEmpty(const std::string s) {return s.empty();}

        template<class T>
        void operator()(const T& val)
        {
            if(!separator::isEmpty(val.get())) {
                if(str.empty()) str = std::format(" WHERE {}={}", val.name(), separator::to_string<decltype(val.get())>(val.get()));
                else str += std::format(" AND {}={}", val.name(), separator::to_string<decltype(val.get())>(val.get()));
            }
        }
    };
    template<class T>
    std::unique_ptr<T> cptr(T* ptr)
    {
        return std::unique_ptr<T>(ptr);
    }

    struct updater {
        std::string& string;

        template<class T>
        void operator()(const T& val)
        {
            if(!separator::isEmpty(val.get())) {
                if(string.empty()) string = std::format("{}={}", val.name(), separator::to_string<decltype(val.get())>(val.get()));
                else string += std::format(" {}={}", val.name(), separator::to_string<decltype(val.get())>(val.get()));
            }
        }

        updater(std::string& _str): string(_str) {}
        ~updater() {}
    };

    struct setter {

        sql::ResultSet& res;
        setter(sql::ResultSet& _res): res(_res){}
        ~setter() {}

        S_HANDLESQL(i32, (*p) = res.getInt(n); )
        S_HANDLESQL(u32, (*p) = res.getUInt(n); )
        S_HANDLESQL(i64, (*p) = res.getInt64(n); )
        S_HANDLESQL(u64, (*p) = res.getUInt64(n); )
        S_HANDLESQL(f32, (*p) = res.getDouble(n); )
        S_HANDLESQL(f64, (*p) = res.getDouble(n); )


        S_HANDLESQL(std::string, (*p) = res.getString(n); )
        S_HANDLESQL(dpp::snowflake, (*p) = res.getUInt64(n); )


        template<class T>
        void operator()(T val)
        {
            using fuckingType = typename std::decay<decltype(val.get())>::type;
            (fuckingType)this->handleSQL<fuckingType>(val.name(), &val.get());
        }
    };

    struct inserter {
        std::string& fields;
        std::string& values;

        template<class T>
        void operator()(T val)
        {
//             if(!separator::isEmpty(val.get())) {
                if(fields.empty())
                    fields = val.name();
                else {
                    fields += ',';
                    fields += val.name();
                }
                if(values.empty())
                    values = separator::to_string<decltype(val.get())>(val.get());
                else {
                    values += ',';
                    values += separator::to_string<decltype(val.get())>(val.get());
                }
//            }
        }

        inserter(std::string& f, std::string& v): fields(f), values(v) {}
        ~inserter() {}
    };
}

class database_t
{
    inline static sql::mysql::MySQL_Driver* driver = nullptr;
    sql::Connection* connection;
public:

    database_t() = default;
    database_t(const database_t&) = delete;
    ~database_t();

    void connect(const char* address, const char* username, const char* password, const char* schema = "hikkarr");
    void _create_tables();

    //all strings must be separated: ' -> \\\'
    template<class T>
    T get(const T& query)
    {
        std::string rq = std::format("SELECT * FROM {}", (boost::typeindex::type_id<T>().pretty_name()));
        {
            std::string qry;
            visit_each(query, separator(qry));
            rq += qry;
        }
        rq += " LIMIT 1";
        auto stat = cptr(connection->prepareStatement(rq));
        auto res = cptr(stat->executeQuery());

        T value;

        if(res->next())
        {
            visit_each(value, setter(*res));
        }

        res->close();
        stat->close();

        return value;
    }
    template<class T>
    std::vector<T> getAll(const T& query)
    {
        std::string rq = "SELECT * FROM ";
        {
            std::string qry;
            visit_each(query, separator(qry));
            rq += qry;
        }
        auto stat = cptr(connection->prepareStatement(rq));
        auto res = cptr(stat->executeQuery());


        std::vector<T> vec;

        while(res->next())
        {
            vec.emplace_back();
            T& value = vec.back();
            visit_each(value, setter(*res));
        }

        stat->close();
        res->close();

        return vec;
    }

    template<class T>
    void insert(const T& arg)
    {
        std::string query;

        {
            std::string fields;
            std::string values;
            std::string qry;
            visit_each(arg, inserter(fields, values));
            query = std::format("INSERT INTO {}({}) VALUES({})",
                                (boost::typeindex::type_id<T>().pretty_name()),
                                fields,
                                values);
        }
        auto stat = cptr(connection->prepareStatement(query));
        auto res = cptr(stat->executeQuery());

        res->close();
        stat->close();
    }
    template<class T>
    void insert(const std::vector<T>& args) { for(const auto& v: args) this->insert(v); }

    template<class T>
    void update(const T& values, const std::variant<std::monostate, T>& where = std::monostate())
    {
        std::string request;
        {
            std::string s_valuesset;
            visit_each(values, updater(s_valuesset));
            request = std::format("UPDATE {} SET {}", (boost::typeindex::type_id<T>().pretty_name()), s_valuesset);
        }

        if(!std::holds_alternative<std::monostate>(where)) {
            std::string s_where;
            visit_each(std::get<T>(where), separator(s_where));
            request += s_where;
        }
        auto stat = cptr(connection->prepareStatement(request));
        auto res = cptr(stat->executeQuery());

        res->close();
        stat->close();
    }
    template<class T>
    void update(const std::initializer_list<std::pair<std::string, std::string>>& params,
                const std::variant<std::monostate, T>& where = std::monostate())
    {
        std::string request;
        {
            request = std::format("UPDATE {} SET", (boost::typeindex::type_id<T>().pretty_name()));
            for(const auto& p: params)
            {
                request += std::format(" {}={}", p.first, p.second);
            }
        }


        if(!std::holds_alternative<std::monostate>(where)) {
            std::string s_where;
            visit_each(std::get<T>(where), separator(s_where));
            request += s_where;
        }
        auto stat = cptr(connection->prepareStatement(request));
        auto res = cptr(stat->executeQuery());

        res->close();
        stat->close();
    }
    template<class T>
    void update(const T& values, const T& where)
    {
        update(values, std::variant<std::monostate, T>(where));
    }
    template<class T>
    void update(const std::initializer_list<std::pair<std::string, std::string>>& params,
                const T& where)
    {
        update(params, std::variant<std::monostate, T>(where));
    }

    template<class T>
    void remove(const T& arg)
    {
        std::string rq = std::format("DELETE FROM {}", (boost::typeindex::type_id<T>().pretty_name()));
        {
            std::string qry;
            visit_each(arg, separator(qry));
            rq += qry;
        }
        auto stat = cptr(connection->prepareStatement(rq));
        auto res = cptr(stat->executeQuery());

        res->close();
        stat->close();
    }

    template<class T>
    i32 count(const T& arg)
    {
        std::string rq = std::format("SELECT COUNT(*) FROM {}", (boost::typeindex::type_id<T>().pretty_name()));
        {
            std::string qry;
            visit_each(arg, separator(qry));
            rq += qry;
        }
        auto stat = cptr(connection->prepareStatement(rq));
        auto res = cptr(stat->executeQuery());

        res->next();
        i32 value = res->getInt(1);

        res->close();
        stat->close();
        return value;
    }


    template<class T, class D>
    void update_ifnotexist_create(const std::initializer_list<std::pair<std::string,std::string>>& args,
                                  const T& where, const D& data)
    {
        if(this->count(where) < 1) {
            this->create(T{}, data);
        }
        this->update(args, where);
    }

    template<class T, class D>
    void create(const T& argument, const D& data)
    {
        T newdata = argument;
        newdata.id = data.id;
        this->insert(newdata);
    }

};
