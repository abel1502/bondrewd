#pragma once

#include <bondrewd/internal/common.hpp>

#include <stdexcept>
#include <exception>


namespace bondrewd::lex {


// TODO: Add location info!
// (Either to the error or to the lexer state)
DECLARE_ERROR(LexicalError, std::runtime_error);


}  // namespace bondrewd::lex
