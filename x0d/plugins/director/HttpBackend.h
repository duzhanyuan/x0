// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include "Backend.h"

/*!
 * implements the HTTP backend.
 *
 * \see FastCgiBackend
 */
class HttpBackend : public Backend {
private:
    class Connection;

public:
    HttpBackend(BackendManager* director, const std::string& name, const x0::SocketSpec& socketSpec, size_t capacity, bool healthChecks);
    ~HttpBackend();

    const std::string& protocol() const override;
    bool process(RequestNotes* rn) override;
};


