// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <base/sql/SqlConnection.h>
#include <base/sql/SqlResult.h>

#include <stdio.h>
#include <mysql.h>
#include <errmsg.h>

namespace base {

SqlConnection::SqlConnection()
    : handle_(), username_(), passwd_(), database_(), hostname_(), port_(-1) {
  mysql_init(&handle_);
}

SqlConnection::~SqlConnection() { mysql_close(&handle_); }

bool SqlConnection::open(const char *hostname, const char *username,
                         const char *passwd, const char *database, int port) {
  if (mysql_real_connect(&handle_, hostname, username, passwd, database, port,
                         0, 0) == NULL)
    return false;

  username_ = username;
  passwd_ = passwd;
  database_ = database;
  hostname_ = hostname;
  port_ = port;
  return true;
}

bool SqlConnection::isOpen() const {
  return mysql_ping(const_cast<MYSQL *>(&handle_)) == 0;
}

bool SqlConnection::ping() { return mysql_ping(&handle_) == 0; }

MYSQL *SqlConnection::handle() { return &handle_; }

SqlConnection::operator MYSQL *() const {
  return const_cast<MYSQL *>(&handle_);
}

std::string SqlConnection::makeQuery(const char *s) {
#if !defined(XZERO_NDEBUG)
  const char *ps = s;

  while (*s) {
    if (*s == '?' && *(++s) != '?') {
      fprintf(stderr, "invalid format string: missing args\n");
    }
    ++s;
  }

  return ps;
#else
  return s;
#endif
}

template <>
std::string SqlConnection::queryField<std::string>(const char *tableName,
                                                   const char *keyName,
                                                   const char *keyValue,
                                                   const char *fieldName) {
  SqlResult result(query("SELECT `?` FROM `?` WHERE `?` = '?'", fieldName,
                         tableName, keyName, keyValue));

  if (result.fetch()) return result.valueOf(fieldName);

  return std::string();
}

unsigned long long SqlConnection::affectedRows() const {
  return mysql_affected_rows(const_cast<MYSQL *>(&handle_));
}

}  // namespace base